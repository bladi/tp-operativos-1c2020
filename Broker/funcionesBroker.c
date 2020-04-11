#include "broker.h"

void configurarLoggerBroker(){

	logger = log_create(unBrokerConfig->logFile, "BROKER", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");
}

void cargarConfiguracionBroker(){

    unBrokerConfig = malloc(sizeof(brokerConfig_t));

    unBrokerArchivoConfig = malloc(sizeof(t_config));

	unBrokerArchivoConfig = config_create(PATH_CONFIG_BROKER);
	    
	if (unBrokerArchivoConfig == NULL) {

        printf("\n=============================================================================================\n");
	    printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL BROKER");

	}else{

        printf("\n=======================================================================================\n");
        printf("\nCONFIGURACION DEL BROKER IMPORTADA CON EXITO");

        unBrokerConfig->tamanioMemoria = config_get_int_value(unBrokerArchivoConfig, TAMANO_MEMORIA);
        unBrokerConfig->tamanioMinimoParticion = config_get_int_value(unBrokerArchivoConfig, TAMANO_MINIMO_PARTICION);
        unBrokerConfig->algoritmoMemoria = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_MEMORIA);
        unBrokerConfig->algoritmoReemplazo = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_REEMPLAZO);
        unBrokerConfig->algoritmoParticionLibre = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_PARTICION_LIBRE);
        unBrokerConfig->ipBroker = config_get_string_value(unBrokerArchivoConfig, IP_BROKER);
        unBrokerConfig->puertoBroker = config_get_int_value(unBrokerArchivoConfig, PUERTO_BROKER);
        unBrokerConfig->frecuenciaCompactacion = config_get_int_value(unBrokerArchivoConfig, FRECUENCIA_COMPACTACION);
        unBrokerConfig->logFile = config_get_string_value(unBrokerArchivoConfig, LOG_FILE);

        printf("\n\n· Tamanio de la Memoria = %d\n", unBrokerConfig->tamanioMemoria);
        printf("· Tamanio Minimo de Particion = %d\n", unBrokerConfig->tamanioMinimoParticion);
        printf("· Algoritmo de Memoria = %s\n", unBrokerConfig->algoritmoMemoria);
        printf("· Algoritmo de Reemplazo = %s\n", unBrokerConfig->algoritmoReemplazo);
        printf("· Algoritmo de Particion Libre = %s\n", unBrokerConfig->algoritmoParticionLibre);
        printf("· IP del Broker = %s\n", unBrokerConfig->ipBroker);
        printf("· Puerto del Broker = %d\n", unBrokerConfig->puertoBroker);
        printf("· Frecuencia de Compactacion = %d\n", unBrokerConfig->frecuenciaCompactacion);
        printf("· Ruta del Archivo Log del Broker = %s\n\n", unBrokerConfig->logFile);

        free(unBrokerArchivoConfig);

    }

}

void inicializarBroker(){

    cargarConfiguracionBroker();

    configurarLoggerBroker();

    t_localizedPokemon* unLocalizedPokemon = malloc(sizeof(t_localizedPokemon));
    int* tamanio = malloc(sizeof(int));

    datosPokemon* nodoUnoDatosPokemon = malloc(sizeof(datosPokemon));
    datosPokemon* nodoDosDatosPokemon = malloc(sizeof(datosPokemon));

    t_list* listaDatosPokemon = list_create();

    nodoUnoDatosPokemon->cantidad = 10;
    nodoUnoDatosPokemon->posicionEnElMapaX = 5;
    nodoUnoDatosPokemon->posicionEnElMapaY = 3;

    list_add(listaDatosPokemon,nodoUnoDatosPokemon);

    nodoDosDatosPokemon->cantidad = 15;
    nodoDosDatosPokemon->posicionEnElMapaX = 20;
    nodoDosDatosPokemon->posicionEnElMapaY = 89;

    list_add(listaDatosPokemon,nodoDosDatosPokemon);

    unLocalizedPokemon->identificador = 1;
    unLocalizedPokemon->identificadorCorrelacional = 2;
    unLocalizedPokemon->nombrePokemon = string_new();
    string_append(&unLocalizedPokemon->nombrePokemon,"Pikachu");
    unLocalizedPokemon->listaDatosPokemon = listaDatosPokemon;

    serializar(tLocalizedPokemon,unLocalizedPokemon,tamanio);

    inicializarHilosYVariablesBroker();

}

