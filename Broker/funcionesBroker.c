#include "broker.h"

void configurarLoggerBroker()
{

    logger = log_create(CONFIG_BROKER->logFile, "BROKER", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");
}

void cargarConfiguracionBroker()
{

    CONFIG_BROKER = malloc(sizeof(brokerConfig_t));

    t_config *unBrokerArchivoConfig = malloc(sizeof(t_config));

    unBrokerArchivoConfig = config_create(PATH_CONFIG_BROKER);

    if (unBrokerArchivoConfig == NULL)
    {

        printf("\n=============================================================================================\n");
        printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL BROKER");
    }
    else
    {

        printf("\n=======================================================================================\n");
        printf("\nCONFIGURACION DEL BROKER IMPORTADA CON EXITO");

        CONFIG_BROKER->tamanioMemoria = config_get_int_value(unBrokerArchivoConfig, TAMANO_MEMORIA);
        CONFIG_BROKER->tamanioMinimoParticion = config_get_int_value(unBrokerArchivoConfig, TAMANO_MINIMO_PARTICION);
        CONFIG_BROKER->algoritmoMemoria = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_MEMORIA);
        CONFIG_BROKER->algoritmoReemplazo = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_REEMPLAZO);
        CONFIG_BROKER->algoritmoParticionLibre = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_PARTICION_LIBRE);
        CONFIG_BROKER->ipBroker = config_get_string_value(unBrokerArchivoConfig, IP_BROKER);
        CONFIG_BROKER->puertoBroker = config_get_int_value(unBrokerArchivoConfig, PUERTO_BROKER);
        CONFIG_BROKER->frecuenciaCompactacion = config_get_int_value(unBrokerArchivoConfig, FRECUENCIA_COMPACTACION);
        CONFIG_BROKER->logFile = config_get_string_value(unBrokerArchivoConfig, LOG_FILE);

        printf("\n\n· Tamanio de la Memoria = %d\n", CONFIG_BROKER->tamanioMemoria);
        printf("· Tamanio Minimo de Particion = %d\n", CONFIG_BROKER->tamanioMinimoParticion);
        printf("· Algoritmo de Memoria = %s\n", CONFIG_BROKER->algoritmoMemoria);
        printf("· Algoritmo de Reemplazo = %s\n", CONFIG_BROKER->algoritmoReemplazo);
        printf("· Algoritmo de Particion Libre = %s\n", CONFIG_BROKER->algoritmoParticionLibre);
        printf("· IP del Broker = %s\n", CONFIG_BROKER->ipBroker);
        printf("· Puerto del Broker = %d\n", CONFIG_BROKER->puertoBroker);
        printf("· Frecuencia de Compactacion = %d\n", CONFIG_BROKER->frecuenciaCompactacion);
        printf("· Ruta del Archivo Log del Broker = %s\n\n", CONFIG_BROKER->logFile);

        free(unBrokerArchivoConfig);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

void inicializarBroker()
{

    cargarConfiguracionBroker();

    configurarLoggerBroker();

    inicializarHilosYVariablesBroker();
    inicializarMemoria();

    /*

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

    */

    inicializarHilosYVariablesBroker();
}

void finalizarBroker()
{

    free(CONFIG_BROKER);
    free(logger);
}

void actualizarConfiguracionBroker()
{

    FILE *archivoConfigFp;

    while (1)
    {

        sleep(10);

        archivoConfigFp = fopen(PATH_CONFIG_BROKER, "rb");

        nuevoIdConfigBroker = 0;

        while (!feof(archivoConfigFp) && !ferror(archivoConfigFp))
        {

            nuevoIdConfigBroker ^= fgetc(archivoConfigFp);
        }

        fclose(archivoConfigFp);

        if (cantidadDeActualizacionesConfigBroker == 0)
        {

            cantidadDeActualizacionesConfigBroker += 1;
        }
        else
        {

            if (nuevoIdConfigBroker != idConfigBroker)
            {

                log_info(logger, "El archivo de configuración del Broker cambió. Se procederá a actualizar.");
                cargarConfiguracionBroker();
                cantidadDeActualizacionesConfigBroker += 1;
            }
        }

        idConfigBroker = nuevoIdConfigBroker;
    }
}

void inicializarHilosYVariablesBroker()
{

    cantidadDeActualizacionesConfigBroker = 0;

    infoServidor_t *unaInfoServidorBroker;

    unaInfoServidorBroker = malloc(sizeof(infoServidor_t));

    unaInfoServidorBroker->puerto = CONFIG_BROKER->puertoBroker;
    unaInfoServidorBroker->ip = string_new();
    //string_append(&unaInfoServidorBroker->ip,CONFIG_BROKER->ipBroker); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    string_append(&unaInfoServidorBroker->ip, "0");

    pthread_create(&hiloActualizadorConfigBroker, NULL, (void *)actualizarConfiguracionBroker, NULL);
    pthread_create(&hiloServidorBroker, NULL, (void *)servidor_inicializar, (void *)unaInfoServidorBroker);

    pthread_create(&hiloNew, NULL, (void *)ejecutarColaNewPokemon, NULL);
    pthread_create(&hiloAppeared, NULL, (void *)ejecutarColaAppearedPokemon, NULL);
    pthread_create(&hiloCatch, NULL, (void *)ejecutarColaCatchPokemon, NULL);
    pthread_create(&hiloCaught, NULL, (void *)ejecutarColaCaughtPokemon, NULL);
    pthread_create(&hiloGet, NULL, (void *)ejecutarColaGetPokemon, NULL);
    pthread_create(&hiloLocalized, NULL, (void *)ejecutarColaLocalizedPokemon, NULL);

    pthread_join(hiloActualizadorConfigBroker, NULL);
}

void inicializarMemoria()
{

    if ((MEMORIA_PRINCIPAL = malloc(CONFIG_BROKER->tamanioMemoria)) == NULL)
    {
        log_error(logger, "- Error! no se pudo alocar MEMORIA_PRINCIPAL.");
        exit(EXIT_FAILURE);
    }

    NUM_SUSCRIPTOR = 0;
    ID_MENSAJE = 0;
    METADATA_MEMORIA = list_create();

    tParticion *unaParti = malloc(sizeof(tParticion));

    unaParti->posicion = MEMORIA_PRINCIPAL;
    unaParti->free = true;
    unaParti->tamanio = CONFIG_BROKER->tamanioMemoria;

    list_add(METADATA_MEMORIA, unaParti);

    NEW_POKEMON_LISTA = list_create();
    APPEARED_POKEMON_LISTA = list_create();
    CATCH_POKEMON_LISTA = list_create();
    CAUGHT_POKEMON_LISTA = list_create();
    GET_POKEMON_LISTA = list_create();
    LOCALIZED_POKEMON_LISTA = list_create();
    SUSCRIPTORES_LISTA = list_create();
}

