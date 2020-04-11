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
        unGameCardConfig->puertoBroker = config_get_string_value(unGameCardArchivoConfig, PUERTO_BROKER);
        unGameCardConfig->ipGameCard = config_get_string_value(unGameCardArchivoConfig, IP_GAMECARD);
        unGameCardConfig->puertoGameCard = config_get_string_value(unGameCardArchivoConfig, PUERTO_GAMECARD);
        unGameCardConfig->logFile = config_get_string_value(unGameCardArchivoConfig, LOG_FILE);

        printf("\n\n· Tiempo de Reintento de Conexion = %d\n", unGameCardConfig->tiempoReintentoConexion);
        printf("· Tiempo de Reintento de Operacion = %d\n", unGameCardConfig->tiempoReintentoOperacion);
        printf("· Punto de Montaje de Tall Grass = %s\n", unGameCardConfig->puntoMontajeTallGrass);
        printf("· IP del Broker = %s\n", unGameCardConfig->ipBroker);
        printf("· Puerto del Broker = %s\n", unGameCardConfig->puertoBroker);
        printf("· IP del Game Card = %s\n", unGameCardConfig->ipGameCard);
        printf("· Puerto del Game Card = %s\n", unGameCardConfig->puertoGameCard);
        printf("· Ruta del Archivo Log del Game Card = %s\n\n", unGameCardConfig->logFile);

        free(unGameCardArchivoConfig);

    }

}

void inicializarGameCard(){

    cargarConfiguracionGameCard();

	configurarLoggerGameCard();

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