void finalizarBroker(){

    free(unBrokerConfig);
    free(logger);

}

void actualizarConfiguracionBroker(){

    FILE *archivoConfigFp;

	while(1){

		sleep(10);

		archivoConfigFp = fopen(PATH_CONFIG_BROKER,"rb");

		nuevoIdConfigBroker = 0;

		while (!feof(archivoConfigFp) && !ferror(archivoConfigFp)) {

   			nuevoIdConfigBroker ^= fgetc(archivoConfigFp);

		}

        fclose(archivoConfigFp);


        if(cantidadDeActualizacionesConfigBroker == 0){

        cantidadDeActualizacionesConfigBroker += 1;

        }else{

            if (nuevoIdConfigBroker != idConfigBroker) {

                log_info(logger,"El archivo de configuración del Broker cambió. Se procederá a actualizar.");
                cargarConfiguracionBroker();
                cantidadDeActualizacionesConfigBroker += 1;
                
            }

        }

        idConfigBroker = nuevoIdConfigBroker;

	}

}

void inicializarHilosYVariablesBroker(){

    cantidadDeActualizacionesConfigBroker = 0;

    infoServidor_t* unaInfoServidorBroker;

    unaInfoServidorBroker = malloc(sizeof(infoServidor_t));

    unaInfoServidorBroker->puerto = unBrokerConfig->puertoBroker;
    unaInfoServidorBroker->ip = string_new();
    //string_append(&unaInfoServidorBroker->ip,unBrokerConfig->ipBroker); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    string_append(&unaInfoServidorBroker->ip,"0");

    pthread_create(&hiloActualizadorConfigBroker, NULL, (void*)actualizarConfiguracionBroker, NULL);
    pthread_create(&hiloServidorBroker,NULL,(void*)servidor_inicializar,(void*)unaInfoServidorBroker);

    pthread_join(hiloActualizadorConfigBroker, NULL);

}