uint32_t generarNuevoIdMensajeBroker()
{

    pthread_mutex_lock(&mutex_ID_MENSAJE);
    ID_MENSAJE++;
    pthread_mutex_unlock(&mutex_ID_MENSAJE);

    return ID_MENSAJE;
}

uint32_t generarNuevoIdSuscriptor()
{

    pthread_mutex_lock(&mutex_NUM_SUSCRIPTOR);
    NUM_SUSCRIPTOR++;
    pthread_mutex_unlock(&mutex_NUM_SUSCRIPTOR);

    return NUM_SUSCRIPTOR;
}

char *getDireccionMemoriaLibre(uint32_t tamanio)
{
    //??* falta comparar con config para buscar que algoritmo ejecutar
    return malloc(tamanio);
}

///////////////////////////////////////////////////////////////////////////////////////////

void administradorDeConexiones(void *infoAdmin)
{

    infoAdminConexiones_t *unaInfoAdmin = (infoAdminConexiones_t *)infoAdmin;

    int idCliente = 0;
    int resultado;

    while ((resultado = recibirInt(unaInfoAdmin->socketCliente, &idCliente)) > 0)
    {

        switch (idCliente)
        {

        case 2:
        {

            manejarRespuestaAGameBoy(unaInfoAdmin->socketCliente, idCliente);
            break;
        }

        case 3:
        {

            manejarRespuestaAGameCard(unaInfoAdmin->socketCliente, idCliente);
            break;
        }

        case 4:
        {

            manejarRespuestaATeam(unaInfoAdmin->socketCliente, idCliente);
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

void manejarRespuestaAGameBoy(int socketCliente, int idCliente)
{

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socket, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje)
    {

    case tSuscriptor:
    {

        t_suscriptor *nuevaSuscripcion = (t_suscriptor *)buffer;

        log_info(logger, "Se suscribe a cola: : %d", nuevaSuscripcion->colaDeMensajes);
        log_info(logger, "Tiempo de Suscripcion: %d", nuevaSuscripcion->tiempoDeSuscripcion);

        ingresarNuevoSuscriber(nuevaSuscripcion);

        break;
    }

    case tNewPokemon:
    {

        t_newPokemon *unNewPokemon = (t_newPokemon *)buffer;

        log_info(logger, "Recibi Pokemon\n El nombre del Pokemón es: %s", unNewPokemon->nombrePokemon);
        //log_info(logger, "La posicion del Pokémon es: %d %d", unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY);

        unNewPokemon->identificador = generarNuevoIdMensajeBroker();

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socket, unNewPokemon->identificador);

        uint32_t desplazamiento = 0;
        uint32_t tamanioNombrePokemon = string_length(unNewPokemon->nombrePokemon);
        uint32_t tamanio = 0;

        tamanio = tamanioNombrePokemon + 4 * sizeof(uint32_t);

        tMensaje *unMensaje = malloc(sizeof(tMensaje));

        unMensaje->acknowledgement = list_create();
        unMensaje->suscriptoresEnviados = list_create();
        unMensaje->idMensaje = unNewPokemon->identificador;
        unMensaje->idMensajeCorrelacional = -1;
        unMensaje->tipoMensaje = tNewPokemon; //NEW
        unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(tamanio);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, unNewPokemon->nombrePokemon, tamanioNombrePokemon);
        desplazamiento += tamanioNombrePokemon;

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unNewPokemon->posicionEnElMapaX, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unNewPokemon->posicionEnElMapaY, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unNewPokemon->cantidadDePokemon, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        //??* falta eliminar unNewPokemon

        list_add(MENSAJES_LISTA, unMensaje);

        break;
    }

    case tGetPokemon:
    {
        t_getPokemon *unGetPokemon = (t_getPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unGetPokemon->nombrePokemon);

        unGetPokemon->identificador = generarNuevoIdMensajeBroker();

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socket, unGetPokemon->identificador);

        uint32_t desplazamiento = 0;
        uint32_t tamanioNombrePokemon = string_length(unGetPokemon->nombrePokemon);
        uint32_t tamanio = 0;

        tamanio = tamanioNombrePokemon + sizeof(uint32_t);

        tMensaje *unMensaje = malloc(sizeof(tMensaje));

        unMensaje->acknowledgement = list_create();
        unMensaje->suscriptoresEnviados = list_create();
        unMensaje->idMensaje = unGetPokemon->identificador;
        unMensaje->idMensajeCorrelacional = unGetPokemon->identificadorCorrelacional;
        unMensaje->tipoMensaje = tGetPokemon; //GET
        unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(tamanio);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, unGetPokemon->nombrePokemon, tamanioNombrePokemon);
        desplazamiento += tamanioNombrePokemon;

        //??* falta eliminar unGetPokemon

        list_add(MENSAJES_LISTA, unMensaje);

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

           Funciones que se invocan luego de recibir un GET_POKEMON_LISTA (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           pedirAGameCardPosicionesDePokemon(unGetPokemon->nombrePokemon, idNuevoMensaje);
           recibirRespuestaGameCard(idNuevoMensaje);
           avisarATeamPokemonLocalizado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

        break;
    }

    case tCatchPokemon:
    {

        t_catchPokemon *unCatchPokemon = (t_catchPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unCatchPokemon->nombrePokemon);
        log_info(logger, "La posicion del Pokémon era: %d %d", unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY);
        log_info(logger, "El nombre del entrenador es: %s", unCatchPokemon->nombrePokemon);

        unCatchPokemon->identificador = generarNuevoIdMensajeBroker();

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socket, unCatchPokemon->identificador);

        uint32_t desplazamiento = 0;
        uint32_t tamanioNombrePokemon = string_length(unCatchPokemon->nombrePokemon);
        uint32_t tamanio = 0;

        tamanio = tamanioNombrePokemon + 3 * sizeof(uint32_t);

        tMensaje *unMensaje = malloc(sizeof(tMensaje));

        unMensaje->acknowledgement = list_create();
        unMensaje->suscriptoresEnviados = list_create();
        unMensaje->idMensaje = unCatchPokemon->identificador;
        unMensaje->idMensajeCorrelacional = unCatchPokemon->identificadorCorrelacional;
        unMensaje->tipoMensaje = tCatchPokemon; //CATCH
        unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(tamanio);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, unCatchPokemon->nombrePokemon, tamanioNombrePokemon);
        desplazamiento += tamanioNombrePokemon;

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unCatchPokemon->posicionEnElMapaX, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unCatchPokemon->posicionEnElMapaY, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        //??* falta eliminar unCatchPokemon

        list_add(MENSAJES_LISTA, unMensaje);

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

            Funciones que se invocan luego de recibir un CATCH_POKEMON_LISTA (ejemplo):

            int idNuevoMensaje = generarNuevoIdMensajeBroker();

            darAGameCardPokemonAtrapado(unCatchPokemon->nombrePokemon,unCatchPokemon->posicion, idNuevoMensaje);
            recibirRespuestaGameCard(idNuevoMensaje);
            avisarATeamPokemonAtrapado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

        break;
    }

    default:
    {

        log_error(logger, "Recibimos algo del Game Boy que no sabemos manejar: %d", *tipoMensaje);
        abort();
        break;
    }
    }

    free(tipoMensaje);
    free(tamanioMensaje);
    free(buffer);

    return;
}

