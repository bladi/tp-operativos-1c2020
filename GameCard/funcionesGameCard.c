#include "gameCard.h"

void configurarLoggerGameCard(){

	logger = log_create(unGameCardConfig->logFile, "GAME CARD", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");

}

void cargarConfiguracionGameCard(){

    unGameCardConfig = malloc(sizeof(gameCardConfig_t));

    unGameCardArchivoConfig = malloc(sizeof(t_config));
	unGameCardArchivoConfig = config_create(PATH_CONFIG_GAMECARD);
	       
	if (unGameCardArchivoConfig == NULL) {

        printf("\n=============================================================================================\n");
	    printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL GAME CARD");

	}else{

        printf("\n=======================================================================================\n");
        printf("\nCONFIGURACION DEL GAME CARD IMPORTADA CON EXITO");

        unGameCardConfig->tiempoReintentoConexion = config_get_int_value(unGameCardArchivoConfig, TIEMPO_DE_REINTENTO_CONEXION);
        unGameCardConfig->tiempoReintentoOperacion = config_get_int_value(unGameCardArchivoConfig, TIEMPO_DE_REINTENTO_OPERACION);
        unGameCardConfig->puntoMontajeTallGrass = config_get_string_value(unGameCardArchivoConfig, PUNTO_MONTAJE_TALLGRASS);
        unGameCardConfig->ipBroker = config_get_string_value(unGameCardArchivoConfig, IP_BROKER);
        unGameCardConfig->puertoBroker = config_get_int_value(unGameCardArchivoConfig, PUERTO_BROKER);
        unGameCardConfig->ipGameCard = config_get_string_value(unGameCardArchivoConfig, IP_GAMECARD);
        unGameCardConfig->puertoGameCard = config_get_int_value(unGameCardArchivoConfig, PUERTO_GAMECARD);
        unGameCardConfig->logFile = config_get_string_value(unGameCardArchivoConfig, LOG_FILE);

        printf("\n\n· Tiempo de Reintento de Conexion = %d\n", unGameCardConfig->tiempoReintentoConexion);
        printf("· Tiempo de Reintento de Operacion = %d\n", unGameCardConfig->tiempoReintentoOperacion);
        printf("· Punto de Montaje de Tall Grass = %s\n", unGameCardConfig->puntoMontajeTallGrass);
        printf("· IP del Broker = %s\n", unGameCardConfig->ipBroker);
        printf("· Puerto del Broker = %d\n", unGameCardConfig->puertoBroker);
        printf("· IP del Game Card = %s\n", unGameCardConfig->ipGameCard);
        printf("· Puerto del Game Card = %d\n", unGameCardConfig->puertoGameCard);
        printf("· Ruta del Archivo Log del Game Card = %s\n\n", unGameCardConfig->logFile);

        char* pathMetadata = string_new();
        string_append_with_format(&pathMetadata,"%sMetadata/Metadata.bin", unGameCardConfig->puntoMontajeTallGrass);

        printf("· pathMetadata = %s\n\n", pathMetadata);

        configMetadata = config_create(pathMetadata);

        if(configMetadata == NULL) {
            printf("\n\n- NO SE PUDO IMPORTAR LA METADATA");
            exit(1);
        }

        unGameCardConfig->cantidadDeBloques = config_get_int_value(configMetadata, BLOCKS);
        unGameCardConfig->tamanioBloques = config_get_int_value(configMetadata, BLOCK_SIZE);
        unGameCardConfig->magicNumber = config_get_string_value(configMetadata, MAGIC_NUMBER);


        free(unGameCardArchivoConfig);
        free(configMetadata);
        free(pathMetadata);

    }

}

void inicializarGameCard(){

    cargarConfiguracionGameCard();

	configurarLoggerGameCard();
    
    inicializarBitMap();

    crearBloquesFileSystem();

    inicializarHilosYVariablesGameCard();

}

void finalizarGameCard(){

    free(unGameCardConfig);
    free(logger);
}

void administradorDeConexiones(void* infoAdmin){

    infoAdminConexiones_t* unaInfoAdmin = (infoAdminConexiones_t*) infoAdmin;

    int idCliente = 0;
    int resultado;

    while((resultado = recibirInt(unaInfoAdmin->socketCliente,&idCliente)) > 0){

        switch(idCliente){

            case 1: {

                manejarRespuestaABroker(unaInfoAdmin->socketCliente,idCliente);
                break;

            }

            case 2: {

                manejarRespuestaAGameBoy(unaInfoAdmin->socketCliente,idCliente);
                break;

            }

            case -1: {

                log_info(logger, "RECIBIMOS UNA PRUEBA DE CONEXION");
                break;

            }

            default: {

                log_warning(logger, "RECIBIMOS UN IDENTIFICADOR DE PROCESO NO VALIDO");
                break;

            }
                      
        }

    }

    if(resultado == 0){

        log_warning(logger, "CLIENTE DESCONECTADO");
        fflush(stdout);
        close(unaInfoAdmin->socketCliente);
        
    }else if(resultado < 0){
        
        log_warning(logger, "ERROR AL RECIBIR");
        close(unaInfoAdmin->socketCliente);
       
    }

    return;

}

void actualizarConfiguracionGameCard(){

    FILE *archivoConfigFp;

	while(1){

		sleep(10);

		archivoConfigFp = fopen(PATH_CONFIG_GAMECARD,"rb");

		nuevoIdConfigGameCard = 0;

		while (!feof(archivoConfigFp) && !ferror(archivoConfigFp)) {

   			nuevoIdConfigGameCard ^= fgetc(archivoConfigFp);

		}

        fclose(archivoConfigFp);


        if(cantidadDeActualizacionesConfigGameCard == 0){

        cantidadDeActualizacionesConfigGameCard += 1;

        }else{

            if (nuevoIdConfigGameCard != idConfigGameCard) {

                log_info(logger,"El archivo de configuración del Game Card cambió. Se procederá a actualizar.");
                cargarConfiguracionGameCard();
                cantidadDeActualizacionesConfigGameCard += 1;
                
            }

        }

        idConfigGameCard = nuevoIdConfigGameCard;

	}

}

void inicializarHilosYVariablesGameCard(){

    cantidadDeActualizacionesConfigGameCard = 0;

    socketBroker = cliente(unGameCardConfig->ipBroker, unGameCardConfig->puertoBroker, ID_BROKER);

    infoServidor_t* unaInfoServidorGameCard;

    unaInfoServidorGameCard = malloc(sizeof(infoServidor_t));

    unaInfoServidorGameCard->puerto = unGameCardConfig->puertoGameCard;
    unaInfoServidorGameCard->ip = string_new();
    //string_append(&unaInfoServidorGameCard->ip,unGameCardConfig->ipGameCard); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    string_append(&unaInfoServidorGameCard->ip,"0");

    pthread_create(&hiloActualizadorConfigGameCard, NULL, (void*)actualizarConfiguracionGameCard, NULL);
    pthread_create(&hiloServidorGameCard,NULL,(void*)servidor_inicializar,(void*)unaInfoServidorGameCard);

    pthread_join(hiloActualizadorConfigGameCard, NULL);

}

void manejarRespuestaAGameBoy(int socketCliente,int idCliente){

    int* tipoMensaje = malloc(sizeof(int));
	int* tamanioMensaje = malloc(sizeof(int));

	void* buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch(*tipoMensaje){

        case tNewPokemon: {
            
            t_newPokemon* unNewPokemon = (t_newPokemon*) buffer;
            int resultado;
            int tipoResultado = 0;

            log_info(logger,"Se recibió un 'NEW_POKEMON':");
            log_info(logger,"El ID del mensaje es: %d",unNewPokemon->identificador);
            log_info(logger,"El ID correlacional del mensaje es: %d.",unNewPokemon->identificadorCorrelacional);
            log_info(logger,"La posición del Pokémon en el mapa es: [%d,%d].", unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY);
            log_info(logger,"El nombre del Pokemón es: %s.",unNewPokemon->nombrePokemon);
            log_info(logger,"La cantidad del pokemón es: %d.", unNewPokemon->cantidadDePokemon);

            int resultadoOperacionPokemon;

            if(existePokemon(unNewPokemon->nombrePokemon)){

                log_info(logger,"El pokemon solicitado ya existia en Game Card. Se procederá a modificar sus valores.");

                while(false/*carpetaAbierta(unNewPokemon->nombrePokemon)*/){

                    log_info(logger,"La carpeta del pokemón esta abierta en este momento, se intentará nuevamente en %d segundos.",unGameCardConfig->tiempoReintentoOperacion);
                    sleep(unGameCardConfig->tiempoReintentoOperacion);

                }

                //Posible mutex
                resultadoOperacionPokemon = 1;//modificarPokemon(unNewPokemon->nombrePokemon,unNewPokemon->posicionEnElMapaX,unNewPokemon->posicionEnElMapaY,unNewPokemon->cantidad);
                //Fin posible mutex

            }else{

                log_info(logger,"El pokemon solicitado no existia en Game Card. Se procederá a crearlo.");
                //Posible mutex
                resultadoOperacionPokemon = crearPokemon(unNewPokemon->nombrePokemon,unNewPokemon->posicionEnElMapaX,unNewPokemon->posicionEnElMapaY,unNewPokemon->cantidadDePokemon);
                //Fin posible mutex

            }
            
            enviarInt(socketCliente,resultadoOperacionPokemon);
            log_info(logger,"Se envió el resultado de la operación al Game Boy.");

            //PREGUNTAR SI HAY QUE MANDAR IGUAL EL APPEARED POKEMON CUANDO EL POKEMON YA EXISTÍA EN UNA POSICIÓN. POR AHORA LO HACEMOS IGUAL.

            t_appearedPokemon* unAppearedPokemon = malloc(sizeof(t_appearedPokemon));

            unAppearedPokemon->identificador = unNewPokemon->identificador;                 //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unAppearedPokemon->identificadorCorrelacional = unNewPokemon->identificador;    //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unAppearedPokemon->nombrePokemon = unNewPokemon->nombrePokemon;
            unAppearedPokemon->posicionEnElMapaX = unNewPokemon->posicionEnElMapaX;
            unAppearedPokemon->posicionEnElMapaY = unNewPokemon->posicionEnElMapaY;

            *tamanioMensaje = 0;

            enviarInt(socketBroker, 3);
            enviarPaquete(socketBroker, tAppearedPokemon, unAppearedPokemon, tamanioMensaje);

            if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

                 if(tipoResultado == 1){

                    log_info(logger,"El APPEARED_POKEMON se envió con éxito al Broker.");

                  }else if(tipoResultado == 0){

                    log_error(logger,"El APPEARED_POKEMON no se envió con éxito al Broker.");

                  }

            }else{

                log_error(logger,"El APPEARED_POKEMON no se envió con éxito al Broker. El Broker está desconectado.");

            }
            
            break;

        }

        case tGetPokemon: {
 
            t_getPokemon* unGetPokemon = (t_getPokemon*) buffer;
            int resultado;
            int tipoResultado = 0;

            log_info(logger,"Se recibió un 'GET_POKEMON':");
            log_info(logger,"El ID del pókemon es %d.", unGetPokemon->identificador);
            log_info(logger,"El ID correlacional del pókemon es %d.",unGetPokemon->identificadorCorrelacional);
            log_info(logger,"El nombre del Pokemón es: %s.",unGetPokemon->nombrePokemon);

            int resultadoOperacion = existePokemon(unGetPokemon->nombrePokemon);

            enviarInt(socketCliente, resultadoOperacion);
            log_info(logger,"Se le envió al Game Boy el resultado de la operación.");
            
            if(resultadoOperacion){

                log_info(logger,"El pokemón requerido existe en Game Card.");

                while(false/*carpetaAbierta(unGetPokemon->nombrePokemon)*/){

                    log_info(logger,"La carpeta del pokemón esta abierta en este momento, se intentará nuevamente en %d segundos.",unGameCardConfig->tiempoReintentoOperacion);
                    sleep(unGameCardConfig->tiempoReintentoOperacion);

                }

                t_localizedPokemon* unLocalizedPokemon = malloc(sizeof(t_localizedPokemon));

                //Posible mutex
                //unLocalizedPokemon->listaDatosPokemon = dameDatosPokemon(unGetPokemon->nombrePokemon);
                //Fin posible mutex

                unLocalizedPokemon->identificador = unGetPokemon->identificador;                 //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unLocalizedPokemon->identificadorCorrelacional = unGetPokemon->identificador;    //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unLocalizedPokemon->nombrePokemon = unGetPokemon->nombrePokemon;

                *tamanioMensaje = 0;

                enviarInt(socketBroker, 3);
                enviarPaquete(socketBroker, tLocalizedPokemon, unLocalizedPokemon, tamanioMensaje);

                if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

                    if(tipoResultado == 1){

                        log_info(logger,"El LOCALIZED_POKEMON se envió con éxito al Broker.");

                    }else if(tipoResultado == 0){

                        log_error(logger,"El LOCALIZED_POKEMON no se envió con éxito al Broker.");

                    }

                }else{

                    log_error(logger,"El LOCALIZED_POKEMON no se envió con éxito al Broker. El Broker está desconectado.");

                }


            }else{

                log_error(logger,"El pokemón requerido no existe en Game Card.");

            }

            break;

        }

        case tCatchPokemon: {
            
            t_catchPokemon* unCatchPokemon = (t_catchPokemon*) buffer;
            int resultado;
            int tipoResultado = 0;

            log_info(logger,"Se recibió un 'CATCH_POKEMON':");
            log_info(logger,"El ID del mensaje es: %d.",unCatchPokemon->identificador);
            log_info(logger,"El ID correlacional del mensaje es: %d.",unCatchPokemon->identificadorCorrelacional);
            log_info(logger,"El nombre del pokemón es: %s.",unCatchPokemon->nombrePokemon);
            log_info(logger,"La posicion del pokémon en el mapa es: [%d,%d].", unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY);

            int resultadoOperacion = existePokemon(unCatchPokemon->nombrePokemon);

            if(resultadoOperacion){

                log_info(logger,"El pokemón a atrapar existe en Game Card.");

                while(false/*carpetaAbierta(unCatchPokemon->nombrePokemon)*/){

                    log_info(logger,"La carpeta del pokemón esta abierta en este momento, se intentará nuevamente en %d segundos.",unGameCardConfig->tiempoReintentoOperacion);
                    sleep(unGameCardConfig->tiempoReintentoOperacion);

                }

                t_caughtPokemon* unCaughtPokemon = malloc(sizeof(t_caughtPokemon));

                //Posible mutex
                //unCaughtPokemon->resultado = atraparPokemon(unCatchPokemon->nombrePokemon, unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY, -1);
                //Fin posible mutex

                unCaughtPokemon->identificador = unCatchPokemon->identificador;                 //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unCaughtPokemon->identificadorCorrelacional = unCatchPokemon->identificador;    //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unCaughtPokemon->nombrePokemon = unCatchPokemon->nombrePokemon;

                enviarInt(socketCliente, unCaughtPokemon->resultado);
                log_info(logger,"Se le envió al Game Boy el resultado de la operación.");

                *tamanioMensaje = 0;

                enviarInt(socketBroker, 3);
                enviarPaquete(socketBroker, tCaughtPokemon, unCaughtPokemon, tamanioMensaje);

                if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

                    if(tipoResultado == 1){

                        log_info(logger,"El CAUGHT_POKEMON se envió con éxito al Broker.");

                    }else if(tipoResultado == 0){

                        log_error(logger,"El CAUGHT_POKEMON no se envió con éxito al Broker.");

                    }

                }else{

                    log_error(logger,"El CAUGHT_POKEMON no se envió con éxito al Broker. El Broker está desconectado.");

                }


            }else{

                log_error(logger,"El pokemón requerido no existe en Game Card.");

            }

            break;

        }

        default:{

            log_error(logger,"Recibimos algo del Game Boy que no sabemos manejar: %d",*tipoMensaje);
            abort();
            break;

        }

    }

    free(tipoMensaje);
    free(tamanioMensaje);
	free(buffer);

    return;
}

