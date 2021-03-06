#include "broker.h"

////////////////////////////////////////GESTION DEL PROCESO BROKER////////////////////////////////////////////////

/*Carga las configuraciones del broker, logger, inicializa la memoria y los hilos*/
void inicializarBroker()
{

    cargarConfiguracionBroker();

    configurarLoggerBroker();

    inicializarMemoria();

    inicializarHilosYVariablesBroker();

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

    //inicializarHilosYVariablesBroker();
}

/*Importa la configuracion del archivo config del brooker*/
void cargarConfiguracionBroker()
{

    CONFIG_BROKER = malloc(sizeof(brokerConfig_t));

    t_config *unBrokerArchivoConfig = malloc(sizeof(t_config));

    unBrokerArchivoConfig = config_create(PATH_CONFIG_BROKER);

    if (unBrokerArchivoConfig == NULL)
    {

        printf(C_ROJO"\n=============================================================================================\n");
        printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL BROKER"C_RESET);
    }
    else
    {

        printf(C_VERDE"\n================================================================================\n");
        printf(C_VERDE"\n================================================================================\n"C_RESET);
        printf(C_AZUL"\nCONFIGURACION DEL BROKER IMPORTADA CON EXITO"C_RESET);

        CONFIG_BROKER->tamanioMemoria = config_get_int_value(unBrokerArchivoConfig, TAMANO_MEMORIA);
        CONFIG_BROKER->tamanioMinimoParticion = config_get_int_value(unBrokerArchivoConfig, TAMANO_MINIMO_PARTICION);
        CONFIG_BROKER->algoritmoMemoria = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_MEMORIA);
        CONFIG_BROKER->algoritmoReemplazo = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_REEMPLAZO);
        CONFIG_BROKER->algoritmoParticionLibre = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_PARTICION_LIBRE);
        CONFIG_BROKER->ipBroker = config_get_string_value(unBrokerArchivoConfig, IP_BROKER);
        CONFIG_BROKER->puertoBroker = config_get_int_value(unBrokerArchivoConfig, PUERTO_BROKER);
        CONFIG_BROKER->frecuenciaCompactacion = config_get_int_value(unBrokerArchivoConfig, FRECUENCIA_COMPACTACION);
        CONFIG_BROKER->logFile = config_get_string_value(unBrokerArchivoConfig, LOG_FILE);
        CONFIG_BROKER->pathDump = config_get_string_value(unBrokerArchivoConfig, PATH_DUMP);

       // printf(“"\033[1;31m");
        printf(C_MAGENTA"\n\n· Tamanio de la Memoria = %d\n", CONFIG_BROKER->tamanioMemoria);
        printf("· Tamanio Minimo de Particion = %d\n", CONFIG_BROKER->tamanioMinimoParticion);
        printf("· Algoritmo de Memoria = %s\n", CONFIG_BROKER->algoritmoMemoria);
        printf("· Algoritmo de Reemplazo = %s\n", CONFIG_BROKER->algoritmoReemplazo);
        printf("· Algoritmo de Particion Libre = %s\n", CONFIG_BROKER->algoritmoParticionLibre);
        printf("· IP del Broker = %s\n", CONFIG_BROKER->ipBroker);
        printf("· Puerto del Broker = %d\n", CONFIG_BROKER->puertoBroker);
        printf("· Frecuencia de Compactacion = %d\n", CONFIG_BROKER->frecuenciaCompactacion);
        printf("· Ruta del Archivo Log del Broker = %s\n", CONFIG_BROKER->logFile);
        printf("· Ruta del Archivo Dump del Broker = %s\n\n"C_RESET, CONFIG_BROKER->pathDump);
        
        free(unBrokerArchivoConfig);

        printf(C_VERDE"\n================================================================================\n");
        printf(C_VERDE"\n================================================================================\n"C_RESET);

    }
}

/*Inicializa logger*/
void configurarLoggerBroker()
{

    logger = log_create(CONFIG_BROKER->logFile, "BROKER", true, LOG_LEVEL_TRACE);
    //log_info(logger, "LOG INICIALIZADO CON EXITO");
}

/*Libera las variables Globales*/
void finalizarBroker()
{

    free(CONFIG_BROKER);
    free(logger);
}