void manejarRespuestaAGameCard(int socketCliente, int idCliente)
{

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socket, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje)
    {

    case tSuscriptor:
    {

        t_suscriptor *nuevaSuscripcion = (t_suscriptor *)buffer;

        log_info(logger, "Se suscribe a cola: : %d", nuevaSuscripcion->colaDeMensajes);
        log_info(logger, "Tiempo de Suscripcion: %d", nuevaSuscripcion->tiempoDeSuscripcion);

        ingresarNuevoSuscriber(nuevaSuscripcion);

        break;
    }

    case tCaughtPokemon:
    {

        t_caughtPokemon *unCaughtPokemon = (t_caughtPokemon *)buffer; //??* LA ESTRUCTURA ESTA MAL SOLO TIENE QUE SER ID + O/1 unCaughtPokemon->RESULTADO TIENE QUE SER UINT32

        log_info(logger, "El nombre del Pokemón es: %s", unCaughtPokemon->nombrePokemon);

        unCaughtPokemon->identificador = generarNuevoIdMensajeBroker();

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socket, unCaughtPokemon->identificador);

        uint32_t desplazamiento = 0;
        uint32_t tamanio = 0;

        tamanio = sizeof(uint32_t);

        tMensaje *unMensaje = malloc(sizeof(tMensaje));

        unMensaje->acknowledgement = list_create();
        unMensaje->suscriptoresEnviados = list_create();
        unMensaje->idMensaje = unCaughtPokemon->identificador;
        unMensaje->idMensajeCorrelacional = unCaughtPokemon->identificadorCorrelacional;
        unMensaje->tipoMensaje = tCaughtPokemon; //CAUGHT
        unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(tamanio);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unCaughtPokemon->resultado, sizeof(uint32_t)); //??* ACORDARSE QUE TIENE Q SER UINT32 Y NO BOOL
        desplazamiento += sizeof(uint32_t);

        //??* falta eliminar unCaughtPokemon

        list_add(MENSAJES_LISTA, unMensaje);

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
        
           Funciones que se invocan luego de recibir un CAUGHT_POKEMON_LISTA (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           avisarATeamPokemonAtrapado(unCaughtPokemon->nombrePokemon,idNuevoMensaje);

            */

        break;
    }

    case tLocalizedPokemon:
    {
        t_localizedPokemon *unLocalizedPokemon = (t_localizedPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unLocalizedPokemon->nombrePokemon);

        unLocalizedPokemon->identificador = generarNuevoIdMensajeBroker();

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socket, unLocalizedPokemon->identificador);

        uint32_t desplazamiento = 0;
        uint32_t tamanioNombrePokemon = string_length(unLocalizedPokemon->nombrePokemon);
        uint32_t tamanioLista = list_size(unLocalizedPokemon->listaDatosPokemon);
        uint32_t tamanio = sizeof(uint32_t) + tamanioNombrePokemon + sizeof(datosPokemon) * tamanioLista; //??*revisar si esta bien
        uint32_t contador = 0;
        tMensaje *unMensaje = malloc(sizeof(tMensaje));

        unMensaje->acknowledgement = list_create();
        unMensaje->suscriptoresEnviados = list_create();
        unMensaje->idMensaje = unLocalizedPokemon->identificador;
        unMensaje->idMensajeCorrelacional = unLocalizedPokemon->identificadorCorrelacional;
        unMensaje->tipoMensaje = tLocalizedPokemon; //LOCALIZED
        unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(tamanio);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, unLocalizedPokemon->nombrePokemon, tamanioNombrePokemon);
        desplazamiento += tamanioNombrePokemon;

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioLista, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        while (contador < tamanioLista)
        {

            datosPokemon *nodoDatosPokemon = list_get(unLocalizedPokemon->listaDatosPokemon, contador); //??* posible error de casteo revisar (datosPokemon*)

            memcpy(unMensaje->posicionEnMemoria + desplazamiento, &nodoDatosPokemon->cantidad, sizeof(uint32_t));
            desplazamiento += sizeof(uint32_t);

            memcpy(unMensaje->posicionEnMemoria + desplazamiento, &nodoDatosPokemon->posicionEnElMapaX, sizeof(uint32_t));
            desplazamiento += sizeof(uint32_t);

            memcpy(unMensaje->posicionEnMemoria + desplazamiento, &nodoDatosPokemon->posicionEnElMapaY, sizeof(uint32_t));
            desplazamiento += sizeof(uint32_t);

            contador += 1;
        }

        //??* falta eliminar unLocalizedPokemon

        list_add(MENSAJES_LISTA, unMensaje);

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

           Funciones que se invocan luego de recibir un LOCALIZED_POKEMON_LISTA (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           avisarATeamPokemonLocalizado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

        break;
    }

    case tAppearedPokemon:
    {
        t_appearedPokemon *unAppeardPokemon = (t_appearedPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unAppeardPokemon->nombrePokemon);
        log_info(logger, "La posicion del Pokémon era: %d %d", unAppeardPokemon->posicionEnElMapaX, unAppeardPokemon->posicionEnElMapaY);

        unAppeardPokemon->identificador = generarNuevoIdMensajeBroker();

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socket, unAppeardPokemon->identificador);

        uint32_t desplazamiento = 0;
        uint32_t tamanioNombrePokemon = string_length(unAppeardPokemon->nombrePokemon);
        uint32_t tamanio = 0;

        tamanio = tamanioNombrePokemon + 3 * sizeof(uint32_t);

        tMensaje *unMensaje = malloc(sizeof(tMensaje));

        unMensaje->acknowledgement = list_create();
        unMensaje->suscriptoresEnviados = list_create();
        unMensaje->idMensaje = unAppeardPokemon->identificador;
        unMensaje->idMensajeCorrelacional = unAppeardPokemon->identificadorCorrelacional;
        unMensaje->tipoMensaje = tAppearedPokemon; //APPEARED
        unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(tamanio);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, unAppeardPokemon->nombrePokemon, tamanioNombrePokemon);
        desplazamiento += tamanioNombrePokemon;

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unAppeardPokemon->posicionEnElMapaX, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unAppeardPokemon->posicionEnElMapaY, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        //??* falta eliminar unAppeardPokemon

        list_add(MENSAJES_LISTA, unMensaje);

        /*
            
            Logueo de lo recibido (ejemplo):

           
            
            */

        /*

            Funciones que se invocan luego de recibir un APPEARED_POKEMON_LISTA (ejemplo):

            int idNuevoMensaje = generarNuevoIdMensajeBroker();

            avisarATeamPokemonAparecido(unAppeardPokemon->nombrePokemon,idNuevoMensaje);

            */

        break;
    }

    default:
    {

        log_error(logger, "Recibimos algo del Game Card que no sabemos manejar: %d", *tipoMensaje);
        abort();
        break;
    }
    }

    free(tipoMensaje);
    free(tamanioMensaje);
    free(buffer);

    return;
}