void manejarRespuestaABroker(int socketCliente,int idCliente){

    int* tipoMensaje = malloc(sizeof(int));
	int* tamanioMensaje = malloc(sizeof(int));

	void* buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch(*tipoMensaje){

        case tNewPokemon: {
            
            t_newPokemon* unNewPokemon = (t_newPokemon*) buffer;
            int resultado;
            int tipoResultado = 0;

            log_info(logger,"Se recibió un 'NEW_POKEMON':");
            log_info(logger,"El ID del mensaje es: %d",unNewPokemon->identificador);
            log_info(logger,"El ID correlacional del mensaje es: %d.",unNewPokemon->identificadorCorrelacional);
            log_info(logger,"La posición del Pokémon en el mapa es: [%d,%d].", unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY);
            log_info(logger,"El nombre del Pokemón es: %s.",unNewPokemon->nombrePokemon);
            log_info(logger,"La cantidad del pokemón es: %d.", unNewPokemon->cantidadDePokemon);

            int resultadoOperacionPokemon;

            if(existePokemon(unNewPokemon->nombrePokemon)){

                log_info(logger,"El pokemon solicitado ya existia en Game Card. Se procederá a modificar sus valores.");

                while(false/*carpetaAbierta(unNewPokemon->nombrePokemon)*/){

                    log_info(logger,"La carpeta del pokemón esta abierta en este momento, se intentará nuevamente en %d segundos.",unGameCardConfig->tiempoReintentoOperacion);
                    sleep(unGameCardConfig->tiempoReintentoOperacion);

                }

                //Posible mutex
                resultadoOperacionPokemon = 1;//modificarPokemon(unNewPokemon->nombrePokemon,unNewPokemon->posicionEnElMapaX,unNewPokemon->posicionEnElMapaY,unNewPokemon->cantidad);
                //Fin posible mutex

            }else{

                log_info(logger,"El pokemon solicitado no existia en Game Card. Se procederá a crearlo.");
                //Posible mutex
                resultadoOperacionPokemon = crearPokemon(unNewPokemon->nombrePokemon,unNewPokemon->posicionEnElMapaX,unNewPokemon->posicionEnElMapaY,unNewPokemon->cantidadDePokemon);
                //Fin posible mutex

            }
            
            enviarInt(socketCliente,resultadoOperacionPokemon);
            log_info(logger,"Se envió el resultado de la operación al Broker.");

            //PREGUNTAR SI HAY QUE MANDAR IGUAL EL APPEARED POKEMON CUANDO EL POKEMON YA EXISTÍA EN UNA POSICIÓN. POR AHORA LO HACEMOS IGUAL.

            t_appearedPokemon* unAppearedPokemon = malloc(sizeof(t_appearedPokemon));

            unAppearedPokemon->identificador = unNewPokemon->identificador;                 //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unAppearedPokemon->identificadorCorrelacional = unNewPokemon->identificador;    //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unAppearedPokemon->nombrePokemon = unNewPokemon->nombrePokemon;
            unAppearedPokemon->posicionEnElMapaX = unNewPokemon->posicionEnElMapaX;
            unAppearedPokemon->posicionEnElMapaY = unNewPokemon->posicionEnElMapaY;

            *tamanioMensaje = 0;

            enviarInt(socketCliente, 3);
            enviarPaquete(socketCliente, tAppearedPokemon, unAppearedPokemon, tamanioMensaje);

            if((resultado = recibirInt(socketCliente,&tipoResultado)) > 0){

                 if(tipoResultado == 1){

                    log_info(logger,"El APPEARED_POKEMON se envió con éxito al Broker.");

                  }else if(tipoResultado == 0){

                    log_error(logger,"El APPEARED_POKEMON no se envió con éxito al Broker.");

                  }

            }else{

                log_error(logger,"El APPEARED_POKEMON no se envió con éxito al Broker. El Broker está desconectado.");

            }
            
            break;

        }

        case tGetPokemon: {
 
            t_getPokemon* unGetPokemon = (t_getPokemon*) buffer;
            int resultado;
            int tipoResultado = 0;

            log_info(logger,"Se recibió un 'GET_POKEMON':");
            log_info(logger,"El ID del pókemon es %d.", unGetPokemon->identificador);
            log_info(logger,"El ID correlacional del pókemon es %d.",unGetPokemon->identificadorCorrelacional);
            log_info(logger,"El nombre del Pokemón es: %s.",unGetPokemon->nombrePokemon);

            int resultadoOperacion = existePokemon(unGetPokemon->nombrePokemon);

            enviarInt(socketCliente, resultadoOperacion);
            log_info(logger,"Se le envió al Broker el resultado de la operación.");
            
            if(resultadoOperacion){

                log_info(logger,"El pokemón requerido existe en Game Card.");

                while(false/*carpetaAbierta(unGetPokemon->nombrePokemon)*/){

                    log_info(logger,"La carpeta del pokemón esta abierta en este momento, se intentará nuevamente en %d segundos.",unGameCardConfig->tiempoReintentoOperacion);
                    sleep(unGameCardConfig->tiempoReintentoOperacion);

                }

                t_localizedPokemon* unLocalizedPokemon = malloc(sizeof(t_localizedPokemon));

                //Posible mutex
                //unLocalizedPokemon->listaDatosPokemon = dameDatosPokemon(unGetPokemon->nombrePokemon);
                //Fin posible mutex

                unLocalizedPokemon->identificador = unGetPokemon->identificador;                 //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unLocalizedPokemon->identificadorCorrelacional = unGetPokemon->identificador;    //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unLocalizedPokemon->nombrePokemon = unGetPokemon->nombrePokemon;

                *tamanioMensaje = 0;

                enviarInt(socketCliente, 3);
                enviarPaquete(socketCliente, tLocalizedPokemon, unLocalizedPokemon, tamanioMensaje);

                if((resultado = recibirInt(socketCliente,&tipoResultado)) > 0){

                    if(tipoResultado == 1){

                        log_info(logger,"El LOCALIZED_POKEMON se envió con éxito al Broker.");

                    }else if(tipoResultado == 0){

                        log_error(logger,"El LOCALIZED_POKEMON no se envió con éxito al Broker.");

                    }

                }else{

                    log_error(logger,"El LOCALIZED_POKEMON no se envió con éxito al Broker. El Broker está desconectado.");

                }


            }else{

                log_error(logger,"El pokemón requerido no existe en Game Card.");

            }

            break;

        }

        case tCatchPokemon: {
            
            t_catchPokemon* unCatchPokemon = (t_catchPokemon*) buffer;
            int resultado;
            int tipoResultado = 0;

            log_info(logger,"Se recibió un 'CATCH_POKEMON':");
            log_info(logger,"El ID del mensaje es: %d.",unCatchPokemon->identificador);
            log_info(logger,"El ID correlacional del mensaje es: %d.",unCatchPokemon->identificadorCorrelacional);
            log_info(logger,"El nombre del pokemón es: %s.",unCatchPokemon->nombrePokemon);
            log_info(logger,"La posicion del pokémon en el mapa es: [%d,%d].", unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY);

            int resultadoOperacion = existePokemon(unCatchPokemon->nombrePokemon);

            if(resultadoOperacion){

                log_info(logger,"El pokemón a atrapar existe en Game Card.");

                while(false/*carpetaAbierta(unCatchPokemon->nombrePokemon)*/){

                    log_info(logger,"La carpeta del pokemón esta abierta en este momento, se intentará nuevamente en %d segundos.",unGameCardConfig->tiempoReintentoOperacion);
                    sleep(unGameCardConfig->tiempoReintentoOperacion);

                }

                t_caughtPokemon* unCaughtPokemon = malloc(sizeof(t_caughtPokemon));

                //Posible mutex
                //unCaughtPokemon->resultado = atraparPokemon(unCaughtPokemon->nombrePokemon);
                //Fin posible mutex

                unCaughtPokemon->identificador = unCatchPokemon->identificador;                 //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unCaughtPokemon->identificadorCorrelacional = unCatchPokemon->identificador;    //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unCaughtPokemon->nombrePokemon = unCatchPokemon->nombrePokemon;

                enviarInt(socketCliente, unCaughtPokemon->resultado);
                log_info(logger,"Se le envió al Broker el resultado de la operación.");

                *tamanioMensaje = 0;

                enviarInt(socketCliente, 3);
                enviarPaquete(socketCliente, tCaughtPokemon, unCaughtPokemon, tamanioMensaje);

                if((resultado = recibirInt(socketCliente,&tipoResultado)) > 0){

                    if(tipoResultado == 1){

                        log_info(logger,"El CAUGHT_POKEMON se envió con éxito al Broker.");

                    }else if(tipoResultado == 0){

                        log_error(logger,"El CAUGHT_POKEMON no se envió con éxito al Broker.");

                    }

                }else{

                    log_error(logger,"El CAUGHT_POKEMON no se envió con éxito al Broker. El Broker está desconectado.");

                }


            }else{

                log_error(logger,"El pokemón requerido no existe en Game Card.");

            }

            break;

        }

        default:{

            log_error(logger,"Recibimos algo del Game Boy que no sabemos manejar: %d",*tipoMensaje);
            abort();
            break;

        }

    }

    free(tipoMensaje);
    free(tamanioMensaje);
	free(buffer);

    return;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////BITMAP///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void inicializarBitMap(){


    char* pathBitmap = string_new();
    string_append_with_format(&pathBitmap,"%sMetadata/Bitmap.bin",unGameCardConfig->puntoMontajeTallGrass);
         
    if (validarArchivo(pathBitmap)){
        log_info(logger,"BITMAP EXISTENTE ENCONTRADO, SE CARGA EN MEMORIA");
        verificarBitmapPrevio(pathBitmap);
    } else {
        log_info(logger,"NO EXISTE UN BITMAP PREVIO, CREO UNO NUEVO");
        crearArchivoBitmap(pathBitmap);
        verificarBitmapPrevio(pathBitmap);
        
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void verificarBitmapPrevio(char* pathBitmap){
    
    int tamBitmap = unGameCardConfig->cantidadDeBloques/8;

	if(unGameCardConfig->cantidadDeBloques % 8 != 0){
		tamBitmap++;
	}

	int fd = open(pathBitmap, O_RDWR);
	ftruncate(fd, tamBitmap);
    
    struct stat mystat;

    if (fstat(fd, &mystat) < 0) {
		log_error(logger, "ERROR AL ESTABLECER FSTAT PARA BITMAP. SE CREARÁ UNO NUEVO.");
		close(fd);
        exit(-1);
	} else {
        char *bitmap = (char *) mmap(NULL, mystat.st_size, PROT_READ | PROT_WRITE,	MAP_SHARED, fd, 0);
        if (bitmap == MAP_FAILED) {
            log_error(logger, "ERROR AL MAPEAR BITMAP A MEMORIA. SE CREARÁ UNO NUEVO.");
            close(fd);
            //crearArchivoBitmap(pathBitmap);
        } else{
            
            bitarray = bitarray_create_with_mode(bitmap, tamBitmap, LSB_FIRST);
            close(fd);
        }
    }

    //free(pathBitmap);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void crearArchivoBitmap(char* pathBitmap){
    
    int tamBitarray = unGameCardConfig->cantidadDeBloques/8;
    int bit = 0;

    if(unGameCardConfig->cantidadDeBloques % 8 != 0){
        tamBitarray++;
    }

    char* data = malloc(tamBitarray);
    t_bitarray* bitarrayInicial = bitarray_create_with_mode(data,tamBitarray,MSB_FIRST);

    while(bit < unGameCardConfig->cantidadDeBloques){
        bitarray_clean_bit(bitarrayInicial, bit);
        bit ++;
    }

	FILE *bitmap;
	bitmap = fopen(pathBitmap, "wb+");

	//fseek(bitmap, 0, SEEK_SET);
	fwrite(bitarrayInicial->bitarray, 1, bitarrayInicial->size, bitmap);

    log_info(logger,"ARCHIVO BITMAP CREADO EN: %s", pathBitmap);

    bitarray_destroy(bitarrayInicial);
	fclose(bitmap);
	//free(pathBitmap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int validarArchivo(char *path) {
	struct stat buffer;
	int status;
	status = stat(path, &buffer);
	if (status < 0) {
		return 0;
	} else {
		return 1;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int buscarBloqueLibreYOcupar(){
    for(int i = 0; i< unGameCardConfig->cantidadDeBloques; i++){
        if(bitarray_test_bit(bitarray, i)==0){
            bitarray_set_bit(bitarray,i);
            return i;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int cantidadBloquesLibres(){

	int posicion = 0;
	int libres = 0;

    //pthread_mutex_lock(&mutexMemtable);
	while(posicion < unGameCardConfig->cantidadDeBloques){
		if(bitarray_test_bit(bitarray, posicion) == 0){
			libres ++;
		}
		posicion ++;
	}
    //pthread_mutex_unlock(&mutexMemtable);

	return libres;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mostrarEstadoBitmap(){
	int posicion = 0;
	bool a;

    //pthread_mutex_lock(&mutexMemtable);
    while(posicion < unGameCardConfig->cantidadDeBloques){
        if((posicion%10) == 0) printf ("\n");
		a = bitarray_test_bit(bitarray,posicion);
		printf("%i", a);
        posicion ++;
	}
    //pthread_mutex_unlock(&mutexMemtable);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////MANEJO BLOQUES/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void crearBloquesFileSystem(){

    char* pathBloques = string_new();
    string_append_with_format(&pathBloques, "%sBlocks/",unGameCardConfig->puntoMontajeTallGrass);

    //printf("\nPath bloques: %s ", pathBloques);
    //printf("\nCantidad de bloques a crear: %d ", unGameCardConfig->cantidadDeBloques);
    //printf("\n!(pathBloques,0777): %d ", !(pathBloques,0777));

    if (!mkdir(pathBloques,0777)){
       int i = 0;
       
        while(i<unGameCardConfig->cantidadDeBloques){
            FILE* bloque;
            char* pathBloquei = string_new();
            string_append_with_format(&pathBloquei, "%s%d.bin",pathBloques,i);
            bloque = fopen(pathBloquei, "wb+");
            fclose(bloque);
            free(pathBloquei);
            i++;
            printf("\nCreado bloque %d",i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////MANEJO BLOQUES/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int existePokemon(char* pokemon){

	struct stat infoArchivo;
	char* path = string_new();
    string_append_with_format(&path,"%sFiles/%s",unGameCardConfig->puntoMontajeTallGrass, pokemon);
    
	if(stat(path,&infoArchivo) == 0 && S_ISDIR(infoArchivo.st_mode))
	{
		return 1; 
	}
	return 0; 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int crearPokemon(char* pokemon, uint32_t posicionX, uint32_t posicionY, uint32_t cantidad){

    FILE* f;
    char* formatoUbicacion = string_new();
    char* pathPokemon      = string_new();
    char* pathMetadata     = string_new();
    
    string_append_with_format(&formatoUbicacion,"%d-%d=%d",posicionX, posicionY, cantidad);
    int tamanioPokemon = string_length(formatoUbicacion);

    string_append_with_format(&pathPokemon, "%sFiles/%s",unGameCardConfig->puntoMontajeTallGrass, pokemon);

    mkdir(pathPokemon,0777);

    string_append_with_format(&pathMetadata,"%s/Metadata.bin",pathPokemon);
    
    f = fopen(pathMetadata,"w");

    if(f == NULL){
    
        log_error(logger,"NO SE PUDO CREAR EL ARCHIVO METADATA PARA EL POKEMON %s", pokemon);
	
    }else{ 

        char* formatoDirectory = string_new();
        char* formatoSize      = string_new();
        char* formatoBlocks    = string_new();
        char* formatoOpen      = string_new();

        int cantBloquesAOcupar = cantBloquesParaSize(tamanioPokemon);
        int bloquesAEscribir[cantBloquesAOcupar];
        
        if(cantBloquesAOcupar <= cantidadBloquesLibres()){

            for(int j=0; j<cantBloquesAOcupar; j++ ){
                bloquesAEscribir[j] = buscarBloqueLibreYOcupar();
            }

            formatoBlocks = generarStringBlocks(cantBloquesAOcupar, bloquesAEscribir);

        } else{
            log_error(logger, "NO HAY BLOQUES SUFICIENTES PARA CREAR AL POKEMON");
            free(formatoDirectory);
            free(formatoSize);
            free(formatoBlocks);
            free(formatoOpen);
            return 0;
        }

        string_append(&formatoDirectory, "DIRECTORY=N");
        string_append_with_format(&formatoSize,"\nSIZE=%d\n", tamanioPokemon);
        string_append(&formatoDirectory, "\nOPEN=Y");

        fputs(formatoDirectory,f);
        fputs(formatoSize,f);
        fputs(formatoBlocks,f);
        fputs(formatoOpen,f);
        fseek(f, 0, SEEK_SET);
        fclose(f);

        log_info(logger,"ARCHIVO METADATA DEL POKEMON %s CREADO CORRECTAMENTE", pokemon); 

        escribirEnBloques(formatoUbicacion,bloquesAEscribir,cantBloquesAOcupar);

        free(formatoDirectory);
        free(formatoSize);
        free(formatoBlocks);
        free(formatoOpen);
        return 1;
        
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int cantBloquesParaSize(int size){
    int cantBloques = size / unGameCardConfig->tamanioBloques;
    int resto = size % unGameCardConfig->tamanioBloques;

    if(resto > 0){
        cantBloques = cantBloques + 1 ;
    }

    return cantBloques;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* generarStringBlocks(int cantBloques, int bloquesAEscribir[]){
    
    char* stringBlocks = string_new();
    string_append(&stringBlocks,"BLOCKS=[");

    for(int j=0; j<cantBloques; j++ ){

        //pthread_mutex_lock(&mutexBitmap);
        //bloquesAEscribir[j] = buscarBloqueLibreYOcupar();
        //pthread_mutex_unlock(&mutexBitmap);
        if (j==(cantBloques-1)){
            string_append_with_format(&stringBlocks,"%d]",bloquesAEscribir[j]);
        }else{
            string_append_with_format(&stringBlocks,"%d,",bloquesAEscribir[j]);
        }
    }

    return stringBlocks;
      
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int escribirEnBloques(char* ubicaciones, int arregloBloques[], int cantBloques){
    
    int totalEscrito = 0;

    for (int i = 0; i<cantBloques-1;i++){
        char* pathBloque = string_new();
        string_append_with_format(&pathBloque,"%sBlocks/%d.bin",unGameCardConfig->puntoMontajeTallGrass,arregloBloques[i]);
        FILE* f;
        //fopen(pathBloque,f);
        char* escrituraPorBloque = string_substring(ubicaciones,totalEscrito,unGameCardConfig->tamanioBloques);
        f = fopen(pathBloque,"wb+");
        fputs(escrituraPorBloque,f);
        fclose(f);
        totalEscrito += string_length(escrituraPorBloque);
        free(pathBloque);
    }
    
    int restante = string_length(ubicaciones) - totalEscrito;
    
    char* ultimaPorcionAEscribir = string_substring(ubicaciones,totalEscrito,restante);
    char* pathUltimoBloque = string_new();

    string_append_with_format(&pathUltimoBloque,"%sBlocks/%d.bin",unGameCardConfig->puntoMontajeTallGrass,arregloBloques[cantBloques-1]);
    FILE* g;
    g = fopen(pathUltimoBloque,"wb+");
    fputs(ultimaPorcionAEscribir,g);
    fclose(g);

    free(pathUltimoBloque);
    free(ultimaPorcionAEscribir);
    free(ubicaciones);
    
    return 1;

}