/*Actualiza cada cierto tiempo el config */
void actualizarConfiguracionBroker()
{
    //?* revisar si funciona la funcion, se podria optimizar
    FILE *archivoConfigFp;

    while (1)
    {

        sleep(60);

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

/*Inicializa server, las colas y el actualizar config*/
void inicializarHilosYVariablesBroker()
{

    cantidadDeActualizacionesConfigBroker = 0;

    infoServidor_t *unaInfoServidorBroker;

    unaInfoServidorBroker = malloc(sizeof(infoServidor_t));

    unaInfoServidorBroker->puerto = CONFIG_BROKER->puertoBroker;
    unaInfoServidorBroker->ip = string_new();
    //string_append(&unaInfoServidorBroker->ip,CONFIG_BROKER->ipBroker); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    string_append(&unaInfoServidorBroker->ip, "0");

    //pthread_create(&hiloActualizadorConfigBroker, NULL, (void *)actualizarConfiguracionBroker, NULL); // ?* creo que no es necesario

    pthread_create(&hiloServidorBroker, NULL, (void *)servidor_inicializar, (void *)unaInfoServidorBroker);

    pthread_create(&hiloNew, NULL, (void *)ejecutarColaNewPokemon, NULL);
    pthread_create(&hiloAppeared, NULL, (void *)ejecutarColaAppearedPokemon, NULL);
    pthread_create(&hiloCatch, NULL, (void *)ejecutarColaCatchPokemon, NULL);
    pthread_create(&hiloCaught, NULL, (void *)ejecutarColaCaughtPokemon, NULL);
    pthread_create(&hiloGet, NULL, (void *)ejecutarColaGetPokemon, NULL);
    pthread_create(&hiloLocalized, NULL, (void *)ejecutarColaLocalizedPokemon, NULL);

    //pthread_create(&hiloDumpCache,NULL,(void*)dumpCache, NULL);

    //pthread_join(hiloActualizadorConfigBroker, NULL);
    pthread_join(hiloServidorBroker, NULL);


}

////////////////////////////////////////ADMINISTRACION MEMORIA////////////////////////////////////////////////

/*Reserva memoria principal, e inicializa estructuras administrativas de las colas y la metadata*/
void inicializarMemoria()
{
    if (string_equals_ignore_case(CONFIG_BROKER->algoritmoMemoria, "BS"))
    {
        truncarMemoriaBuddySystem();
    }

    if ((MEMORIA_PRINCIPAL = malloc(CONFIG_BROKER->tamanioMemoria)) == NULL)
    {
        log_error(logger, "- ERROR no se pudo RESERVAR MEMORIA_PRINCIPAL.");
        exit(EXIT_FAILURE);
    }

    NUM_SUSCRIPTOR = 0;
    ID_MENSAJE = 0;
    IDS_CORRELATIVOS = list_create();
    METADATA_MEMORIA = list_create();
    ID_PARTICION = 0;
    CANTIDAD_PARTICIONES_LIBERADAS = 0;

    tParticion *unaParti = malloc(sizeof(tParticion));

    unaParti->posicion = MEMORIA_PRINCIPAL;
    unaParti->free = true;
    unaParti->tamanio = CONFIG_BROKER->tamanioMemoria;
    unaParti->idMensaje = 0;
    unaParti->lru = nuevoTimeStamp();
    unaParti->timeInit = nuevoTimeStamp();
    unaParti->idParticion = ID_PARTICION;

    pthread_mutex_lock(&mutex_METADATA_MEMORIA);

    list_add(METADATA_MEMORIA, unaParti);
    
    pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

    NEW_POKEMON_LISTA = list_create();
    APPEARED_POKEMON_LISTA = list_create();
    CATCH_POKEMON_LISTA = list_create();
    CAUGHT_POKEMON_LISTA = list_create();
    GET_POKEMON_LISTA = list_create();
    LOCALIZED_POKEMON_LISTA = list_create();
    SUSCRIPTORES_LISTA = list_create();
    MENSAJES_LISTA = list_create();

    // log_info(logger, "\n\n\n\t--Nuevo Suscriber a SUSCRIPTORES_LISTA");
    // log_trace(logger, "\n\n\n\t--Nuevo Suscriber a SUSCRIPTORES_LISTA");
    // log_debug(logger, "\n\n\n\t--Nuevo Suscriber a SUSCRIPTORES_LISTA");
    // log_warning(logger, "\n\n\n\t--Nuevo Suscriber a SUSCRIPTORES_LISTA");
    // log_error(logger, "\n\n\n\t--Nuevo Suscriber a SUSCRIPTORES_LISTA");
}

/*cambia valor de memoria a una potencia de dos*/
void truncarMemoriaBuddySystem()
{

    double tamReal = CONFIG_BROKER->tamanioMemoria;
    double potencia = 0;
    double tamFinal = 0;
    double valorDos = 2;
    double auxiliar = 0;

    while (auxiliar <= tamReal)
    {

        potencia++;
        tamFinal = pow(valorDos, potencia);
        auxiliar = pow(valorDos, potencia + 1);
    }

    CONFIG_BROKER->tamanioMemoria = (uint32_t)tamFinal; //?*verificar si este casteo es esta bien
}

/*Devuelve un nuevo id para un mensaje asociado*/
uint32_t generarNuevoIdMensajeBroker()
{

    pthread_mutex_lock(&mutex_ID_MENSAJE);
    ID_MENSAJE = ID_MENSAJE +1;
    pthread_mutex_unlock(&mutex_ID_MENSAJE);

    return ID_MENSAJE;
}

/*Devuelve un nuevo id para un suscriptor asociado*/
uint32_t generarNuevoIdSuscriptor()
{

    pthread_mutex_lock(&mutex_NUM_SUSCRIPTOR);
    NUM_SUSCRIPTOR++;
    pthread_mutex_unlock(&mutex_NUM_SUSCRIPTOR);

    return NUM_SUSCRIPTOR;
}

/*Devuelve un nuevo id para un mensaje asociado*/
uint32_t generarNuevoIdParticion()
{

    pthread_mutex_lock(&mutex_ID_PARTICION);
    ID_PARTICION++;
    pthread_mutex_unlock(&mutex_ID_PARTICION);

    return ID_PARTICION;
}

/*Devuelve una particion libre de la memoria general*/
char *getDireccionMemoriaLibre(uint32_t idMensaje, uint32_t tamanio)
{
    char *aDevolver = NULL;

    if (tamanio < CONFIG_BROKER->tamanioMinimoParticion)
    {

        tamanio = CONFIG_BROKER->tamanioMinimoParticion;
    }

    if (string_equals_ignore_case(CONFIG_BROKER->algoritmoMemoria, "BS"))
    {
        //ingresamos siempre por la primera particion 0
        pthread_mutex_lock(&mutex_METADATA_MEMORIA);

        aDevolver = getDireccionMemoriaLibreBuddySystem(idMensaje, tamanio, 0);

        pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

        while (aDevolver == NULL)
        {
            ejecutarEliminarParticionBuddy();

            pthread_mutex_lock(&mutex_METADATA_MEMORIA);
            aDevolver = getDireccionMemoriaLibreBuddySystem(idMensaje, tamanio, 0);
            pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
        }
    }
    else
    {
        // ES PARTICION DINAMICA

        if (string_equals_ignore_case(CONFIG_BROKER->algoritmoParticionLibre, "FF"))
        {
            pthread_mutex_lock(&mutex_METADATA_MEMORIA);
            tParticion *unaParticion = buscarParticionLibreEnMemoria(tamanio);

            if (unaParticion != NULL)
            {
                unaParticion = splitParticion(unaParticion, tamanio);

                unaParticion->free = false;
                unaParticion->idMensaje = idMensaje;
                unaParticion->lru = nuevoTimeStamp();
                unaParticion->timeInit = nuevoTimeStamp();
                aDevolver = unaParticion->posicion;
                pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
            }
            else
            {
                pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
                //SE COMPACTA SEGUN CRITERIO DEL CONFIG
                compactarMemoria();

                pthread_mutex_lock(&mutex_METADATA_MEMORIA);
                unaParticion = buscarParticionLibreEnMemoria(tamanio);
                if (unaParticion != NULL)
                {
                    unaParticion = splitParticion(unaParticion, tamanio);

                    unaParticion->free = false;
                    unaParticion->idMensaje = idMensaje;
                    unaParticion->lru = nuevoTimeStamp();
                    unaParticion->timeInit = nuevoTimeStamp();
                    aDevolver = unaParticion->posicion;
                    pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
                }
                else
                {
                    pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
                    ejecutarEliminarParticion();
                    aDevolver = getDireccionMemoriaLibre(idMensaje, tamanio);
                }
            }
        }
        else
        {
            //BF
            t_list *ParticionesOrdenadas;

            pthread_mutex_lock(&mutex_METADATA_MEMORIA);
            ParticionesOrdenadas = buscarListaDeParticionesLibresEnMemoriaOrdenadas(tamanio);

            if (!list_is_empty(ParticionesOrdenadas))
            {

                tParticion *unaParticion = (tParticion *)list_get(ParticionesOrdenadas, 0);

                unaParticion = splitParticion(unaParticion, tamanio);

                unaParticion->free = false;
                unaParticion->idMensaje = idMensaje;
                unaParticion->lru = nuevoTimeStamp();
                unaParticion->timeInit = nuevoTimeStamp();
                aDevolver = unaParticion->posicion;
                pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
            }
            else
            {
                list_destroy(ParticionesOrdenadas);

                pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

                compactarMemoria();

                pthread_mutex_lock(&mutex_METADATA_MEMORIA);

                ParticionesOrdenadas = buscarListaDeParticionesLibresEnMemoriaOrdenadas(tamanio);

                if (!list_is_empty(ParticionesOrdenadas))
                {
                    tParticion *unaParticion = (tParticion *)list_get(ParticionesOrdenadas, 0);

                    unaParticion = splitParticion(unaParticion, tamanio);

                    unaParticion->free = false;
                    unaParticion->idMensaje = idMensaje;
                    unaParticion->lru = nuevoTimeStamp();
                    unaParticion->timeInit = nuevoTimeStamp();
                    aDevolver = unaParticion->posicion;

                    pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
                }
                else
                {
                    pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
                    ejecutarEliminarParticion();
                    aDevolver = getDireccionMemoriaLibre(idMensaje, tamanio);
                }
            }
            list_destroy(ParticionesOrdenadas);
        }
    }

    return aDevolver;
}

/*Devuelve una particion libre de la memoria bs*/
char *getDireccionMemoriaLibreBuddySystem(uint32_t idMensaje, uint32_t tamanio, uint32_t index)
{
    //LOS MUTEX DE TPARTICION METADATA_MEMORIA ESTAN EN getDireccionMemoria que los utiliza

    char *aDevolver = NULL;

    pthread_mutex_lock(&mutex_idParticionABuscar);

    idParticionABuscar = index;
    tParticion *unaParticion = (tParticion *)list_find(METADATA_MEMORIA, &existeIdParticion);

    pthread_mutex_unlock(&mutex_idParticionABuscar);

    if (unaParticion != NULL)
    {

        if (tamanio > unaParticion->tamanio)
        {

            if (unaParticion->idParticion == 0)
            {
                log_error(logger, "\n\t--ERROR TAMANIO BUDDY SYSTEM PEDIDO ES MAYOR A MEMORIA TOTAL: %d ", tamanio); // Seguro rompe si pasa esto
            }

            return NULL;
        }

        if (unaParticion->free == true)
        {
            if (tamanio <= unaParticion->tamanio && tamanio > (unaParticion->tamanio / 2))
            {
                unaParticion->free = false;
                unaParticion->idMensaje = idMensaje;
                unaParticion->lru = nuevoTimeStamp();
                unaParticion->timeInit = nuevoTimeStamp();
                aDevolver = unaParticion->posicion;
            }
            else
            {
                splitBuddy(index);
                aDevolver = getDireccionMemoriaLibreBuddySystem(idMensaje, tamanio, index);
            }
        }
        else
        { //PAPA ESTA OCUPADO

            aDevolver = getDireccionMemoriaLibreBuddySystem(idMensaje, tamanio, 2 * index + 1);
            if (!aDevolver)
            {
                aDevolver = getDireccionMemoriaLibreBuddySystem(idMensaje, tamanio, 2 * index + 2);
            }
        }
    }

    return aDevolver;
}

/*Divide el segmento de memoria en 2*/
void splitBuddy(uint32_t index)
{
    //LOS MUTEX DE TPARTICION METADATA_MEMORIA ESTAN EN getDireccionMemoriaLibreBuddySystem que los utiliza

    pthread_mutex_lock(&mutex_idParticionABuscar);

    idParticionABuscar = index;
    tParticion *father = (tParticion *)list_find(METADATA_MEMORIA, &existeIdParticion);

    pthread_mutex_unlock(&mutex_idParticionABuscar);

    if (father != NULL)
    {

        father->free = false;
        father->idMensaje = 0; //?*agrego esto verificar si no rompe

        tParticion *leftChild = malloc(sizeof(tParticion));

        leftChild->posicion = father->posicion;
        leftChild->free = true;
        leftChild->tamanio = father->tamanio / 2;
        leftChild->idMensaje = 0;
        leftChild->lru = nuevoTimeStamp();
        leftChild->timeInit = nuevoTimeStamp();
        leftChild->idParticion = 2 * index + 1;

        tParticion *rightChild = malloc(sizeof(tParticion));

        rightChild->posicion = father->posicion + father->tamanio / 2; //SE RESTA UNO PORQUE EMPIEZA DESDE LA POSICION 0 ?*realizar prueba si esta bien
        rightChild->free = true;
        rightChild->tamanio = father->tamanio / 2;
        rightChild->idMensaje = 0;
        rightChild->lru = nuevoTimeStamp();
        rightChild->timeInit = nuevoTimeStamp();
        rightChild->idParticion = 2 * index + 2;

        list_add(METADATA_MEMORIA, leftChild);
        list_add(METADATA_MEMORIA, rightChild);
    }
}

/*Se decide que particion eliminar y se elimina dentro de Buddy*/
void ejecutarEliminarParticionBuddy()
{
    if (string_equals_ignore_case(CONFIG_BROKER->algoritmoReemplazo, "FIFO"))
    {
        pthread_mutex_lock(&mutex_METADATA_MEMORIA);
        t_list *ParticionesOrdenadasPorTimeInit;

        ParticionesOrdenadasPorTimeInit = list_filter(METADATA_MEMORIA, &esParticionOcupadaConMensaje);

        list_sort(ParticionesOrdenadasPorTimeInit, &sortInitMenor);

        tParticion *unaParticion = (tParticion *)list_get(ParticionesOrdenadasPorTimeInit, 0);

        list_destroy(ParticionesOrdenadasPorTimeInit);
        
        pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

        eliminarMensaje(unaParticion->idMensaje); //SE ELIMINA EL MENSAJE ANTES DE ELIMINAR LA PARTICION PARA QUE NO SE GENERE ERRORES

        unaParticion->free = true;
        unaParticion->idMensaje = 0;

        log_trace(logger, "\n\t--SE ELIMINA PARTICION-BS OCUPADA %d EN LA POSICION %d ", unaParticion->idParticion, (unaParticion->posicion - MEMORIA_PRINCIPAL));

        killMe(unaParticion->idParticion); //?*falta matar a los hijos de ser necesario
    }
    else
    {
        //LRU

        pthread_mutex_lock(&mutex_METADATA_MEMORIA);

        t_list *ParticionesOrdenadasPorTime;

        ParticionesOrdenadasPorTime = list_filter(METADATA_MEMORIA, &esParticionOcupadaConMensaje);

        list_sort(ParticionesOrdenadasPorTime, &sortTimeMenor);

        tParticion *unaParticion = (tParticion *)list_get(ParticionesOrdenadasPorTime, 0);

        list_destroy(ParticionesOrdenadasPorTime);

        pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

        eliminarMensaje(unaParticion->idMensaje); //SE ELIMINA EL MENSAJE ANTES DE ELIMINAR LA PARTICION PARA QUE NO SE GENERE ERRORES

        unaParticion->free = true;
        unaParticion->idMensaje = 0;

        log_trace(logger, "\n\t--SE ELIMINA PARTICION-BS OCUPADA %d EN LA POSICION %d ", unaParticion->idParticion, (unaParticion->posicion - MEMORIA_PRINCIPAL));

        killMe(unaParticion->idParticion);
    }
}

/*Se elimina particion bs y se rearma la estructura*/
void killMe(uint32_t index)
{

    if (index == 0)
    {
        pthread_mutex_lock(&mutex_METADATA_MEMORIA);
        pthread_mutex_lock(&mutex_idParticionABuscar);

        idParticionABuscar = index;
        tParticion *father = (tParticion *)list_find(METADATA_MEMORIA, &existeIdParticion);

        pthread_mutex_unlock(&mutex_idParticionABuscar);
        
        pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

        log_trace(logger, "\n\t--LA MEMORIA QUEDO VACIA");

        father->free = true;
        father->idMensaje = 0;
        father->timeInit = nuevoTimeStamp();
        father->lru = nuevoTimeStamp();
    }
    else
    {
        if (index % 2 == 0)
        {
            //ES HIJO DERECHO
            pthread_mutex_lock(&mutex_METADATA_MEMORIA);
            pthread_mutex_lock(&mutex_idParticionABuscar);

            idParticionABuscar = index;
            tParticion *right = (tParticion *)list_find(METADATA_MEMORIA, &existeIdParticion);

            pthread_mutex_unlock(&mutex_idParticionABuscar);
            
            pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

            right->free = true;
            right->idMensaje = 0;
            right->timeInit = nuevoTimeStamp();
            right->lru = nuevoTimeStamp();

            uint32_t indexFather = (index - 2) / 2;

            pthread_mutex_lock(&mutex_METADATA_MEMORIA);

            pthread_mutex_lock(&mutex_idParticionABuscar);

            idParticionABuscar = 2 * indexFather + 1; //Busco al izquierdo
            tParticion *left = (tParticion *)list_find(METADATA_MEMORIA, &existeIdParticion);

            pthread_mutex_unlock(&mutex_idParticionABuscar);
            
            pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

            if (left)
            {
                if (left->free == true)
                {
                    pthread_mutex_lock(&mutex_METADATA_MEMORIA);

                    pthread_mutex_lock(&mutex_idParticionABuscar);

                    idParticionABuscar = 2 * indexFather + 1; //lo saco de la lista
                    left = (tParticion *)list_remove_by_condition(METADATA_MEMORIA, &existeIdParticion);

                    pthread_mutex_unlock(&mutex_idParticionABuscar);
                    

                    pthread_mutex_lock(&mutex_idParticionABuscar);

                    idParticionABuscar = 2 * indexFather + 2;
                    right = (tParticion *)list_remove_by_condition(METADATA_MEMORIA, &existeIdParticion);

                    pthread_mutex_unlock(&mutex_idParticionABuscar);
                    
                    pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

                    log_trace(logger, "\n\t--SE ELIMINA PARTICION-I %d EN LA POSICION %d ", left->idParticion, (left->posicion - MEMORIA_PRINCIPAL));
                    log_trace(logger, "\n\t--SE ELIMINA PARTICION-D %d EN LA POSICION %d ", right->idParticion, (right->posicion - MEMORIA_PRINCIPAL));
                    log_trace(logger, "\n\t--SE MANDA A LIBERAR AL PADRE %d",indexFather);


                    free(left);
                    free(right);
                    killMe(indexFather);
                }
            }
            else
            {
                log_error(logger, "ERROR KILLME(): NO EXISTE EL HIJO IZQUIERDO ");
            }
        }
        else
        {
            //ES HIJO IZQUIERDO
            
            pthread_mutex_lock(&mutex_METADATA_MEMORIA);

            pthread_mutex_lock(&mutex_idParticionABuscar);

            idParticionABuscar = index;
            tParticion *left = (tParticion *)list_find(METADATA_MEMORIA, &existeIdParticion);

            pthread_mutex_unlock(&mutex_idParticionABuscar);
            
            pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

            left->free = true;
            left->idMensaje = 0;
            left->timeInit = nuevoTimeStamp();
            left->lru = nuevoTimeStamp();

            uint32_t indexFather = (index - 1) / 2;

            pthread_mutex_lock(&mutex_METADATA_MEMORIA);

            pthread_mutex_lock(&mutex_idParticionABuscar);

            idParticionABuscar = 2 * indexFather + 2; //Busco al derecho
            tParticion *right = (tParticion *)list_find(METADATA_MEMORIA, &existeIdParticion);

            pthread_mutex_unlock(&mutex_idParticionABuscar);
            
            pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

            if (right)
            {
                if (right->free == true)
                {
                    pthread_mutex_lock(&mutex_METADATA_MEMORIA);

                    pthread_mutex_lock(&mutex_idParticionABuscar);

                    idParticionABuscar = 2 * indexFather + 1; //lo saco de la lista
                    left = (tParticion *)list_remove_by_condition(METADATA_MEMORIA, &existeIdParticion);

                    pthread_mutex_unlock(&mutex_idParticionABuscar);

                    pthread_mutex_lock(&mutex_idParticionABuscar);

                    idParticionABuscar = 2 * indexFather + 2;
                    right = (tParticion *)list_remove_by_condition(METADATA_MEMORIA, &existeIdParticion);

                    pthread_mutex_unlock(&mutex_idParticionABuscar);
                    
                    pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

                    log_trace(logger, "\n\t--SE ELIMINA PARTICION-I %d EN LA POSICION %d ", left->idParticion, (left->posicion - MEMORIA_PRINCIPAL));
                    log_trace(logger, "\n\t--SE ELIMINA PARTICION-D %d EN LA POSICION %d ", right->idParticion, (right->posicion - MEMORIA_PRINCIPAL));
                    log_trace(logger, "\n\t--SE MANDA A LIBERAR AL PADRE %d",indexFather);


                    free(left);
                    free(right);
                    killMe(indexFather);
                }
            }
            else
            {
                log_error(logger, "ERROR KILLME(): NO EXISTE EL HIJO DERECHO ");
            }
        }
    }
}

/*Se elimina particion dentro de particion dinamica y se consolida*/
void ejecutarEliminarParticion()
{
    if (string_equals_ignore_case(CONFIG_BROKER->algoritmoReemplazo, "FIFO"))
    {
        //log_warning(logger,"---Elimino particion por fifo");

        pthread_mutex_lock(&mutex_METADATA_MEMORIA);

        t_list *ParticionesOrdenadasPorTimeInit;

        ParticionesOrdenadasPorTimeInit = list_filter(METADATA_MEMORIA, &esParticionOcupada);
        
        list_sort(ParticionesOrdenadasPorTimeInit, &sortInitMenor); //?*sortPidMenor quedo en desuso        

        tParticion *unaParticion = (tParticion *)list_get(ParticionesOrdenadasPorTimeInit, 0);
        
        pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

        list_destroy(ParticionesOrdenadasPorTimeInit);

        eliminarMensaje(unaParticion->idMensaje); //SE ELIMINA EL MENSAJE ANTES DE ELIMINAR LA PARTICION PARA QUE NO SE GENERE ERRORES

        pthread_mutex_lock(&mutex_METADATA_MEMORIA);

        log_trace(logger, "\n\t--SE ELIMINA PARTICION %d EN LA POSICION %d ", unaParticion->idParticion, (unaParticion->posicion - MEMORIA_PRINCIPAL));
        
        unaParticion->free = true;
        unaParticion->idMensaje = 0;
        consolidarCache(unaParticion);
        CANTIDAD_PARTICIONES_LIBERADAS++;


        pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
    }
    else
    {
        //log_warning(logger,"---Elimino particion por LRU");
        //LRU

        pthread_mutex_lock(&mutex_METADATA_MEMORIA);
        t_list *ParticionesOrdenadasPorTime;

        ParticionesOrdenadasPorTime = list_filter(METADATA_MEMORIA, &esParticionOcupada);

        list_sort(ParticionesOrdenadasPorTime, &sortTimeMenor);

        tParticion *unaParticion = (tParticion *)list_get(ParticionesOrdenadasPorTime, 0);
        
        pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

        //printf(C_AZUL"---particion a eliminar: ");
        //printf("---Particion: %d", unaParticion->idParticion);
        //printf("---IdMensaje: %d"C_RESET, unaParticion->idMensaje);

        list_destroy(ParticionesOrdenadasPorTime);

        eliminarMensaje(unaParticion->idMensaje); //SE ELIMINA EL MENSAJE ANTES DE ELIMINAR LA PARTICION PARA QUE NO SE GENERE ERRORES

        pthread_mutex_lock(&mutex_METADATA_MEMORIA);

        log_trace(logger, "\n\t--SE ELIMINA PARTICION %d EN LA POSICION %d ", unaParticion->idParticion, (unaParticion->posicion - MEMORIA_PRINCIPAL));

        unaParticion->free = true;
        unaParticion->idMensaje = 0;
        consolidarCache(unaParticion);
        CANTIDAD_PARTICIONES_LIBERADAS++;
        pthread_mutex_unlock(&mutex_METADATA_MEMORIA);
    }
}

/*Divide una particion en dos si el tamaño de la particion es mas grande que el requerido*/
tParticion *splitParticion(tParticion *unaParticion, uint32_t tamanio)
{
    if (unaParticion->tamanio > tamanio)
    {

        tParticion *nuevaParti = malloc(sizeof(tParticion));

        nuevaParti->posicion = unaParticion->posicion + tamanio; //SE RESTA UNO PORQUE EMPIEZA DESDE LA POSICION 0
        nuevaParti->free = true;
        nuevaParti->tamanio = unaParticion->tamanio - tamanio;
        nuevaParti->idMensaje = 0;
        nuevaParti->lru = nuevoTimeStamp();
        nuevaParti->timeInit = nuevoTimeStamp();
        nuevaParti->idParticion = generarNuevoIdParticion();

        unaParticion->tamanio = tamanio;

        list_add(METADATA_MEMORIA, nuevaParti);
    }

    return unaParticion;
}

/*Consolida la memoria a izquierda y derecha*/
void consolidarCache(tParticion *unaParticion)
{
    //MUTEX METADATA HECHO

    //CONSOLIDO A DERECHA

    pthread_mutex_lock(&mutex_posicionParticionABuscar);

    posicionParticionABuscar = unaParticion->posicion + unaParticion->tamanio;
    tParticion *derecha = (tParticion *)list_find(METADATA_MEMORIA, &existePosicionParticion);

    pthread_mutex_unlock(&mutex_posicionParticionABuscar);

    if (derecha && derecha->free == true)
    {

        pthread_mutex_lock(&mutex_posicionParticionABuscar);

        posicionParticionABuscar = unaParticion->posicion + unaParticion->tamanio;

        tParticion *derecha = (tParticion *)list_remove_by_condition(METADATA_MEMORIA, &existePosicionParticion);

        pthread_mutex_unlock(&mutex_posicionParticionABuscar);

        unaParticion->tamanio += derecha->tamanio;
        free(derecha);
    }

    //CONSOLIDO A IZQUIERDA

    pthread_mutex_lock(&mutex_posicionParticionABuscar);

    posicionParticionABuscar = unaParticion->posicion;
    tParticion *izquierda = (tParticion *)list_find(METADATA_MEMORIA, &existePosicionParticionIzquierda);

    pthread_mutex_unlock(&mutex_posicionParticionABuscar);

    if (izquierda && izquierda->free == true)
    {

        pthread_mutex_lock(&mutex_posicionParticionABuscar);

        posicionParticionABuscar = unaParticion->posicion;
        tParticion *izquierda = (tParticion *)list_remove_by_condition(METADATA_MEMORIA, &existePosicionParticionIzquierda);

        pthread_mutex_unlock(&mutex_posicionParticionABuscar);

        unaParticion->posicion = izquierda->posicion;
        unaParticion->tamanio += izquierda->tamanio;
        free(izquierda);
    }
}

/*Gestiona la compactacion y la manda a ejecutar*/
void compactarMemoria()
{
    //?*revisar En el enunciado si esto es asi
    if ((CONFIG_BROKER->frecuenciaCompactacion == -1 || CONFIG_BROKER->frecuenciaCompactacion == 0 || CONFIG_BROKER->frecuenciaCompactacion == 1) && CANTIDAD_PARTICIONES_LIBERADAS > 0)
    {
        ejecutarCompactacion();
    }
    else
    {

        if (CONFIG_BROKER->frecuenciaCompactacion == CANTIDAD_PARTICIONES_LIBERADAS)
        {
            ejecutarCompactacion();
        }
    }
}

/*Ejecuta la Compactacion y Actualiza id Particion*/
void ejecutarCompactacion()
{
    pthread_mutex_lock(&mutex_MENSAJES_LISTA);

    pthread_mutex_lock(&mutex_MENSAJES_NEW_POKEMON);

    pthread_mutex_lock(&mutex_MENSAJES_APPEARED_POKEMON);

    pthread_mutex_lock(&mutex_MENSAJES_CATCH_POKEMON);

    pthread_mutex_lock(&mutex_MENSAJES_CAUGHT_POKEMON);

    pthread_mutex_lock(&mutex_MENSAJES_GET_POKEMON);

    pthread_mutex_lock(&mutex_MENSAJES_LOCALIZED_POKEMON);

    pthread_mutex_lock(&mutex_METADATA_MEMORIA);

    log_trace(logger, "\n\t--SE EJECUTA LA COMPACTACION--");

    compactacion(MEMORIA_PRINCIPAL);

    u_int32_t cantParticiones = list_size(METADATA_MEMORIA);
    tParticion *unaParticion;
    tMensaje *unMensaje;

    for (int i = 0; i < cantParticiones; i++)
    {
        unaParticion = list_get(METADATA_MEMORIA, i);

        if (unaParticion)
        {
            unaParticion->idParticion = i;

            if (unaParticion->free == false)
            {

                pthread_mutex_lock(&mutex_idMensajeABuscar);

                idMensajeABuscar = unaParticion->idMensaje;

                unMensaje = list_find(MENSAJES_LISTA, &existeIdMensaje);

                pthread_mutex_unlock(&mutex_idMensajeABuscar);

                unMensaje->posicionEnMemoria = unaParticion->posicion;
            }
            else
            {
                unaParticion->idMensaje = 0;
            }
        }
    }

    pthread_mutex_lock(&mutex_ID_PARTICION);

    ID_PARTICION = cantParticiones + 1;

    pthread_mutex_unlock(&mutex_ID_PARTICION);

    CANTIDAD_PARTICIONES_LIBERADAS = 0;

    pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

    pthread_mutex_unlock(&mutex_MENSAJES_NEW_POKEMON);

    pthread_mutex_unlock(&mutex_MENSAJES_APPEARED_POKEMON);

    pthread_mutex_unlock(&mutex_MENSAJES_CATCH_POKEMON);

    pthread_mutex_unlock(&mutex_MENSAJES_CAUGHT_POKEMON);

    pthread_mutex_unlock(&mutex_MENSAJES_GET_POKEMON);

    pthread_mutex_unlock(&mutex_MENSAJES_LOCALIZED_POKEMON);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);

}


/*Compacta la memoria dejando una unica particion libre al final de la MEMORIA_PRINCIPAL */
void compactacion(char *posicion)
{
    //MUTEX METADATA ESTA EN FUNCION QUE LA LLAMA

    pthread_mutex_lock(&mutex_posicionParticionABuscar);

    posicionParticionABuscar = posicion;
    tParticion *primera = (tParticion *)list_find(METADATA_MEMORIA, &existePosicionParticion);

    pthread_mutex_unlock(&mutex_posicionParticionABuscar);

    if (primera)
    {
        if (primera->free == true)
        {
            pthread_mutex_lock(&mutex_posicionParticionABuscar);

            posicionParticionABuscar = primera->posicion + primera->tamanio;
            tParticion *next = (tParticion *)list_find(METADATA_MEMORIA, &existePosicionParticion);

            pthread_mutex_unlock(&mutex_posicionParticionABuscar);

            if (next)
            {
                if (next->free == true)
                {

                    pthread_mutex_lock(&mutex_posicionParticionABuscar);

                    posicionParticionABuscar = primera->posicion + primera->tamanio;
                    next = (tParticion *)list_remove_by_condition(METADATA_MEMORIA, &existePosicionParticion);

                    pthread_mutex_unlock(&mutex_posicionParticionABuscar);

                    primera->tamanio += next->tamanio;

                    free(next);
                    compactacion(primera->posicion);
                }
                else
                {
                    if (primera->tamanio >= next->tamanio)
                    {

                        memcpy(primera->posicion, next->posicion, sizeof(next->tamanio));

                        uint32_t auxTam = primera->tamanio;

                        primera->tamanio = next->tamanio;

                        next->tamanio = auxTam;

                        next->posicion = primera->posicion + primera->tamanio;

                        primera->free = false;

                        next->free = true;

                        primera->idMensaje = next->idMensaje;

                        primera->lru = next->lru;

                        primera->timeInit = next->timeInit;

                        compactacion(next->posicion);
                    }
                    else
                    {

                        memmove(primera->posicion, next->posicion, sizeof(next->tamanio)); //PERMITE OVERLAPPING EN MEMORIA PERO ES MAS LENTO

                        uint32_t auxTam = primera->tamanio;

                        primera->tamanio = next->tamanio;

                        next->tamanio = auxTam;

                        next->posicion = primera->posicion + primera->tamanio;

                        primera->free = false;

                        next->free = true;

                        primera->idMensaje = next->idMensaje;

                        primera->lru = next->lru;

                        primera->timeInit = next->timeInit;

                        compactacion(next->posicion);
                    }
                }
            }
        }
        else
        {
            compactacion(primera->posicion + primera->tamanio);
        }
    }
}

/*Destruye el Mensaje de la lista*/
void eliminarMensaje(uint32_t unIdMensaje)
{
    //MUTEX DE LA LISTA MENSAJES SE UTILIZAN EN ENVIAR MENSAJES
    tMensaje *unMensaje;

    pthread_mutex_lock(&mutex_MENSAJES_LISTA);

    pthread_mutex_lock(&mutex_idMensajeABuscar);

    idMensajeABuscar = unIdMensaje;

    unMensaje = list_find(MENSAJES_LISTA, &existeIdMensaje);//?*FALTARIA MUTEX TOTAL DE MENSAJES LISTA

    pthread_mutex_unlock(&mutex_idMensajeABuscar);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);

    if (unMensaje != NULL)
    {
        switch (unMensaje->tipoMensaje)
        {
        case tNewPokemon:
        {
            pthread_mutex_lock(&mutex_MENSAJES_NEW_POKEMON);

            pthread_mutex_lock(&mutex_idMensajeABuscar);

            idMensajeABuscar = unIdMensaje;

            unMensaje = list_remove_by_condition(MENSAJES_LISTA, &existeIdMensaje);

            pthread_mutex_unlock(&mutex_idMensajeABuscar);

            list_destroy(unMensaje->acknowledgement);
            list_destroy(unMensaje->suscriptoresEnviados);
            free(unMensaje);

            pthread_mutex_unlock(&mutex_MENSAJES_NEW_POKEMON);

            break;
        }
        case tAppearedPokemon:
        {
            pthread_mutex_lock(&mutex_MENSAJES_APPEARED_POKEMON);

            pthread_mutex_lock(&mutex_idMensajeABuscar);

            idMensajeABuscar = unIdMensaje;

            unMensaje = list_remove_by_condition(MENSAJES_LISTA, &existeIdMensaje);

            pthread_mutex_unlock(&mutex_idMensajeABuscar);

            list_destroy(unMensaje->acknowledgement);
            list_destroy(unMensaje->suscriptoresEnviados);
            free(unMensaje);

            pthread_mutex_unlock(&mutex_MENSAJES_APPEARED_POKEMON);

            break;
        }
        case tCatchPokemon:
        {
            pthread_mutex_lock(&mutex_MENSAJES_CATCH_POKEMON);

            pthread_mutex_lock(&mutex_idMensajeABuscar);

            idMensajeABuscar = unIdMensaje;

            unMensaje = list_remove_by_condition(MENSAJES_LISTA, &existeIdMensaje);

            pthread_mutex_unlock(&mutex_idMensajeABuscar);

            list_destroy(unMensaje->acknowledgement);
            list_destroy(unMensaje->suscriptoresEnviados);
            free(unMensaje);

            pthread_mutex_unlock(&mutex_MENSAJES_CATCH_POKEMON);

            break;
        }
        case tCaughtPokemon:
        {
            pthread_mutex_lock(&mutex_MENSAJES_CAUGHT_POKEMON);

            pthread_mutex_lock(&mutex_idMensajeABuscar);

            idMensajeABuscar = unIdMensaje;

            unMensaje = list_remove_by_condition(MENSAJES_LISTA, &existeIdMensaje);

            pthread_mutex_unlock(&mutex_idMensajeABuscar);

            list_destroy(unMensaje->acknowledgement);
            list_destroy(unMensaje->suscriptoresEnviados);
            free(unMensaje);

            pthread_mutex_unlock(&mutex_MENSAJES_CAUGHT_POKEMON);

            break;
        }
        case tGetPokemon:
        {
            pthread_mutex_lock(&mutex_MENSAJES_GET_POKEMON);

            pthread_mutex_lock(&mutex_idMensajeABuscar);

            idMensajeABuscar = unIdMensaje;

            unMensaje = list_remove_by_condition(MENSAJES_LISTA, &existeIdMensaje);

            pthread_mutex_unlock(&mutex_idMensajeABuscar);

            list_destroy(unMensaje->acknowledgement);
            list_destroy(unMensaje->suscriptoresEnviados);
            free(unMensaje);

            pthread_mutex_unlock(&mutex_MENSAJES_GET_POKEMON);

            break;
        }
        case tLocalizedPokemon:
        {
            pthread_mutex_lock(&mutex_MENSAJES_LOCALIZED_POKEMON);

            pthread_mutex_lock(&mutex_idMensajeABuscar);

            idMensajeABuscar = unIdMensaje;

            unMensaje = list_remove_by_condition(MENSAJES_LISTA, &existeIdMensaje);

            pthread_mutex_unlock(&mutex_idMensajeABuscar);

            list_destroy(unMensaje->acknowledgement);
            list_destroy(unMensaje->suscriptoresEnviados);
            free(unMensaje);

            pthread_mutex_unlock(&mutex_MENSAJES_LOCALIZED_POKEMON);

            break;
        }
        default:
        {

            log_warning(logger, "ERROR TIPO DE MENSAJE NO VALIDO- ELIMINARMENSAJE(): ");
            break;
        }
        }
    }
}