void manejarRespuestaATeam(int socketCliente, int idCliente)
{

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socket, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje)
    {

    case tSuscriptor:
    {

        t_suscriptor *nuevaSuscripcion = (t_suscriptor *)buffer;

        log_info(logger, "Se suscribe a cola: : %d", nuevaSuscripcion->colaDeMensajes);
        log_info(logger, "Tiempo de Suscripcion: %d", nuevaSuscripcion->tiempoDeSuscripcion);

        ingresarNuevoSuscriber(nuevaSuscripcion);

        break;
    }

    case tGetPokemon:
    {
        t_getPokemon *unGetPokemon = (t_getPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unGetPokemon->nombrePokemon);

        unGetPokemon->identificador = generarNuevoIdMensajeBroker();

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socket, unGetPokemon->identificador);

        uint32_t desplazamiento = 0;
        uint32_t tamanioNombrePokemon = string_length(unGetPokemon->nombrePokemon);
        uint32_t tamanio = 0;

        tamanio = tamanioNombrePokemon + sizeof(uint32_t);

        tMensaje *unMensaje = malloc(sizeof(tMensaje));

        unMensaje->acknowledgement = list_create();
        unMensaje->suscriptoresEnviados = list_create();
        unMensaje->idMensaje = unGetPokemon->identificador;
        unMensaje->idMensajeCorrelacional = unGetPokemon->identificadorCorrelacional;
        unMensaje->tipoMensaje = tGetPokemon; //GET
        unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(tamanio);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, unGetPokemon->nombrePokemon, tamanioNombrePokemon);
        desplazamiento += tamanioNombrePokemon;

        //??* falta eliminar unGetPokemon

        list_add(MENSAJES_LISTA, unMensaje);

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

           Funciones que se invocan luego de recibir un GET_POKEMON_LISTA (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           pedirAGameCardPosicionesDePokemon(unGetPokemon->nombrePokemon, idNuevoMensaje);
           recibirRespuestaGameCard(idNuevoMensaje);
           avisarATeamPokemonLocalizado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

        break;
    }

    case tCatchPokemon:
    {

        t_catchPokemon *unCatchPokemon = (t_catchPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unCatchPokemon->nombrePokemon);
        log_info(logger, "La posicion del Pokémon era: %d %d", unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY);
        log_info(logger, "El nombre del entrenador es: %s", unCatchPokemon->nombrePokemon);

        unCatchPokemon->identificador = generarNuevoIdMensajeBroker();

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socket, unCatchPokemon->identificador);

        uint32_t desplazamiento = 0;
        uint32_t tamanioNombrePokemon = string_length(unCatchPokemon->nombrePokemon);
        uint32_t tamanio = 0;

        tamanio = tamanioNombrePokemon + 3 * sizeof(uint32_t);

        tMensaje *unMensaje = malloc(sizeof(tMensaje));

        unMensaje->acknowledgement = list_create();
        unMensaje->suscriptoresEnviados = list_create();
        unMensaje->idMensaje = unCatchPokemon->identificador;
        unMensaje->idMensajeCorrelacional = unCatchPokemon->identificadorCorrelacional;
        unMensaje->tipoMensaje = tCatchPokemon; //CATCH
        unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(tamanio);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, unCatchPokemon->nombrePokemon, tamanioNombrePokemon);
        desplazamiento += tamanioNombrePokemon;

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unCatchPokemon->posicionEnElMapaX, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unCatchPokemon->posicionEnElMapaY, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        //??* falta eliminar unCatchPokemon

        list_add(MENSAJES_LISTA, unMensaje);

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

            Funciones que se invocan luego de recibir un CATCH_POKEMON_LISTA (ejemplo):

            int idNuevoMensaje = generarNuevoIdMensajeBroker();

            darAGameCardPokemonAtrapado(unCatchPokemon->nombrePokemon,unCatchPokemon->posicion, idNuevoMensaje);
            recibirRespuestaGameCard(idNuevoMensaje);
            avisarATeamPokemonAtrapado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

        break;
    }

    default:
    {

        log_error(logger, "Recibimos algo del Game Boy que no sabemos manejar: %d", *tipoMensaje);
        abort();
        break;
    }
    }

    free(tipoMensaje);
    free(tamanioMensaje);
    free(buffer);

    return;
}