void administradorDeConexiones(void* infoAdmin){

    infoAdminConexiones_t* unaInfoAdmin = (infoAdminConexiones_t*) infoAdmin;

    int idCliente = 0;
    int resultado;

    while((resultado = recibirInt(unaInfoAdmin->socketCliente,&idCliente)) > 0){

        switch(idCliente){

            case 2: {

                manejarRespuestaAGameBoy(unaInfoAdmin->socketCliente,idCliente);
                break;

            }

            case 3: {

                manejarRespuestaAGameCard(unaInfoAdmin->socketCliente,idCliente);
                break;
            }

            case 4: {

                manejarRespuestaATeam(unaInfoAdmin->socketCliente,idCliente);
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

void manejarRespuestaAGameBoy(int socketCliente,int idCliente){

    int* tipoMensaje = malloc(sizeof(int));
	int* tamanioMensaje = malloc(sizeof(int));

	void* buffer = recibirPaquete(socket, tipoMensaje, tamanioMensaje);

    switch(*tipoMensaje){

        case tNewPokemon: {

            /*
            
            Casteo de estructura (ejemplo): 
            
            t_newPokemon* unNewPokemon = (t_newPokemon*) buffer;

            */

            /*
            
            Logueo de lo recibido (ejemplo):

            log_info(logger,"El nombre del Pokemón es: %s",unNewPokemon->nombre);
            log_info(logger,"La posicion del Pokémon es: %d %d", unNewPokemon->posicion[0], unNewPokemon->posicion[1]);
            
            */

           /*
        
           Funciones que se invocan luego de recibir un NEW_POKEMON (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           darAGameCardPosicionesDePokemon(unNewPokemon->nombrePokemon, unNewPokemon->cantidad,unNewPokemon->posicion, idNuevoMensaje);
           recibirRespuestaGameCard(idNuevoMensaje);
           avisarATeamPokemonAparecido(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */
            
            break;
        }

        case tGetPokemon: {

            /*
            
            Casteo de estructura (ejemplo): 
            
            t_getPokemon* unGetPokemon = (t_getPokemon*) buffer;

            */

            /*
            
            Logueo de lo recibido (ejemplo):

            log_info(logger,"El nombre del Pokemón es: %s",unGetPokemon->nombrePokemon);
            log_info(logger,"Las posiciones del Pokémon son: %d %d", unGetPokemon->posicion[0], unGetPokemon->posicion[1]);
            log_info(logger,"La cantidad que hay de ese Pokémon es: %d",unGetPokemon->cantidad);
            
            */

           /*

           Funciones que se invocan luego de recibir un GET_POKEMON (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           pedirAGameCardPosicionesDePokemon(unGetPokemon->nombrePokemon, idNuevoMensaje);
           recibirRespuestaGameCard(idNuevoMensaje);
           avisarATeamPokemonLocalizado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

            break;

        }

        case tCatchPokemon: {

            /*
            
            Casteo de estructura (ejemplo): 
            
            t_catchPokemon* unCatchPokemon = (t_catchPokemon*) buffer;

            */

            /*
            
            Logueo de lo recibido (ejemplo):

            log_info(logger,"El nombre del Pokemón es: %s",unCatchPokemon->nombrePokemon);
            log_info(logger,"La posicion del Pokémon era: %d %d", unCatchPokemon->posicion[0], unCatchPokemon->posicion[1]);
            log_info(logger,"El nombre del entrenador es: %s",unCatchPokemon->nombreEntrenador);
            
            */

            /*

            Funciones que se invocan luego de recibir un CATCH_POKEMON (ejemplo):

            int idNuevoMensaje = generarNuevoIdMensajeBroker();

            darAGameCardPokemonAtrapado(unCatchPokemon->nombrePokemon,unCatchPokemon->posicion, idNuevoMensaje);
            recibirRespuestaGameCard(idNuevoMensaje);
            avisarATeamPokemonAtrapado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

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

void manejarRespuestaAGameCard(int socketCliente,int idCliente){

    int* tipoMensaje = malloc(sizeof(int));
	int* tamanioMensaje = malloc(sizeof(int));

	void* buffer = recibirPaquete(socket, tipoMensaje, tamanioMensaje);

    switch(*tipoMensaje){

        case tCaughtPokemon: {

            /*
            
            Casteo de estructura (ejemplo): 
            
            t_caughtPokemon* unCaughtPokemon = (t_caughtPokemon*) buffer;

            */

            /*
            
            Logueo de lo recibido (ejemplo):

            log_info(logger,"El nombre del Pokemón es: %s",unCaughtPokemon->nombre);
            log_info(logger,"La posicion del Pokémon es: %d %d", unCaughtPokemon->posicion[0], unCaughtPokemon->posicion[1]);
            
            */

           /*
        
           Funciones que se invocan luego de recibir un CAUGHT_POKEMON (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           avisarATeamPokemonAtrapado(unCaughtPokemon->nombrePokemon,idNuevoMensaje);

            */
            
            break;
        }

        case tLocalizedPokemon: {

            /*
            
            Casteo de estructura (ejemplo): 
            
            t_localizedPokemon* unLocalizedPokemon = (t_localizedPokemon*) buffer;

            */

            /*
            
            Logueo de lo recibido (ejemplo):

            log_info(logger,"El nombre del Pokemón es: %s",unLocalizedPokemon->nombrePokemon);
            log_info(logger,"Las posiciones del Pokémon son: %d %d", unLocalizedPokemon->posicion[0], unLocalizedPokemon->posicion[1]);
            log_info(logger,"La cantidad que hay de ese Pokémon es: %d",unLocalizedPokemon->cantidad);
            
            */

           /*

           Funciones que se invocan luego de recibir un LOCALIZED_POKEMON (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           avisarATeamPokemonLocalizado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

            break;

        }

        case tAppearedPokemon: {

            /*
            
            Casteo de estructura (ejemplo): 
            
            t_appeardPokemon* unAppeardPokemon = (t_appeardPokemon*) buffer;

            */

            /*
            
            Logueo de lo recibido (ejemplo):

            log_info(logger,"El nombre del Pokemón es: %s",unAppeardPokemon->nombrePokemon);
            log_info(logger,"La posicion del Pokémon era: %d %d", unAppeardPokemon->posicion[0], unAppeardPokemon->posicion[1]);
            
            */

            /*

            Funciones que se invocan luego de recibir un APPEARED_POKEMON (ejemplo):

            int idNuevoMensaje = generarNuevoIdMensajeBroker();

            avisarATeamPokemonAparecido(unAppeardPokemon->nombrePokemon,idNuevoMensaje);

            */

            break;

        }

        default:{

            log_error(logger,"Recibimos algo del Game Card que no sabemos manejar: %d",*tipoMensaje);
            abort();
            break;

        }

    }

    free(tipoMensaje);
    free(tamanioMensaje);
	free(buffer);

    return;
}

void manejarRespuestaATeam(int socketCliente,int idCliente){

    int* tipoMensaje = malloc(sizeof(int));
	int* tamanioMensaje = malloc(sizeof(int));

	void* buffer = recibirPaquete(socket, tipoMensaje, tamanioMensaje);

    switch(*tipoMensaje){

        case tGetPokemon: {

            /*
            
            Casteo de estructura (ejemplo): 
            
            t_getPokemon* unGetPokemon = (t_getPokemon*) buffer;

            */

            /*
            
            Logueo de lo recibido (ejemplo):

            log_info(logger,"El nombre del Pokemón es: %s",unGetPokemon->nombrePokemon);
            log_info(logger,"Las posiciones del Pokémon son: %d %d", unGetPokemon->posicion[0], unGetPokemon->posicion[1]);
            log_info(logger,"La cantidad que hay de ese Pokémon es: %d",unGetPokemon->cantidad);
            
            */

           /*

           Funciones que se invocan luego de recibir un GET_POKEMON (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           pedirAGameCardPosicionesDePokemon(unGetPokemon->nombrePokemon, idNuevoMensaje);
           recibirRespuestaGameCard(idNuevoMensaje);
           avisarATeamPokemonLocalizado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

            break;

        }

        case tCatchPokemon: {

            /*
            
            Casteo de estructura (ejemplo): 
            
            t_catchPokemon* unCatchPokemon = (t_catchPokemon*) buffer;

            */

            /*
            
            Logueo de lo recibido (ejemplo):

            log_info(logger,"El nombre del Pokemón es: %s",unCatchPokemon->nombrePokemon);
            log_info(logger,"La posicion del Pokémon era: %d %d", unCatchPokemon->posicion[0], unCatchPokemon->posicion[1]);
            log_info(logger,"El nombre del entrenador es: %s",unCatchPokemon->nombreEntrenador);
            
            */

            /*

            Funciones que se invocan luego de recibir un CATCH_POKEMON (ejemplo):

            int idNuevoMensaje = generarNuevoIdMensajeBroker();

            darAGameCardPokemonAtrapado(unCatchPokemon->nombrePokemon,unCatchPokemon->posicion, idNuevoMensaje);
            recibirRespuestaGameCard(idNuevoMensaje);
            avisarATeamPokemonAtrapado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

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