////////////////////////////////////////RECEPCION DE PAQUETES CON PROCESOS EXTERNOS////////////////////////////////////////////////

/*Administra el primer encuentro con la conexion asociada*/
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
            printf(C_VERDE"\n================================================================================\n");
            printf(C_VERDE"\n================================================================================\n"C_RESET);
            log_trace(logger, "SE CONECTO UN PROCESO GAMEBOY AL BROKER");

            manejarRespuestaAGameBoy(unaInfoAdmin->socketCliente, idCliente);
            break;
        }

        case 3:
        {
            printf(C_VERDE"\n================================================================================\n");
            printf(C_VERDE"\n================================================================================\n"C_RESET);
            log_trace(logger, "SE CONECTO UN PROCESO GAMECARD AL BROKER");

            manejarRespuestaAGameCard(unaInfoAdmin->socketCliente, idCliente);
            break;
        }

        case 4:
        {
            printf(C_VERDE"\n================================================================================\n");
            printf(C_VERDE"\n================================================================================\n"C_RESET);
            log_trace(logger, "SE CONECTO UN PROCESO TEAM AL BROKER");

            manejarRespuestaATeam(unaInfoAdmin->socketCliente, idCliente);
            break;
        }

        case -1:
        {

            //log_info(logger, "RECIBIMOS UNA PRUEBA DE CONEXION");
            break;
        }

        default:
        {   

            printf(C_VERDE"\n================================================================================\n");
            printf(C_VERDE"\n================================================================================\n"C_RESET);
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
    else if (resultado == -1)
    {

        log_warning(logger, "CLIENTE DESCONECTADO");
        close(unaInfoAdmin->socketCliente);
    }
    else if (resultado == -2)
    {
        printf("ME HICIERON UN PING\n\n\n");
    }
    else if (resultado < -2)
    {

        log_warning(logger, "Se desconectó el broker, o bien, terminó el tiempo de suscripcióna la cola de mensajes.");
        close(unaInfoAdmin->socketCliente);
    }

    return;
}