///////////////////////////////////////////////////////////////////////////////////////////

void ingresarNuevoSuscriber(void *unaNuevaSuscripcion)
{

    t_suscriptor *nuevaSuscripcion = (t_suscriptor *)unaNuevaSuscripcion;
    // LO BUSCO EN LISTA DE SUSCRIPTORES_LISTA

    pthread_mutex_lock(&mutex_ipServerABuscar);
    pthread_mutex_lock(&mutex_PuertoEschuchaABuscar);

    ipServerABuscar = string_new();
    string_append(&ipServerABuscar, nuevaSuscripcion->ip);
    PuertoEschuchaABuscar = nuevaSuscripcion->puerto;

    t_suscriptor *unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeNuevoSuscriber);

    pthread_mutex_unlock(&mutex_ipServerABuscar);
    pthread_mutex_unlock(&mutex_PuertoEschuchaABuscar);

    if (unSuscriptor != NULL)
    {
        // 1 NEW_POKEMON_LISTA 2 APPEARED_POKEMON_LISTA 3 CATCH_POKEMON_LISTA 4 CAUGHT_POKEMON_LISTA 5 GET_POKEMON_LISTA 6 LOCALIZED_POKEMON_LISTA

        switch (nuevaSuscripcion->colaDeMensajes)
        {

        case tNewPokemon:
        {
            // LO BUSCO EN LISTA DE NEW

            log_info(logger, "Nuevo Suscriber a New");

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(NEW_POKEMON_LISTA, &existeIdSuscriberEnCola);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;
                list_add(NEW_POKEMON_LISTA, unSuscriptorEnCola);
                // enviarMensajesAnteriores(1, unSuscriptorEnCola); //??* agregar en los otros
            }

            break;
        }

        case tAppearedPokemon:
        {
            // LO BUSCO EN LISTA DE APPEARED

            log_info(logger, "Nuevo Suscriber a Appeared");

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(APPEARED_POKEMON_LISTA, &existeIdSuscriberEnCola);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;
                list_add(APPEARED_POKEMON_LISTA, unSuscriptorEnCola);
            }

            break;
        }
        case tCatchPokemon:
        {
            // LO BUSCO EN LISTA DE CATCH

            log_info(logger, "Nuevo Suscriber a Catch");

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(CATCH_POKEMON_LISTA, &existeIdSuscriberEnCola);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;
                list_add(CATCH_POKEMON_LISTA, unSuscriptorEnCola);
            }

            break;
        }

        case tCaughtPokemon:
        {
            // LO BUSCO EN LISTA DE CAUGHT

            log_info(logger, "Nuevo Suscriber a Caught");

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(CAUGHT_POKEMON_LISTA, &existeIdSuscriberEnCola);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;
                list_add(CAUGHT_POKEMON_LISTA, unSuscriptorEnCola);
            }

            break;
        }

        case tGetPokemon:
        {
            // LO BUSCO EN LISTA DE GET

            log_info(logger, "Nuevo Suscriber a Get");

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(GET_POKEMON_LISTA, &existeIdSuscriberEnCola);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;
                list_add(GET_POKEMON_LISTA, unSuscriptorEnCola);
            }

            break;
        }
        case tLocalizedPokemon:
        {
            // LO BUSCO EN LISTA DE LOCALIZED_POKEMON_LISTA

            log_info(logger, "Nuevo Suscriber a Localized");

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(LOCALIZED_POKEMON_LISTA, &existeIdSuscriberEnCola);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;
                list_add(LOCALIZED_POKEMON_LISTA, unSuscriptorEnCola);
            }

            break;
        }
        default:
        {

            log_warning(logger, "SUSCRIPCION DE COLA NO VALIDA");
            break;
        }
        }
    }
    else
    {
        //AGREGO NUEVO t_suscriptor Y AUMENTO NUM_SUSCRIPTOR

        unSuscriptor = malloc(sizeof(t_suscriptor));
        unSuscriptor->identificador = generarNuevoIdSuscriptor();

        unSuscriptor->ip = string_new();
        string_append(&unSuscriptor->ip, nuevaSuscripcion->ip);
        unSuscriptor->puerto = nuevaSuscripcion->puerto;

        //ME HAGO CLIENTE DEL SUSCRIPTOR

        unSuscriptor->identificadorCorrelacional = cliente(unSuscriptor->ip, unSuscriptor->puerto, 1);

        list_add(SUSCRIPTORES_LISTA, unSuscriptor);

        log_info(logger, "Nuevo Suscriber a SUSCRIPTORES_LISTA");
        ingresarNuevoSuscriber(nuevaSuscripcion);
    }
}

/*

typedef struct t_suscriptor{

	uint32_t colaDeMensajes;
	uint32_t tiempoDeSuscripcion;
	char* ip;
	uint32_t puerto;

	
} __attribute__((packed)) t_suscriptor;

*/

/*

void enviarMensajesAnteriores(uint32_t nroCola, tSuscriptorEnCola *unSuscriptorEnCola) //??* sacar quedo obsoleto
{

    uint32_t tiempoActual = (uint32_t)time(NULL);

    if (unSuscriptorEnCola->timeToLive == 0 || unSuscriptorEnCola->timeToLive >= (tiempoActual - unSuscriptorEnCola->startTime))
    {

        t_list *mensajesAEnviar = list_create();

        pthread_mutex_lock(&mutex_tipoMensajeABuscar);

        tipoMensajeABuscar = nroCola;

        mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

        pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

        //??* enviar todos los mensaje ANteriores

        pthread_mutex_lock(&mutex_idSuscriptorABuscar);

        idSuscriptorABuscar = unSuscriptorEnCola->identificador;
        t_suscriptor *suscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

        pthread_mutex_unlock(&mutex_idSuscriptorABuscar);

        uint32_t tamLista = list_size(mensajesAEnviar);
        tMensaje *unMensaje;

        for (int i = 0; i < tamLista; i++)
        {
            unMensaje = list_get(mensajesAEnviar, i);

            switch (nroCola)
            {
            case 1:
            {
                enviarMensajeNewPokemon(unMensaje, suscriptor); //??* falta completar con resto de MENSAJES_LISTA
            }
            default:
            {

                log_error(logger, "ERROR EN enviarMensajesAnteriores() ");
                abort();
                break;
            }
            }
        }
    }
}
*/

void enviarMensajeNewPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion)
{
    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;

    uint32_t desplazamiento = 0;
    uint32_t tamanioNombrePokemon = 0;

    t_newPokemon *unNewPokemon = malloc(sizeof(t_newPokemon));

    unNewPokemon->identificador = unMensaje->idMensaje;
    unNewPokemon->identificadorCorrelacional = unMensaje->idMensajeCorrelacional;

    memcpy(&tamanioNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    char *bufferNombrePokemon = malloc(tamanioNombrePokemon + 1);
    memcpy(bufferNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, tamanioNombrePokemon);
    bufferNombrePokemon[tamanioNombrePokemon] = '\0';
    desplazamiento += tamanioNombrePokemon;

    unNewPokemon->nombrePokemon = string_new();
    string_append(&unNewPokemon->nombrePokemon, bufferNombrePokemon);

    memcpy(&unNewPokemon->posicionEnElMapaX, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&unNewPokemon->posicionEnElMapaY, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&unNewPokemon->cantidadDePokemon, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    free(bufferNombrePokemon);

    int tamanioNewPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 4);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tNewPokemon, unNewPokemon, tamanioNewPokemon);

    bool anyAck = false;

    pthread_mutex_lock(&mutex_ackABuscar);

    ackABuscar = unSuscriptor->identificador;

    anyAck = list_any_satisfy(unMensaje->suscriptoresEnviados, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

    pthread_mutex_unlock(&mutex_ackABuscar);

    if (!anyAck)
    {
        list_add(unMensaje->suscriptoresEnviados, unSuscriptor->identificador);
    }

    int resultado;
    int tipoResultado = 0;

    if ((resultado = recibirInt(unSuscriptor->identificadorCorrelacional, &tipoResultado)) > 0)
    {

        if (tipoResultado == 1)
        {

            log_info(logger, "enviarMensajeNewPokemon() ok realizando ack");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {

            log_info(logger, "ERROR enviarMensajeNewPokemon()"); //??* este caso no contemplar nunca ocurre creo, preguntar por las dudas
        }
    }
    else
    {

        log_error(logger, "ERROR enviarMensajeNewPokemon() no recibi nada server apagado "); //??* en este caso agregar en cola enviados y volvera a intentar en proxima ejecutarColaNewPokemon()
    }

    free(unNewPokemon);
}

void enviarMensajeAppearedPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;

    t_appearedPokemon *unAppearedPokemon = malloc(sizeof(t_appearedPokemon));

    uint32_t desplazamiento = 0;
    uint32_t tamanioNombrePokemon = 0;

    unAppearedPokemon->identificador = unMensaje->idMensaje;
    unAppearedPokemon->identificadorCorrelacional = unMensaje->idMensajeCorrelacional;

    memcpy(&tamanioNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    char *bufferNombrePokemon = malloc(tamanioNombrePokemon + 1);

    memcpy(bufferNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, tamanioNombrePokemon);
    bufferNombrePokemon[tamanioNombrePokemon] = '\0';
    desplazamiento += tamanioNombrePokemon;

    memcpy(&unAppearedPokemon->posicionEnElMapaX, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&unAppearedPokemon->posicionEnElMapaY, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    unAppearedPokemon->nombrePokemon = string_new();

    string_append(&unAppearedPokemon->nombrePokemon, bufferNombrePokemon);

    free(bufferNombrePokemon);

    int tamanioPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 4);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tAppearedPokemon, unAppearedPokemon, tamanioPokemon);

    bool anyAck = false;

    pthread_mutex_lock(&mutex_ackABuscar);

    ackABuscar = unSuscriptor->identificador;

    anyAck = list_any_satisfy(unMensaje->suscriptoresEnviados, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

    pthread_mutex_unlock(&mutex_ackABuscar);

    if (!anyAck)
    {
        list_add(unMensaje->suscriptoresEnviados, unSuscriptor->identificador);
    }

    int resultado;
    int tipoResultado = 0;

    if ((resultado = recibirInt(unSuscriptor->identificadorCorrelacional, &tipoResultado)) > 0)
    {

        if (tipoResultado == 1)
        {

            log_info(logger, "enviarMensajeNewPokemon() ok realizando ack");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {

            log_info(logger, "ERROR enviarMensajeNewPokemon()"); //??* este caso no contemplar nunca ocurre creo, preguntar por las dudas
        }
    }
    else
    {

        log_error(logger, "ERROR enviarMensajeNewPokemon() no recibi nada server apagado "); //??* en este caso agregar en cola enviados y volvera a intentar en proxima ejecutarColaNewPokemon()
    }

    free(unAppearedPokemon);
}

void enviarMensajeCatchPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;

    uint32_t desplazamiento = 0;
    uint32_t tamanioNombrePokemon = 0;

    t_catchPokemon *unCatchPokemon = malloc(sizeof(t_catchPokemon));

    unCatchPokemon->identificador = unMensaje->idMensaje;
    unCatchPokemon->identificadorCorrelacional = unMensaje->idMensajeCorrelacional;

    memcpy(&tamanioNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    char *bufferNombrePokemon = malloc(tamanioNombrePokemon + 1);
    memcpy(bufferNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, tamanioNombrePokemon);
    bufferNombrePokemon[tamanioNombrePokemon] = '\0';
    desplazamiento += tamanioNombrePokemon;

    memcpy(&unCatchPokemon->posicionEnElMapaX, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&unCatchPokemon->posicionEnElMapaY, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    unCatchPokemon->nombrePokemon = string_new();

    string_append(&unCatchPokemon->nombrePokemon, bufferNombrePokemon);

    free(bufferNombrePokemon);

    int tamanioPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 4);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tCatchPokemon, unCatchPokemon, tamanioPokemon);

    bool anyAck = false;

    pthread_mutex_lock(&mutex_ackABuscar);

    ackABuscar = unSuscriptor->identificador;

    anyAck = list_any_satisfy(unMensaje->suscriptoresEnviados, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

    pthread_mutex_unlock(&mutex_ackABuscar);

    if (!anyAck)
    {
        list_add(unMensaje->suscriptoresEnviados, unSuscriptor->identificador);
    }

    int resultado;
    int tipoResultado = 0;

    if ((resultado = recibirInt(unSuscriptor->identificadorCorrelacional, &tipoResultado)) > 0)
    {

        if (tipoResultado == 1)
        {

            log_info(logger, "enviarMensajeNewPokemon() ok realizando ack");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {

            log_info(logger, "ERROR enviarMensajeNewPokemon()"); //??* este caso no contemplar nunca ocurre creo, preguntar por las dudas
        }
    }
    else
    {

        log_error(logger, "ERROR enviarMensajeNewPokemon() no recibi nada server apagado "); //??* en este caso agregar en cola enviados y volvera a intentar en proxima ejecutarColaNewPokemon()
    }

    free(unCatchPokemon);
}

void enviarMensajeCaughtPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;

    t_caughtPokemon *unCaughtPokemon = malloc(sizeof(t_caughtPokemon));

    unCaughtPokemon->identificador = unMensaje->idMensaje;
    unCaughtPokemon->identificadorCorrelacional = unMensaje->idMensajeCorrelacional;

    memcpy(&unCaughtPokemon->resultado, unMensaje->posicionEnMemoria, sizeof(uint32_t));

    int tamanioPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 4);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tCaughtPokemon, unCaughtPokemon, tamanioPokemon);

    bool anyAck = false;

    pthread_mutex_lock(&mutex_ackABuscar);

    ackABuscar = unSuscriptor->identificador;

    anyAck = list_any_satisfy(unMensaje->suscriptoresEnviados, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

    pthread_mutex_unlock(&mutex_ackABuscar);

    if (!anyAck)
    {
        list_add(unMensaje->suscriptoresEnviados, unSuscriptor->identificador);
    }

    int resultado;
    int tipoResultado = 0;

    if ((resultado = recibirInt(unSuscriptor->identificadorCorrelacional, &tipoResultado)) > 0)
    {

        if (tipoResultado == 1)
        {

            log_info(logger, "enviarMensajeNewPokemon() ok realizando ack");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {

            log_info(logger, "ERROR enviarMensajeNewPokemon()"); //??* este caso no contemplar nunca ocurre creo, preguntar por las dudas
        }
    }
    else
    {

        log_error(logger, "ERROR enviarMensajeNewPokemon() no recibi nada server apagado "); //??* en este caso agregar en cola enviados y volvera a intentar en proxima ejecutarColaNewPokemon()
    }

    free(unCaughtPokemon);
}

void enviarMensajeGetPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;

    uint32_t desplazamiento = 0;
    uint32_t tamanioNombrePokemon = 0;

    t_getPokemon *unGetPokemon = malloc(sizeof(t_getPokemon));

    unGetPokemon->identificador = unMensaje->idMensaje;
    unGetPokemon->identificadorCorrelacional = unMensaje->idMensajeCorrelacional;

    memcpy(&tamanioNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    char *bufferNombrePokemon = malloc(tamanioNombrePokemon + 1);
    memcpy(bufferNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, tamanioNombrePokemon);
    bufferNombrePokemon[tamanioNombrePokemon] = '\0';
    desplazamiento += tamanioNombrePokemon;

    unGetPokemon->nombrePokemon = string_new();

    string_append(&unGetPokemon->nombrePokemon, bufferNombrePokemon);

    free(bufferNombrePokemon);

    int tamanioPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 4);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tGetPokemon, unGetPokemon, tamanioPokemon);

    bool anyAck = false;

    pthread_mutex_lock(&mutex_ackABuscar);

    ackABuscar = unSuscriptor->identificador;

    anyAck = list_any_satisfy(unMensaje->suscriptoresEnviados, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

    pthread_mutex_unlock(&mutex_ackABuscar);

    if (!anyAck)
    {
        list_add(unMensaje->suscriptoresEnviados, unSuscriptor->identificador);
    }

    int resultado;
    int tipoResultado = 0;

    if ((resultado = recibirInt(unSuscriptor->identificadorCorrelacional, &tipoResultado)) > 0)
    {

        if (tipoResultado == 1)
        {

            log_info(logger, "enviarMensajeNewPokemon() ok realizando ack");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {

            log_info(logger, "ERROR enviarMensajeNewPokemon()"); //??* este caso no contemplar nunca ocurre creo, preguntar por las dudas
        }
    }
    else
    {

        log_error(logger, "ERROR enviarMensajeNewPokemon() no recibi nada server apagado "); //??* en este caso agregar en cola enviados y volvera a intentar en proxima ejecutarColaNewPokemon()
    }

    free(unGetPokemon);
}

void enviarMensajeLocalizedPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;

    uint32_t desplazamiento = 0;
    uint32_t tamanioNombrePokemon = 0;
    uint32_t tamanioBuffer = 0;

    t_localizedPokemon *unLocalizedPokemon = malloc(sizeof(t_localizedPokemon));

    unLocalizedPokemon->identificador = unMensaje->idMensaje;
    unLocalizedPokemon->identificadorCorrelacional = unMensaje->idMensajeCorrelacional;

    memcpy(&tamanioNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    char *bufferNombrePokemon = malloc(tamanioNombrePokemon + 1);
    memcpy(bufferNombrePokemon, unMensaje->posicionEnMemoria + desplazamiento, tamanioNombrePokemon);
    bufferNombrePokemon[tamanioNombrePokemon] = '\0';
    desplazamiento += tamanioNombrePokemon;

    unLocalizedPokemon->nombrePokemon = string_new();
    string_append(&unLocalizedPokemon->nombrePokemon, bufferNombrePokemon);

    free(bufferNombrePokemon);

    uint32_t cantidadListaDatosPokemon = 0;

    memcpy(&cantidadListaDatosPokemon, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    t_list *unaListaDatosPokemon = list_create();

    int contador = 0;

    while (contador < cantidadListaDatosPokemon)
    {

        datosPokemon *nodoDatosPokemon = malloc(sizeof(datosPokemon));

        memcpy(&nodoDatosPokemon->cantidad, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(&nodoDatosPokemon->posicionEnElMapaX, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(&nodoDatosPokemon->posicionEnElMapaY, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        contador += 1;

        list_add(unaListaDatosPokemon, nodoDatosPokemon);
    }

    unLocalizedPokemon->listaDatosPokemon = unaListaDatosPokemon;

    int tamanioPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 4);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tLocalizedPokemon, unLocalizedPokemon, tamanioPokemon);

    bool anyAck = false;

    pthread_mutex_lock(&mutex_ackABuscar);

    ackABuscar = unSuscriptor->identificador;

    anyAck = list_any_satisfy(unMensaje->suscriptoresEnviados, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

    pthread_mutex_unlock(&mutex_ackABuscar);

    if (!anyAck)
    {
        list_add(unMensaje->suscriptoresEnviados, unSuscriptor->identificador);
    }

    int resultado;
    int tipoResultado = 0;

    if ((resultado = recibirInt(unSuscriptor->identificadorCorrelacional, &tipoResultado)) > 0)
    {

        if (tipoResultado == 1)
        {

            log_info(logger, "enviarMensajeNewPokemon() ok realizando ack");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {

            log_info(logger, "ERROR enviarMensajeNewPokemon()"); //??* este caso no contemplar nunca ocurre creo, preguntar por las dudas
        }
    }
    else
    {

        log_error(logger, "ERROR enviarMensajeNewPokemon() no recibi nada server apagado "); //??* en este caso agregar en cola enviados y volvera a intentar en proxima ejecutarColaNewPokemon()
    }

    free(unLocalizedPokemon);
}

void ejecutarColaNewPokemon()
{

    t_list *mensajesAEnviar = list_create();
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;

    while (true)
    {

        if (!list_is_empty(NEW_POKEMON_LISTA))
        {

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tNewPokemon; // 1== newPokemon

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);
                suscriptoresCant = list_size(NEW_POKEMON_LISTA);

                for (int i = 0; i < suscriptoresCant; i++)
                {

                    unSuscriptorCola = list_get(NEW_POKEMON_LISTA, i);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);

                            enviarMensajeNewPokemon(unMensaje, unSuscriptor);
                        }
                    }
                }
            }
        }
    }
}

void ejecutarColaAppearedPokemon()
{

    t_list *mensajesAEnviar = list_create();
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;

    while (true)
    {

        if (!list_is_empty(APPEARED_POKEMON_LISTA))
        {

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tAppearedPokemon; // 2 == APPEARED_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);
                suscriptoresCant = list_size(APPEARED_POKEMON_LISTA);

                for (int i = 0; i < suscriptoresCant; i++)
                {

                    unSuscriptorCola = list_get(APPEARED_POKEMON_LISTA, i);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);

                            enviarMensajeAppearedPokemon(unMensaje, unSuscriptor);
                        }
                    }
                }
            }
        }
    }
}

