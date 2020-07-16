#include "gameBoy.h"

void configurarLoggerGameBoy(){

	logger = log_create(unGameBoyConfig->logFile, "GAME BOY", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");
}

void cargarConfiguracionGameBoy(){

    unGameBoyConfig = malloc(sizeof(gameBoyConfig_t));

    unGameBoyArchivoConfig = malloc(sizeof(t_config));
    unGameBoyArchivoConfig = config_create(PATH_CONFIG_GAMEBOY);

	if (unGameBoyArchivoConfig == NULL) {

        printf("\n=============================================================================================\n");
	    printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL GAME BOY");

	}else{

        printf("\n=======================================================================================\n");
        printf("\nCONFIGURACION DEL GAME BOY IMPORTADA CON EXITO");

        unGameBoyConfig->ipBroker = config_get_string_value(unGameBoyArchivoConfig, IP_BROKER);
        unGameBoyConfig->ipTeam = config_get_string_value(unGameBoyArchivoConfig, IP_TEAM);
        unGameBoyConfig->ipGameCard = config_get_string_value(unGameBoyArchivoConfig, IP_GAMECARD);
        unGameBoyConfig->ipGameBoy = config_get_string_value(unGameBoyArchivoConfig, IP_GAMEBOY);
        unGameBoyConfig->puertoBroker = config_get_int_value(unGameBoyArchivoConfig, PUERTO_BROKER);
        unGameBoyConfig->puertoTeam = config_get_int_value(unGameBoyArchivoConfig, PUERTO_TEAM);
        unGameBoyConfig->puertoGameCard = config_get_int_value(unGameBoyArchivoConfig, PUERTO_GAMECARD);
        unGameBoyConfig->puertoGameBoy = config_get_int_value(unGameBoyArchivoConfig, PUERTO_GAMEBOY);
        unGameBoyConfig->logFile = config_get_string_value(unGameBoyArchivoConfig, LOG_FILE);

        printf("\n\n· IP del Broker = %s\n", unGameBoyConfig->ipBroker);
        printf("· IP del Team = %s\n", unGameBoyConfig->ipTeam);
        printf("· IP del Game Card = %s\n", unGameBoyConfig->ipGameCard);
        printf("· IP del Game Boy = %s\n", unGameBoyConfig->ipGameBoy);       
        printf("· Puerto del Broker = %d\n", unGameBoyConfig->puertoBroker);
        printf("· Puerto del Team = %d\n", unGameBoyConfig->puertoTeam);
        printf("· Puerto del Game Card = %d\n", unGameBoyConfig->puertoGameCard);
        printf("· Puerto del Game Boy = %d\n", unGameBoyConfig->puertoGameBoy);
        printf("· Ruta del Archivo Log del Game Boy = %s\n\n", unGameBoyConfig->logFile);

        free(unGameBoyArchivoConfig);

    }

}

void inicializarGameBoy(){

    cargarConfiguracionGameBoy();

    configurarLoggerGameBoy();

    inicializarHilosYVariablesGameBoy();

}

void finalizarGameBoy(){

    free(unGameBoyConfig);
    free(logger);

}

void administradorDeConexiones(void* infoAdmin){

    infoAdminConexiones_t* unaInfoAdmin = (infoAdminConexiones_t*) infoAdmin;

    int idCliente = 0;
    int resultado;

    while((resultado = recibirInt(unaInfoAdmin->socketCliente,&idCliente)) > 0){

        manejarRespuestaABroker(unaInfoAdmin->socketCliente,idCliente);

    }

    if(resultado == 0){

        log_warning(logger, "Se desconectó el broker, o bien, terminó el tiempo de suscripción a la cola de mensajes.");
        fflush(stdout);
        close(unaInfoAdmin->socketCliente);
        kill(getpid(),9);
        
    }else if (resultado == -2){

        log_info(logger, "ME HICIERON UN PING\n\n\n");
        
    }
    else if(resultado == -1 || resultado < -2){
        
        log_warning(logger, "Se desconectó el broker, o bien, terminó el tiempo de suscripción a la cola de mensajes.");
        close(unaInfoAdmin->socketCliente);
        kill(getpid(),9);
       
    }

    return;

}

void actualizarConfiguracionGameBoy(){

    FILE *archivoConfigFp;

	while(1){

		sleep(10);

		archivoConfigFp = fopen(PATH_CONFIG_GAMEBOY,"rb");

		nuevoIdConfigGameBoy = 0;

		while (!feof(archivoConfigFp) && !ferror(archivoConfigFp)) {

   			nuevoIdConfigGameBoy ^= fgetc(archivoConfigFp);

		}

        fclose(archivoConfigFp);


        if(cantidadDeActualizacionesConfigGameBoy == 0){

        cantidadDeActualizacionesConfigGameBoy += 1;

        }else{

            if (nuevoIdConfigGameBoy != idConfigGameBoy) {

                log_info(logger,"El archivo de configuración del Game Boy cambió. Se procederá a actualizar.");
                cargarConfiguracionGameBoy();
                cantidadDeActualizacionesConfigGameBoy += 1;
                
            }

        }

        idConfigGameBoy = nuevoIdConfigGameBoy;

	}

}

void inicializarHilosYVariablesGameBoy(){

    cantidadDeActualizacionesConfigGameBoy = 0;

    pthread_create(&hiloActualizadorConfigGameBoy, NULL, (void*)actualizarConfiguracionGameBoy, NULL);

}

void enviarNewPokemonABroker(char* nombrePokemon,char* posicionEnX,char* posicionEnY,char* cantidad){

    socketBroker = cliente(unGameBoyConfig->ipBroker, unGameBoyConfig->puertoBroker, ID_BROKER);

    t_newPokemon* unNewPokemon = malloc(sizeof(t_newPokemon));

    unNewPokemon->identificador = 0;
	unNewPokemon->identificadorCorrelacional = 0;

    unNewPokemon->nombrePokemon = string_new();
	string_append(&unNewPokemon->nombrePokemon,nombrePokemon);

	unNewPokemon->posicionEnElMapaX = atoi(posicionEnX);
	unNewPokemon->posicionEnElMapaY = atoi(posicionEnY);
	unNewPokemon->cantidadDePokemon = atoi(cantidad);

    int tamanioNewPokemon = 0;
    enviarInt(socketBroker, 2);
    enviarPaquete(socketBroker, tNewPokemon, unNewPokemon, tamanioNewPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado >= 1){
            
            log_info(logger,"Pókemon creado con éxito");

          }else if(tipoResultado == 0){

            log_info(logger,"No se pudo crear el pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker");

    }

    free(unNewPokemon);

}

void enviarAppearedPokemonABroker(char* nombrePokemon,char* posicionEnX,char* posicionEnY,char* idMensaje){

    socketBroker = cliente(unGameBoyConfig->ipBroker, unGameBoyConfig->puertoBroker, ID_BROKER);

    t_appearedPokemon* unAppearedPokemon = malloc(sizeof(t_appearedPokemon));

    unAppearedPokemon->identificador = atoi(idMensaje);
	unAppearedPokemon->identificadorCorrelacional = 0;

    unAppearedPokemon->nombrePokemon = string_new();
	string_append(&unAppearedPokemon->nombrePokemon,nombrePokemon);

	unAppearedPokemon->posicionEnElMapaX = atoi(posicionEnX);
	unAppearedPokemon->posicionEnElMapaY = atoi(posicionEnY);

    int tamanioAppearedPokemon = 0;

    enviarInt(socketBroker, 2);
    enviarPaquete(socketBroker, tAppearedPokemon, unAppearedPokemon, tamanioAppearedPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado >= 1){
            
            log_info(logger,"Pókemon apareció con éxito");

          }else if(tipoResultado == 0){

            log_info(logger,"No se pudo hacer aparecer el pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker");

    }

    free(unAppearedPokemon);

}

void enviarCatchPokemonABroker(char* nombrePokemon,char* posicionEnX,char* posicionEnY){

    socketBroker = cliente(unGameBoyConfig->ipBroker, unGameBoyConfig->puertoBroker, ID_BROKER);

    t_catchPokemon* unCatchPokemon = malloc(sizeof(t_catchPokemon));

    unCatchPokemon->identificador = 0;
	unCatchPokemon->identificadorCorrelacional = 0;

    unCatchPokemon->nombrePokemon = string_new();
	string_append(&unCatchPokemon->nombrePokemon,nombrePokemon);

	unCatchPokemon->posicionEnElMapaX = atoi(posicionEnX);
	unCatchPokemon->posicionEnElMapaY = atoi(posicionEnY);

    int tamanioCatchPokemon = 0;

    enviarInt(socketBroker, 2);
    enviarPaquete(socketBroker, tCatchPokemon, unCatchPokemon, tamanioCatchPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado >= 1){
            
            log_info(logger,"Pókemon mandado a atrapar con éxito");

          }else if(tipoResultado == 0){

            log_info(logger,"No se pudo intentar atrapar al pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker");

    }

    free(unCatchPokemon);

}

void enviarCaughtPokemonABroker(char* idMensajeCorrelacional,char* resultado){

    socketBroker = cliente(unGameBoyConfig->ipBroker, unGameBoyConfig->puertoBroker, ID_BROKER);

    t_caughtPokemon* unCaughtPokemon = malloc(sizeof(t_caughtPokemon));

    unCaughtPokemon->identificador = 0;
	unCaughtPokemon->identificadorCorrelacional = (uint32_t) atoi(idMensajeCorrelacional);

    if(strcmp(resultado, "OK") == 0){

        unCaughtPokemon->resultado = true;

    }else{

        unCaughtPokemon->resultado = false;

    }

    int tamanioCaughtPokemon = 0;

    enviarInt(socketBroker, 2);
    enviarPaquete(socketBroker, tCaughtPokemon, unCaughtPokemon, tamanioCaughtPokemon);

    int resultadoOperacion;
    int tipoResultado = 0;

    if((resultadoOperacion = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado >= 1){
            
            log_info(logger,"Aviso de resultado de pokemón atrapado realizado con éxito");

          }else if(tipoResultado == 0){

            log_info(logger,"No se pudo avisar sobre el resultado de atrapar al pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker");

    }

    free(unCaughtPokemon);

}

void enviarGetPokemonABroker(char* nombrePokemon){

    socketBroker = cliente(unGameBoyConfig->ipBroker, unGameBoyConfig->puertoBroker, ID_BROKER);

    t_getPokemon* unGetPokemon = malloc(sizeof(t_getPokemon));

    unGetPokemon->identificador = 0;
	unGetPokemon->identificadorCorrelacional = 0;

    unGetPokemon->nombrePokemon = string_new();
	string_append(&unGetPokemon->nombrePokemon,nombrePokemon);

    int tamanioGetPokemon = 0;

    enviarInt(socketBroker, 2);
    enviarPaquete(socketBroker, tGetPokemon, unGetPokemon, tamanioGetPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado >= 1){
            
            log_info(logger,"Pedido de get pokemón realizado con éxito");

          }else if(tipoResultado == 0){

            log_info(logger,"No se pudo realizar el pedido de get pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker");

    }

    free(unGetPokemon);

}

void enviarAppearedPokemonATeam(char* nombrePokemon,char* posicionEnX,char* posicionEnY){

    socketTeam = cliente(unGameBoyConfig->ipTeam, unGameBoyConfig->puertoTeam, ID_TEAM);

    t_appearedPokemon* unAppearedPokemon = malloc(sizeof(t_appearedPokemon));

    unAppearedPokemon->identificador = 0;
	unAppearedPokemon->identificadorCorrelacional = 0;

    unAppearedPokemon->nombrePokemon = string_new();
	string_append(&unAppearedPokemon->nombrePokemon,nombrePokemon);

	unAppearedPokemon->posicionEnElMapaX = atoi(posicionEnX);
	unAppearedPokemon->posicionEnElMapaY = atoi(posicionEnY);

    int tamanioAppearedPokemon = 0;

    enviarInt(socketTeam, 2);
    enviarPaquete(socketTeam, tAppearedPokemon, unAppearedPokemon, tamanioAppearedPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketTeam,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pókemon apareció con éxito");

          }else if(tipoResultado == 0){

            log_info(logger,"No se pudo hacer aparecer el pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Team");

    }

    free(unAppearedPokemon);

}

void enviarNewPokemonAGameCard(char* nombrePokemon,char* posicionEnX,char* posicionEnY,char* cantidad,char* idMensaje){

    socketGameCard = cliente(unGameBoyConfig->ipGameCard, unGameBoyConfig->puertoGameCard, ID_GAMECARD);

    t_newPokemon* unNewPokemon = malloc(sizeof(t_newPokemon));

    unNewPokemon->identificador = atoi(idMensaje);
	unNewPokemon->identificadorCorrelacional = atoi(idMensaje);

    unNewPokemon->nombrePokemon = string_new();
	string_append(&unNewPokemon->nombrePokemon,nombrePokemon);

	unNewPokemon->posicionEnElMapaX = atoi(posicionEnX);
	unNewPokemon->posicionEnElMapaY = atoi(posicionEnY);
	unNewPokemon->cantidadDePokemon = atoi(cantidad);

    int tamanioNewPokemon = 0;

    enviarInt(socketGameCard, 2);
    enviarPaquete(socketGameCard, tNewPokemon, unNewPokemon, tamanioNewPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketGameCard,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pókemon creado con éxito");

          }else if(tipoResultado == 0){

            log_info(logger,"No se pudo crear el pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Game Card");

    }

    free(unNewPokemon);

}

void enviarCatchPokemonAGameCard(char* nombrePokemon,char* posicionEnX,char* posicionEnY,char* idMensaje){

    socketGameCard = cliente(unGameBoyConfig->ipGameCard, unGameBoyConfig->puertoGameCard, ID_GAMECARD);

    t_catchPokemon* unCatchPokemon = malloc(sizeof(t_catchPokemon));

    unCatchPokemon->identificador = atoi(idMensaje);
	unCatchPokemon->identificadorCorrelacional = atoi(idMensaje);

    unCatchPokemon->nombrePokemon = string_new();
	string_append(&unCatchPokemon->nombrePokemon,nombrePokemon);

	unCatchPokemon->posicionEnElMapaX = atoi(posicionEnX);
	unCatchPokemon->posicionEnElMapaY = atoi(posicionEnY);

    int tamanioCatchPokemon = 0;

    enviarInt(socketGameCard, 2);
    enviarPaquete(socketGameCard, tCatchPokemon, unCatchPokemon, tamanioCatchPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketGameCard,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pókemon mandado a atrapar con éxito");

          }else if(tipoResultado == 2){

            log_info(logger,"No se pudo intentar atrapar al pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Game Card");

    }

    free(unCatchPokemon);

}

void enviarGetPokemonAGameCard(char* nombrePokemon){

    socketGameCard = cliente(unGameBoyConfig->ipGameCard, unGameBoyConfig->puertoGameCard, ID_GAMECARD);

    t_getPokemon* unGetPokemon = malloc(sizeof(t_getPokemon));

    unGetPokemon->identificador = 0;
	unGetPokemon->identificadorCorrelacional = 0;

    unGetPokemon->nombrePokemon = string_new();
	string_append(&unGetPokemon->nombrePokemon,nombrePokemon);

    int tamanioGetPokemon = 0;

    enviarInt(socketGameCard, 2);
    enviarPaquete(socketGameCard, tGetPokemon, unGetPokemon, tamanioGetPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketGameCard,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pedido de locación de pokemón realizado con éxito");

          }else if(tipoResultado == 2){

            log_info(logger,"No se pudo realizar el pedido de locación del pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Game Card");

    }

    free(unGetPokemon);

}

void enviarSuscriptorABroker(char* colaDeMensajes,char* tiempoDeSuscripcion){

    socketBroker = cliente(unGameBoyConfig->ipBroker, unGameBoyConfig->puertoBroker, ID_BROKER);

    t_suscriptor* unSuscriptor = malloc(sizeof(t_suscriptor));

    unSuscriptor->identificador = 0;
	unSuscriptor->identificadorCorrelacional = 0;

    if(strcmp(colaDeMensajes, APPEARED_POKEMON) == 0){

        unSuscriptor->colaDeMensajes = tAppearedPokemon;

    }else if(strcmp(colaDeMensajes, NEW_POKEMON) == 0){

        unSuscriptor->colaDeMensajes = tNewPokemon;

    }else if(strcmp(colaDeMensajes, GET_POKEMON) == 0){

        unSuscriptor->colaDeMensajes = tGetPokemon;

    }else if(strcmp(colaDeMensajes, CATCH_POKEMON) == 0){

        unSuscriptor->colaDeMensajes = tCatchPokemon;

     }else if(strcmp(colaDeMensajes, CAUGHT_POKEMON) == 0){

        unSuscriptor->colaDeMensajes = tCaughtPokemon;

    }else if(strcmp(colaDeMensajes, LOCALIZED_POKEMON) == 0){

        unSuscriptor->colaDeMensajes = tLocalizedPokemon;

    }

    unSuscriptor->tiempoDeSuscripcion = atoi(tiempoDeSuscripcion);
    unSuscriptor->puerto = unGameBoyConfig->puertoGameBoy;

    unSuscriptor->ip = string_new();
    string_append(&unSuscriptor->ip,unGameBoyConfig->ipGameBoy); 

    infoServidor_t* unaInfoServidorGameBoy;

    unaInfoServidorGameBoy = malloc(sizeof(infoServidor_t));

    unaInfoServidorGameBoy->puerto = unGameBoyConfig->puertoGameBoy;
    unaInfoServidorGameBoy->ip = string_new();
    //string_append(&unaInfoServidorGameBoy->ip,unGameBoyConfig->ipGameBoy); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    string_append(&unaInfoServidorGameBoy->ip,"0");

    pthread_create(&hiloServidorGameBoy,NULL,(void*)servidor_inicializar,(void*)unaInfoServidorGameBoy);
    


    int tamanioSuscriptor = 0;

    enviarInt(socketBroker, 2);
    enviarPaquete(socketBroker, tSuscriptor, unSuscriptor, tamanioSuscriptor);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pedido de suscripción realizado con éxito");

          }else if(tipoResultado == 0){

            log_info(logger,"No se pudo realizar el pedido de suscripción");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker");

    }
    
    pthread_join(hiloServidorGameBoy, NULL);
    free(unSuscriptor);

}

void manejarRespuestaABroker(int socketCliente,int idCliente){

    int* tipoMensaje = malloc(sizeof(int));
	int* tamanioMensaje = malloc(sizeof(int));

    int* cantidadListaDatosPokemon = malloc(sizeof(int));
	int* contador = malloc(sizeof(int));
    datosPokemon* nodoDatosPokemon;

	void* buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch(*tipoMensaje){

        case tNewPokemon: {
            
            t_newPokemon* unNewPokemon = (t_newPokemon*) buffer;

            log_info(logger,"Se recibió un 'NEW_POKEMON': \n");
            log_info(logger,"El ID del mensaje es: %d",unNewPokemon->identificador);
            log_info(logger,"El ID correlacional del mensaje es: %d.",unNewPokemon->identificadorCorrelacional);
            log_info(logger,"La posición del Pokémon en el mapa es: [%d,%d].", unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY);
            log_info(logger,"El nombre del Pokemón es: %s.",unNewPokemon->nombrePokemon);
            log_info(logger,"La cantidad del pokemón es: %d.", unNewPokemon->cantidadDePokemon);

            log_info(logger,"Se le avisará al Broker que se recibió correctamente el NEW_POKEMON");
            enviarInt(socketCliente, 2);
            
            break;

        }

        case tGetPokemon: {
 
            t_getPokemon* unGetPokemon = (t_getPokemon*) buffer;

            log_info(logger,"Se recibió un 'GET_POKEMON' del Broker: \n");
            log_info(logger,"El ID del pókemon es %d.", unGetPokemon->identificador);
            log_info(logger,"El ID correlacional del pókemon es %d.",unGetPokemon->identificadorCorrelacional);
            log_info(logger,"El nombre del Pokemón es: %s.",unGetPokemon->nombrePokemon);

            log_info(logger,"Se le avisará al Broker que se recibió correctamente el GET_POKEMON");
            enviarInt(socketCliente, 2);

            break;

        }

        case tCatchPokemon: {
            
            t_catchPokemon* unCatchPokemon = (t_catchPokemon*) buffer;

            log_info(logger,"Se recibió un 'CATCH_POKEMON' del Broker: \n");
            log_info(logger,"El ID del mensaje es: %d.",unCatchPokemon->identificador);
            log_info(logger,"El ID correlacional del mensaje es: %d.",unCatchPokemon->identificadorCorrelacional);
            log_info(logger,"El nombre del pokemón es: %s.",unCatchPokemon->nombrePokemon);
            log_info(logger,"La posicion del pokémon en el mapa es: [%d,%d].", unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY);

            log_info(logger,"Se le avisará al Broker que se recibió correctamente el CATCH_POKEMON");
            enviarInt(socketCliente, 2);

            break;

        }

        case tAppearedPokemon: {
            
            t_appearedPokemon* unAppearedPokemon = (t_appearedPokemon*) buffer;

            log_info(logger,"Se recibió un 'APPEARED_POKEMON' del Broker: \n");
            log_info(logger,"El ID del mensaje es: %d.",unAppearedPokemon->identificador);
            log_info(logger,"El ID correlacional del mensaje es: %d.",unAppearedPokemon->identificadorCorrelacional);
            log_info(logger,"El nombre del pokemón es: %s.",unAppearedPokemon->nombrePokemon);
            log_info(logger,"La posicion del pokémon en el mapa es: [%d,%d].", unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);

            log_info(logger,"Se le avisará al Broker que se recibió correctamente el APPEARED_POKEMON");
            enviarInt(socketCliente, 2);

            break;

        }

        case tCaughtPokemon: {
            
            t_caughtPokemon* unCaughtPokemon = (t_caughtPokemon*) buffer;

            log_info(logger,"Se recibió un 'CAUGHT_POKEMON' del Broker: \n");
            log_info(logger,"El ID del mensaje es: %d.",unCaughtPokemon->identificador);
            log_info(logger,"El ID correlacional del mensaje es: %d.",unCaughtPokemon->identificadorCorrelacional);
            
            log_info(logger,"Se le avisará al Broker que se recibió correctamente el CAUGHT_POKEMON");
            enviarInt(socketCliente, 2);

            break;

        }

        case tLocalizedPokemon: {
            
            t_localizedPokemon* unLocalizedPokemon = (t_localizedPokemon*) buffer;

            log_info(logger,"Se recibió un 'LOCALIZED_POKEMON' del Broker: \n");
            log_info(logger,"El ID del mensaje es: %d.",unLocalizedPokemon->identificador);
            log_info(logger,"El ID correlacional del mensaje es: %d.",unLocalizedPokemon->identificadorCorrelacional);
            log_info(logger,"El nombre del pokemón es: %s.",unLocalizedPokemon->nombrePokemon);


            *cantidadListaDatosPokemon = list_size(unLocalizedPokemon->listaDatosPokemon);
	        *contador = 0;

            while(*contador < *cantidadListaDatosPokemon){

		        nodoDatosPokemon = list_get(unLocalizedPokemon->listaDatosPokemon,contador);
		
		        printf("\nCantidad de pokemón en %d° ubicación: %d", *contador, nodoDatosPokemon->cantidad);
		        printf("\nUbicacion en 'x': %d", nodoDatosPokemon->posicionEnElMapaX);
		        printf("\nUbicacion en 'y': %d\n", nodoDatosPokemon->posicionEnElMapaY);

		        *contador+=1;

	        }

            log_info(logger,"Se le avisará al Broker que se recibió correctamente el LOCALIZED_POKEMON");
            enviarInt(socketCliente, 2);

            break;

        }

        default:{

            log_error(logger,"Recibimos algo del Broker que no sabemos manejar: %d",*tipoMensaje);
            abort();
            break;

        }

    }

    free(cantidadListaDatosPokemon);
	free(contador);
    free(tipoMensaje);
    free(tamanioMensaje);
	free(buffer);

    return;

}

void sacarBarraNFinal(int posicion, char **argv){

    char* elString = argv[posicion];

    int i = 0;

    while(elString[i] != '\0'){

        //printf("valor: %d",elString[i]);

        if(elString[i] == 13)
            elString[i] = '\0';
        i++;
    }

}