/*Determina que hacer dependiendo el tipo de paquete*/
void manejarRespuestaAGameBoy(int socketCliente, int idCliente)
{

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje)
    {

    case tSuscriptor:
    {

        t_suscriptor *nuevaSuscripcion = (t_suscriptor *)buffer;

        //log_debug(logger, "\n\t--GAMEBOY SUSCRIBE TO : %d", nuevaSuscripcion->colaDeMensajes);

        log_debug(logger, "\n\t--SE SUSCRIBIO UN GAMEBOY A LA COLA %d", nuevaSuscripcion->colaDeMensajes);

        enviarInt(socketCliente, 1);
        sleep(1);
        ingresarNuevoSuscriber(nuevaSuscripcion);


        break;
    }

    case tNewPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE NEW --> DE GAMEBOY");

        t_newPokemon *unNewPokemon = (t_newPokemon *)buffer;

        unNewPokemon->identificador = generarNuevoIdMensajeBroker();

        guardarEnMemoriaNewPokemon(unNewPokemon);

        //free(unNewPokemon); SE LIBERA AL FINAL DEL SWITCH EL BUFFER COMPLETO

        enviarInt(socketCliente, unNewPokemon->identificador); //ARREGLO PARA QUE RECIBA EL ID MENSAJE

        break;
    }

    case tGetPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE GET --> DE GAMEBOY");

        t_getPokemon *unGetPokemon = (t_getPokemon *)buffer;

        guardarEnMemoriaGetPokemon(unGetPokemon);

        //free(unGetPokemon);

        enviarInt(socketCliente, unGetPokemon->identificador); //ARREGLO PARA QUE RECIBA EL ID MENSAJE

        break;
    }

    case tCatchPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE CATCH --> DE GAMEBOY");

        t_catchPokemon *unCatchPokemon = (t_catchPokemon *)buffer;

        guardarEnMemoriaCatchPokemon(unCatchPokemon);

        //free(unCatchPokemon);

        enviarInt(socketCliente, unCatchPokemon->identificador); //ARREGLO PARA QUE RECIBA EL ID MENSAJE

        break;
    }

    case tCaughtPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE CAUGHT --> DE GAMEBOY");

        t_caughtPokemon *unCaughtPokemon = (t_caughtPokemon *)buffer; //??* LA ESTRUCTURA ESTA MAL SOLO TIENE QUE SER ID + O/1 unCaughtPokemon->RESULTADO TIENE QUE SER UINT32

        // log_info(logger, "El nombre del Pokemón es: %s", unCaughtPokemon->nombrePokemon);

        //unCaughtPokemon->identificador = generarNuevoIdMensajeBroker();
        
        if(unCaughtPokemon->identificadorCorrelacional == 0){
            unCaughtPokemon->identificadorCorrelacional = generarNuevoIdMensajeBroker();
        }        

        guardarEnMemoriaCaughtPokemon(unCaughtPokemon);

        //free(unCaughtPokemon);

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socketCliente, unCaughtPokemon->identificador); //ARREGLO PARA QUE RECIBA EL ID MENSAJE

        //enviarInt(socketCliente, 1);

        break;
    }

    case tAppearedPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE APPEARED --> DE GAMEBOY");

        t_appearedPokemon *unAppeardPokemon = (t_appearedPokemon *)buffer;

        unAppeardPokemon->identificadorCorrelacional = generarNuevoIdMensajeBroker();

        guardarEnMemoriaAppearedPokemon(unAppeardPokemon);

        //free(unAppeardPokemon);

        enviarInt(socketCliente, unAppeardPokemon->identificador); //ARREGLO PARA QUE RECIBA EL ID MENSAJE

        break;
    }

    default:
    {

        log_error(logger, "\n\t--ERROR PAQUETE GAMEBOY: %d NO IDENTIFICADO", *tipoMensaje);
        abort();
        break;
    }
    }

    free(tipoMensaje);
    free(tamanioMensaje);
    //free(buffer);

    return;
}

/*Determina que hacer dependiendo el tipo de paquete*/
void manejarRespuestaAGameCard(int socketCliente, int idCliente)
{

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje)
    {

    case tSuscriptor:
    {

        t_suscriptor *nuevaSuscripcion = (t_suscriptor *)buffer;

        // log_debug(logger, "\n\t--GAMECARD SUSCRIBE TO NEW ");
        // log_debug(logger, "\n\t--GAMECARD SUSCRIBE TO GET");
        // log_debug(logger, "\n\t--GAMECARD SUSCRIBE TO CATCH");

        // EL GAMECARD SE SUSCRIBE A LAS TRES COLAS GLOBALES

        enviarInt(socketCliente, 1);
        sleep(1);

        nuevaSuscripcion->colaDeMensajes = tNewPokemon;

        ingresarNuevoSuscriber(nuevaSuscripcion);

        log_debug(logger, "\n\t--SE SUSCRIBIO UN GAMECARD A LA COLA DE NEW");

        nuevaSuscripcion->colaDeMensajes = tGetPokemon;

        ingresarNuevoSuscriber(nuevaSuscripcion);

        log_debug(logger, "\n\t--SE SUSCRIBIO UN GAMECARD A LA COLA DE GET");

        nuevaSuscripcion->colaDeMensajes = tCatchPokemon;

        ingresarNuevoSuscriber(nuevaSuscripcion);

        log_debug(logger, "\n\t--SE SUSCRIBIO UN GAMECARD A LA COLA DE CATCH");


        break;
    }

    case tCaughtPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE CAUGHT --> DE GAMECARD");

        t_caughtPokemon *unCaughtPokemon = (t_caughtPokemon *)buffer; //??* LA ESTRUCTURA ESTA MAL SOLO TIENE QUE SER ID + O/1 unCaughtPokemon->RESULTADO TIENE QUE SER UINT32

        // log_info(logger, "El nombre del Pokemón es: %s", unCaughtPokemon->nombrePokemon);

        if (verificarIdCorrelativo(unCaughtPokemon->identificadorCorrelacional))
        {
            unCaughtPokemon->identificador = buscaridMensajeDelCorrelativo(unCaughtPokemon->identificadorCorrelacional);

            if (unCaughtPokemon->identificador == 0)
            {
                //unCaughtPokemon->identificador = generarNuevoIdMensajeBroker();

                guardarEnMemoriaCaughtPokemon(unCaughtPokemon);
            }
        }
        else
        {

            //unCaughtPokemon->identificador = generarNuevoIdMensajeBroker();

            guardarEnMemoriaCaughtPokemon(unCaughtPokemon);
        }

        //free(unCaughtPokemon);

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socketCliente, unCaughtPokemon->identificador);

        //enviarInt(socketCliente, 1);

        break;
    }

    case tLocalizedPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE LOCALIZED --> DE GAMECARD");

        t_localizedPokemon *unLocalizedPokemon = (t_localizedPokemon *)buffer;

        if (verificarIdCorrelativo(unLocalizedPokemon->identificadorCorrelacional))
        {
            unLocalizedPokemon->identificador = buscaridMensajeDelCorrelativo(unLocalizedPokemon->identificadorCorrelacional);

            if (unLocalizedPokemon->identificador == 0)
            {
                //unLocalizedPokemon->identificador = generarNuevoIdMensajeBroker();

                guardarEnMemoriaLocalizedPokemon(unLocalizedPokemon);
            }
        }
        else
        {

            //unLocalizedPokemon->identificador = generarNuevoIdMensajeBroker();

            guardarEnMemoriaLocalizedPokemon(unLocalizedPokemon);
        }

        //free(unLocalizedPokemon);

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socketCliente, unLocalizedPokemon->identificador);

        //enviarInt(socketCliente, 1);

        break;
    }

    case tAppearedPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE APPEARED --> DE GAMECARD");
        enviarInt(socketCliente, 1);

        t_appearedPokemon *unAppeardPokemon = (t_appearedPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unAppeardPokemon->nombrePokemon);
        log_info(logger, "La posicion del Pokémon era: %d %d", unAppeardPokemon->posicionEnElMapaX, unAppeardPokemon->posicionEnElMapaY);
        //log_warning(logger, "IDENTIFICADOR CORRE %d", unAppeardPokemon->identificadorCorrelacional);

        if (verificarIdCorrelativo(unAppeardPokemon->identificadorCorrelacional))
        {
            unAppeardPokemon->identificador = buscaridMensajeDelCorrelativo(unAppeardPokemon->identificadorCorrelacional);

            if (unAppeardPokemon->identificador == 0)
            {
                //unAppeardPokemon->identificador = generarNuevoIdMensajeBroker();

                guardarEnMemoriaAppearedPokemon(unAppeardPokemon);
                //log_error(logger, "Estoy dentro de appeared if");
            }
        }
        else
        {

            //unAppeardPokemon->identificador = generarNuevoIdMensajeBroker();

            guardarEnMemoriaAppearedPokemon(unAppeardPokemon);
            //log_error(logger, "Estoy dentro de appeared else");
        }

        //free(unAppeardPokemon);

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        //enviarInt(socketCliente, unAppeardPokemon->identificador);

        break;
    }

    default:
    {

        log_error(logger, "\n\t--ERROR PAQUETE GAMECAR: %d NO IDENTIFICADO", *tipoMensaje);
        abort();
        break;
    }
    }

    free(tipoMensaje);
    free(tamanioMensaje);
    free(buffer);

    return;
}

/*Se fija si existe idCorr si no Existe lo Agrega*/
bool verificarIdCorrelativo(uint32_t idCorr)
{

    //?* SIEMPRE ENVIAR LOS PAQUETES FLUJO INICIAL CON IDCORRELATIVO EN 0
    if (idCorr != 0)
    {

        pthread_mutex_lock(&mutex_idCorrelativoABuscar);

        idCorrelativoABuscar = idCorr;

        pthread_mutex_lock(&mutex_IDS_CORRELATIVOS);

        bool existeId = list_any_satisfy(IDS_CORRELATIVOS, &existeIdCorrelativo);
        
        pthread_mutex_unlock(&mutex_IDS_CORRELATIVOS);

        if (!existeId)
        {

            uint32_t *nuevoValor = malloc(sizeof(uint32_t));

            *nuevoValor = idCorr;

            pthread_mutex_lock(&mutex_IDS_CORRELATIVOS);

            list_add(IDS_CORRELATIVOS, nuevoValor);
            
            pthread_mutex_unlock(&mutex_IDS_CORRELATIVOS);

            pthread_mutex_unlock(&mutex_idCorrelativoABuscar);

            return false;
        }

        pthread_mutex_unlock(&mutex_idCorrelativoABuscar);
    }
    return true;
}

/*devuelve idMensaje de idCorrelativoAsociado*/
uint32_t buscaridMensajeDelCorrelativo(uint32_t idCorr)
{
    pthread_mutex_lock(&mutex_MENSAJES_LISTA);

    pthread_mutex_lock(&mutex_idCorrelativoABuscarEnMensaje);

    idCorrelativoABuscarEnMensaje = idCorr;

    tMensaje *unMensaje = list_find(MENSAJES_LISTA, &existeMensajeConIdCorrelativo);

    pthread_mutex_unlock(&mutex_idCorrelativoABuscarEnMensaje);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);

    if (unMensaje)
    {
        return unMensaje->idMensaje;
    }

    return 0;
}

/*Determina que hacer dependiendo el tipo de paquete*/
void manejarRespuestaATeam(int socketCliente, int idCliente)
{

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje)
    {

    case tSuscriptor:
    {

        t_suscriptor *nuevaSuscripcion = (t_suscriptor *)buffer;

        //log_info(logger, "Se suscribe a cola: : %d", nuevaSuscripcion->colaDeMensajes);
        //log_info(logger, "Tiempo de Suscripcion: %d", nuevaSuscripcion->tiempoDeSuscripcion);

        enviarInt(socketCliente, 1);
        sleep(1);

        nuevaSuscripcion->colaDeMensajes = tAppearedPokemon;

        ingresarNuevoSuscriber(nuevaSuscripcion);

        log_debug(logger, "\n\t--SE SUSCRIBIO UN TEAM A LA COLA DE APPEARED");

        nuevaSuscripcion->colaDeMensajes = tLocalizedPokemon;

        ingresarNuevoSuscriber(nuevaSuscripcion);

        log_debug(logger, "\n\t--SE SUSCRIBIO UN TEAM A LA COLA DE LOCALIZED");

        nuevaSuscripcion->colaDeMensajes = tCaughtPokemon;

        ingresarNuevoSuscriber(nuevaSuscripcion);

        log_debug(logger, "\n\t--SE SUSCRIBIO UN TEAM A LA COLA DE CAUGHT");

        break;
    }

    case tGetPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE GET --> DE TEAM");

        t_getPokemon *unGetPokemon = (t_getPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unGetPokemon->nombrePokemon);

        //unGetPokemon->identificador = generarNuevoIdMensajeBroker();

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        guardarEnMemoriaGetPokemon(unGetPokemon);

        //free(unGetPokemon);

        enviarInt(socketCliente, unGetPokemon->identificador);

        //enviarInt(socketCliente, 1);

        break;
    }

    case tCatchPokemon:
    {
        log_trace(logger, "\n\t--LLEGO UN NUEVO MENSAJE A LA COLA DE CATCH --> DE TEAM");

        t_catchPokemon *unCatchPokemon = (t_catchPokemon *)buffer;

        //log_info(logger, "El nombre del Pokemón es: %s", unCatchPokemon->nombrePokemon);
        //log_info(logger, "La posicion del Pokémon era: %d %d", unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY);
        //log_info(logger, "El nombre del entrenador es: %s", unCatchPokemon->nombrePokemon);

        //unCatchPokemon->identificador = generarNuevoIdMensajeBroker();

        guardarEnMemoriaCatchPokemon(unCatchPokemon);

        //free(unCatchPokemon);

        //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER

        enviarInt(socketCliente, unCatchPokemon->identificador);

        //enviarInt(socketCliente, 1);

        break;
    }

    default:
    {

        log_error(logger, "\n\t--ERROR PAQUETE TEAM: %d NO IDENTIFICADO", *tipoMensaje);
        abort();
        break;
    }
    }

    free(tipoMensaje);
    free(tamanioMensaje);
    free(buffer);

    return;
}

////////////////////////////////////////FUNCIONES ADMINISTRAR SUSCRIBER////////////////////////////////////////////////