void ejecutarColaCatchPokemon()
{

    t_list *mensajesAEnviar = list_create();
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;

    while (true)
    {

        if (!list_is_empty(CATCH_POKEMON_LISTA))
        {

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tCatchPokemon; // 3== CATCH_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);
                suscriptoresCant = list_size(CATCH_POKEMON_LISTA);

                for (int i = 0; i < suscriptoresCant; i++)
                {

                    unSuscriptorCola = list_get(CATCH_POKEMON_LISTA, i);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);

                            enviarMensajeCatchPokemon(unMensaje, unSuscriptor);
                        }
                    }
                }
            }
        }
    }
}

void ejecutarColaCaughtPokemon()
{

    t_list *mensajesAEnviar = list_create();
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;

    while (true)
    {

        if (!list_is_empty(CAUGHT_POKEMON_LISTA))
        {

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tCaughtPokemon; // 4= CAUGHT_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);
                suscriptoresCant = list_size(CAUGHT_POKEMON_LISTA);

                for (int i = 0; i < suscriptoresCant; i++)
                {

                    unSuscriptorCola = list_get(CAUGHT_POKEMON_LISTA, i);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);

                            enviarMensajeCaughtPokemon(unMensaje, unSuscriptor);
                        }
                    }
                }
            }
        }
    }
}

