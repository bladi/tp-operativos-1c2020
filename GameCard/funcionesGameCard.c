#include "gameCard.h"

void configurarLoggerGameCard(){

    logger = log_create(unGameCardConfig->logFile, "GAME CARD", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");
}

void cargarConfiguracionGameCard(){

    unGameCardConfig = malloc(sizeof(gameCardConfig_t));

    unGameCardArchivoConfig = malloc(sizeof(t_config));
    unGameCardArchivoConfig = config_create(PATH_CONFIG_GAMECARD);

    if (unGameCardArchivoConfig == NULL)
    {

        printf("\n=============================================================================================\n");
        printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL GAME CARD");
    }
    else
    {

        printf("\n=======================================================================================\n");
        printf("\nCONFIGURACION DEL GAME CARD IMPORTADA CON EXITO");

        unGameCardConfig->tiempoReintentoConexion = config_get_int_value(unGameCardArchivoConfig, TIEMPO_DE_REINTENTO_CONEXION);
        unGameCardConfig->tiempoReintentoOperacion = config_get_int_value(unGameCardArchivoConfig, TIEMPO_DE_REINTENTO_OPERACION);
        unGameCardConfig->tiempoRetardoOperacion = config_get_int_value(unGameCardArchivoConfig, TIEMPO_RETARDO_OPERACION);
        unGameCardConfig->puntoMontajeTallGrass = config_get_string_value(unGameCardArchivoConfig, PUNTO_MONTAJE_TALLGRASS);
        unGameCardConfig->ipBroker = config_get_string_value(unGameCardArchivoConfig, IP_BROKER);
        unGameCardConfig->puertoBroker = config_get_int_value(unGameCardArchivoConfig, PUERTO_BROKER);
        unGameCardConfig->ipGameCard = config_get_string_value(unGameCardArchivoConfig, IP_GAMECARD);
        unGameCardConfig->puertoGameCard = config_get_int_value(unGameCardArchivoConfig, PUERTO_GAMECARD);
        unGameCardConfig->logFile = config_get_string_value(unGameCardArchivoConfig, LOG_FILE);

        printf("\n\n· Tiempo de Reintento de Conexion = %d\n", unGameCardConfig->tiempoReintentoConexion);
        printf("· Tiempo de Reintento de Operacion = %d\n", unGameCardConfig->tiempoReintentoOperacion);
        printf("· Tiempo de Retardo de Operacion = %d\n", unGameCardConfig->tiempoRetardoOperacion);
        printf("· Punto de Montaje de Tall Grass = %s\n", unGameCardConfig->puntoMontajeTallGrass);
        printf("· IP del Broker = %s\n", unGameCardConfig->ipBroker);
        printf("· Puerto del Broker = %d\n", unGameCardConfig->puertoBroker);
        printf("· IP del Game Card = %s\n", unGameCardConfig->ipGameCard);
        printf("· Puerto del Game Card = %d\n", unGameCardConfig->puertoGameCard);
        printf("· Ruta del Archivo Log del Game Card = %s\n\n", unGameCardConfig->logFile);

        char *pathMetadata = string_new();
        string_append_with_format(&pathMetadata, "%sMetadata/Metadata.bin", unGameCardConfig->puntoMontajeTallGrass);

        printf("· pathMetadata = %s\n\n", pathMetadata);

        configMetadata = config_create(pathMetadata);

        if (configMetadata == NULL)
        {
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

void crearCarpetaFiles(){

    char* pathFiles = string_new();
    string_append_with_format(&pathFiles, "%sFiles/", unGameCardConfig->puntoMontajeTallGrass);

    if (!mkdir(pathFiles, 0777))
    {
        log_info(logger,"CREADA CARPETA FILES.");
    }

    free(pathFiles);
}

void inicializarGameCard(){

    cargarConfiguracionGameCard();

    configurarLoggerGameCard();

    inicializarBitMap();

    crearBloquesFileSystem();

    crearCarpetaFiles();

    inicializarHilosYVariablesGameCard();
}

void finalizarGameCard(){

    free(unGameCardConfig);
    free(logger);
}

void administradorDeConexiones(void *infoAdmin){

    infoAdminConexiones_t *unaInfoAdmin = (infoAdminConexiones_t *)infoAdmin;

    int idCliente = 0;
    int resultado;

    while ((resultado = recibirInt(unaInfoAdmin->socketCliente, &idCliente)) > 0)
    {

        switch (idCliente)
        {

        case 1:
        {

            manejarRespuestaABroker(unaInfoAdmin->socketCliente, idCliente);
            break;
        }

        case 2:
        {

            manejarRespuestaAGameBoy(unaInfoAdmin->socketCliente, idCliente);
            break;
        }

        case -1:
        {

            log_info(logger, "RECIBIMOS UNA PRUEBA DE CONEXION");
            break;
        }

        default:
        {

            log_warning(logger, "RECIBIMOS UN IDENTIFICADOR DE PROCESO NO VALIDO");
            break;
        }
        }
    }

    if (resultado == 0)
    {

        log_warning(logger, "CLIENTE DESCONECTADO");
        fflush(stdout);
        close(unaInfoAdmin->socketCliente);
    }
    else if (resultado < 0)
    {

        log_warning(logger, "ERROR AL RECIBIR");
        close(unaInfoAdmin->socketCliente);
    }

    return;
    
}

void actualizarConfiguracionGameCard(){

    FILE *archivoConfigFp;

    while (1)
    {

        sleep(10);

        archivoConfigFp = fopen(PATH_CONFIG_GAMECARD, "rb");

        nuevoIdConfigGameCard = 0;

        while (!feof(archivoConfigFp) && !ferror(archivoConfigFp))
        {

            nuevoIdConfigGameCard ^= fgetc(archivoConfigFp);
        }

        fclose(archivoConfigFp);

        if (cantidadDeActualizacionesConfigGameCard == 0)
        {

            cantidadDeActualizacionesConfigGameCard += 1;
        }
        else
        {

            if (nuevoIdConfigGameCard != idConfigGameCard)
            {

                log_trace(logger, "ACTUALIZANDO ARCHIVO DE CONFIGURACIÓN");
                cargarConfiguracionGameCard();
                cantidadDeActualizacionesConfigGameCard += 1;
            }
        }

        idConfigGameCard = nuevoIdConfigGameCard;
    }
}

void inicializarHilosYVariablesGameCard(){
    
    int resultado;
    int tipoResultado = 0;
    int tamanioSuscriptor = 0;
    infoServidor_t *unaInfoServidorGameCard;

    cantidadDeActualizacionesConfigGameCard = 0;
    socketBroker = cliente(unGameCardConfig->ipBroker, unGameCardConfig->puertoBroker, ID_BROKER);

    pthread_mutex_lock(&mutexSocketBroker);
    brokerActivo = probarConexionSocket(socketBroker);
    pthread_mutex_unlock(&mutexSocketBroker);

    unaInfoServidorGameCard = malloc(sizeof(infoServidor_t));

    unaInfoServidorGameCard->puerto = unGameCardConfig->puertoGameCard;
    unaInfoServidorGameCard->ip = string_new();
    //string_append(&unaInfoServidorGameCard->ip,unGameCardConfig->ipGameCard); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    string_append(&unaInfoServidorGameCard->ip, "0");

    //pthread_create(&hiloActualizadorSocketBrocker, NULL, (void *)actualizarConexionConBroker, NULL);
    pthread_create(&hiloActualizadorConfigGameCard, NULL, (void *)actualizarConfiguracionGameCard, NULL);
    pthread_create(&hiloServidorGameCard, NULL, (void *)servidor_inicializar, (void *)unaInfoServidorGameCard);

    t_suscriptor *unSuscriptor = malloc(sizeof(t_suscriptor));

    unSuscriptor->identificador = 0;
    unSuscriptor->identificadorCorrelacional = 0;

    unSuscriptor->colaDeMensajes = tNewPokemon;

    unSuscriptor->tiempoDeSuscripcion = 0;
    unSuscriptor->puerto = unGameCardConfig->puertoGameCard;

    unSuscriptor->ip = string_new();
    string_append(&unSuscriptor->ip, unGameCardConfig->ipGameCard);

    pthread_mutex_lock(&mutexSocketBroker);

    brokerActivo = probarConexionSocket(socketBroker);

    if(brokerActivo > 0){

        enviarInt(socketBroker, 3);
        enviarPaquete(socketBroker, tSuscriptor, unSuscriptor, tamanioSuscriptor);

        if ((resultado = recibirInt(socketBroker, &tipoResultado)) > 0){

            if (tipoResultado == 1){

                log_info(logger, "SUSCRIPCIÓN REALIZADA CON ÉXITO");

            }else if (tipoResultado == 0){

                log_info(logger, "FALLÓ PEDIDO DE SUSCRIPCIÓN");

            }

        }else{
            

            log_error(logger, "ERROR AL RECIBIR EL RESULTADO DE LA OPERACION");

            brokerActivo = probarConexionSocket(socketBroker);

        }
    }else{

        log_error(logger, "ERROR AL RECIBIR EL RESULTADO DE LA OPERACION");

    }

    pthread_mutex_unlock(&mutexSocketBroker);

    free(unSuscriptor);
    
    pthread_join(hiloActualizadorConfigGameCard, NULL);

}

void manejarRespuestaAGameBoy(int socketCliente, int idCliente){

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje)
    {

    case tNewPokemon:
    {

        t_newPokemon *unNewPokemon = (t_newPokemon *)buffer;
        int resultado;
        int tipoResultado = 0;

        log_info(logger, "'NEW_POKEMON' RECIBIDO: \n");
        log_info(logger, "NOMBRE: %s.", unNewPokemon->nombrePokemon);
        log_info(logger, "POSICIÓN: [%d,%d].", unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY);
        log_info(logger, "CANTIDAD: %d.", unNewPokemon->cantidadDePokemon);
        log_info(logger, "ID MENSAJE: %d", unNewPokemon->identificador);
        log_info(logger, "ID CORRELACIONAL: %d.", unNewPokemon->identificadorCorrelacional);

        int resultadoOperacionPokemon;

        if (existePokemon(unNewPokemon->nombrePokemon))
        {

            log_trace(logger, "POKEMON YA EXISTENTE, MODIFICANDO METADATA.");

            while (leerEstadoPokemon(unNewPokemon->nombrePokemon))
            {

                log_warning(logger, "ACTUALMENTE NO SE PUEDE ACCEDER AL POKEMON. REINTENTO EN %d SEGUNDOS.", unGameCardConfig->tiempoReintentoOperacion);
                sleep(unGameCardConfig->tiempoReintentoOperacion);
            }

            cambiarEstadoPokemon(unNewPokemon->nombrePokemon, 1);
            resultadoOperacionPokemon = actualizarUbicacionPokemon(unNewPokemon->nombrePokemon, unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY, unNewPokemon->cantidadDePokemon);

        }
        else
        {

            log_trace(logger, "CREANDO POKEMON");
            resultadoOperacionPokemon = crearPokemon(unNewPokemon->nombrePokemon, unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY, unNewPokemon->cantidadDePokemon);
        }

        //log_trace(logger,"TARDO %d SEGS EN CERRAR EL ARCHIVO", unGameCardConfig->tiempoRetardoOperacion);
        
        sleep(unGameCardConfig->tiempoRetardoOperacion);
        cambiarEstadoPokemon(unNewPokemon->nombrePokemon, 0);
        
        enviarInt(socketCliente, resultadoOperacionPokemon);
        log_info(logger, "RESULTADO DE LA OPERACIÓN ENVIADA AL GAMEBOY.");

        //PREGUNTAR SI HAY QUE MANDAR IGUAL EL APPEARED POKEMON CUANDO EL POKEMON YA EXISTÍA EN UNA POSICIÓN. POR AHORA LO HACEMOS IGUAL.

        pthread_mutex_lock(&mutexSocketBroker);

        brokerActivo = probarConexionSocket(socketBroker);
    
        if(brokerActivo > 0){

            t_appearedPokemon *unAppearedPokemon = malloc(sizeof(t_appearedPokemon));

            unAppearedPokemon->identificador = unNewPokemon->identificador;              //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unAppearedPokemon->identificadorCorrelacional = unNewPokemon->identificador; //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unAppearedPokemon->nombrePokemon = string_new();
            string_append(&unAppearedPokemon->nombrePokemon, unNewPokemon->nombrePokemon);
            unAppearedPokemon->posicionEnElMapaX = unNewPokemon->posicionEnElMapaX;
            unAppearedPokemon->posicionEnElMapaY = unNewPokemon->posicionEnElMapaY;

            int tamanioAppearedPokemon = 0;

            enviarInt(socketBroker, 3);
            enviarPaquete(socketBroker, tAppearedPokemon, unAppearedPokemon, tamanioAppearedPokemon);
            
            if ((resultado = recibirInt(socketBroker, &tipoResultado)) > 0){

                if (tipoResultado == 1){

                    log_trace(logger, "APPEARED_POKEMON ENVIADO AL BROKER EXITOSAMENTE");

                } else if (tipoResultado == 0){

                    log_error(logger, "FALLÓ ENVÍO DE APPEARED_POKEMON AL BROKER");
                }
            }

            else{

                log_error(logger, "FALLÓ ENVÍO DE APPEARED_POKEMON AL BROKER");

                brokerActivo = probarConexionSocket(socketBroker);

            }

        }else{

            log_error(logger, "FALLÓ ENVÍO DE APPEARED_POKEMON AL BROKER");

        }

        pthread_mutex_unlock(&mutexSocketBroker);

        break;

    }

    case tGetPokemon:
    {

        t_getPokemon *unGetPokemon = (t_getPokemon *)buffer;
        int resultado;
        int tipoResultado = 0;

        log_info(logger, "'GET_POKEMON' RECIBIDO: \n");
        log_info(logger, "NOMBRE POKEMON: %s.", unGetPokemon->nombrePokemon);
        log_info(logger, "ID DEL POKEMON: %d", unGetPokemon->identificador);
        log_info(logger, "ID CORRELACIONAL: %d", unGetPokemon->identificadorCorrelacional);

        int resultadoOperacion = existePokemon(unGetPokemon->nombrePokemon);

        enviarInt(socketCliente, resultadoOperacion);
        log_info(logger, "RESULTADO DE LA OPERACIÓN ENVIADO AL GAMEBOY");

        if (resultadoOperacion)
        {

            log_trace(logger, "POKEMON ENCONTRADO EN TALL_GRASS");

            while (leerEstadoPokemon(unGetPokemon->nombrePokemon))
            {

                log_warning(logger, "ACTUALMENTE NO SE PUEDE ACCEDER AL POKEMON. REINTENTO EN %d SEGUNDOS.", unGameCardConfig->tiempoReintentoOperacion);
                sleep(unGameCardConfig->tiempoReintentoOperacion);
            }

            pthread_mutex_lock(&mutexSocketBroker);

            brokerActivo = probarConexionSocket(socketBroker);
    
            if(brokerActivo > 0){

                t_localizedPokemon *unLocalizedPokemon = malloc(sizeof(t_localizedPokemon));

                //Posible mutex
                cambiarEstadoPokemon(unGetPokemon->nombrePokemon, 1);
                unLocalizedPokemon->listaDatosPokemon = generarListaUbicaciones(unGetPokemon->nombrePokemon);
                //Fin posible mutex

                unLocalizedPokemon->identificador = unGetPokemon->identificador;              //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unLocalizedPokemon->identificadorCorrelacional = unGetPokemon->identificador; //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
                unLocalizedPokemon->nombrePokemon = string_new();
                string_append(&unLocalizedPokemon->nombrePokemon, unGetPokemon->nombrePokemon);

                int tamanioLocalizedPokemon = 0;

                sleep(unGameCardConfig->tiempoRetardoOperacion);
                cambiarEstadoPokemon(unGetPokemon->nombrePokemon, 0);

                enviarInt(socketBroker, 3);
                enviarPaquete(socketBroker, tLocalizedPokemon, unLocalizedPokemon, tamanioLocalizedPokemon);


                if ((resultado = recibirInt(socketBroker, &tipoResultado)) > 0)
                {

                    if (tipoResultado == 1)
                    {

                        log_trace(logger, "LOCALIZED_POKEMON ENVIADO AL BROKER EXITOSAMENTE");
                    }
                    else if (tipoResultado == 0)
                    {

                        log_error(logger, "FALLÓ ENVÍO DE LOCALIZED_POKEMON AL BROKER");
                    }

                }

                else{

                    log_error(logger, "FALLÓ ENVÍO DE LOCALIZED_POKEMON AL BROKER");

                    brokerActivo = probarConexionSocket(socketBroker);

                }

            }else{


                log_error(logger, "FALLÓ ENVÍO DE LOCALIZED_POKEMON AL BROKER");


            }

            pthread_mutex_unlock(&mutexSocketBroker);

        }else{

            log_error(logger, "POKEMON NO ENCONTRADO EN TALL_GRASS");
        
        }

        break;

    }

    case tCatchPokemon:
    {

        t_catchPokemon *unCatchPokemon = (t_catchPokemon *)buffer;
        int resultado;
        int tipoResultado = 0;

        log_info(logger, "'CATCH_POKEMON' RECIBIDO: \n");
        log_info(logger, "NOMBRE POKEMON: %s", unCatchPokemon->nombrePokemon);
        log_info(logger, "POSICION EN MAPA: [%d,%d]", unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY);
        log_info(logger, "ID MENSAJE: %d", unCatchPokemon->identificador);
        log_info(logger, "ID CORRELACIONAL: %d", unCatchPokemon->identificadorCorrelacional);

        int resultadoOperacion = existePokemon(unCatchPokemon->nombrePokemon);

        if (resultadoOperacion)
        {

            log_trace(logger, "POKEMON ENCONTRADO EN TALL_GRASS");

            while (leerEstadoPokemon(unCatchPokemon->nombrePokemon))
            {

                log_warning(logger, "ACTUALMENTE NO SE PUEDE ACCEDER AL POKEMON. REINTENTO EN %d SEGUNDOS.", unGameCardConfig->tiempoReintentoOperacion);
                sleep(unGameCardConfig->tiempoReintentoOperacion);
            }
            
            cambiarEstadoPokemon(unCatchPokemon->nombrePokemon, 1);

            t_caughtPokemon *unCaughtPokemon = malloc(sizeof(t_caughtPokemon));

            unCaughtPokemon->resultado = (uint32_t)actualizarUbicacionPokemon(unCatchPokemon->nombrePokemon, unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY, -1);

            unCaughtPokemon->identificador = 0;              //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unCaughtPokemon->identificadorCorrelacional = unCatchPokemon->identificador; //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            
            sleep(unGameCardConfig->tiempoRetardoOperacion);
            cambiarEstadoPokemon(unCatchPokemon->nombrePokemon, 0);

            enviarInt(socketCliente, unCaughtPokemon->resultado);
            log_info(logger, "RESULTADO DE LA OPERACIÓN ENVIADO AL GAMEBOY");


            pthread_mutex_lock(&mutexSocketBroker);

            brokerActivo = probarConexionSocket(socketBroker);

            if(brokerActivo > 0){

                int tamanioCaughtPokemon = 0;

                enviarInt(socketBroker, 3);
                enviarPaquete(socketBroker, tCaughtPokemon, unCaughtPokemon, tamanioCaughtPokemon);

                if ((resultado = recibirInt(socketBroker, &tipoResultado)) > 0)
                {

                    if (tipoResultado == 1)
                    {

                        log_trace(logger, "CAUGHT_POKEMON ENVIADO AL BROKER EXITOSAMENTE");
                    }
                    else if (tipoResultado == 0)
                    {

                        log_error(logger, "FALLÓ ENVÍO DE CAUGHT_POKEMON AL BROKER");
                    }
                }
                else
                {

                    log_error(logger, "FALLÓ ENVÍO DE CAUGHT_POKEMON AL BROKER");

                    brokerActivo = probarConexionSocket(socketBroker);

                }

            }else{

                log_error(logger, "FALLÓ ENVÍO DE CAUGHT_POKEMON AL BROKER");

            }

            pthread_mutex_unlock(&mutexSocketBroker);

        }else{

            log_error(logger, "POKEMON NO ENCONTRADO EN TALL_GRASS");

        }

        break;
    }

    default:{

        log_error(logger, "MENSAJE DEL GAMEBOY NO RECONOCIDO: %d", *tipoMensaje);
        abort();
        break;

    }

    }

    free(tipoMensaje);
    free(tamanioMensaje);
    free(buffer);

    return;

}

void manejarRespuestaABroker(int socketCliente, int idCliente){

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje)
    {

    case tNewPokemon:
    {

        t_newPokemon *unNewPokemon = (t_newPokemon *)buffer;
        int resultado;
        int tipoResultado = 0;

        log_info(logger, "'NEW_POKEMON' RECIBIDO: \n");
        log_info(logger, "NOMBRE POKEMON: %s", unNewPokemon->nombrePokemon);
        log_info(logger, "POSICION EN MAPA: [%d,%d]", unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY);
        log_info(logger, "CANTIDAD: %d", unNewPokemon->cantidadDePokemon);
        log_info(logger, "ID MENSAJE: %d", unNewPokemon->identificador);
        log_info(logger, "ID CORRELACIONAL: %d.", unNewPokemon->identificadorCorrelacional);

        int resultadoOperacionPokemon;
        //int existiaPokemon = 0;

        if (existePokemon(unNewPokemon->nombrePokemon))
        {

            log_trace(logger, "POKEMON YA EXISTENTE, MODIFICANDO METADATA");

            while (leerEstadoPokemon(unNewPokemon->nombrePokemon))
            {

                log_warning(logger, "ACTUALMENTE NO SE PUEDE ACCEDER AL POKEMON. REINTENTO EN %d SEGUNDOS.", unGameCardConfig->tiempoReintentoOperacion);
                sleep(unGameCardConfig->tiempoReintentoOperacion);
            }

            //existiaPokemon = 1;
            cambiarEstadoPokemon(unNewPokemon->nombrePokemon, 1);
            resultadoOperacionPokemon = actualizarUbicacionPokemon(unNewPokemon->nombrePokemon, unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY, unNewPokemon->cantidadDePokemon);
            
        }
        else
        {

            log_trace(logger, "CREANDO POKEMON");
            resultadoOperacionPokemon = crearPokemon(unNewPokemon->nombrePokemon, unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY, unNewPokemon->cantidadDePokemon);
        }

        sleep(unGameCardConfig->tiempoRetardoOperacion);
        cambiarEstadoPokemon(unNewPokemon->nombrePokemon, 0);
        log_warning(logger, "--------Cerre el archivo pokemon");
        enviarInt(socketCliente, 3);//ENVIAR ID PARA ACK DEL MSG
        log_warning(logger, "--------DESPUES DE INT pokemon");
        log_info(logger, "RESULTADO DE LA OPERACIÓN ENVIADO AL BROKER");

        //PREGUNTAR SI HAY QUE MANDAR IGUAL EL APPEARED POKEMON CUANDO EL POKEMON YA EXISTÍA EN UNA POSICIÓN. POR AHORA LO HACEMOS IGUAL.

        t_appearedPokemon *unAppearedPokemon = malloc(sizeof(t_appearedPokemon));

        unAppearedPokemon->identificador = 0;              //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
        unAppearedPokemon->identificadorCorrelacional = unNewPokemon->identificador; //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
        unAppearedPokemon->nombrePokemon = string_new();
        string_append(&unAppearedPokemon->nombrePokemon, unNewPokemon->nombrePokemon);
        unAppearedPokemon->posicionEnElMapaX = unNewPokemon->posicionEnElMapaX;
        unAppearedPokemon->posicionEnElMapaY = unNewPokemon->posicionEnElMapaY;

        int tamanioAppearedPokemon = 0;

        pthread_mutex_lock(&mutexSocketBroker);

        brokerActivo = probarConexionSocket(socketBroker);

        if(brokerActivo > 0){
      
            enviarInt(socketBroker, 3);
            enviarPaquete(socketBroker, tAppearedPokemon, unAppearedPokemon, tamanioAppearedPokemon); //CAMBIAMOS EL socketCliente POR socketBroker PARA APROVECHAR LA FUNCIONALIDAD DE SERVIDOR DE BROKER. CONSULTAR SI ESTA BIEN.

            if ((resultado = recibirInt(socketBroker, &tipoResultado)) > 0)
            {   

                log_warning(logger, "RECIBO RESULTADO BROKER"); 
                if (tipoResultado >= 1){

                    log_trace(logger, "APPEARED_POKEMON ENVIADO AL BROKER EXITOSAMENTE");
                }

                else if (tipoResultado == 0){

                    log_error(logger, "FALLÓ ENVÍO DE APPEARED_POKEMON AL BROKER");
                }
            }
            else{

                log_error(logger, "FALLÓ ENVÍO DE APPEARED_POKEMON AL BROKER");
                
                brokerActivo = probarConexionSocket(socketBroker);

            }

        }else{

            log_error(logger, "FALLÓ ENVÍO DE APPEARED_POKEMON AL BROKER");

        }

        pthread_mutex_unlock(&mutexSocketBroker);

        break;

    }

    case tGetPokemon:
    {

        t_getPokemon *unGetPokemon = (t_getPokemon *)buffer;
        int resultado;
        int tipoResultado = 0;

        log_info(logger, "'GET_POKEMON' RECIBIDO: \n");
        log_info(logger, "NOMBRE POKEMON: %s", unGetPokemon->nombrePokemon);
        log_info(logger, "ID POKEMON: %d", unGetPokemon->identificador);// REVISAR (ID MENSAJE?)
        log_info(logger, "ID CORRELACIONAL: %d", unGetPokemon->identificadorCorrelacional);

        int resultadoOperacion = existePokemon(unGetPokemon->nombrePokemon);

        enviarInt(socketCliente, 3);//FIJARSE SI ES CONVENIENTE 3 O resultadoOperacion 
        log_trace(logger, "RESULTADO DE LA OPERACION ENVIADO AL BROKER");

        if (resultadoOperacion){

            log_trace(logger, "POKEMON ENCONTRADO EN TALL_GRASS");

            while (leerEstadoPokemon(unGetPokemon->nombrePokemon))
            {

                log_warning(logger, "ACTUALMENTE NO SE PUEDE ACCEDER AL POKEMON. REINTENTO EN %d SEGUNDOS.", unGameCardConfig->tiempoReintentoOperacion);
                sleep(unGameCardConfig->tiempoReintentoOperacion);
            }

            t_localizedPokemon *unLocalizedPokemon = malloc(sizeof(t_localizedPokemon));

            cambiarEstadoPokemon(unGetPokemon->nombrePokemon,1);

            unLocalizedPokemon->listaDatosPokemon = generarListaUbicaciones(unGetPokemon->nombrePokemon);

            unLocalizedPokemon->identificador = unGetPokemon->identificador;              //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unLocalizedPokemon->identificadorCorrelacional = unGetPokemon->identificador; //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unLocalizedPokemon->nombrePokemon = string_new();
            string_append(&unLocalizedPokemon->nombrePokemon, unGetPokemon->nombrePokemon);

            int tamanioLocalizedPokemon = 0;

            sleep(unGameCardConfig->tiempoRetardoOperacion);
            cambiarEstadoPokemon(unGetPokemon->nombrePokemon,0);
            
            pthread_mutex_lock(&mutexSocketBroker);

            brokerActivo = probarConexionSocket(socketBroker);

            if(brokerActivo > 0){

                enviarInt(socketBroker, 3);
                enviarPaquete(socketBroker, tLocalizedPokemon, unLocalizedPokemon, tamanioLocalizedPokemon);
                
                
                if ((resultado = recibirInt(socketBroker, &tipoResultado)) > 0)
                {

                    if (tipoResultado >= 1)
                    {
                        log_info(logger, "LOCALIZED_POKEMON ENVIADO AL BROKER EXITOSAMENTE");
                    }
                    else if (tipoResultado == 0)
                    {

                        log_error(logger, "FALLÓ ENVÍO DE LOCALIZED_POKEMON AL BROKER");
                    }
                }
                else
                {

                    log_error(logger, "FALLÓ ENVÍO DE LOCALIZED_POKEMON AL BROKER");

                    brokerActivo = probarConexionSocket(socketBroker);

                }
            }else{

                log_error(logger, "FALLÓ ENVÍO DE LOCALIZED_POKEMON AL BROKER");

            }

            pthread_mutex_unlock(&mutexSocketBroker);

        }else{

            log_error(logger, "POKEMON NO ENCONTRADO EN TALL_GRASS");

        }

        break;

    }

    case tCatchPokemon:{

        t_catchPokemon *unCatchPokemon = (t_catchPokemon *)buffer;
        int resultado;
        int tipoResultado = 0;

        log_info(logger, "'CATCH_POKEMON' RECIBIDO: \n");
        log_info(logger, "NOMBRE POKEMON: %s", unCatchPokemon->nombrePokemon);
        log_info(logger, "POSICION EN MAPA: [%d,%d]", unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY);
        log_info(logger, "ID MENSAJE: %d", unCatchPokemon->identificador);
        log_info(logger, "ID CORRELACIONAL: %d", unCatchPokemon->identificadorCorrelacional);

        int resultadoOperacion = existePokemon(unCatchPokemon->nombrePokemon);

        if (resultadoOperacion){

            log_trace(logger, "POKEMON ENCONTRADO EN TALL_GRASS");

            while (leerEstadoPokemon(unCatchPokemon->nombrePokemon)){

                log_warning(logger, "ACTUALMENTE NO SE PUEDE ACCEDER AL POKEMON. REINTENTO EN %d SEGUNDOS.", unGameCardConfig->tiempoReintentoOperacion);
                sleep(unGameCardConfig->tiempoReintentoOperacion);
            }

            t_caughtPokemon *unCaughtPokemon = malloc(sizeof(t_caughtPokemon));

            cambiarEstadoPokemon(unCatchPokemon->nombrePokemon,1);
            unCaughtPokemon->resultado = (uint32_t)actualizarUbicacionPokemon(unCatchPokemon->nombrePokemon, unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY, -1);

            unCaughtPokemon->identificador = 0;              //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            unCaughtPokemon->identificadorCorrelacional = unCatchPokemon->identificador; //CHEQUEAR QUÉ HACER CON ESTO CUANDO VIENE DEL GAME BOY
            
            sleep(unGameCardConfig->tiempoRetardoOperacion);
            cambiarEstadoPokemon(unCatchPokemon->nombrePokemon,0);
            
            enviarInt(socketCliente,3);
            log_info(logger, "RESULTADO DE LA OPERACION ENVIADO AL BROKER");
            
            pthread_mutex_lock(&mutexSocketBroker);

            brokerActivo = probarConexionSocket(socketBroker);

            if(brokerActivo > 0){

                int tamanioCaughtPokemon = 0;

                enviarInt(socketBroker, 3);
                enviarPaquete(socketBroker, tCaughtPokemon, unCaughtPokemon, tamanioCaughtPokemon);


                if ((resultado = recibirInt(socketBroker, &tipoResultado)) > 0){

                    if (tipoResultado >= 1){

                        log_trace(logger, "CAUGHT_POKEMON ENVIADO AL BROKER EXITOSAMENTE");
                    }
                    
                    else if (tipoResultado == 0){

                        log_error(logger, "FALLÓ ENVÍO DE CAUGHT_POKEMON AL BROKER");
                    }
                }
                else
                {

                    log_error(logger, "FALLÓ ENVÍO DE CAUGHT_POKEMON AL BROKER");

                    brokerActivo = probarConexionSocket(socketBroker);

                }
            }else{

                    log_error(logger, "FALLÓ ENVÍO DE CAUGHT_POKEMON AL BROKER");

            }

            pthread_mutex_unlock(&mutexSocketBroker);

        }else{

            log_error(logger, "POKEMON NO ENCONTRADO EN TALL_GRASS");

        }

        break;

    }

    default:
    {

        log_error(logger, "MENSAJE DEL BROKER NO RECONOCIDO: %d", *tipoMensaje);
        abort();
        break;
    }
    }

    free(tipoMensaje);
    free(tamanioMensaje);
    free(buffer);

    return;
}

void actualizarConexionConBroker(){

    int resultado;
    int tipoResultado = 0;
    int tamanioSuscriptor = 0;

    while (1)
    {

        sleep(unGameCardConfig->tiempoReintentoConexion);

        //log_debug(logger, "Variable brokerActivo: %d", brokerActivo);

        pthread_mutex_lock(&mutexSocketBroker);

        brokerActivo = probarConexionSocket(socketBroker);

        //log_debug(logger, "Variable brokerActivo luego de testeo de conexión: %d", brokerActivo);

        if (brokerActivo <= 0){

            log_warning(logger, "BROKER DESCONECTADO. INTENTANDO RECONEXIÓN.");

            socketBroker = cliente(unGameCardConfig->ipBroker, unGameCardConfig->puertoBroker, ID_BROKER);

            if (socketBroker > 0){

                log_trace(logger, "BROKER RECONECTADO");

                t_suscriptor *unSuscriptor = malloc(sizeof(t_suscriptor));

                unSuscriptor->identificador = 0;
                unSuscriptor->identificadorCorrelacional = 0;

                unSuscriptor->colaDeMensajes = tNewPokemon;

                unSuscriptor->tiempoDeSuscripcion = 0;
                unSuscriptor->puerto = unGameCardConfig->puertoGameCard;

                unSuscriptor->ip = string_new();
                string_append(&unSuscriptor->ip, unGameCardConfig->ipGameCard);

                enviarInt(socketBroker, 3);
                enviarPaquete(socketBroker, tSuscriptor, unSuscriptor, tamanioSuscriptor);

                if ((resultado = recibirInt(socketBroker, &tipoResultado)) > 0){

                    if (tipoResultado == 1){

                        log_trace(logger, "SUSCRIPCIÓN REALIZADA CON ÉXITO");

                    }else if (tipoResultado == 0){

                        log_error(logger, "FALLÓ PEDIDO DE SUSCRIPCIÓN");

                    }

                }else{

                    log_error(logger, "ERROR AL RECIBIR EL RESULTADO DE LA OPERACION");

                    brokerActivo = probarConexionSocket(socketBroker);

                }

            }else{

                log_error(logger, "INTENTO DE RECONEXIÓN FALLIDO. REINTENTO EN %d SEGUNDOS.", unGameCardConfig->tiempoReintentoConexion);

            }
        
        }/*else{

            log_debug(logger, "El Broker continúa conectado");

        }
        */
        pthread_mutex_unlock(&mutexSocketBroker);

    }

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////BITMAP///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void inicializarBitMap(){

    char *pathBitmap = string_new();
    string_append_with_format(&pathBitmap, "%sMetadata/Bitmap.bin", unGameCardConfig->puntoMontajeTallGrass);

    if (validarArchivo(pathBitmap))
    {
        log_info(logger, "BITMAP EXISTENTE ENCONTRADO, SE CARGA EN MEMORIA");
        verificarBitmapPrevio(pathBitmap);
    }
    else
    {
        log_info(logger, "CREANDO BITMAP");
        crearArchivoBitmap(pathBitmap);
        verificarBitmapPrevio(pathBitmap);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void verificarBitmapPrevio(char *pathBitmap){

    int tamBitmap = unGameCardConfig->cantidadDeBloques / 8;

    if (unGameCardConfig->cantidadDeBloques % 8 != 0)
    {
        tamBitmap++;
    }

    int fd = open(pathBitmap, O_RDWR);
    ftruncate(fd, tamBitmap);

    struct stat mystat;

    if (fstat(fd, &mystat) < 0)
    {
        log_error(logger, "ERROR AL ESTABLECER FSTAT PARA BITMAP. SE CREARÁ UNO NUEVO.");
        close(fd);
        exit(-1);
    }
    else
    {
        char *bitmap = (char *)mmap(NULL, mystat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (bitmap == MAP_FAILED)
        {
            log_error(logger, "ERROR AL MAPEAR BITMAP A MEMORIA. SE CREARÁ UNO NUEVO.");
            close(fd);
            //crearArchivoBitmap(pathBitmap);
        }
        else
        {

            bitarray = bitarray_create_with_mode(bitmap, tamBitmap, LSB_FIRST);
            close(fd);
        }
    }

    //free(pathBitmap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void crearArchivoBitmap(char *pathBitmap){

    int tamBitarray = unGameCardConfig->cantidadDeBloques / 8;
    int bit = 0;

    if (unGameCardConfig->cantidadDeBloques % 8 != 0)
    {
        tamBitarray++;
    }

    char *data = malloc(tamBitarray);
    t_bitarray *bitarrayInicial = bitarray_create_with_mode(data, tamBitarray, MSB_FIRST);

    while (bit < unGameCardConfig->cantidadDeBloques)
    {
        bitarray_clean_bit(bitarrayInicial, bit);
        bit++;
    }

    FILE *bitmap;
    bitmap = fopen(pathBitmap, "wb+");

    //fseek(bitmap, 0, SEEK_SET);
    fwrite(bitarrayInicial->bitarray, 1, bitarrayInicial->size, bitmap);

    log_info(logger, "ARCHIVO BITMAP CREADO EN: %s", pathBitmap);

    bitarray_destroy(bitarrayInicial);
    fclose(bitmap);
    //free(pathBitmap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int validarArchivo(char *path){
    struct stat buffer;
    int status;
    status = stat(path, &buffer);
    if (status < 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int buscarBloqueLibreYOcupar(){
    for (int i = 0; i < unGameCardConfig->cantidadDeBloques; i++)
    {
        if (bitarray_test_bit(bitarray, i) == 0)
        {
            bitarray_set_bit(bitarray, i);
            return i;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int cantidadBloquesLibres(){

    int posicion = 0;
    int libres = 0;

    pthread_mutex_lock(&mutexBitmap);
    while (posicion < unGameCardConfig->cantidadDeBloques)
    {
        if (bitarray_test_bit(bitarray, posicion) == 0)
        {
            libres++;
        }
        posicion++;
    }
    pthread_mutex_unlock(&mutexBitmap);

    return libres;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mostrarEstadoBitmap(){
    int posicion = 0;
    bool a;

    pthread_mutex_lock(&mutexBitmap);
    while (posicion < unGameCardConfig->cantidadDeBloques)
    {
        if ((posicion % 20) == 0)
            printf("\n");
        a = bitarray_test_bit(bitarray, posicion);
        printf("%i", a);
        posicion++;
    }
    pthread_mutex_unlock(&mutexBitmap);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////MANEJO BLOQUES/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void crearBloquesFileSystem(){

    char *pathBloques = string_new();
    string_append_with_format(&pathBloques, "%sBlocks/", unGameCardConfig->puntoMontajeTallGrass);

    if (!mkdir(pathBloques, 0777))
    {
        int i = 0;

        while (i < unGameCardConfig->cantidadDeBloques)
        {
            FILE *bloque;
            char *pathBloquei = string_new();
            string_append_with_format(&pathBloquei, "%s%d.bin", pathBloques, i);
            bloque = fopen(pathBloquei, "wb+");
            fclose(bloque);
            free(pathBloquei);
            i++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////MANEJO BLOQUES/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int existePokemon(char *pokemon){

    struct stat infoArchivo;
    char *path = string_new();
    string_append_with_format(&path, "%sFiles/%s", unGameCardConfig->puntoMontajeTallGrass, pokemon);

    if (stat(path, &infoArchivo) == 0 && S_ISDIR(infoArchivo.st_mode))
    {
        return 1;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int crearPokemon(char *pokemon, uint32_t posicionX, uint32_t posicionY, uint32_t cantidad){

    char* pathPokemon = string_new();
    char* ubicacion = string_new();

    string_append_with_format(&pathPokemon, "%sFiles/%s", unGameCardConfig->puntoMontajeTallGrass, pokemon);

    mkdir(pathPokemon, 0777);
    free(pathPokemon);

    string_append_with_format(&ubicacion, "%d-%d=%d\n", posicionX, posicionY, cantidad);
    int tamanioPokemon = string_length(ubicacion);

    return actualizarPokemon(pokemon, ubicacion, tamanioPokemon);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int actualizarPokemon(char *pokemon, char *stringUbicaciones, int sizeUbicaciones){

    FILE *f;
    char *pathMetadata = string_new();

    string_append_with_format(&pathMetadata, "%sFiles/%s/Metadata.bin", unGameCardConfig->puntoMontajeTallGrass, pokemon);

    char* contenidoMetadata = leerArchivo(pathMetadata);

    log_info(logger,"ACTUALIZANDO ARCHIVO METADATA DEL POKEMON: %s", pokemon);

    f = fopen(pathMetadata, "w+");

    if (f == NULL){

        log_error(logger, "NO SE PUDO ACTUALIZAR EL ARCHIVO METADATA PARA EL POKEMON %s", pokemon);
        free(pathMetadata);
        free(contenidoMetadata);
        free(stringUbicaciones);
        return 0;
    
    }else{

        int cantBloquesAOcupar = cantBloquesParaSize(sizeUbicaciones);

        if (cantBloquesAOcupar > cantidadBloquesLibres()){

            log_error(logger, "NO HAY BLOQUES SUFICIENTES PARA ACTUALIZAR AL POKEMON %s", pokemon);
            fputs(contenidoMetadata,f);
            fseek(f, 0, SEEK_SET);
            fclose(f);
            free(pathMetadata);
            free(contenidoMetadata);

            return 0;
        
        } else {
        
        
            char* formatoDirectory = string_new();
            char* formatoSize = string_new();
            char* formatoBlocks = string_new();
            char* formatoOpen = string_new();
            int bloquesAEscribir[cantBloquesAOcupar];
            
            for (int j = 0; j < cantBloquesAOcupar; j++)
            {
                pthread_mutex_lock(&mutexBitmap);
                bloquesAEscribir[j] = buscarBloqueLibreYOcupar();
                pthread_mutex_unlock(&mutexBitmap);
                log_info(logger,"RESERVANDO BLOQUE %d PARA EL POKEMON %s", bloquesAEscribir[j], pokemon);
            }
            
            string_append(&formatoDirectory, "DIRECTORY=N");
            string_append_with_format(&formatoSize, "\nSIZE=%d\n", sizeUbicaciones);
            string_append(&formatoOpen, "\nOPEN=Y");
            formatoBlocks = generarStringBlocks(cantBloquesAOcupar, bloquesAEscribir);

            fputs(formatoDirectory, f);
            fputs(formatoSize, f);
            fputs(formatoBlocks, f);
            fputs(formatoOpen, f);
            fseek(f, 0, SEEK_SET);

            fclose(f);

            log_trace(logger, "ARCHIVO METADATA DEL POKEMON %s ACTUALIZADO CORRECTAMENTE", pokemon);

            escribirEnBloques(stringUbicaciones, bloquesAEscribir, cantBloquesAOcupar);

            free(pathMetadata);
            free(contenidoMetadata);
            free(formatoDirectory);
            free(formatoSize);
            free(formatoBlocks);
            free(formatoOpen);
            //free(stringUbicaciones);

            return 1;
        
        }

    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *leerUbicacionesPokemon(char *pokemon){

    char *pathMetadata = string_new();
    string_append_with_format(&pathMetadata, "%s/Files/%s/Metadata.bin", unGameCardConfig->puntoMontajeTallGrass, pokemon);

    t_config *configMetadata = config_create(pathMetadata);
    char **bloques = config_get_array_value(configMetadata, "BLOCKS");

    free(configMetadata);
    free(pathMetadata);

    int cantBloques;
    char *todasLasUbicaciones = string_new();

    for (cantBloques = 0; bloques[cantBloques] != NULL; cantBloques++)
    {

        char *pathBloque = string_new();
        char *ubicacion = string_new();
        size_t tamLinea = 0;

        string_append_with_format(&pathBloque, "%s/Blocks/%s.bin", unGameCardConfig->puntoMontajeTallGrass, bloques[cantBloques]);
        FILE *f;
        f = fopen(pathBloque, "r");

        while (getline(&ubicacion, &tamLinea, f) > 0)
        {
            string_append(&todasLasUbicaciones, ubicacion);
        }

        free(ubicacion);
        free(pathBloque);
        fclose(f);
    }

    free(bloques);

    return todasLasUbicaciones;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

t_list *generarListaUbicaciones(char *pokemon){

    t_list *listaUbicaciones = list_create();

    char *ubicacionesEnString = leerUbicacionesPokemon(pokemon);
    char **arregloUbicaciones = string_split(ubicacionesEnString, "\n");

    int cantUbicaciones;

    for (cantUbicaciones = 0; arregloUbicaciones[cantUbicaciones] != NULL; cantUbicaciones++)
    {
        uint32_t posX;
        uint32_t posY;
        uint32_t cant;

        char **igualdad = string_split(arregloUbicaciones[cantUbicaciones], "=");
        char **posiciones = string_split(igualdad[0], "-");

        posX = (uint32_t)atoi(posiciones[0]);
        posY = (uint32_t)atoi(posiciones[1]);
        cant = (uint32_t)atoi(igualdad[1]);

        datosPokemon_t *ubicacion = malloc(sizeof(datosPokemon_t));
        ubicacion->posicionEnElMapaX = posX;
        ubicacion->posicionEnElMapaY = posY;
        ubicacion->cantidad = cant;

        list_add(listaUbicaciones, ubicacion);

        free(igualdad);
        free(posiciones);
    }

    free(ubicacionesEnString);
    free(arregloUbicaciones);

    return listaUbicaciones;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int actualizarUbicacionPokemon(char *pokemon, uint32_t posX, uint32_t posY, int cant){


    t_list *listaUbicaciones = generarListaUbicaciones(pokemon);
    
    pthread_mutex_lock(&mutexPosicion);
    busquedaX = posX;
    busquedaY = posY;
    datosPokemon_t *ubicacionEncontrada = list_find(listaUbicaciones, (void *)mismaUbicacion);
    pthread_mutex_unlock(&mutexPosicion);

    if (ubicacionEncontrada == NULL)
    {
        
       // log_debug(logger, "No encontré la ubicación solicitada, la creo.");

        list_destroy_and_destroy_elements(listaUbicaciones, eliminarNodoDatosPokemon);

        if (cant > 0)
        {
            // añadir ubicacion a los bloques
            char *ubicacionesPokemon = leerUbicacionesPokemon(pokemon);
            char *nuevaUbicacion = string_new();
            string_append_with_format(&nuevaUbicacion, "%d-%d=%d\n", posX, posY, cant);
            string_append(&ubicacionesPokemon, nuevaUbicacion);
            int sizeUbicaciones = string_length(ubicacionesPokemon);
            liberarBloquesDelPokemon(pokemon);

            return actualizarPokemon(pokemon, ubicacionesPokemon, sizeUbicaciones);
        }
        else
        {
            log_error(logger, "NO SE ENCUENTRAN %s EN LA UBICACION %d-%d", pokemon, posX, posY);
            return 0;
        }
    }
    else
    {
        //actualizar ubicacion, reescribir bloques y actualizar metadata.
        if (ubicacionEncontrada->cantidad + cant < 0)
        {

            log_error(logger, "NO HAY SUFICIENTES POKEMON EN LA UBICACION SELECCIONADA");
            return 0;
        }
        else if (ubicacionEncontrada->cantidad + cant == 0)
        {
            list_remove_and_destroy_by_condition(listaUbicaciones, (void *)mismaUbicacion, eliminarNodoDatosPokemon);
        }
        else
        {
            log_debug(logger, "ACTUALIZO LA UBICACIÓN ENCONTRADA.");
            ubicacionEncontrada->cantidad = ubicacionEncontrada->cantidad + cant;
        }

       // log_debug(logger, "GENERÉ UNA NUEVA LISTA DE UBICACIONES, PROCEDO A ACTUALIZAR LOS BLOQUES.");
        char *nuevoStringUbicaciones = generarStringUbicacionesSegunLista(listaUbicaciones);
        int sizeUbicaciones = string_length(nuevoStringUbicaciones);
        liberarBloquesDelPokemon(pokemon);
        return actualizarPokemon(pokemon, nuevoStringUbicaciones, sizeUbicaciones);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *generarStringUbicacionesSegunLista(t_list *listaUbicaciones){

    //por ahi no tome la lista como una lista de datosPokemon_t de una, maybe castear
    int tamLista = list_size(listaUbicaciones);
    char *stringUbicaciones = string_new();

    for (int i = 0; i < tamLista; i++)
    {
        datosPokemon_t *nodo = list_get(listaUbicaciones, i);
        string_append_with_format(&stringUbicaciones, "%d-%d=%d\n", nodo->posicionEnElMapaX, nodo->posicionEnElMapaY, nodo->cantidad);
    }

    list_destroy_and_destroy_elements(listaUbicaciones,eliminarNodoDatosPokemon);

    return stringUbicaciones;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Devuelve 0 si ya existe el archivo ó si hubo algún error, 1 si salió bien
int crearDirectorio(char *path, char *nombreDirectorio){

    struct stat infoDirectorio;
    char *pathDirectorio = string_new();
    string_append_with_format(&pathDirectorio, "%s/%s", path, nombreDirectorio);

    if (stat(path, &infoDirectorio) == 0 && S_ISDIR(infoDirectorio.st_mode))
    {

        free(pathDirectorio);
        log_warning(logger, "YA EXISTE EL DIRECTORIO %s", pathDirectorio);
        return 0;
    }
    else
    {

        mkdir(pathDirectorio, 0777);

        FILE *f;

        char *pathMetadata = string_new();
        string_append_with_format(&pathMetadata, "%s/Metadata.bin", pathDirectorio);

        f = fopen(pathMetadata, "w");

        if (f == NULL)
        {

            free(pathMetadata);
            log_error(logger, "NO SE PUDO CREAR EL ARCHIVO METADATA PARA EL DIRECTORIO %s", nombreDirectorio);
            return 0;
        }
        else
        {
            fputs("DIRECTORY=Y\0", f);
            fseek(f, 0, SEEK_SET);

            fclose(f);
            free(pathMetadata);
            log_info(logger, "SE CREÓ EL DIRECTORIO %s EN %s", nombreDirectorio, path);
            return 1;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int cantBloquesParaSize(int size){
    int cantBloques = size / unGameCardConfig->tamanioBloques;
    int resto = size % unGameCardConfig->tamanioBloques;

    if (resto > 0)
    {
        cantBloques = cantBloques + 1;
    }

    return cantBloques;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *generarStringBlocks(int cantBloques, int bloquesAEscribir[]){

    char *stringBlocks = string_new();
    string_append(&stringBlocks, "BLOCKS=[");

    for (int j = 0; j < cantBloques; j++)
    {
        if (j == (cantBloques - 1))
        {
            string_append_with_format(&stringBlocks, "%d]", bloquesAEscribir[j]);
        }
        else
        {
            string_append_with_format(&stringBlocks, "%d,", bloquesAEscribir[j]);
        }
    }

    return stringBlocks;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int escribirEnBloques(char *ubicaciones, int arregloBloques[], int cantBloques){

    int totalEscrito = 0;

    for (int i = 0; i < cantBloques - 1; i++)
    {
        char *pathBloque = string_new();
        log_info(logger,"ESCRIBIENDO EN BLOQUE %d", arregloBloques[i]);
        string_append_with_format(&pathBloque, "%sBlocks/%d.bin", unGameCardConfig->puntoMontajeTallGrass, arregloBloques[i]);
        FILE *f;
        char *escrituraPorBloque = string_substring(ubicaciones, totalEscrito, unGameCardConfig->tamanioBloques);
        f = fopen(pathBloque, "wb+");
        fputs(escrituraPorBloque, f);
        fclose(f);
        totalEscrito += string_length(escrituraPorBloque);
        free(pathBloque);
    }

    int restante = string_length(ubicaciones) - totalEscrito;

    char *ultimaPorcionAEscribir = string_substring(ubicaciones, totalEscrito, restante);
    char *pathUltimoBloque = string_new();

    string_append_with_format(&pathUltimoBloque, "%sBlocks/%d.bin", unGameCardConfig->puntoMontajeTallGrass, arregloBloques[cantBloques - 1]);
    log_info(logger,"ESCRIBIENDO EN BLOQUE %d", arregloBloques[cantBloques - 1]);
    FILE *g;
    g = fopen(pathUltimoBloque, "wb+");
    fputs(ultimaPorcionAEscribir, g);
    fclose(g);

    free(pathUltimoBloque);
    free(ultimaPorcionAEscribir);
    free(ubicaciones);

    return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void liberarBloquesDelPokemon(char *pokemon){

    char *pathMetadata = string_new();
    t_config *metadata;

    string_append_with_format(&pathMetadata, "%sFiles/%s/Metadata.bin", unGameCardConfig->puntoMontajeTallGrass, pokemon);

    log_info(logger, "INGRESANDO A LA METADATA DE PATH %s", pathMetadata);
    metadata = config_create(pathMetadata);

    free(pathMetadata);

    if (metadata == NULL)
    {

        free(metadata);
        log_error(logger, "ERROR: NO SE PUDO LEER LA INFORMACION DEL ARCHIVO");
        exit(1);
    }
    else
    {

        char **bloques = config_get_array_value(metadata, "BLOCKS");

        for (int i = 0; bloques[i] != NULL; i++)
        {
            int bloqueALiberar = atoi(bloques[i]);
            pthread_mutex_lock(&mutexBitmap);
            bitarray_clean_bit(bitarray, bloqueALiberar);
            pthread_mutex_unlock(&mutexBitmap);
        }

        free(bloques);
        free(metadata);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//DEVUELVE 0 si OPEN=N, 1 si OPEN=Y, 2 si no existe
int leerEstadoPokemon(char *pokemon){

    if (existePokemon(pokemon))
    {

        char *pathMetadata = string_new();
        string_append_with_format(&pathMetadata, "%sFiles/%s/Metadata.bin", unGameCardConfig->puntoMontajeTallGrass, pokemon);

        t_config *metadata;
        log_info(logger, "INGRESANDO A LA METADATA DE PATH %s", pathMetadata);
        metadata = config_create(pathMetadata);

        free(pathMetadata);

        if (metadata == NULL){

            free(metadata);
            log_error(logger, "NO SE PUDO LEER LA INFORMACION DEL ARCHIVO");
            exit(1);
        }
        else{
            
            char *estadoPokemon = config_get_string_value(metadata, "OPEN");
            if (!strcmp(estadoPokemon, "N"))
            {
                free(metadata);
                free(estadoPokemon);
                return 0;
            }
            else
            {
                free(metadata);
                free(estadoPokemon);
                return 1;
            }
        }
    }
    else return 2;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lo del estado se puede hacer dentro de la misma funcion, pero me pareció que iba a ser mejor separarlo así se puede controlar mejor el tema del "semaforo"
int cambiarEstadoPokemon(char *pokemon, int estado){

    char *pathMetadata = string_new();
    string_append_with_format(&pathMetadata, "%s/Files/%s/Metadata.bin", unGameCardConfig->puntoMontajeTallGrass, pokemon);

    char *nuevoEstado = string_new();
    if (estado == 1)
    {
        string_append(&nuevoEstado, "OPEN=Y\0");
    }
    else
    {
        string_append(&nuevoEstado, "OPEN=N\0");
    }

    t_config *metaPokemon = config_create(pathMetadata);

    char *directory = config_get_string_value(metaPokemon, "DIRECTORY");
    //char* open      = config_get_string_value(metaPokemon,"OPEN");
    char *size = config_get_string_value(metaPokemon, "SIZE");
    char *blocks = config_get_string_value(metaPokemon, "BLOCKS");

    free(metaPokemon);

    char *nuevaMeta = string_new();
    string_append_with_format(&nuevaMeta, "DIRECTORY=%s\n%s\nSIZE=%s\nBLOCKS=%s\n", directory, nuevoEstado, size, blocks);

    FILE *f;
    f = fopen(pathMetadata, "w");
    fputs(nuevaMeta, f);
    fclose(f);

    free(pathMetadata);
    free(nuevaMeta);
    free(nuevoEstado);
    free(directory);
    free(size);
    free(blocks);

    return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void casoDePrueba(){
    //if(crearPokemon("AlvaritoGUEI", 15, 24, 51)){
    // crearPokemon("AlvaritoGUEI", 15, 24, 51);
    // printf("\nCREE AL POKEMON 1");
    // crearPokemon("Sceptile",15,20,2);
    // printf("\nCREE AL POKEMON 2");
    //crearDirectorio(unGameCardConfig->puntoMontajeTallGrass,"Prueba");
    //char* pathCarpeta2 = string_new();
    //string_append_with_format(pathCarpeta2,"%s/Prueba",unGameCardConfig->puntoMontajeTallGrass);

    /*
        int estado1 = leerEstadoPokemon("Sceptile");
        log_trace(logger, "ESTADO INICIAL DE Sceptile: %d", estado1);
        cambiarEstadoPokemon("Sceptile", !estado1);
        int estado2 = leerEstadoPokemon("Sceptile");
        log_trace(logger, "ESTADO NUEVO DE Sceptile: %d", estado2);
        cambiarEstadoPokemon("Sceptile", !estado2);
        cambiarEstadoPokemon("AlvaritoGUEI", 0);
        actualizarUbicacionPokemon("AlvaritoGUEI",16,450,25);
        actualizarUbicacionPokemon("AlvaritoGUEI",162,450,25);
        actualizarUbicacionPokemon("AlvaritoGUEI",163,450,25);
        actualizarUbicacionPokemon("AlvaritoGUEI",164,450,25);
        actualizarUbicacionPokemon("AlvaritoGUEI",165,450,25);
        actualizarUbicacionPokemon("Sceptile",987,1235,4);
        actualizarUbicacionPokemon("Sceptile",9873,1235,4);
        actualizarUbicacionPokemon("Sceptile",9874,1235,4);
        actualizarUbicacionPokemon("Sceptile",9867,1235,4);
        actualizarUbicacionPokemon("Sceptile",9877,1235,4);
        actualizarUbicacionPokemon("Sceptile",9999,1235,1);
        actualizarUbicacionPokemon("Sceptile",9999,1235,2);
        actualizarUbicacionPokemon("AlvaritoGUEI",165,450,-25);
        actualizarUbicacionPokemon("Sceptile",987,1235,4);
        printf("\nUbicacion y cantidad P1: \n");
        char* ubicacionesAlvarito = leerUbicacionesPokemon("AlvaritoGUEI");
        printf(ubicacionesAlvarito);
        printf("\nUbicacion y cantidad P2: \n");
        char* ubicacionesSceptile = leerUbicacionesPokemon("Sceptile");
        printf(ubicacionesSceptile);
        mostrarEstadoBitmap();
        liberarBloquesDelPokemon("AlvaritoGUEI");
        liberarBloquesDelPokemon("Sceptile");
        mostrarEstadoBitmap();
        */

    // } else{
    //     printf("\nERROR CREANDO AL POKEMON 1");
    // }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool mismaUbicacion(datosPokemon_t *ubicacion){
    if (ubicacion->posicionEnElMapaX == busquedaX && ubicacion->posicionEnElMapaY == busquedaY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* leerArchivo(char* path){

    FILE* f;
    char* contenido = string_new();
    char* linea = NULL;
    size_t len = 0;
    ssize_t tamLinea;

    f = fopen(path, "r");
    
    if (f == NULL) return NULL;

    while ((tamLinea = getline(&linea, &len, f)) != -1) {
        log_info(logger, "LEIDA LINEA DE %d BYTES", len);
        string_append(&contenido, linea);
    }
    
    printf("\nContenido del archivo leído: \n%s",contenido);

    fclose(f);
    free(linea);
    return contenido;
}