/*Se crea nuevo suscriptor y se lo asocia a una cola */
void ingresarNuevoSuscriber(void *unaNuevaSuscripcion)
{

    t_suscriptor *nuevaSuscripcion = (t_suscriptor *)unaNuevaSuscripcion;
    // LO BUSCO EN LISTA DE SUSCRIPTORES_LISTA

    pthread_mutex_lock(&mutex_ipServerABuscar);
    pthread_mutex_lock(&mutex_PuertoEschuchaABuscar);
    pthread_mutex_lock(&mutex_SUSCRIPTORES_LISTA);

    ipServerABuscar = string_new();
    string_append(&ipServerABuscar, nuevaSuscripcion->ip);
    PuertoEschuchaABuscar = nuevaSuscripcion->puerto;

    t_suscriptor *unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeNuevoSuscriber);

    pthread_mutex_unlock(&mutex_ipServerABuscar);
    pthread_mutex_unlock(&mutex_PuertoEschuchaABuscar);
    pthread_mutex_unlock(&mutex_SUSCRIPTORES_LISTA);

    if (unSuscriptor != NULL)
    {
        // 1 NEW_POKEMON_LISTA 2 APPEARED_POKEMON_LISTA 3 CATCH_POKEMON_LISTA 4 CAUGHT_POKEMON_LISTA 5 GET_POKEMON_LISTA 6 LOCALIZED_POKEMON_LISTA

        switch (nuevaSuscripcion->colaDeMensajes)
        {

        case tNewPokemon:
        {
            // LO BUSCO EN LISTA DE NEW

            log_trace(logger, "\n\t--NUEVO SUSCRIPTOR ID:%d A LA COLA DE NEW", unSuscriptor->identificador);

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;
            pthread_mutex_lock(&mutex_NEW_POKEMON_LISTA);

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(NEW_POKEMON_LISTA, &existeIdSuscriberEnCola);
            
            pthread_mutex_unlock(&mutex_NEW_POKEMON_LISTA);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            /*Si existe no hago nada*/
            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;
                pthread_mutex_lock(&mutex_NEW_POKEMON_LISTA);
                
                list_add(NEW_POKEMON_LISTA, unSuscriptorEnCola);
                
                pthread_mutex_unlock(&mutex_NEW_POKEMON_LISTA);
            }

            break;
        }

        case tAppearedPokemon:
        {
            // LO BUSCO EN LISTA DE APPEARED

            log_trace(logger, "\n\t--NUEVO SUSCRIPTOR ID:%d A LA COLA DE APPEARED", unSuscriptor->identificador);

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            pthread_mutex_lock(&mutex_APPEARED_POKEMON_LISTA);

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(APPEARED_POKEMON_LISTA, &existeIdSuscriberEnCola);

            pthread_mutex_unlock(&mutex_APPEARED_POKEMON_LISTA);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;

                pthread_mutex_lock(&mutex_APPEARED_POKEMON_LISTA);

                list_add(APPEARED_POKEMON_LISTA, unSuscriptorEnCola);
            
                pthread_mutex_unlock(&mutex_APPEARED_POKEMON_LISTA);
            }

            break;
        }
        case tCatchPokemon:
        {
            // LO BUSCO EN LISTA DE CATCH

            log_trace(logger, "\n\t--NUEVO SUSCRIPTOR ID:%d A LA COLA DE CATCH", unSuscriptor->identificador);

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            pthread_mutex_lock(&mutex_CATCH_POKEMON_LISTA);

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(CATCH_POKEMON_LISTA, &existeIdSuscriberEnCola);
            
            pthread_mutex_unlock(&mutex_CATCH_POKEMON_LISTA);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;


                pthread_mutex_lock(&mutex_CATCH_POKEMON_LISTA);

                list_add(CATCH_POKEMON_LISTA, unSuscriptorEnCola);
            
                pthread_mutex_unlock(&mutex_CATCH_POKEMON_LISTA);
            }

            break;
        }

        case tCaughtPokemon:
        {
            // LO BUSCO EN LISTA DE CAUGHT

            log_trace(logger, "\n\t--NUEVO SUSCRIPTOR ID:%d A LA COLA DE CAUGHT", unSuscriptor->identificador);

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            pthread_mutex_lock(&mutex_CAUGHT_POKEMON_LISTA);

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(CAUGHT_POKEMON_LISTA, &existeIdSuscriberEnCola);
            
            pthread_mutex_unlock(&mutex_CAUGHT_POKEMON_LISTA);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;


                pthread_mutex_lock(&mutex_CAUGHT_POKEMON_LISTA);
                
                list_add(CAUGHT_POKEMON_LISTA, unSuscriptorEnCola);
                
                pthread_mutex_unlock(&mutex_CAUGHT_POKEMON_LISTA);
            }

            break;
        }

        case tGetPokemon:
        {
            // LO BUSCO EN LISTA DE GET

            log_trace(logger, "\n\t--NUEVO SUSCRIPTOR ID:%d A LA COLA DE GET", unSuscriptor->identificador);

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            pthread_mutex_lock(&mutex_GET_POKEMON_LISTA);

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(GET_POKEMON_LISTA, &existeIdSuscriberEnCola);
            
            pthread_mutex_unlock(&mutex_GET_POKEMON_LISTA);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;

                pthread_mutex_lock(&mutex_GET_POKEMON_LISTA);
                
                list_add(GET_POKEMON_LISTA, unSuscriptorEnCola);
                
                pthread_mutex_unlock(&mutex_GET_POKEMON_LISTA);
            }

            break;
        }
        case tLocalizedPokemon:
        {
            // LO BUSCO EN LISTA DE LOCALIZED_POKEMON_LISTA

            log_trace(logger, "\n\t--NUEVO SUSCRIPTOR ID:%d A LA COLA DE LOCALIZED", unSuscriptor->identificador);

            pthread_mutex_lock(&mutex_idSuscriberABuscar);

            idSuscriberABuscar = unSuscriptor->identificador;

            pthread_mutex_lock(&mutex_LOCALIZED_POKEMON_LISTA);

            tSuscriptorEnCola *unSuscriptorEnCola = (tSuscriptorEnCola *)list_find(LOCALIZED_POKEMON_LISTA, &existeIdSuscriberEnCola);
            
            pthread_mutex_unlock(&mutex_LOCALIZED_POKEMON_LISTA);

            pthread_mutex_unlock(&mutex_idSuscriberABuscar);

            if (unSuscriptorEnCola == NULL)
            {
                unSuscriptorEnCola = malloc(sizeof(tSuscriptorEnCola));
                unSuscriptorEnCola->identificador = unSuscriptor->identificador;
                unSuscriptorEnCola->startTime = (uint32_t)time(NULL);
                unSuscriptorEnCola->timeToLive = nuevaSuscripcion->tiempoDeSuscripcion;

                pthread_mutex_lock(&mutex_LOCALIZED_POKEMON_LISTA);
                
                list_add(LOCALIZED_POKEMON_LISTA, unSuscriptorEnCola);
                
                pthread_mutex_unlock(&mutex_LOCALIZED_POKEMON_LISTA);
            }

            break;
        }
        default:
        {

            log_error(logger, "\n\t--ERROR SUSCRIPCION A COLA: %d NO IDENTIFICADA", nuevaSuscripcion->colaDeMensajes);
            break;
        }
        }
    }
    else
    {

        log_info(logger, "\n\t--INGRESA SUSCRIPTOR A LA LISTA DE SUSCRIPTORES");

        //AGREGO NUEVO t_suscriptor

        unSuscriptor = malloc(sizeof(t_suscriptor));
        unSuscriptor->identificador = generarNuevoIdSuscriptor();

        unSuscriptor->ip = string_new();
        string_append(&unSuscriptor->ip, nuevaSuscripcion->ip);
        unSuscriptor->puerto = nuevaSuscripcion->puerto;

        //ME HAGO CLIENTE DEL SUSCRIPTOR

        unSuscriptor->identificadorCorrelacional = cliente(unSuscriptor->ip, unSuscriptor->puerto, 5);

        log_warning(logger, "\n\t--NUEVO CLIENTE: %d\n\t--IP: %s\n\t--PUERTO: %d\n\t--SOCKET: %d ", unSuscriptor->identificador, unSuscriptor->ip, unSuscriptor->puerto, unSuscriptor->identificadorCorrelacional);

        /* ?* Nota de editor: en la lista de suscriptores solo va a quedar asociada la primer cola suscripta*/
        pthread_mutex_lock(&mutex_SUSCRIPTORES_LISTA);
        list_add(SUSCRIPTORES_LISTA, unSuscriptor);
        pthread_mutex_unlock(&mutex_SUSCRIPTORES_LISTA);

        // log_info(logger, "\n\nSUSCRIPTOR ............................: \n");
        // log_info(logger, "\nip: %s", unSuscriptor->ip);
        // log_info(logger, "\npuerto: %d", unSuscriptor->puerto);
        // log_info(logger, "\nIdentificador: %d", unSuscriptor->identificador);
        // log_info(logger, "\nIdentificador Correlacional: %d", unSuscriptor->identificadorCorrelacional);
        // log_info(logger, "\nCola de mensajes a suscribirse: %d", unSuscriptor->colaDeMensajes);
        // log_info(logger, "\nTiempo de suscripción: %d", unSuscriptor->tiempoDeSuscripcion);

        ingresarNuevoSuscriber(nuevaSuscripcion);
    }
}

/*En caso de que un suscriptor se desconecte voy a tratar de reconectarme haciendo un ping y determinar si esta vivo y se actualiza el socket = identificadorCorrelacional*/
void reconectarSuscriptor(void *unaNuevaSuscripcion)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;

    if (enviarInt(unSuscriptor->identificadorCorrelacional, PING) < 0)
    {

        log_warning(logger, "\n\t----------Reconectando con Suscriptor-----------------");

        unSuscriptor->identificadorCorrelacional = cliente(unSuscriptor->ip, unSuscriptor->puerto, 5);

        log_warning(logger, "\n\t--Suscriptor reconectado: %d\n\t--IP: %s\n\t--PUERTO: %d\n\t--SOCKET: %d ",
                    unSuscriptor->identificador, unSuscriptor->ip, unSuscriptor->puerto, unSuscriptor->identificadorCorrelacional);
    }
}

void eliminarSuscriptor(uint32_t identificador)
{
    pthread_mutex_lock(&mutex_eliminarSuscriptor);

    t_suscriptor *unSuscriptor;

    pthread_mutex_lock(&mutex_SUSCRIPTORES_LISTA);
    pthread_mutex_lock(&mutex_idSuscriptorABuscar);

    idSuscriptorABuscar = identificador;
    unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

    pthread_mutex_unlock(&mutex_idSuscriptorABuscar);
    pthread_mutex_unlock(&mutex_SUSCRIPTORES_LISTA);

    if (unSuscriptor->puerto != 0)
    {
        unSuscriptor->puerto = 0;

        int resultado = shutdown(unSuscriptor->identificadorCorrelacional, SHUT_RDWR);

        if (resultado >= 0)
        {
            log_info(logger, "El suscriptor de socket %d terminó su tiempo de suscripción. Se terminó correctamente la suscripción.", unSuscriptor->identificadorCorrelacional);
        }
        else
        {
            log_error(logger, "Hubo un error al intentar desconectar al suscriptor de socket %d por término de tiempo de suscripción.", unSuscriptor->identificadorCorrelacional);
        }
    }

    pthread_mutex_unlock(&mutex_eliminarSuscriptor);
}
////////////////////////////////////////HILOS DE COLAS DE MENSAJES////////////////////////////////////////////////

/*Se encarga de Gestionar la Cola New*/
void ejecutarColaNewPokemon()
{

    t_list *mensajesAEnviar;
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;
    t_newPokemon *unNewPokemon;

    while (true)
    {

        if (!list_is_empty(NEW_POKEMON_LISTA))
        {
            pthread_mutex_lock(&mutex_MENSAJES_NEW_POKEMON); //bloquea al momento de eliminar un mensaje de tipo new

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tNewPokemon; // BUSCO DE LA LISTA GLOBAL MENSAJES LOS DE LA COLA NEW

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);//no hace falta bloquear ya esta el mutex_MENSAJES_NEW_POKEMON

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);

                pthread_mutex_lock(&mutex_NEW_POKEMON_LISTA);

                suscriptoresCant = list_size(NEW_POKEMON_LISTA);

                pthread_mutex_unlock(&mutex_NEW_POKEMON_LISTA);

                unNewPokemon = (t_newPokemon *)buscarEnMemoriaNewPokemon(unMensaje);

                for (int i = 0; i < suscriptoresCant; i++)
                {
                    pthread_mutex_lock(&mutex_NEW_POKEMON_LISTA);

                    unSuscriptorCola = list_get(NEW_POKEMON_LISTA, i);
                    
                    pthread_mutex_unlock(&mutex_NEW_POKEMON_LISTA);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola && (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime))) //??*FALTA ELIMINAR SUSCRIPTORES SI EL TIME SE ACABA Buscar si es realmente asi
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck);

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {
                            pthread_mutex_lock(&mutex_SUSCRIPTORES_LISTA);
                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);
                            pthread_mutex_unlock(&mutex_SUSCRIPTORES_LISTA);

                            enviarMensajeNewPokemon(unMensaje, unSuscriptor, unNewPokemon);
                            actualizarLru(unMensaje->idMensaje);
                        }
                    }
                    else
                    {
                        eliminarSuscriptor(unSuscriptorCola->identificador);
                    }
                }

                free(unNewPokemon);
            }
            //SE NECESITA ELIMINAR LA LISTA mensajesAEnviar  
            list_destroy(mensajesAEnviar);
            pthread_mutex_unlock(&mutex_MENSAJES_NEW_POKEMON);
        }

        sleep(2);
    }
}

/*Se encarga de Gestionar la Cola Appeared*/
void ejecutarColaAppearedPokemon()
{

    t_list *mensajesAEnviar;
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;
    t_appearedPokemon *unPokemon;

    while (true)
    {

        if (!list_is_empty(APPEARED_POKEMON_LISTA))
        {
            pthread_mutex_lock(&mutex_MENSAJES_APPEARED_POKEMON); //este mutex cuando se elimina algun mensaje de lista Mensajes

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tAppearedPokemon; // 2 == APPEARED_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);            
            

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);
                pthread_mutex_lock(&mutex_APPEARED_POKEMON_LISTA);

                suscriptoresCant = list_size(APPEARED_POKEMON_LISTA);
                
                pthread_mutex_unlock(&mutex_APPEARED_POKEMON_LISTA);

                unPokemon = (t_appearedPokemon *)buscarEnMemoriaAppearedPokemon(unMensaje);                

                for (int i = 0; i < suscriptoresCant; i++)
                {
                    pthread_mutex_lock(&mutex_APPEARED_POKEMON_LISTA);

                    unSuscriptorCola = list_get(APPEARED_POKEMON_LISTA, i);
                    
                    pthread_mutex_unlock(&mutex_APPEARED_POKEMON_LISTA);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola && (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime)))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {
                            pthread_mutex_lock(&mutex_SUSCRIPTORES_LISTA);

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);
                            
                            pthread_mutex_unlock(&mutex_SUSCRIPTORES_LISTA);

                            enviarMensajeAppearedPokemon(unMensaje, unSuscriptor, unPokemon);
                            actualizarLru(unMensaje->idMensaje);
                        }
                    }
                    else
                    {
                        eliminarSuscriptor(unSuscriptorCola->identificador);
                    }
                }

                free(unPokemon);
            }
            //SE NECESITA ELIMINAR LA LISTA mensajesAEnviar
            list_destroy(mensajesAEnviar);
            pthread_mutex_unlock(&mutex_MENSAJES_APPEARED_POKEMON);
        }

        sleep(2);
    }
}