void ejecutarColaGetPokemon()
{

    t_list *mensajesAEnviar = list_create();
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;

    while (true)
    {

        if (!list_is_empty(GET_POKEMON_LISTA))
        {

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tGetPokemon; // 5 == GET_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);
                suscriptoresCant = list_size(GET_POKEMON_LISTA);

                for (int i = 0; i < suscriptoresCant; i++)
                {

                    unSuscriptorCola = list_get(GET_POKEMON_LISTA, i);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);

                            enviarMensajeGetPokemon(unMensaje, unSuscriptor);
                        }
                    }
                }
            }
        }
    }
}

void ejecutarColaLocalizedPokemon()
{

    t_list *mensajesAEnviar = list_create();
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;

    while (true)
    {

        if (!list_is_empty(LOCALIZED_POKEMON_LISTA))
        {

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tLocalizedPokemon; // 6== LOCALIZED_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);
                suscriptoresCant = list_size(LOCALIZED_POKEMON_LISTA);

                for (int i = 0; i < suscriptoresCant; i++)
                {

                    unSuscriptorCola = list_get(LOCALIZED_POKEMON_LISTA, i);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);

                            enviarMensajeLocalizedPokemon(unMensaje, unSuscriptor);
                        }
                    }
                }
            }
        }
    }
}

////////////////////////////////////////FUNCIONES LISTAS////////////////////////////////////////////////

bool existeNuevoSuscriber(void *unSuscriber)
{

    t_suscriptor *p = (t_suscriptor *)unSuscriber;

    bool existe = false;

    if ((p->puerto == PuertoEschuchaABuscar) && (!strcmp(p->ip, ipServerABuscar)))
    {
        existe = true;
    }

    return existe;
}

bool existeIdSuscriberEnCola(void *suscriptorEnCola)
{

    tSuscriptorEnCola *p = (tSuscriptorEnCola *)suscriptorEnCola;

    bool existe = false;

    if ((p->identificador == idSuscriberABuscar))
    {
        existe = true;
    }

    return existe;
}

bool existeTipoMensaje(void *mensaje)
{

    tMensaje *p = (tMensaje *)mensaje;

    bool existe = false;

    if ((p->tipoMensaje == tipoMensajeABuscar))
    {
        existe = true;
    }

    return existe;
}

bool existeIdSuscriptor(void *suscriptor)
{

    t_suscriptor *p = (t_suscriptor *)suscriptor;

    bool existe = false;

    if ((p->identificador == idSuscriptorABuscar))
    {
        existe = true;
    }

    return existe;
}

bool existeAck(void *numero)
{

    uint32_t *p = (uint32_t *)numero;

    bool existe = false;

    if ((p == ackABuscar))
    {
        existe = true;
    }

    return existe;
}