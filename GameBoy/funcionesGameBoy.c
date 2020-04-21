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
        unGameBoyConfig->puertoBroker = config_get_int_value(unGameBoyArchivoConfig, PUERTO_BROKER);
        unGameBoyConfig->puertoTeam = config_get_int_value(unGameBoyArchivoConfig, PUERTO_TEAM);
        unGameBoyConfig->puertoGameCard = config_get_int_value(unGameBoyArchivoConfig, PUERTO_GAMECARD);
        unGameBoyConfig->logFile = config_get_string_value(unGameBoyArchivoConfig, LOG_FILE);

        printf("\n\n· IP del Broker = %s\n", unGameBoyConfig->ipBroker);
        printf("· IP del Team = %s\n", unGameBoyConfig->ipTeam);
        printf("· IP del Game Card = %s\n", unGameBoyConfig->ipGameCard);
        printf("· Puerto del Broker = %d\n", unGameBoyConfig->puertoBroker);
        printf("· Puerto del Team = %d\n", unGameBoyConfig->puertoTeam);
        printf("· Puerto del Game Card = %d\n", unGameBoyConfig->puertoGameCard);
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

    enviarInt(socketBroker, 4);
    enviarPaquete(socketBroker, tNewPokemon, unNewPokemon, tamanioNewPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pókemon creado con éxito");

          }else if(tipoResultado == 2){

            log_info(logger,"No se pudo crear el pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker");

    }

    free(unNewPokemon);

}

void enviarAppearedPokemonABroker(char* nombrePokemon,char* posicionEnX,char* posicionEnY,char* id){

    socketBroker = cliente(unGameBoyConfig->ipBroker, unGameBoyConfig->puertoBroker, ID_BROKER);

    t_appearedPokemon* unAppearedPokemon = malloc(sizeof(t_appearedPokemon));

    unAppearedPokemon->identificador = atoi(id);
	unAppearedPokemon->identificadorCorrelacional = 0;

    unAppearedPokemon->nombrePokemon = string_new();
	string_append(&unAppearedPokemon->nombrePokemon,nombrePokemon);

	unAppearedPokemon->posicionEnElMapaX = atoi(posicionEnX);
	unAppearedPokemon->posicionEnElMapaY = atoi(posicionEnY);

    int tamanioAppearedPokemon = 0;

    enviarInt(socketBroker, 4);
    enviarPaquete(socketBroker, tAppearedPokemon, unAppearedPokemon, tamanioAppearedPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pókemon apareció con éxito");

          }else if(tipoResultado == 2){

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

    enviarInt(socketBroker, 4);
    enviarPaquete(socketBroker, tCatchPokemon, unCatchPokemon, tamanioCatchPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pókemon mandado a atrapar con éxito");

          }else if(tipoResultado == 2){

            log_info(logger,"No se pudo intentar atrapar al pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker");

    }

    free(unCatchPokemon);

}

void enviarCaughtPokemonABroker(char* nombrePokemon,char* resultado){

    socketBroker = cliente(unGameBoyConfig->ipBroker, unGameBoyConfig->puertoBroker, ID_BROKER);

    t_caughtPokemon* unCaughtPokemon = malloc(sizeof(t_caughtPokemon));

    unCaughtPokemon->identificador = 0;
	unCaughtPokemon->identificadorCorrelacional = 0;

    unCaughtPokemon->nombrePokemon = string_new();
	string_append(&unCaughtPokemon->nombrePokemon,nombrePokemon);

    if(strcmp(resultado, "OK") == 0){

        unCaughtPokemon->resultado = true;

    }else{

        unCaughtPokemon->resultado = false;

    }

    int tamanioCaughtPokemon = 0;

    enviarInt(socketBroker, 4);
    enviarPaquete(socketBroker, tCaughtPokemon, unCaughtPokemon, tamanioCaughtPokemon);

    int resultadoOperacion;
    int tipoResultado = 0;

    if((resultadoOperacion = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Aviso de resultado de pokemón atrapado realizado con éxito");

          }else if(tipoResultado == 2){

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

    enviarInt(socketBroker, 4);
    enviarPaquete(socketBroker, tGetPokemon, unGetPokemon, tamanioGetPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pedido de locación de pokemón realizado con éxito");

          }else if(tipoResultado == 2){

            log_info(logger,"No se pudo realizar el pedido de locación del pókemon");

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

    enviarInt(socketTeam, 4);
    enviarPaquete(socketTeam, tAppearedPokemon, unAppearedPokemon, tamanioAppearedPokemon);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketTeam,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pókemon apareció con éxito");

          }else if(tipoResultado == 2){

            log_info(logger,"No se pudo hacer aparecer el pókemon");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Team");

    }

    free(unAppearedPokemon);

}

void enviarNewPokemonAGameCard(char* nombrePokemon,char* posicionEnX,char* posicionEnY,char* cantidad){

    socketGameCard = cliente(unGameBoyConfig->ipGameCard, unGameBoyConfig->puertoGameCard, ID_GAMECARD);

    t_newPokemon* unNewPokemon = malloc(sizeof(t_newPokemon));

    unNewPokemon->identificador = 0;
	unNewPokemon->identificadorCorrelacional = 0;

    unNewPokemon->nombrePokemon = string_new();
	string_append(&unNewPokemon->nombrePokemon,nombrePokemon);

	unNewPokemon->posicionEnElMapaX = atoi(posicionEnX);
	unNewPokemon->posicionEnElMapaY = atoi(posicionEnY);
	unNewPokemon->cantidadDePokemon = atoi(cantidad);

    int tamanioNewPokemon = 0;

    enviarInt(socketGameCard, 4);
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

void enviarCatchPokemonAGameCard(char* nombrePokemon,char* posicionEnX,char* posicionEnY){

    socketGameCard = cliente(unGameBoyConfig->ipGameCard, unGameBoyConfig->puertoGameCard, ID_GAMECARD);

    t_catchPokemon* unCatchPokemon = malloc(sizeof(t_catchPokemon));

    unCatchPokemon->identificador = 0;
	unCatchPokemon->identificadorCorrelacional = 0;

    unCatchPokemon->nombrePokemon = string_new();
	string_append(&unCatchPokemon->nombrePokemon,nombrePokemon);

	unCatchPokemon->posicionEnElMapaX = atoi(posicionEnX);
	unCatchPokemon->posicionEnElMapaY = atoi(posicionEnY);

    int tamanioCatchPokemon = 0;

    enviarInt(socketGameCard, 4);
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

    enviarInt(socketGameCard, 4);
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

    unSuscriptor->colaDeMensajes = string_new();
	string_append(&unSuscriptor->colaDeMensajes,colaDeMensajes);

    unSuscriptor->tiempoDeSuscripcion = atoi(tiempoDeSuscripcion);

    int tamanioSuscriptor = 0;

    enviarInt(socketBroker, 4);
    enviarPaquete(socketBroker, tSuscriptor, unSuscriptor, tamanioSuscriptor);

    int resultado;
    int tipoResultado = 0;

    if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

          if(tipoResultado == 1){
            
            log_info(logger,"Pedido de suscripción realizado con éxito");

          }else if(tipoResultado == 2){

            log_info(logger,"No se pudo realizar el pedido de suscripción");

          }

    }else{

        log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker");

    }

    free(unSuscriptor);

}