/*Se encarga de Gestionar la Cola Catch*/
void ejecutarColaCatchPokemon()
{

    t_list *mensajesAEnviar;
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;
    t_catchPokemon *unPokemon;

    while (true)
    {

        if (!list_is_empty(CATCH_POKEMON_LISTA))
        {
            pthread_mutex_lock(&mutex_MENSAJES_CATCH_POKEMON); //BLOQUEA LA LISTA DE MENSAJES AL ELIMINAR PARTICION

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tCatchPokemon; // 3== CATCH_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);

                pthread_mutex_lock(&mutex_CATCH_POKEMON_LISTA);
                
                suscriptoresCant = list_size(CATCH_POKEMON_LISTA);
                
                pthread_mutex_unlock(&mutex_CATCH_POKEMON_LISTA);

                unPokemon = (t_catchPokemon *)buscarEnMemoriaCatchPokemon(unMensaje);               
                

                for (int i = 0; i < suscriptoresCant; i++)
                {

                    pthread_mutex_lock(&mutex_CATCH_POKEMON_LISTA);

                    unSuscriptorCola = list_get(CATCH_POKEMON_LISTA, i); //?* puede llegar a romper en el caso de que se eliminen suscriptores en medio de la ejecucion entre list_size(CATCH_POKEMON_LISTA)y linea actual
                    
                    pthread_mutex_unlock(&mutex_CATCH_POKEMON_LISTA);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola && (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime)))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {
                            pthread_mutex_lock(&mutex_SUSCRIPTORES_LISTA);

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);
                            
                            pthread_mutex_unlock(&mutex_SUSCRIPTORES_LISTA);

                            enviarMensajeCatchPokemon(unMensaje, unSuscriptor, unPokemon);
                            actualizarLru(unMensaje->idMensaje);
                        }
                    }
                    else
                    {
                        eliminarSuscriptor(unSuscriptorCola->identificador);
                    }
                }

                free(unPokemon);
            }
            //SE NECESITA ELIMINAR LA LISTA mensajesAEnviar
            list_destroy(mensajesAEnviar);
            pthread_mutex_unlock(&mutex_MENSAJES_CATCH_POKEMON);
        }

        sleep(2);
    }
}

/*Se encarga de Gestionar la Cola Caught*/
void ejecutarColaCaughtPokemon()
{

    t_list *mensajesAEnviar = list_create();
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;
    t_caughtPokemon *unPokemon;

    while (true)
    {

        if (!list_is_empty(CAUGHT_POKEMON_LISTA))
        {
            pthread_mutex_lock(&mutex_MENSAJES_CAUGHT_POKEMON); //este mutex cuando se elimina algun mensaje de MENSAJES_LISTA

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tCaughtPokemon; // 4= CAUGHT_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);

                pthread_mutex_lock(&mutex_CAUGHT_POKEMON_LISTA);
                
                suscriptoresCant = list_size(CAUGHT_POKEMON_LISTA);
                
                pthread_mutex_unlock(&mutex_CAUGHT_POKEMON_LISTA);

                unPokemon = (t_caughtPokemon *)buscarEnMemoriaCaughtPokemon(unMensaje);

                //CHEQUEAR CON RODRI EL FUNCIONAMIENTO: SE AGREGA IF NULL

                if (unPokemon)
                {

                    // log_debug(logger, "---------------------SE ENCONTRÓ EN CAUGHT EL POKEMON SOLICITADO POR IDMENSAJE------------------------------");

                    for (int i = 0; i < suscriptoresCant; i++)
                    {

                        pthread_mutex_lock(&mutex_CAUGHT_POKEMON_LISTA);

                        unSuscriptorCola = list_get(CAUGHT_POKEMON_LISTA, i);
                        
                        pthread_mutex_unlock(&mutex_CAUGHT_POKEMON_LISTA);

                        tiempoActual = (uint32_t)time(NULL);

                        if (unSuscriptorCola && (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime)))
                        {
                            pthread_mutex_lock(&mutex_ackABuscar);

                            ackABuscar = unSuscriptorCola->identificador;

                            anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                            pthread_mutex_unlock(&mutex_ackABuscar);

                            if (!anyAck)
                            {
                                pthread_mutex_lock(&mutex_SUSCRIPTORES_LISTA);

                                pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                                idSuscriptorABuscar = unSuscriptorCola->identificador;
                                unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                                pthread_mutex_unlock(&mutex_idSuscriptorABuscar);
                                
                                pthread_mutex_unlock(&mutex_SUSCRIPTORES_LISTA);

                                enviarMensajeCaughtPokemon(unMensaje, unSuscriptor, unPokemon);
                                actualizarLru(unMensaje->idMensaje);
                            }
                        }
                        else
                        {
                            eliminarSuscriptor(unSuscriptorCola->identificador);
                        }
                    }

                    free(unPokemon);
                }
                else
                {

                    log_debug(logger, "----------------------------NO SE ENCONTRÓ EL POKEMON BUSCADO EN MEMORIA-----------------------------");
                }

                
            }
            //SE NECESITA ELIMINAR LA LISTA mensajesAEnviar
            list_destroy(mensajesAEnviar);
            pthread_mutex_unlock(&mutex_MENSAJES_CAUGHT_POKEMON);
        }

        sleep(2);
    }
}

/*Se encarga de Gestionar la Cola Get*/
void ejecutarColaGetPokemon()
{

    t_list *mensajesAEnviar;
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;
    t_getPokemon *unPokemon;

    while (true)
    {

        if (!list_is_empty(GET_POKEMON_LISTA))
        {
            pthread_mutex_lock(&mutex_MENSAJES_GET_POKEMON); //este mutex cuando se elimina algun mensaje de MENSAJES_LISTA

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tGetPokemon; // 5 == GET_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);

                pthread_mutex_lock(&mutex_GET_POKEMON_LISTA);
                
                suscriptoresCant = list_size(GET_POKEMON_LISTA);
                
                pthread_mutex_unlock(&mutex_GET_POKEMON_LISTA);
                
                unPokemon = (t_getPokemon *)buscarEnMemoriaGetPokemon(unMensaje);

                for (int i = 0; i < suscriptoresCant; i++)
                {

                    pthread_mutex_lock(&mutex_GET_POKEMON_LISTA);

                    unSuscriptorCola = list_get(GET_POKEMON_LISTA, i);
                    
                    pthread_mutex_unlock(&mutex_GET_POKEMON_LISTA);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola && (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime)))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {
                            pthread_mutex_lock(&mutex_SUSCRIPTORES_LISTA);

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);
                            
                            pthread_mutex_unlock(&mutex_SUSCRIPTORES_LISTA);

                            enviarMensajeGetPokemon(unMensaje, unSuscriptor, unPokemon);
                            actualizarLru(unMensaje->idMensaje);
                        }
                    }
                    else
                    {
                        eliminarSuscriptor(unSuscriptorCola->identificador);
                    }
                }

                free(unPokemon);
            }
            //SE NECESITA ELIMINAR LA LISTA mensajesAEnviar
            list_destroy(mensajesAEnviar);
            pthread_mutex_unlock(&mutex_MENSAJES_GET_POKEMON);
        }

        sleep(2);
    }
}

/*Se encarga de Gestionar la Cola Localized*/
void ejecutarColaLocalizedPokemon()
{

    t_list *mensajesAEnviar;
    tMensaje *unMensaje;
    int suscriptoresCant;
    tSuscriptorEnCola *unSuscriptorCola;
    bool anyAck = false;
    t_suscriptor *unSuscriptor;
    uint32_t tiempoActual;
    t_localizedPokemon *unPokemon;

    while (true)
    {

        if (!list_is_empty(LOCALIZED_POKEMON_LISTA))
        {
            pthread_mutex_lock(&mutex_MENSAJES_LOCALIZED_POKEMON); //este mutex cuando se elimina algun mensaje de lista Mensajes

            pthread_mutex_lock(&mutex_tipoMensajeABuscar);

            tipoMensajeABuscar = tLocalizedPokemon; // 6== LOCALIZED_POKEMON_LISTA

            mensajesAEnviar = list_filter(MENSAJES_LISTA, &existeTipoMensaje);

            pthread_mutex_unlock(&mutex_tipoMensajeABuscar);

            while (!list_is_empty(mensajesAEnviar))
            {

                unMensaje = list_remove(mensajesAEnviar, 0);

                pthread_mutex_lock(&mutex_LOCALIZED_POKEMON_LISTA);
                
                suscriptoresCant = list_size(LOCALIZED_POKEMON_LISTA);
                
                pthread_mutex_unlock(&mutex_LOCALIZED_POKEMON_LISTA);

                unPokemon = (t_localizedPokemon *)buscarEnMemoriaLocalizedPokemon(unMensaje);

                for (int i = 0; i < suscriptoresCant; i++)
                {
                    pthread_mutex_lock(&mutex_LOCALIZED_POKEMON_LISTA);

                    unSuscriptorCola = list_get(LOCALIZED_POKEMON_LISTA, i);
                    
                    pthread_mutex_unlock(&mutex_LOCALIZED_POKEMON_LISTA);

                    tiempoActual = (uint32_t)time(NULL);

                    if (unSuscriptorCola && (unSuscriptorCola->timeToLive == 0 || unSuscriptorCola->timeToLive >= (tiempoActual - unSuscriptorCola->startTime)))
                    {
                        pthread_mutex_lock(&mutex_ackABuscar);

                        ackABuscar = unSuscriptorCola->identificador;

                        anyAck = list_any_satisfy(unMensaje->acknowledgement, &existeAck); //ANTES DE AGREGAR EL MENSAJE a MENSAJES_LISTA CREAR LISTAS ACK Y ENVIADOS

                        pthread_mutex_unlock(&mutex_ackABuscar);

                        if (!anyAck)
                        {
                            pthread_mutex_lock(&mutex_SUSCRIPTORES_LISTA);

                            pthread_mutex_lock(&mutex_idSuscriptorABuscar);

                            idSuscriptorABuscar = unSuscriptorCola->identificador;
                            unSuscriptor = (t_suscriptor *)list_find(SUSCRIPTORES_LISTA, &existeIdSuscriptor);

                            pthread_mutex_unlock(&mutex_idSuscriptorABuscar);
                            
                            pthread_mutex_unlock(&mutex_SUSCRIPTORES_LISTA);

                            enviarMensajeLocalizedPokemon(unMensaje, unSuscriptor, unPokemon);
                            actualizarLru(unMensaje->idMensaje);
                        }
                    }
                    else
                    {
                        eliminarSuscriptor(unSuscriptorCola->identificador);
                    }
                }

                free(unPokemon);
            }
            //SE NECESITA ELIMINAR LA LISTA mensajesAEnviar
            list_destroy(mensajesAEnviar);
            pthread_mutex_unlock(&mutex_MENSAJES_LOCALIZED_POKEMON);
        }

        sleep(2);
    }
}

////////////////////////////////////////FUNCIONES ENVIAR MENSAJES A SUSCRIBER////////////////////////////////////////////////

/*Se encarga de mandar un NewPokemon a un suscriptor*/
void enviarMensajeNewPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion, void *unPokemon)
{
    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;
    t_newPokemon *unNewPokemon = (t_newPokemon *)unPokemon;

    int tamanioNewPokemon = 0;

    //log_warning(logger, "\n\t-->  socket Suscriptor valor %d", unSuscriptor->identificadorCorrelacional);

    enviarInt(unSuscriptor->identificadorCorrelacional, 1);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tNewPokemon, unNewPokemon, tamanioNewPokemon);

    log_trace(logger, "\n\t--> SE ENVIO EL MENSAJE %d DE LA COLA NEW AL SUSCRIPTOR %d", unMensaje->idMensaje, unSuscriptor->identificador);

    //LO AGREGO A LA COLA DE ENVIADOS

    bool anyAck = false;

    pthread_mutex_lock(&mutex_ackABuscar);

    ackABuscar = unSuscriptor->identificador;

    anyAck = list_any_satisfy(unMensaje->suscriptoresEnviados, &existeAck);

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

            log_trace(logger,  "\n\t-- RECIBI ACK DE NEWPOKEMON --> DE BROKER");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {
            log_trace(logger,  "\n\t-- RECIBI ACK DE NEWPOKEMON -->DE GAMEBOY");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 3)
        {
            log_trace(logger,  "\n\t-- RECIBI ACK DE NEWPOKEMON -->DE GAMECARD");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 4)
        {
            log_trace(logger,  "\n\t-- RECIBI ACK DE NEWPOKEMON -->DE TEAM");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
    }
    else
    {
        log_error(logger, "ERROR enviarMensaje-NewPokemon no recibi nada server apagado ");
        reconectarSuscriptor(unSuscriptor);
    }
}

/*Se encarga de mandar un AppearedPokemon a un suscriptor*/
void enviarMensajeAppearedPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion, void *unPokemon)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;
    t_appearedPokemon *unAppearedPokemon = (t_appearedPokemon *)unPokemon;

    int tamanioPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 1);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tAppearedPokemon, unAppearedPokemon, tamanioPokemon);

    log_trace(logger, "\n\t--> SE ENVIO EL MENSAJE %d DE LA COLA APPEARED AL SUSCRIPTOR %d", unMensaje->idMensaje, unSuscriptor->identificador);

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

            log_trace(logger,  "\n\t-- RECIBI ACK DE APPEAREDPOKEMON -->DE BROKER");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {
            log_trace(logger,  "\n\t-- RECIBI ACK DE APPEAREDPOKEMON -->DE GAMEBOY");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 3)
        {
            log_trace(logger,  "\n\t-- RECIBI ACK DE APPEAREDPOKEMON -->DE GAMECARD");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 4)
        {
            log_trace(logger,  "\n\t-- RECIBI ACK DE APPEAREDPOKEMON -->DE TEAM");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
    }
    else
    {

        log_error(logger, "\n\tNO FUE POSIBLE ENVIAR APPEARED RECONECTAR SUSCRIPTOR.."); //??* en este caso agregar en cola enviados y volvera a intentar en proxima ejecutarColaNewPokemon()
        reconectarSuscriptor(unSuscriptor);
    }
    
}

/*Se encarga de mandar un CatchPokemon a un suscriptor*/
void enviarMensajeCatchPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion, void *unPokemon)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;
    t_catchPokemon *unCatchPokemon = (t_catchPokemon *)unPokemon;

    int tamanioPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 1);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tCatchPokemon, unCatchPokemon, tamanioPokemon);

    log_trace(logger, "\n\t--> SE ENVIO EL MENSAJE %d DE LA COLA CATCH AL SUSCRIPTOR %d", unMensaje->idMensaje, unSuscriptor->identificador);

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

            log_trace(logger, "\n\t-- RECIBI ACK DE CATCHPOKEMON -->DE BROKER");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE CATCHPOKEMON -->DE GAMEBOY");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 3)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE CATCHPOKEMON -->DE GAMECARD");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 4)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE CATCHPOKEMON -->DE TEAM");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
    }
    else
    {
        log_error(logger, "ERROR enviarMensajecatch() no recibi nada server apagado ");
        reconectarSuscriptor(unSuscriptor);
    }

    
}

/*Se encarga de mandar un CaughtPokemon a un suscriptor*/
void enviarMensajeCaughtPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion, void *unPokemon)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;
    t_caughtPokemon *unCaughtPokemon = (t_caughtPokemon *)unPokemon;

    int tamanioPokemon = 0;

    //log_debug(logger, "\n\tENVIAMOS A SUSCRIPTOR %d EL CAUGHT POKEMON CON IDMENSAJE: %d", unSuscriptor->identificador, unCaughtPokemon->identificador);
    //log_debug(logger, "\n\tENVIAMOS A SUSCRIPTOR EL CAUGHT POKEMON CON IDMENSAJECORRELACIONAL: %d", unCaughtPokemon->identificadorCorrelacional);
    //log_debug(logger, "\n\tENVIAMOS A SUSCRIPTOR EL CAUGHT POKEMON CON RESULTADO: %d", unCaughtPokemon->resultado);

    enviarInt(unSuscriptor->identificadorCorrelacional, 1);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tCaughtPokemon, unCaughtPokemon, tamanioPokemon);

    log_trace(logger, "\n\t--> SE ENVIO EL MENSAJE %d DE LA COLA CAUGHT AL SUSCRIPTOR %d", unMensaje->idMensaje, unSuscriptor->identificador);

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

            log_trace(logger, "\n\t-- RECIBI ACK DE CAUGHTPOKEMON -->DE BROKER");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE CAUGHTPOKEMON -->DE GAMEBOY");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 3)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE CAUGHTPOKEMON -->DE GAMECARD");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 4)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE CAUGHTPOKEMON -->DE TEAM");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
    }
    else
    {

        log_error(logger, "ERROR enviarMensaje-CAUGHT no recibi nada server apagado "); //??* en este caso agregar en cola enviados y volvera a intentar en proxima ejecutarColaNewPokemon()
    }

    
}

/*Se encarga de mandar un GetPokemon a un suscriptor*/
void enviarMensajeGetPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion, void *unPokemon)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;
    t_getPokemon *unGetPokemon = (t_getPokemon *)unPokemon;

    int tamanioPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 1);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tGetPokemon, unGetPokemon, tamanioPokemon);

    log_trace(logger, "\n\t--> SE ENVIO EL MENSAJE %d DE LA COLA GET AL SUSCRIPTOR %d", unMensaje->idMensaje, unSuscriptor->identificador);

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

            log_trace(logger, "\n\t-- RECIBI ACK DE GETPOKEMON -->DE BROKER");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE GETPOKEMON -->DE GAMEBOY");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 3)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE GETPOKEMON -->DE GAMECARD");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 4)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE GETPOKEMON -->DE TEAM");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
    }
    else
    {

        log_error(logger, "ERROR enviarMensajeGET() no recibi nada server apagado ");
    }

   
}

/*Se encarga de mandar un LocalizedPokemon a un suscriptor*/
void enviarMensajeLocalizedPokemon(tMensaje *unMensaje, void *unaNuevaSuscripcion, void *unPokemon)
{

    t_suscriptor *unSuscriptor = (t_suscriptor *)unaNuevaSuscripcion;
    t_localizedPokemon *unLocalizedPokemon = (t_localizedPokemon *)unPokemon;

    int tamanioPokemon = 0;

    enviarInt(unSuscriptor->identificadorCorrelacional, 1);
    enviarPaquete(unSuscriptor->identificadorCorrelacional, tLocalizedPokemon, unLocalizedPokemon, tamanioPokemon);

    log_trace(logger, "\n\t--> SE ENVIO EL MENSAJE %d DE LA COLA LOCALIZED AL SUSCRIPTOR %d", unMensaje->idMensaje, unSuscriptor->identificador);

    bool anyAck = false;

    pthread_mutex_lock(&mutex_ackABuscar);

    ackABuscar = unSuscriptor->identificador;

    anyAck = list_any_satisfy(unMensaje->suscriptoresEnviados, &existeAck);

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

            log_trace(logger, "\n\t-- RECIBI ACK DE LOCALIZED -->DE BROKER");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 2)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE LOCALIZED -->DE GAMEBOY");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 3)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE LOCALIZED -->DE GAMECARD");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
        else if (tipoResultado == 4)
        {
            log_trace(logger, "\n\t-- RECIBI ACK DE LOCALIZED -->DE TEAM");
            list_add(unMensaje->acknowledgement, unSuscriptor->identificador);
        }
    }
    else
    {
        log_error(logger, "ERROR enviarMensajeunLocalizedPokemon() no recibi nada server apagado ");
        reconectarSuscriptor(unSuscriptor);
    }

    
}

////////////////////////////////////////LEVANTAR POKEMON DE MEMORIA////////////////////////////////////////////////

/*Crea Estructura NewPokemon, la copia de memoria y Actualiza LRU de la particion*/
void *buscarEnMemoriaNewPokemon(tMensaje *unMensaje)
{

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

    // pthread_mutex_lock(&mutex_idMensajeABuscar);

    // idMensajeABuscar = unMensaje->idMensaje;

    // tParticion *unaParti = list_find(METADATA_MEMORIA, &existeIdMensajeEnParticion);

    // pthread_mutex_unlock(&mutex_idMensajeABuscar);

    // if (unaParti)
    // {
    //     unaParti->lru = nuevoTimeStamp();
    // }

    return unNewPokemon;
}

/*Crea Estructura AppearedPokemon, la copia de memoria y Actualiza LRU de la particion*/
void *buscarEnMemoriaAppearedPokemon(tMensaje *unMensaje)
{

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

    // pthread_mutex_lock(&mutex_idMensajeABuscar);

    // idMensajeABuscar = unMensaje->idMensaje;

    // tParticion *unaParti = list_find(METADATA_MEMORIA, &existeIdMensajeEnParticion);

    // pthread_mutex_unlock(&mutex_idMensajeABuscar);

    // if (unaParti)
    // {
    //     unaParti->lru = nuevoTimeStamp();
    // }

    return unAppearedPokemon;
}

/*Crea Estructura CatchPokemon, la copia de memoria y Actualiza LRU de la particion*/
void *buscarEnMemoriaCatchPokemon(tMensaje *unMensaje)
{

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

    // pthread_mutex_lock(&mutex_idMensajeABuscar);

    // idMensajeABuscar = unMensaje->idMensaje;

    // tParticion *unaParti = list_find(METADATA_MEMORIA, &existeIdMensajeEnParticion);

    // pthread_mutex_unlock(&mutex_idMensajeABuscar);

    // if (unaParti)
    // {
    //     unaParti->lru = nuevoTimeStamp();
    // }

    return unCatchPokemon;
}

/*Crea Estructura CaughtPokemon, la copia de memoria y Actualiza LRU de la particion*/
void *buscarEnMemoriaCaughtPokemon(tMensaje *unMensaje)
{
    uint32_t desplazamiento = 0;
    uint32_t tamanioNombrePokemon = 0;
    uint32_t tamanioBuffer = 0;

    t_caughtPokemon *unCaughtPokemon = malloc(sizeof(t_caughtPokemon));

    unCaughtPokemon->identificador = unMensaje->idMensaje;
    unCaughtPokemon->identificadorCorrelacional = unMensaje->idMensajeCorrelacional;

    memcpy(&unCaughtPokemon->resultado, unMensaje->posicionEnMemoria + desplazamiento, sizeof(uint32_t));

    // pthread_mutex_lock(&mutex_idMensajeABuscar);

    // idMensajeABuscar = unMensaje->idMensaje;

    // tParticion *unaParti = list_find(METADATA_MEMORIA, &existeIdMensajeEnParticion);

    // pthread_mutex_unlock(&mutex_idMensajeABuscar);

    // if (unaParti)
    // {
    //     unaParti->lru = nuevoTimeStamp();
    // }

    return unCaughtPokemon;
}

/*Crea Estructura GetPokemon, la copia de memoria y Actualiza LRU de la particion*/
void *buscarEnMemoriaGetPokemon(tMensaje *unMensaje)
{

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

    // pthread_mutex_lock(&mutex_idMensajeABuscar);

    // idMensajeABuscar = unMensaje->idMensaje;

    // tParticion *unaParti = list_find(METADATA_MEMORIA, &existeIdMensajeEnParticion);

    // pthread_mutex_unlock(&mutex_idMensajeABuscar);

    // if (unaParti)
    // {
    //     unaParti->lru = nuevoTimeStamp();
    // }

    return unGetPokemon;
}

/*Crea Estructura LocalizedPokemon, la copia de memoria y Actualiza LRU de la particion*/
void *buscarEnMemoriaLocalizedPokemon(tMensaje *unMensaje)
{

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

    // pthread_mutex_lock(&mutex_idMensajeABuscar);

    // idMensajeABuscar = unMensaje->idMensaje;

    // tParticion *unaParti = list_find(METADATA_MEMORIA, &existeIdMensajeEnParticion);

    // pthread_mutex_unlock(&mutex_idMensajeABuscar);

    // if (unaParti)
    // {
    //     unaParti->lru = nuevoTimeStamp();
    // }

    return unLocalizedPokemon;
}

void actualizarLru(uint32_t elIdMensaje){

    //log_warning(logger,"actualizo lru de IdMensaje %d",elIdMensaje);
    pthread_mutex_lock(&mutex_METADATA_MEMORIA);

    pthread_mutex_lock(&mutex_idMensajeABuscar);

    idMensajeABuscar = elIdMensaje;

    tParticion *unaParti = list_find(METADATA_MEMORIA, &existeIdMensajeEnParticion);

    pthread_mutex_unlock(&mutex_idMensajeABuscar);

    if (unaParti)
    {
        unaParti->lru = nuevoTimeStamp();
        //log_warning(logger,"LRU: Existe parti lru de %d",elIdMensaje);
    }
    
    pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

}

////////////////////////////////////////GUARDAR POKEMON EN MEMORIA////////////////////////////////////////////////

/*Recibe un NewPokemon,lo guarda en memoria, crea la estructura Administrativa tMensaje y lo manda a la lista*/
void guardarEnMemoriaNewPokemon(void *unPokemon)
{

    t_newPokemon *unNewPokemon = (t_newPokemon *)unPokemon;

    //log_debug(logger, "\n\t--GAMEBOY PUBLISH -> NEWPOKEMON: %s", unNewPokemon->nombrePokemon);
    //log_info(logger, "La posicion del Pokémon es: %d %d", unNewPokemon->posicionEnElMapaX, unNewPokemon->posicionEnElMapaY);

    //?*SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER -> NO EN NEWPOKEMON DE GAMEBOY

    //enviarInt(socketCliente, unNewPokemon->identificador);

    uint32_t desplazamiento = 0;
    uint32_t tamanioNombrePokemon = string_length(unNewPokemon->nombrePokemon);
    uint32_t tamanio = 0;

    tamanio = tamanioNombrePokemon + 4 * sizeof(uint32_t);

    tMensaje *unMensaje = malloc(sizeof(tMensaje));

    unMensaje->acknowledgement = list_create();
    unMensaje->suscriptoresEnviados = list_create();
    unMensaje->idMensaje = generarNuevoIdMensajeBroker();
    unMensaje->idMensajeCorrelacional = 0;
    unMensaje->tipoMensaje = tNewPokemon; //NEW
    //getDireccionMemoriaLibreMejorado(unMensaje->idMensaje,unMensaje->idMensajeCorrelacional,&unMensaje->posicionEnMemoria, tamanio);
    unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(unMensaje->idMensaje, tamanio);

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
    

    log_trace(logger, "\n\t--SE ALMACENA UN MENSAJE NEW CON ID %d EN LA POSICION %d ", unMensaje->idMensaje, (unMensaje->posicionEnMemoria - MEMORIA_PRINCIPAL));

    pthread_mutex_lock(&mutex_MENSAJES_LISTA);
    
    list_add(MENSAJES_LISTA, unMensaje);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);
}

/*Recibe un AppearedPokemon,lo guarda en memoria, crea la estructura Administrativa tMensaje y lo manda a la lista*/
void guardarEnMemoriaAppearedPokemon(void *unPokemon)
{

    t_appearedPokemon *unAppeardPokemon = (t_appearedPokemon *)unPokemon;

    //log_info(logger, "El nombre del Pokemón es: %s", unAppeardPokemon->nombrePokemon);
    //log_info(logger, "La posicion del Pokémon era: %d %d", unAppeardPokemon->posicionEnElMapaX, unAppeardPokemon->posicionEnElMapaY);

    if (unAppeardPokemon->identificador == 0)
    {
        unAppeardPokemon->identificador = generarNuevoIdMensajeBroker();
    }

    //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER-> NO EN GAMEBOY

    //enviarInt(socketCliente, unAppeardPokemon->identificador);

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
    unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(unMensaje->idMensaje, tamanio);

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, unAppeardPokemon->nombrePokemon, tamanioNombrePokemon);
    desplazamiento += tamanioNombrePokemon;

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unAppeardPokemon->posicionEnElMapaX, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unAppeardPokemon->posicionEnElMapaY, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    log_trace(logger, "\n\t--SE ALMACENA UN MENSAJE APPEARED CON ID %d EN LA POSICION %d ", unMensaje->idMensaje, (unMensaje->posicionEnMemoria - MEMORIA_PRINCIPAL));

    pthread_mutex_lock(&mutex_MENSAJES_LISTA);
    
    list_add(MENSAJES_LISTA, unMensaje);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);
}

/*Recibe un CatchPokemon,lo guarda en memoria, crea la estructura Administrativa tMensaje y lo manda a la lista*/
void guardarEnMemoriaCatchPokemon(void *unPokemon)
{

    t_catchPokemon *unCatchPokemon = (t_catchPokemon *)unPokemon;

    //log_info(logger, "El nombre del Pokemón es: %s", unCatchPokemon->nombrePokemon);
    //log_info(logger, "La posicion del Pokémon era: %d %d", unCatchPokemon->posicionEnElMapaX, unCatchPokemon->posicionEnElMapaY);
    //log_info(logger, "El nombre del entrenador es: %s", unCatchPokemon->nombrePokemon);

    unCatchPokemon->identificador = generarNuevoIdMensajeBroker();

    //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER -> NO EN GAMEBOY

    // enviarInt(socketCliente, unCatchPokemon->identificador);

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
    unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(unMensaje->idMensaje, tamanio);

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, unCatchPokemon->nombrePokemon, tamanioNombrePokemon);
    desplazamiento += tamanioNombrePokemon;

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unCatchPokemon->posicionEnElMapaX, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unCatchPokemon->posicionEnElMapaY, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    log_trace(logger, "\n\t--SE ALMACENA UN MENSAJE CATCH CON ID %d EN LA POSICION %d ", unMensaje->idMensaje, (unMensaje->posicionEnMemoria - MEMORIA_PRINCIPAL));
    
    pthread_mutex_lock(&mutex_MENSAJES_LISTA);
    
    list_add(MENSAJES_LISTA, unMensaje);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);
}

/*Recibe un CaughtPokemon,lo guarda en memoria, crea la estructura Administrativa tMensaje y lo manda a la lista*/
void guardarEnMemoriaCaughtPokemon(void *unPokemon)
{

    t_caughtPokemon *unCaughtPokemon = (t_caughtPokemon *)unPokemon;

    unCaughtPokemon->identificador = generarNuevoIdMensajeBroker();
    //uint32_t tamanioNombrePokemon = string_length(unCaughtPokemon->nombrePokemon);
    uint32_t desplazamiento = 0;
    uint32_t tamanio = 0;

    tamanio = sizeof(uint32_t);

    tMensaje *unMensaje = malloc(sizeof(tMensaje));

    unMensaje->acknowledgement = list_create();
    unMensaje->suscriptoresEnviados = list_create();
    unMensaje->idMensaje = unCaughtPokemon->identificador;
    unMensaje->idMensajeCorrelacional = unCaughtPokemon->identificadorCorrelacional;
    unMensaje->tipoMensaje = tCaughtPokemon; //CAUGHT
    unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(unMensaje->idMensaje, tamanio);

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, &unCaughtPokemon->resultado, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    log_trace(logger, "\n\t--SE ALMACENA UN MENSAJE CAUGHT CON ID %d EN LA POSICION %d ", unMensaje->idMensaje, (unMensaje->posicionEnMemoria - MEMORIA_PRINCIPAL));

    pthread_mutex_lock(&mutex_MENSAJES_LISTA);
    
    list_add(MENSAJES_LISTA, unMensaje);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);
}

/*Recibe un GetPokemon,lo guarda en memoria, crea la estructura Administrativa tMensaje y lo manda a la lista*/
void guardarEnMemoriaGetPokemon(void *unPokemon)
{

    t_getPokemon *unGetPokemon = (t_getPokemon *)unPokemon;

    //log_debug(logger, "\n\t--GAMEBOY PUBLISH -> El nombre del Pokemón es: .%s.", unGetPokemon->nombrePokemon);

    unGetPokemon->identificador = generarNuevoIdMensajeBroker();

    //SE LE ENVIA EL IDENTIFICADOR AL PUBLISHER -> NO EN GAMEBOY //MODIFICADO SE LE ENVIA A TODOS

    // enviarInt(socketCliente, unGetPokemon->identificador);

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
    unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(unMensaje->idMensaje, tamanio);
    //log_trace(logger,"TAMANIO tamanioNombrePokemon: %d, uint32: %d",tamanioNombrePokemon,sizeof(uint32_t));

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, &tamanioNombrePokemon, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(unMensaje->posicionEnMemoria + desplazamiento, unGetPokemon->nombrePokemon, tamanioNombrePokemon);
    desplazamiento += tamanioNombrePokemon;

    log_trace(logger, "\n\t--SE ALMACENA UN MENSAJE GET CON ID %d EN LA POSICION %d ", unMensaje->idMensaje, (unMensaje->posicionEnMemoria - MEMORIA_PRINCIPAL));

    pthread_mutex_lock(&mutex_MENSAJES_LISTA);
    
    list_add(MENSAJES_LISTA, unMensaje);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);
}

/*Recibe un LocalizedPokemon,lo guarda en memoria, crea la estructura Administrativa tMensaje y lo manda a la lista*/
void guardarEnMemoriaLocalizedPokemon(void *unPokemon)
{

    t_localizedPokemon *unLocalizedPokemon = (t_localizedPokemon *)unPokemon;

    //log_info(logger, "El nombre del Pokemón es: %s", unLocalizedPokemon->nombrePokemon);

    int cantidadListaDatosPokemon = list_size(unLocalizedPokemon->listaDatosPokemon);

    int contadorito = 0;

    /*printf(C_VERDE"\n\nLOCALIZED_POKEMON DESERIALIZADO: \n");
    printf("\nIdentificador: %d", unLocalizedPokemon->identificador);
    printf("\nIdentificador Correlacional: %d", unLocalizedPokemon->identificadorCorrelacional);
    printf("\nNombre del Pokemón: %s", unLocalizedPokemon->nombrePokemon);
    */
    datosPokemon *nodoDatosPokemon;

    while (contadorito < cantidadListaDatosPokemon)
    {

        nodoDatosPokemon = list_get(unLocalizedPokemon->listaDatosPokemon, contadorito);

        //printf("\nCantidad de pokemón en %d° ubicación: %d", contadorito, nodoDatosPokemon->cantidad);
        //printf("\nUbicacion en 'x': %d", nodoDatosPokemon->posicionEnElMapaX);
        //printf("\nUbicacion en 'y': %d\n", nodoDatosPokemon->posicionEnElMapaY);

        contadorito += 1;
    }

    //printf(C_RESET"");

    //HAsta aca

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
    unMensaje->posicionEnMemoria = getDireccionMemoriaLibre(unMensaje->idMensaje, tamanio);

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

    log_trace(logger, "\n\t--SE ALMACENA UN MENSAJE LOCALIZED CON ID %d EN LA POSICION %d ", unMensaje->idMensaje, (unMensaje->posicionEnMemoria - MEMORIA_PRINCIPAL));

    //??* falta eliminar unLocalizedPokemon
    pthread_mutex_lock(&mutex_MENSAJES_LISTA);
    
    list_add(MENSAJES_LISTA, unMensaje);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);
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

bool existeIdMensaje(void *mensaje)
{

    tMensaje *p = (tMensaje *)mensaje;

    bool existe = false;

    if ((p->idMensaje == idMensajeABuscar))
    {
        existe = true;
    }

    return existe;
}

bool existeMensajeConIdCorrelativo(void *mensaje)
{

    tMensaje *p = (tMensaje *)mensaje;

    bool existe = false;

    if ((p->idMensajeCorrelacional == idCorrelativoABuscarEnMensaje))
    {
        existe = true;
    }

    return existe;
}

bool existeIdMensajeEnParticion(void *parti)
{

    tParticion *p = (tParticion *)parti;

    bool existe = false;

    if ((p->idMensaje == idMensajeABuscar))
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

bool existeParticionLibre(void *particion)
{

    tParticion *p = (tParticion *)particion;

    bool existe = false;

    if ((p->free == true) && (p->tamanio >= tamanioParticionABuscar))
    {
        existe = true;
    }

    return existe;
}

bool sortParticionMenor(tParticion *p, tParticion *q)
{
    return p->tamanio < q->tamanio;
}

bool sortPidMenor(tParticion *p, tParticion *q)
{
    return p->idParticion < q->idParticion;
}

bool sortTimeMenor(tParticion *p, tParticion *q)
{
    return p->lru < q->lru;
}

bool sortInitMenor(tParticion *p, tParticion *q)
{
    return p->timeInit < q->timeInit;
}

bool esParticionLibre(void *unaParticion)
{

    tParticion *p = (tParticion *)unaParticion;
    return p->free == true;
}

bool esParticionOcupada(void *unaParticion)
{

    tParticion *p = (tParticion *)unaParticion;
    return p->free == false;
}

bool esParticionOcupadaConMensaje(void *particion)
{

    tParticion *p = (tParticion *)particion;

    bool existe = false;

    if ((p->free == false) && (p->idMensaje != 0))
    {
        existe = true;
    }

    return existe;
}

tParticion *buscarParticionLibreEnMemoria(uint32_t tamanio)
{
    //YA ESTA INCLUIDO EL MUTEX META MEMORIA EN LA LLAMADA DE LA FUNCION

    pthread_mutex_lock(&mutex_tamanioParticionABuscar);

    tamanioParticionABuscar = tamanio;
    tParticion *unaParticion = (tParticion *)list_find(METADATA_MEMORIA, &existeParticionLibre);

    pthread_mutex_unlock(&mutex_tamanioParticionABuscar);

    return unaParticion;
}

t_list *buscarListaDeParticionesLibresEnMemoriaOrdenadas(uint32_t tamanio)
{
    //YA ESTA INCLUIDO EL MUTEX META MEMORIA EN LA LLAMADA DE LA FUNCION 
    
    t_list *ParticionesOrdenadas;

    pthread_mutex_lock(&mutex_tamanioParticionABuscar);

    tamanioParticionABuscar = tamanio;
    ParticionesOrdenadas = list_filter(METADATA_MEMORIA, &existeParticionLibre);

    pthread_mutex_unlock(&mutex_tamanioParticionABuscar);
    list_sort(ParticionesOrdenadas, &sortParticionMenor);
    return ParticionesOrdenadas;
}

bool existeIdParticion(void *partition)
{

    tParticion *p = (tParticion *)partition;

    bool existe = false;

    if ((p->idParticion == idParticionABuscar))
    {
        existe = true;
    }

    return existe;
}

bool existePosicionParticion(void *unaParticion)
{

    tParticion *p = (tParticion *)unaParticion;
    return p->posicion == posicionParticionABuscar;
}

bool existePosicionParticionIzquierda(void *unaParticion)
{

    tParticion *p = (tParticion *)unaParticion;
    return (p->posicion + p->tamanio) == posicionParticionABuscar;
}

bool existeIdCorrelativo(void *numero)
{

    uint32_t *p = (uint32_t *)numero;

    bool existe = false;

    if ((*p == idCorrelativoABuscar))
    {
        existe = true;
    }

    return existe;
}



//////////////////////////////////////////////DUMP CACHE////////////////////////////////////////////////

void dumpCache()
{
    //while(1){

    FILE *f;
    int cantParticiones;

    char *contenidoDump = string_new();
    char *fecha = string_new();
    char *horaCompleta = temporal_get_string_time();
    char *hora = string_substring(horaCompleta, 0, 8);

    time_t t;
    t = time(NULL);
    struct tm tm = *localtime(&t);
    //strftime(fecha,11,"%d/%m/%Y", tm);
    string_append_with_format(&fecha, "%d/%d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    string_append_with_format(&contenidoDump, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    string_append_with_format(&contenidoDump, "Dump:                            %s                              %s\n", fecha, hora);
    tParticion *particion; // = malloc(sizeof(tParticion));
    char *limiteParticion;
    char *nombreCola;
    char libre;

    pthread_mutex_lock(&mutex_METADATA_MEMORIA);
    cantParticiones = list_size(METADATA_MEMORIA);

    if (cantParticiones > 0)
    {

        log_debug(logger, "CANTIDAD DE PARTICIONES: %d", cantParticiones);

        for (int i = 0; i < cantParticiones; i++)
        {

            particion = list_get(METADATA_MEMORIA, i);

            if (particion->free)
            {
                libre = 'L';
                nombreCola = string_new();
            }
            else
            {
                libre = 'X';
                nombreCola = buscarColaAPartirDeIdMensaje(particion->idMensaje);
            }

            limiteParticion = particion->posicion + particion->tamanio; //in rodri we trust.
            string_append_with_format(&contenidoDump, "Particion %d: %p (%d)", i, particion->posicion, (particion->posicion - MEMORIA_PRINCIPAL));
            string_append_with_format(&contenidoDump, " - %p (%d).", limiteParticion, (limiteParticion - MEMORIA_PRINCIPAL));
            string_append_with_format(&contenidoDump, "    [%c]    Size: %db\n", libre, particion->tamanio);
            string_append_with_format(&contenidoDump, "   LRU[%lld] Cola:[%s]    ID:[%d]\n", particion->lru, nombreCola, particion->idMensaje);
        }

        pthread_mutex_unlock(&mutex_METADATA_MEMORIA);

        string_append_with_format(&contenidoDump, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        f = fopen(CONFIG_BROKER->pathDump, "w+");

        log_debug(logger, "\n\tCREANDO ARCHIVO DUMP EN %s", CONFIG_BROKER->pathDump);

        fputs(contenidoDump, f);
        fseek(f, 0, SEEK_SET);
        fclose(f);

        free(contenidoDump);
        free(fecha);
        free(hora);
        free(nombreCola);
        //sleep(5);
    }
    else
    {

        log_info(logger, "No hay mensajes actualmente");
    }

    //}
}

// int getPosicionRelativa(char* posicionAbsoluta)
// {
//     char * posicionRelativaHex = string_new();

//     }
//     return num;
// }
//////////////////////////////////////////////PRUEBAS////////////////////////////////////////////////

// void prueba()
// {

//     t_list *particionesReLocas = list_create();

//     tParticion *particion1 = malloc(sizeof(tParticion));
//     tParticion *particion2 = malloc(sizeof(tParticion));
//     tParticion *particion3 = malloc(sizeof(tParticion));
//     /*
//     char* posicion1 = string_new();
//     char* posicion2 = string_new();
//     char* posicion3 = string_new();

//     string_append(&posicion1, "unHexaReLoco");
//     string_append(&posicion2, "Cocholates");
//     string_append(&posicion3, "alvaritoReloca");
//     */

//     particion1->free = 1;
//     particion1->posicion = (char *)particion1;
//     particion1->tamanio = 11;

//     particion2->free = 0;
//     particion2->posicion = (char *)particion2;
//     particion2->tamanio = 22;

//     particion3->free = 1;
//     particion3->posicion = (char *)particion3;
//     particion3->tamanio = 33;

//     list_add(particionesReLocas, particion1);
//     list_add(particionesReLocas, particion2);
//     list_add(particionesReLocas, particion3);

//     dumpCache(particionesReLocas);

//     free(particionesReLocas);
// }

void manejarSeniales(int signum)
{

    //printf("LCDTM ALLBOYS");

    if (signum == SIGUSR1)
    {

        dumpCache();
    }
    else
    {

        log_error(logger, "Se recibió una señal que no es conocida y/o no puede ser manejada");
    }
}

uint64_t nuevoTimeStamp(){

    struct timeval valor;

    gettimeofday(&valor, NULL);

    unsigned long long result = (((unsigned long long )valor.tv_sec) * 1000 + ((unsigned long) valor.tv_usec));

    uint64_t tiempo = result;

    return tiempo;
}

/*typedef struct {

	uint32_t idParticion;
	char* posicion;
	uint32_t tamanio;
	bool free;
	uint32_t idMensaje;
	uint32_t lru;
	uint32_t timeInit;
			
} tParticion;


typedef struct t_suscriptor{

	uint32_t colaDeMensajes;
	uint32_t tiempoDeSuscripcion;
	char* ip;
	uint32_t puerto;

	
} __attribute__((packed)) t_suscriptor;*/

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

char *buscarColaAPartirDeIdMensaje(uint32_t idMensaje)
{

    tMensaje *unMensaje;

    pthread_mutex_lock(&mutex_MENSAJES_LISTA);

    pthread_mutex_lock(&mutex_idMensajeABuscar);

    idMensajeABuscar = idMensaje;

    unMensaje = list_find(MENSAJES_LISTA, &existeIdMensaje);

    pthread_mutex_unlock(&mutex_idMensajeABuscar);
    
    pthread_mutex_unlock(&mutex_MENSAJES_LISTA);

    char *nombreCola = string_new();

    if( unMensaje){

    

        switch (unMensaje->tipoMensaje)
        {

        case tNewPokemon:

            string_append(&nombreCola, "NEW_POKEMON");

            break;

        case tAppearedPokemon:

            string_append(&nombreCola, "APPEARED_POKEMON");

            break;

        case tCatchPokemon:

            string_append(&nombreCola, "CATCH_POKEMON");

            break;

        case tCaughtPokemon:

            string_append(&nombreCola, "CAUGHT_POKEMON_LISTA");

            break;

        case tGetPokemon:

            string_append(&nombreCola, "GET_POKEMON");

            break;

        case tLocalizedPokemon:

            string_append(&nombreCola, "LOCALIZED_POKEMON");

            break;

        default:

            string_append(&nombreCola, "VALOR INVALIDO DE COLA");

            log_error(logger, "Se encontró un %s a partir del idMensaje %d. Error.", nombreCola, idMensaje);

            break;
        }
    }else{
        string_append(&nombreCola, "N/A");
    }

    return nombreCola;
}