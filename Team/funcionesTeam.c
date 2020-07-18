#include "team.h"

///////////////////////// Manejo de Configuracion y Logger ////////////////////////////////////////////////

void configurarLoggerTeam()
{

	logger = log_create(unTeamConfig->logFile, "TEAM", false, LOG_LEVEL_TRACE);
    printf("LOG INICIALIZADO CON EXITO\n");
}

void cargarConfiguracionTeam()
{

    unTeamConfig = malloc(sizeof(teamConfig_t));

    unTeamArchivoConfig = malloc(sizeof(t_config));

	unTeamArchivoConfig = config_create(PATH_CONFIG_TEAM);
	    
	if (unTeamArchivoConfig == NULL) {

        printf("\n=============================================================================================\n");
	    printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL TEAM");

	}else{

        printf("\n=======================================================================================\n");
        printf("\nCONFIGURACION DEL TEAM IMPORTADA");

        unTeamConfig->posicionEntrenadores = config_get_array_value(unTeamArchivoConfig, POSICIONES_ENTRENADORES);
        unTeamConfig->pokemonEntrenadores = config_get_array_value(unTeamArchivoConfig, POKEMON_ENTRENADORES);
        unTeamConfig->objetivosEntrenadores = config_get_array_value(unTeamArchivoConfig, OBJETIVOS_ENTRENADORES);
        unTeamConfig->tiempoReconexion = config_get_int_value(unTeamArchivoConfig, TIEMPO_RECONEXION);
        unTeamConfig->retardoCicloCPU = config_get_int_value(unTeamArchivoConfig, RETARDO_CICLO_CPU);
        unTeamConfig->algoritmoPlanificacion = config_get_string_value(unTeamArchivoConfig, ALGORITMO_PLANIFICACION);
        unTeamConfig->quantum = config_get_int_value(unTeamArchivoConfig, QUANTUM);
        unTeamConfig->alpha = config_get_double_value(unTeamArchivoConfig, ALPHA);
        unTeamConfig->ipBroker = config_get_string_value(unTeamArchivoConfig, IP_BROKER);
        unTeamConfig->estimacionInicial = config_get_int_value(unTeamArchivoConfig, ESTIMACION_INICIAL);
        unTeamConfig->puertoBroker = config_get_int_value(unTeamArchivoConfig, PUERTO_BROKER);
        unTeamConfig->ipTeam = config_get_string_value(unTeamArchivoConfig, IP_TEAM);
        unTeamConfig->puertoTeam = config_get_int_value(unTeamArchivoConfig, PUERTO_TEAM);
        unTeamConfig->logFile = config_get_string_value(unTeamArchivoConfig, LOG_FILE);
        
        char *stringPosicionEntrenadores = string_new();
	    char *stringPokemonEntrenadores = string_new();
	    char *stringObjetivosEntrenadores = string_new();

	    string_append(&stringPosicionEntrenadores, "[");
	    string_append(&stringPokemonEntrenadores, "[");
	    string_append(&stringObjetivosEntrenadores, "[");

	    for (int size = 0; unTeamConfig->posicionEntrenadores[size] != NULL; size++){

		    if (size == 0){

			    string_append(&stringPosicionEntrenadores, unTeamConfig->posicionEntrenadores[size]);

		    }else{

			    string_append_with_format(&stringPosicionEntrenadores, ",%s",unTeamConfig->posicionEntrenadores[size]);

		    }
	    }

        for (int size = 0; unTeamConfig->pokemonEntrenadores[size] != NULL; size++){

		    if (size == 0){

			    string_append(&stringPokemonEntrenadores, unTeamConfig->pokemonEntrenadores[size]);

		    }else{

			    string_append_with_format(&stringPokemonEntrenadores, ",%s",unTeamConfig->pokemonEntrenadores[size]);
		    }
	    }

        for (int size = 0; unTeamConfig->objetivosEntrenadores[size] != NULL; size++){

		    if (size == 0){

			    string_append(&stringObjetivosEntrenadores, unTeamConfig->objetivosEntrenadores[size]);

		    }else{

			    string_append_with_format(&stringObjetivosEntrenadores, ",%s",unTeamConfig->objetivosEntrenadores[size]);
		    }
	    }

	    string_append(&stringPosicionEntrenadores, "]");
	    string_append(&stringPokemonEntrenadores, "]");
	    string_append(&stringObjetivosEntrenadores, "]");
        
        printf("\n\n· Posicion de los Entrenadores = %s\n", stringPosicionEntrenadores);
        printf("· Pokemon de los Entrenadores = %s\n", stringPokemonEntrenadores);
        printf("· Objetivos de los Entrenadores = %s\n", stringObjetivosEntrenadores);
        printf("· Tiempo de Reconexion = %d\n", unTeamConfig->tiempoReconexion);
        printf("· Retarde de Ciclo de CPU = %d\n", unTeamConfig->retardoCicloCPU);
        printf("· Algoritmo de Planificacion = %s\n", unTeamConfig->algoritmoPlanificacion);
        printf("· Quantum = %d\n", unTeamConfig->quantum);
        printf("· Alpha = %f\n", unTeamConfig->alpha);
        printf("· IP del Broker = %s\n", unTeamConfig->ipBroker);
        printf("· Estimacion Inicial = %d\n", unTeamConfig->estimacionInicial);
        printf("· Puerto del Broker = %d\n", unTeamConfig->puertoBroker);
        printf("· IP del Team = %s\n", unTeamConfig->ipTeam);
        printf("· Puerto del Team = %d\n", unTeamConfig->puertoTeam);
        printf("· Ruta del Archivo Log del Team = %s\n\n", unTeamConfig->logFile);
        
        free(stringPosicionEntrenadores);
        free(stringPokemonEntrenadores);
        free(stringObjetivosEntrenadores);
        
        free(unTeamArchivoConfig);

    }

}

void actualizarConfiguracionTeam()
{

    FILE *archivoConfigFp;

	while(1){

		sleep(10);

		archivoConfigFp = fopen(PATH_CONFIG_TEAM,"rb");

		nuevoIdConfigTeam = 0;

		while (!feof(archivoConfigFp) && !ferror(archivoConfigFp)) {

   			nuevoIdConfigTeam ^= fgetc(archivoConfigFp);

		}

        fclose(archivoConfigFp);


        if(cantidadDeActualizacionesConfigTeam == 0){

        cantidadDeActualizacionesConfigTeam += 1;

        }else{

            if (nuevoIdConfigTeam != idConfigTeam) {

                printf("El archivo de configuración del Team cambió. Se procederá a actualizar.\n");
                cargarConfiguracionTeam();
                cantidadDeActualizacionesConfigTeam += 1;
                
            }

        }

        idConfigTeam = nuevoIdConfigTeam;

	}

}

///////////////////////// Manejo de Conexiones ////////////////////////////////////////////////////////////

void administradorDeConexiones(void* infoAdmin)
{

    infoAdminConexiones_t* unaInfoAdmin = (infoAdminConexiones_t*) infoAdmin;

    int idCliente = 0;
    int resultado;

    while((resultado = recibirInt(unaInfoAdmin->socketCliente,&idCliente)) > 0){

        switch(idCliente){

            case 2:{

                manejarRespuestaAGameBoy(unaInfoAdmin->socketCliente,idCliente);
                break;

            }

            case 1:{

                manejarRespuestaABroker(unaInfoAdmin->socketCliente,idCliente);
                break;
            }

            case -1:{
                //printf("Recibimos una prueba de conexion\n");
                log_warning(logger, "RECIBIMOS UNA PRUEBA DE CONEXION");
                break;

            }

            default:{

                log_warning(logger, "RECIBIMOS UN IDENTIFICADOR DE PROCESO NO VALIDO");
                break;

            }
                      
        }

    }

    if(resultado == 0){

        log_warning(logger, "CLIENTE DESCONECTADO");
        fflush(stdout);
        close(unaInfoAdmin->socketCliente);
        
    }else if (resultado == -2){

        log_warning(logger, "ME HICIERON UN PING\n\n\n");
        
    }

    else if(resultado == -1 || resultado < -2){
        
        log_warning(logger, "ERROR AL RECIBIR");
        close(unaInfoAdmin->socketCliente);
       
    }

    return;

}

void manejarRespuestaAGameBoy(int socketCliente, int idCliente)
{

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje)
    {
        /*
        case tAppearedPokemon:
        {

            log_trace(logger, "Llegó un APPEARD_POKEMON del Game Boy");

                t_appearedPokemon *unAppearedPokemon = (t_appearedPokemon *)buffer;

                enviarInt(socketCliente, 1); //Le avisamos al Broker que recibimos bien la solicitud.

                //SEMAFORO (POSIBLE DENTRO DE FUNCION)

                int posicion = posicionPokeEnListaMapaSinPosicion(unAppearedPokemon->nombrePokemon);

                //FIN SEMAFORO (POSIBLE DENTRO DE FUNCION)

                if(posicion != -1){

                    log_warning(logger, "El pokémon ya existía en el mapa. Nombre del Pokemón: %s - Posición en X: %d - Posición en Y: %d.", unAppearedPokemon->nombrePokemon, unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);

                }else{

                    log_warning(logger, "El pokémon no existía en el mapa, procedemos a agregarlo. Nombre del Pokemón: %s - Posición en X: %d - Posición en Y: %d.", unAppearedPokemon->nombrePokemon, unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);
                    
                    t_posicionPokemon* unaPosicionPokemon = malloc(sizeof(t_posicionPokemon));

                    t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
                    unPokemon->nombre = string_new();

                    string_append(&unPokemon->nombre,unAppearedPokemon->nombrePokemon);
                    unPokemon->cantidad = 1;

                    unaPosicionPokemon->pokemon = unPokemon;
                    unaPosicionPokemon->posicionX = unAppearedPokemon->posicionEnElMapaX;
                    unaPosicionPokemon->posicionY = unAppearedPokemon->posicionEnElMapaY;

                    pthread_mutex_lock(&mutexMapa);
                    list_add(mapa, unaPosicionPokemon);
                    pthread_mutex_unlock(&mutexMapa);

                    planificarReady(unAppearedPokemon->posicionEnElMapaX,unAppearedPokemon->posicionEnElMapaY,unAppearedPokemon->nombrePokemon,1);

                }

                break;

        }*/

        case tAppearedPokemon:
        {

            //log_trace(logger, "Llegó un APPEARD_POKEMON del Game Boy");

            t_appearedPokemon *unAppearedPokemon = (t_appearedPokemon *)buffer;

            enviarInt(socketCliente, 4); //Le avisamos al Game Boy que recibimos bien la solicitud.

            log_warning(logger, "Llego un appeared del Game Boy. Nombre del Pokemón: %s - Posición en X: %d - Posición en Y: %d.", unAppearedPokemon->nombrePokemon, unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);
            
            t_posicionPokemon* unaPosicionPokemon = malloc(sizeof(t_posicionPokemon));

            t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
            unPokemon->nombre = string_new();

            string_append(&unPokemon->nombre,unAppearedPokemon->nombrePokemon);
            unPokemon->cantidad = 1;

            unaPosicionPokemon->pokemon = unPokemon;
            unaPosicionPokemon->posicionX = unAppearedPokemon->posicionEnElMapaX;
            unaPosicionPokemon->posicionY = unAppearedPokemon->posicionEnElMapaY;

            pthread_mutex_lock(&mutexMapa);
            list_add(mapa, unaPosicionPokemon);
            pthread_mutex_unlock(&mutexMapa);

            planificarReady(unAppearedPokemon->posicionEnElMapaX,unAppearedPokemon->posicionEnElMapaY,unAppearedPokemon->nombrePokemon,1);

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

void manejarRespuestaABroker(int socketCliente, int idCliente){

    int *tipoMensaje = malloc(sizeof(int));
    int *tamanioMensaje = malloc(sizeof(int));

    void *buffer = recibirPaquete(socketCliente, tipoMensaje, tamanioMensaje);

    switch (*tipoMensaje){

        case tAppearedPokemon:{
            
            //log_trace(logger, "Llegó un APPEARD_POKEMON del Broker");

            t_appearedPokemon *unAppearedPokemon = (t_appearedPokemon *)buffer;

            enviarInt(socketCliente, 4); //Le avisamos al Broker que recibimos bien la solicitud.

            //int posicion = posicionPokeEnListaMapaSinPosicion(unAppearedPokemon->nombrePokemon);

            //log_warning(logger, "El pokémon no existía en el mapa, procedemos a agregarlo. Nombre del Pokemón: %s - Posición en X: %d - Posición en Y: %d.", unAppearedPokemon->nombrePokemon, unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);
            
            log_warning(logger, "Llego un appeared del Broker. Nombre del Pokemón: %s - Posición en X: %d - Posición en Y: %d.", unAppearedPokemon->nombrePokemon, unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);

            t_posicionPokemon* unaPosicionPokemon = malloc(sizeof(t_posicionPokemon));

            t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
            unPokemon->nombre = string_new();

            string_append(&unPokemon->nombre,unAppearedPokemon->nombrePokemon);
            unPokemon->cantidad = 1;

            unaPosicionPokemon->pokemon = unPokemon;
            unaPosicionPokemon->posicionX = unAppearedPokemon->posicionEnElMapaX;
            unaPosicionPokemon->posicionY = unAppearedPokemon->posicionEnElMapaY;

            pthread_mutex_lock(&mutexMapa);
            list_add(mapa, unaPosicionPokemon);
            pthread_mutex_unlock(&mutexMapa);

            planificarReady(unAppearedPokemon->posicionEnElMapaX,unAppearedPokemon->posicionEnElMapaY,unAppearedPokemon->nombrePokemon,1);

            break;

        }

        case tCaughtPokemon:
        {
            //log_trace(logger, "Llegó un CAUGHT_POKEMON del Broker");

            t_caughtPokemon *unCaughtPokemon = (t_caughtPokemon *)buffer;

            enviarInt(socketCliente, 4); //Le avisamos al Broker que recibimos bien la solicitud.

            log_warning(logger, "Llegó un CAUGHT_POKEMON del Broker. ID correlacional es: %d. Resultado es: %d", unCaughtPokemon->identificadorCorrelacional, unCaughtPokemon->resultado);

            int contador = 0;

            t_Entrenador* unEntrenador;

            pthread_mutex_lock(&mutexBloqueados);
            bool listaVaciaBloqueados = list_is_empty(BLOQUEADOS);

            if(!listaVaciaBloqueados){
                
                unEntrenador = list_get(BLOQUEADOS,contador);

                while(unEntrenador != NULL && unEntrenador->identificadorCorrelacional != unCaughtPokemon->identificadorCorrelacional){

                    contador ++;
                    unEntrenador = list_get(BLOQUEADOS,contador);

                }

                pthread_mutex_unlock(&mutexBloqueados);

                if(unEntrenador && (unEntrenador->identificadorCorrelacional == unCaughtPokemon->identificadorCorrelacional)){

                    pthread_mutex_lock(&mutexBloqueados);
                    int pos = posicionEntrenadorEnLista(BLOQUEADOS, unEntrenador->id);
                    t_Entrenador* entrenadorEsperando = list_remove(BLOQUEADOS, pos);
                    pthread_mutex_unlock(&mutexBloqueados);

                    if(unCaughtPokemon->resultado) 
                    {
                        log_warning("El entrenador: %d pudo atrapar correctamente al pokémon que fue a buscar.", entrenadorEsperando->id);
                        agregarPokeALista(entrenadorEsperando->pokemones, entrenadorEsperando->objetivoPokemon->nombre); //Chequear que funcione bien
                        pthread_mutex_lock(&mutexPokemonesAtrapados);
                        agregarPokeALista(pokemonesAtrapados, entrenadorEsperando->objetivoPokemon->nombre);
                        pthread_mutex_unlock(&mutexPokemonesAtrapados);
                        pthread_mutex_lock(&mutexPokemonesBuscandose);
                        quitarPokeDeLista(pokemonesBuscandose, entrenadorEsperando->objetivoPokemon->nombre);
                        pthread_mutex_unlock(&mutexPokemonesBuscandose);
                        entrenadorEsperando->objetivoPokemon->cantidad --;

                        if(entrenadorEsperando->objetivoPokemon->cantidad == 0){

                            entrenadorFinalizoSuTarea(entrenadorEsperando);

                        }else{

                            cambiarEstado(entrenadorEsperando,READY);
                            log_info(logger,"Se agrego al entrenador con ID %d a la cola de listos porque no termino de atrapar todos los pokemones que fue a buscar", unEntrenador->id);
                        }
                    }
                    else
                    {
                        log_warning("El entrenador: %d no pudo atrapar correctamente al pokémon que fue a buscar.", entrenadorEsperando->id);
                        pthread_mutex_lock(&mutexPokemonesBuscandose);
                        quitarPokeDeLista(pokemonesBuscandose, entrenadorEsperando->objetivoPokemon->nombre);
                        pthread_mutex_unlock(&mutexPokemonesBuscandose);
                        entrenadorFinalizoSuTarea(entrenadorEsperando);
                    }

                }else{

                    log_error(logger,"No hay ningun entrenador esperando el resultado de ese CATCH_POKEMON.");

                }

            }else{

                log_error(logger,"No hay ningun entrenador esperando el resultado de ese CATCH_POKEMON ya que no hay bloqueados.");
                pthread_mutex_unlock(&mutexBloqueados);

            }

            break;

        }

        case tLocalizedPokemon:
        {
        
            //log_trace(logger, "Llegó un LOCALIZED_POKEMON del Broker.");

            t_localizedPokemon *unLocalizedPokemon = (t_localizedPokemon *)buffer;
            
            enviarInt(socketCliente, 4);

            log_warning(logger, "Llegó un LOCALIZED_POKEMON del Broker. El nombre del Pokemón es: %s y el ID correlacional es: %d", unLocalizedPokemon->nombrePokemon, unLocalizedPokemon->identificadorCorrelacional);

            if(estaIdEnLista(unLocalizedPokemon->identificadorCorrelacional))
            {

                int cantidadListaDatosPokemon = list_size(unLocalizedPokemon->listaDatosPokemon);

                int contador = 0;

                int posicionEnLista = 0;

                posicionEnLista = posicionPokeEnListaMapaSinPosicion(unLocalizedPokemon->nombrePokemon);

                if(posicionEnLista != -1){

                    log_warning(logger, "Ya existe el pokémon: %s en el mapa", unLocalizedPokemon->nombrePokemon);

                }else{

                    log_warning(logger, "El pokémon: %s no existe en el mapa, se procederá a agregarlo", unLocalizedPokemon->nombrePokemon);

                    datosPokemon* nodoDatosPokemon;

                    while (contador < cantidadListaDatosPokemon){

                        nodoDatosPokemon = list_get(unLocalizedPokemon->listaDatosPokemon, contador);

                        t_posicionPokemon* unaPosicionPokemon = malloc(sizeof(t_posicionPokemon));

                        t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
                        unPokemon->nombre = string_new();

                        string_append(&unPokemon->nombre, unLocalizedPokemon->nombrePokemon);
                        unPokemon->cantidad = nodoDatosPokemon->cantidad;
                        unaPosicionPokemon->pokemon = unPokemon;
                        unaPosicionPokemon->posicionX = nodoDatosPokemon->posicionEnElMapaX;
                        unaPosicionPokemon->posicionY = nodoDatosPokemon->posicionEnElMapaY;

                        pthread_mutex_lock(&mutexMapa);
                        list_add(mapa, unaPosicionPokemon);
                        pthread_mutex_unlock(&mutexMapa);

                        planificarReady(nodoDatosPokemon->posicionEnElMapaX,nodoDatosPokemon->posicionEnElMapaY,unLocalizedPokemon->nombrePokemon,nodoDatosPokemon->cantidad);

                        contador += 1;

                        //log_warning(logger,"Se agregaron %d %s en la posición: [%d,%d].", nodoDatosPokemon->cantidad, unLocalizedPokemon->nombrePokemon, nodoDatosPokemon->posicionEnElMapaX,nodoDatosPokemon->posicionEnElMapaY);

                    }

                }
            }
            else
            {
                log_warning(logger, "Llego un localized que no corresponde a ningun Get enviado");
            }
            break;

        }

        default:{

            log_error(logger, "Recibimos algo del Broker que no sabemos manejar: %d", *tipoMensaje);
            abort();
            break;

        }

    }

    free(tipoMensaje);
    free(tamanioMensaje);
    free(buffer);

    return;

}

bool estaIdEnLista(int pIdCorrelacional)
{
    pthread_mutex_lock(&mutexIdentificadoresGet);
    bool listaVacia = list_is_empty(identificadoresGet);

    if(!listaVacia)
    {
        int tamanioIdentificadoresGet = list_size(identificadoresGet);
        for(int i = 0; i < tamanioIdentificadoresGet; i++)
        {
            int idCorre = list_get(identificadoresGet, i);
            if(idCorre == pIdCorrelacional)
            {
                pthread_mutex_unlock(&mutexIdentificadoresGet);
                return true;
            }
        }
    }
    pthread_mutex_unlock(&mutexIdentificadoresGet);
    return false;
}

///////////////////////// Funciones Auxiliares para Estados ///////////////////////////////////////////////

void cambiarEstado(t_Entrenador *pEntrenador, Estado pEstado)
{
    pEntrenador->estado = pEstado;
    switch(pEstado)
	{
		case READY:
		{
            pthread_mutex_lock(&mutexListos);
			list_add(LISTOS, pEntrenador);
            pthread_mutex_unlock(&mutexListos);
            sem_post(semaforoPlanificador);
			break;
		}

		case BLOCK:
		{
            pthread_mutex_lock(&mutexBloqueados);
			list_add(BLOQUEADOS, pEntrenador);
            pthread_mutex_unlock(&mutexBloqueados);
            //planificarPokemonPendiente(pEntrenador);
			break;
		}

        case EXEC:
		{
            pthread_mutex_lock(&mutexEntrenadorEjecutando);
			entrenadorEjecutando = pEntrenador;
            pthread_mutex_unlock(&mutexEntrenadorEjecutando);
            pthread_mutex_lock(&mutexCantidadCambiosDeContexto);
            cantidadCambiosDeContexto ++;
            pthread_mutex_unlock(&mutexCantidadCambiosDeContexto);
			break;
		}

        case EXIT:
		{
            pthread_mutex_lock(&mutexFinalizados);
			list_add(FINALIZADOS, pEntrenador);
            pthread_mutex_unlock(&mutexFinalizados);
			break;
		}

        default:
        {
            //log_error(logger, "No se reconocio el estado indicado");
            break;
        }
    }
}

void bloquearEntrenador(t_Entrenador* pEntrenador)
{
    cambiarEstado(pEntrenador, BLOCK);
}

void finalizarEntrenador(t_Entrenador* pEntrenador)
{
    cambiarEstado(pEntrenador, EXIT);
    log_info(logger,"Se paso al entrenador con ID %d a la cola de finalizados",pEntrenador->id);
}

///////////////////////// Inicio y Fin de Team ////////////////////////////////////////////////////////////

void inicializarTeam() {

    cargarConfiguracionTeam();

	configurarLoggerTeam();

    inicializarHilosYVariablesTeam();

}

void finalizarTeam() {
    pthread_mutex_lock(&mutexCantidadCambiosDeContexto);
    cantidadCambiosDeContexto ++;
    pthread_mutex_unlock(&mutexCantidadCambiosDeContexto);
    mostrarMetricas();
    liberarEntrenadores();
    /*
    pthread_mutex_lock(&mutexPIDHilos);
    for(int i = 0; i < list_size(pidHilos); i++)
    {
        pthread_cancel(list_get(pidHilos,i));
    }
    list_destroy_and_destroy_elements(pidHilos,(void*)eliminarHilos);
    pthread_mutex_unlock(&mutexPIDHilos);
    */
    free(unTeamConfig);
    free(logger);
    kill(getpid(),9);
}

liberarEntrenadores()
{
    for(int i = 0; i < list_size(listaDeEntrenadores); i++)
    {
        t_Entrenador* unEntrenador = list_remove(listaDeEntrenadores, 0);
        for(int j = 0; j < list_size(unEntrenador->pokemones); j++)
        {
            t_Pokemon* unPokemon = list_remove(unEntrenador->pokemones,0);
            free(unPokemon->nombre);
            free(unPokemon);
        }
        for(int j = 0; j < list_size(unEntrenador->pokemones); j++)
        {
            t_Pokemon* unPokemon = list_remove(unEntrenador->pokemones,0);
            free(unPokemon->nombre);
            free(unPokemon);
        }
        t_Pokemon* pokeObjetivo = unEntrenador->objetivoPokemon;
        free(pokeObjetivo->nombre);
        free(pokeObjetivo);
        free(unEntrenador);
    }
    free(listaDeEntrenadores);
}

/*
void eliminarHilos(pthread_t* pHilo)
{
    free(pHilo);
}*/

void inicializarHilosYVariablesTeam()
{
    int resultado;
    int tipoResultado = 0;
    int tamanioSuscriptor = 0;

    cantidadDeActualizacionesConfigTeam = 0;
    
    listaDeEntrenadores = list_create();
    NUEVOS = list_create();
    LISTOS = list_create();
    BLOQUEADOS = list_create();
    FINALIZADOS = list_create();
    entrenadorEjecutando = NULL;
    pokemonesAtrapados = list_create();
    pokemonesObjetivos = list_create();
    pokemonesBuscandose = list_create();
    mapa = list_create();
    mapaPendientes = list_create();
    identificadoresGet = list_create();

    cantidadCiclosCPU = 0;
    cantidadCambiosDeContexto = 0;
    cantidadDeadlocks = 0;
    cantidadDeadlocksResueltos = 0;

    semaforosEntrenador = list_create();
    
    cargarEntrenadoresYListasGlobales();

    semaforoPlanificador = malloc(sizeof(sem_t));
    sem_init(semaforoPlanificador, 0, 0);

    semaforoTerminoEjecucion = malloc(sizeof(sem_t));
    sem_init(semaforoTerminoEjecucion, 0, 0);

    pthread_create(&hiloPlanificador, NULL, (void*)planificarExec, NULL);
    
    //pruebasSanty();

    socketBroker = cliente(unTeamConfig->ipBroker, unTeamConfig->puertoBroker, ID_BROKER);

    infoServidor_t* unaInfoServidorTeam = malloc(sizeof(infoServidor_t));

    unaInfoServidorTeam->puerto = unTeamConfig->puertoTeam;
    unaInfoServidorTeam->ip = string_new();
    //string_append(&unaInfoServidorTeam->ip,unTeamConfig->ipTeam); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    string_append(&unaInfoServidorTeam->ip,"0");

    pthread_create(&hiloActualizadorSocketBrocker, NULL, (void *)actualizarConexionConBroker, NULL);

    pthread_create(&hiloActualizadorConfigTeam, NULL, (void*)actualizarConfiguracionTeam, NULL);

    pthread_create(&hiloServidorTeam,NULL,(void*)servidor_inicializar,(void*)unaInfoServidorTeam);

    t_suscriptor *unSuscriptor = malloc(sizeof(t_suscriptor));

    unSuscriptor->identificador = 0;
    unSuscriptor->identificadorCorrelacional = 0;

    unSuscriptor->colaDeMensajes = tAppearedPokemon;

    unSuscriptor->tiempoDeSuscripcion = 0;
    unSuscriptor->puerto = unTeamConfig->puertoTeam;

    unSuscriptor->ip = string_new();
    string_append(&unSuscriptor->ip, unTeamConfig->ipTeam);

    enviarInt(socketBroker, 4);
    enviarPaquete(socketBroker, tSuscriptor, unSuscriptor, tamanioSuscriptor);

    if ((resultado = recibirInt(socketBroker, &tipoResultado)) > 0){

        if (tipoResultado == 1){

            log_warning(logger, "Pedido de suscripción realizado con éxito");

        }else if (tipoResultado == 0){

            log_warning(logger, "No se pudo realizar el pedido de suscripción");

        }

    }else{

        log_error(logger, "Hubo un error al recibir el resultado de la operación desde el Broker");

    }

    envioDeGetsPokemon();

    pthread_join(hiloActualizadorConfigTeam, NULL);

}

void cargarEntrenadoresYListasGlobales()
{
    int cantidadDeEntrenadores = 0;
    for (int i = 0; unTeamConfig->posicionEntrenadores[i] != NULL; i++)
	{
        cantidadDeEntrenadores++;
    }
    for (int i = 0; i < cantidadDeEntrenadores; i++)
    {
        t_Entrenador *unEntrenador = malloc(sizeof(t_Entrenador));
        unEntrenador->pokemones = list_create();
        unEntrenador->objetivos = list_create();
        unEntrenador->id = i;
        char** posiciones = string_split(unTeamConfig->posicionEntrenadores[i], "|");
        unEntrenador->posicionX = atoi(posiciones[0]);
        unEntrenador->posicionY = atoi(posiciones[1]);
        free(posiciones[0]);
        free(posiciones[1]);
        free(posiciones);
        char** pokemones = string_split(unTeamConfig->pokemonEntrenadores[i], "|");
        for (int j = 0; pokemones[j] != NULL; j++)
        {
            agregarPokeALista(unEntrenador->pokemones, pokemones[j]);
            pthread_mutex_lock(&mutexPokemonesAtrapados);
            agregarPokeALista(pokemonesAtrapados, pokemones[j]);
            pthread_mutex_unlock(&mutexPokemonesAtrapados);
        }
        string_iterate_lines(pokemones, (void*) free);
        free(pokemones);
        char** objetivos = string_split(unTeamConfig->objetivosEntrenadores[i], "|");
        for (int j = 0; objetivos[j] != NULL; j++)
        {
            agregarPokeALista(unEntrenador->objetivos, objetivos[j]);
            pthread_mutex_lock(&mutexPokemonesObjetivos);
            agregarPokeALista(pokemonesObjetivos, objetivos[j]);
            pthread_mutex_unlock(&mutexPokemonesObjetivos);
        }
        string_iterate_lines(objetivos, (void*) free);
        free(objetivos);
        unEntrenador->estado = NEW;
        unEntrenador->cuantosPuedeAtrapar = cantidadTotalDePokemonesEnLista(unEntrenador->objetivos);
        unEntrenador->objetivo = Ninguno;
        t_Pokemon* unPokemonObjetivo = malloc(sizeof(t_Pokemon));
        unPokemonObjetivo->nombre = string_new();
        unEntrenador->objetivoPokemon = unPokemonObjetivo;
        unEntrenador->estimacionAnterior = 0;
        unEntrenador->rafagaAnterior = 0;
        unEntrenador->estimacionActual = unTeamConfig->estimacionInicial;
        unEntrenador->rafagaActual = 0;
        unEntrenador->cpuIntercambio = 0;
        unEntrenador->rafagasTotales = 0;
        
        sem_t* unSemaforo = malloc(sizeof(sem_t));
        sem_init(unSemaforo, 0, 0);

        pthread_mutex_lock(&mutexSemaforosEntrenador);
        list_add(semaforosEntrenador, unSemaforo);
        pthread_mutex_unlock(&mutexSemaforosEntrenador);

        char *hiloEntrenador = string_new();
        string_append_with_format(&hiloEntrenador, ",%d", unEntrenador->id);
        pthread_create(&hiloEntrenador, NULL, (void*)ejecutar, unEntrenador->id);

        pthread_mutex_lock(&mutexListaDeEntrenadores);
        list_add(listaDeEntrenadores, unEntrenador);
        pthread_mutex_unlock(&mutexListaDeEntrenadores);
        pthread_mutex_lock(&mutexNuevos);
        list_add(NUEVOS, unEntrenador);
        log_info(logger,"Se agrego el entrenador con ID %d a la cola de nuevos al levantar la configuracion", unEntrenador->id);
        pthread_mutex_unlock(&mutexNuevos);

    }
}

void envioDeGetsPokemon()
{
    t_getPokemon* unGetPokemon = malloc(sizeof(t_getPokemon));
    pthread_mutex_lock(&mutexPokemonesObjetivos);
    int cantidadDeObjetivos = list_size(pokemonesObjetivos);
    for(int i = 0; i < cantidadDeObjetivos; i++)
    {
        t_Pokemon* unPokemon = list_get(pokemonesObjetivos, i);
        pthread_mutex_lock(&mutexSocketBroker);

        brokerActivo = probarConexionSocket(socketBroker);

        if(brokerActivo > 0){

            unGetPokemon->identificador = 0;
            unGetPokemon->identificadorCorrelacional = 0;

            unGetPokemon->nombrePokemon = string_new();
            string_append(&unGetPokemon->nombrePokemon, unPokemon->nombre);
            
            int tamanioGetPokemon = 0;

            enviarInt(socketBroker, 4);
            enviarPaquete(socketBroker, tGetPokemon, unGetPokemon, tamanioGetPokemon);

            int resultado;
            int tipoResultado = 0;

            if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

                if(tipoResultado > 0){
                    
                    printf("Get enviado con éxito\n");

                    pthread_mutex_lock(&mutexIdentificadoresGet);
                    list_add(identificadoresGet, tipoResultado);
                    pthread_mutex_unlock(&mutexIdentificadoresGet);
                    
                }else if(tipoResultado == 0){

                    printf("No se pudo enviar el get\n");
                    
                }

            }else{

                log_error(logger,"Hubo un error al recibir el resultado del getPokemon desde el Broker. Está desconectado.");
                
                brokerActivo = probarConexionSocket(socketBroker);

                //ANALIZAR DESCONEXION DEL BROKER Y VER QUÉ SE HACE
            }

            free(unGetPokemon->nombrePokemon);

        }else{

            log_error(logger, "FALLÓ ENVÍO DE GET_POKEMON AL BROKER");

        }

        pthread_mutex_unlock(&mutexSocketBroker);
    }
    pthread_mutex_unlock(&mutexPokemonesObjetivos);
    free(unGetPokemon);
}

////////////////////////////// Pruebas ////////////////////////////////////////////////////////////////////
/*
void pruebasSanty()
{

    for(int i = 0; i < list_size(listaDeEntrenadores); i++)
    {
        t_Entrenador* aEntrenador = list_get(listaDeEntrenadores, i);
        printf("El entrenador %d tiene los siguientes pokemons:\n", aEntrenador->id);
        printf("List size de pokemones: %d\n", list_size(aEntrenador->pokemones));
        for(int j = 0; j < list_size(aEntrenador->pokemones); j++)
        {
            t_Pokemon* aPokemon = list_get(aEntrenador->pokemones, j);
            printf("Nombre: %s y cantidad: %d\n", aPokemon->nombre, aPokemon->cantidad);
        }
        printf("Y los siguientes objetivos:\n");
        for(int j = 0; j < list_size(aEntrenador->objetivos); j++)
        {
            t_Pokemon* aPokemon = list_get(aEntrenador->objetivos, j);
            printf("Nombre: %s y cantidad: %d\n", aPokemon->nombre, aPokemon->cantidad);
        }
    }

    planificarReady(2,2,"Charizard",1); //0

    //planificarExec();
    
    //planificarExec();

    planificarReady(4,7,"Charizard",1); //1

    planificarReady(6,5,"Charizard",1); //2

    planificarReady(2,2,"Charizard",1); //0

    planificarReady(2,2,"Charizard",1); //0

    //planificarExec();

    //planificarExec();

    for(int i = 0; i < list_size(listaDeEntrenadores); i++)
    {
        t_Entrenador* aEntrenador = list_get(listaDeEntrenadores, i);
        printf("El entrenador %d tiene los siguientes pokemons:\n", aEntrenador->id);
        for(int j = 0; j < list_size(aEntrenador->pokemones); j++)
        {
            t_Pokemon* aPokemon = list_get(aEntrenador->pokemones, j);
            printf("Nombre: %s y cantidad: %d\n", aPokemon->nombre, aPokemon->cantidad);
        }
        printf("Y los siguientes objetivos:\n");
        for(int j = 0; j < list_size(aEntrenador->objetivos); j++)
        {
            t_Pokemon* aPokemon = list_get(aEntrenador->objetivos, j);
            printf("Nombre: %s y cantidad: %d\n", aPokemon->nombre, aPokemon->cantidad);
        }
        printf("Estado del entrenador: %d\n",aEntrenador->estado);
        printf("Objetivo del entrenador: %d\n",aEntrenador->objetivo);
    }

    printf("Termine la ejecucion del programa\n");

}*/

////////////////////////// Metricas ///////////////////////////////////////////////////////////////////////

void mostrarMetricas(){

    log_info(logger, "----------------------------------------MÉTRICAS----------------------------------------");
    pthread_mutex_lock(&mutexCantidadCambiosDeContexto);
    log_info(logger, "Cantidad de cambios de contexto: %d", cantidadCambiosDeContexto);
    pthread_mutex_unlock(&mutexCantidadCambiosDeContexto);
    pthread_mutex_lock(&mutexCantidadDeadlocks);
    log_info(logger, "Cantidad de deadlocks: %d",cantidadDeadlocks);
    pthread_mutex_unlock(&mutexCantidadDeadlocks);
    pthread_mutex_lock(&mutexCantidadDeadlocksResueltos);
    log_info(logger, "Cantidad de deadlocks resueltos: %d",cantidadDeadlocksResueltos);
    pthread_mutex_unlock(&mutexCantidadDeadlocksResueltos);
    pthread_mutex_lock(&mutexListaDeEntrenadores);
    int tamanioListaEntrenadores = list_size(listaDeEntrenadores);
    for(int i = 0; i < tamanioListaEntrenadores; i++)
    {
        t_Entrenador* unEntrenador = list_get(listaDeEntrenadores, i);
        log_info(logger, "El entrenador con ID %d ejecuto %d rafagas de CPU", unEntrenador->id, unEntrenador->rafagasTotales);
    }
    pthread_mutex_unlock(&mutexListaDeEntrenadores);
    pthread_mutex_lock(&mutexCantidadCiclosCPU);
    log_info(logger, "Cantidad de ciclos de CPU: %d", cantidadCiclosCPU);
    pthread_mutex_unlock(&mutexCantidadCiclosCPU);
    log_info(logger, "----------------------------------------------------------------------------------------");

}

////////////////////////// Funciones para listas de pokemon ///////////////////////////////////////////////

int posicionPokeEnLista(t_list* pLista, char* pPokemon) //Retorna la posicion donde se encuentra o -1 si no esta
{
    for(int i = 0; i < list_size(pLista); i++)
    {
        t_Pokemon* unPokemon = list_get(pLista,i);
        char* pokeNombre = unPokemon->nombre;
        if(strcmp(pPokemon, pokeNombre) == 0)
        {
            return i;
        }
    }
    return -1; //Si no existe
}
/*
int posicionPokeEnListaMapa(char* nombrePokemon, int posicionEnX, int posicionEnY){

    pthread_mutex_lock(&mutexMapa);
    int tamanioMapa = list_size(mapa);
    pthread_mutex_unlock(&mutexMapa);

    for(int i = 0; i < tamanioMapa; i++){
        
        pthread_mutex_lock(&mutexMapa);
        t_posicionPokemon* unaPosicionPokemon = list_get(mapa, i);
        pthread_mutex_unlock(&mutexMapa);

        if(!strcmp(unaPosicionPokemon->pokemon->nombre, nombrePokemon) && (unaPosicionPokemon->posicionX == posicionEnX) && (unaPosicionPokemon->posicionY == posicionEnY)){

            return i;

        }
        
    }

    return -1;

}
*/

int posicionPokeEnListaMapaSinPosicion(char* nombrePokemon){
    
    pthread_mutex_lock(&mutexMapa);
    int tamanioMapa = list_size(mapa);

    for(int i = 0; i < tamanioMapa; i++){
        
        t_posicionPokemon* unaPosicionPokemon = list_get(mapa, i);
        if(!strcmp(unaPosicionPokemon->pokemon->nombre, nombrePokemon))
        {
            pthread_mutex_unlock(&mutexMapa);
            return i;

        }
        
    }
    pthread_mutex_unlock(&mutexMapa);
    return -1;

}

void agregarPokeALista(t_list* pLista, char* pPokemon)
{
    int posicion = posicionPokeEnLista(pLista, pPokemon);
    if(posicion != -1)
    {
        t_Pokemon* unPokemon = list_get(pLista, posicion);
        unPokemon->cantidad++;
    }
    else
    {
        t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
        unPokemon->nombre = string_new();
        string_append(&unPokemon->nombre, pPokemon);
        unPokemon->cantidad = 1;
        list_add(pLista, unPokemon);
    }
}

void quitarPokeDeLista(t_list* pLista, char* pPokemon)
{
    int posicion = posicionPokeEnLista(pLista, pPokemon);
    if(posicion != -1)
    {
        t_Pokemon* unPokemon = list_get(pLista, posicion);
        if(unPokemon->cantidad > 1)
        {
            unPokemon->cantidad--;
        }
        else
        {
            free(unPokemon->nombre);
            free(unPokemon);
            list_remove(pLista, posicion);
        }
    }
    else
    {
        //log_error(logger, "La lista no tiene el pokemon a quitar");
    }
}

int cantidadDeUnPokemonEnLista(t_list* pLista, char* pPokemon)
{
    int posicion = posicionPokeEnLista(pLista, pPokemon);
    if(posicion == -1)
    {
        return 0;
    }
    else
    {
        t_Pokemon* unPokemon = list_get(pLista, posicion);
        return unPokemon->cantidad;
    }
}

int cantidadTotalDePokemonesEnLista(t_list* pLista)
{
    int cantidad = 0;
    for(int i = 0; i < list_size(pLista); i++)
    {
        t_Pokemon* unPokemon = list_get(pLista,i);
        cantidad += unPokemon->cantidad;
    }
    return cantidad;
}

////////////////////////// Funciones de planificacion /////////////////////////////////////////////////////

void planificarReady(int posXpokemon,int posYpokemon, char* pPokemonNombre, int pPokemonCantidad)
{
    if(puedoPlanificarReady())
    {
        int cuantosPrecisa = cuantosPrecisaGlobalmente(pPokemonNombre);
        if(cuantosPrecisa >= pPokemonCantidad)
        {
            t_Entrenador* unEntrenador = entrenadorMasCercano(posXpokemon, posYpokemon, pPokemonNombre, pPokemonCantidad);
            cambiarEstado(unEntrenador, READY);
            log_info(logger,"Se agrego al entrenador con ID %d a la cola de listos por ser el mas cercano al objetivo", unEntrenador->id);
        }
        else
        {
            if(cuantosPrecisa > 0)
            {
                t_Entrenador* unEntrenador = entrenadorMasCercano(posXpokemon, posYpokemon, pPokemonNombre, cuantosPrecisa);
                cambiarEstado(unEntrenador, READY);
                log_info(logger,"Se agrego al entrenador con ID %d a la cola de listos por ser el mas cercano al objetivo", unEntrenador->id);
            }
            else
            {
                //log_error(logger, "No preciso ningun pokemon de esa especie");
                guardarPokemonABuscar(posXpokemon, posYpokemon, pPokemonNombre, pPokemonCantidad);
            }
        }
    }
    else
    {
        guardarPokemonABuscar(posXpokemon, posYpokemon, pPokemonNombre, pPokemonCantidad);
    }
}

void guardarPokemonABuscar(int posXpokemon,int posYpokemon, char* pPokemonNombre, int pPokemonCantidad)
{
    if(cuantosPrecisaGlobalmente(pPokemonNombre) != -1)
    {
        t_posicionPokemon* unaPosicionPokemon = malloc(sizeof(t_posicionPokemon));
        t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
        unPokemon->nombre = string_new();

        string_append(&unPokemon->nombre,pPokemonNombre);
        unPokemon->cantidad = pPokemonCantidad;

        unaPosicionPokemon->pokemon = unPokemon;
        unaPosicionPokemon->posicionX = posXpokemon;
        unaPosicionPokemon->posicionY = posYpokemon;
        pthread_mutex_lock(&mutexMapaPendientes);
        list_add(mapaPendientes, unaPosicionPokemon);
        pthread_mutex_unlock(&mutexMapaPendientes);
    }
}
/*
void planificarPokemonPendiente(t_Entrenador* pEntrenador)
{
    pthread_mutex_lock(&mutexMapaPendientes);
    bool listaVacia = list_is_empty(mapaPendientes);
    if(!listaVacia && puedeDesbloquearse(pEntrenador))
    {
        t_posicionPokemon* unaPosicionPokemon = list_remove(mapaPendientes, 0);
        pthread_mutex_unlock(&mutexMapaPendientes);
        t_Pokemon* unPokemon = unaPosicionPokemon->pokemon;
        planificarReady(unaPosicionPokemon->posicionX, unaPosicionPokemon->posicionY, unPokemon->nombre, unPokemon->cantidad);
        free(unPokemon->nombre);
        free(unPokemon);
        free(unaPosicionPokemon);
    }
    pthread_mutex_unlock(&mutexMapaPendientes);
}*/

void planificarPokemonPendiente()
{
    //pthread_mutex_lock(&mutexListos);
    //bool listaVaciaListos = list_is_empty(LISTOS);
    //printf("\nLIST SIZE LISTOS: %d\n", list_size(LISTOS));
    //pthread_mutex_unlock(&mutexListos);
    pthread_mutex_lock(&mutexMapaPendientes);
    bool listaVaciaMapaPendientes = list_is_empty(mapaPendientes);
    //printf("\nLIST SIZE PENDIENTES: %d\n", list_size(mapaPendientes));
    //if(!listaVaciaMapaPendientes && listaVaciaListos)
    if(!listaVaciaMapaPendientes)
    {
        t_posicionPokemon* unaPosicionPokemon = list_remove(mapaPendientes, 0);
        pthread_mutex_unlock(&mutexMapaPendientes);
        t_Pokemon* unPokemon = unaPosicionPokemon->pokemon;
        planificarReady(unaPosicionPokemon->posicionX, unaPosicionPokemon->posicionY, unPokemon->nombre, unPokemon->cantidad);
        free(unPokemon->nombre);
        free(unPokemon);
        free(unaPosicionPokemon);
    }
    else
    {
        pthread_mutex_unlock(&mutexMapaPendientes);
    }
}

bool puedoPlanificarReady()
{
    pthread_mutex_lock(&mutexNuevos);
    bool listaVaciaNuevos = list_is_empty(NUEVOS);
    pthread_mutex_unlock(&mutexNuevos);
    if(!listaVaciaNuevos)
    {
        return true;
    }
    else
    {
        pthread_mutex_lock(&mutexBloqueados);
        bool listaVaciaBloqueados = list_is_empty(BLOQUEADOS);

        if(!listaVaciaBloqueados)
        {
            int tamanioBloqueados = list_size(BLOQUEADOS);

            for(int i = 0; i < tamanioBloqueados; i++)
            {
                t_Entrenador* unEntrenador = list_get(BLOQUEADOS, i);
                if(puedeDesbloquearse(unEntrenador))
                {
                    pthread_mutex_unlock(&mutexBloqueados);
                    return true;
                }
            }

            pthread_mutex_unlock(&mutexBloqueados);
            return false;
        }
        else
        {
            pthread_mutex_unlock(&mutexBloqueados);
            return false;
        }
    }
}

void planificarExec()
{
    while(1)
    {
        sem_wait(semaforoPlanificador);
            if(strcmp(unTeamConfig->algoritmoPlanificacion,"FIFO") == 0)
            {
                planificarFIFO();
            }
            else if(strcmp(unTeamConfig->algoritmoPlanificacion,"RR") == 0)
            {
                planificarRR();
            }
            else if(strcmp(unTeamConfig->algoritmoPlanificacion,"SJF-SD") == 0)
            {
                planificarSJF();
            }
            else if(strcmp(unTeamConfig->algoritmoPlanificacion,"SJF-CD") == 0)
            {
                planificarSRT();
            }
            else
            {
                printf("No existe el algoritmo de planificacion especificado\n");
            }
            if(puedoPlanificarReady())
            {
                planificarPokemonPendiente();
            }
            pthread_mutex_lock(&mutexListos);
            int cantidadReady = list_size(LISTOS);
            pthread_mutex_unlock(&mutexListos);
            pthread_mutex_lock(&mutexCantidadDeadlocks);
            int cantDeadlocks = cantidadDeadlocks;
            pthread_mutex_unlock(&mutexCantidadDeadlocks);
            if(cantidadReady == 0 && cantDeadlocks == 0)
            {
                calcularDeadlock();
            }
            if(teamCumplioObjetivos())
            {
                log_debug(logger,"Se cumplieron todos los objetivos");
                printf("Se cumplieron todos los objetivos\n");
                finalizarTeam();
            }
            else
            {
                log_debug(logger,"Todavia no se cumplieron todos los objetivos");
                printf("Todavia no se cumplieron todos los objetivos\n");
            }
}
}

void planificarFIFO()
{
    //printf("Planifique FIFO\n");
    pthread_mutex_lock(&mutexListos);
    t_Entrenador* unEntrenador = list_remove(LISTOS, 0);
    log_info(logger,"Se quito el entrenador con ID %d de la cola de listos, por ser el que esta primero, para llevarlo a ejecutar", unEntrenador->id);
    pthread_mutex_unlock(&mutexListos);
    cambiarEstado(unEntrenador, EXEC);
    log_info(logger,"Se paso el entrenador con ID %d a ejecucion segun el algoritmo FIFO", unEntrenador->id);
    //printf("Posicion de X a donde tiene que ir: %d\n", unEntrenador->objetivoX);
    //printf("Posicion de Y a donde tiene que ir: %d\n", unEntrenador->objetivoY);
    while(entrenadorEjecutando != NULL){
        pthread_mutex_lock(&mutexSemaforosEntrenador);
        sem_post(list_get(semaforosEntrenador, entrenadorEjecutando->id));
        pthread_mutex_unlock(&mutexSemaforosEntrenador);
        //log_warning(logger, "Hay un entrenador en ejecución");
        sem_wait(semaforoTerminoEjecucion);
        sleep(unTeamConfig->retardoCicloCPU);
    }
    //log_debug(logger,"Termino de ejecutar\n");
}

void planificarRR()
{
    //printf("Planifique Round Robin\n");
    pthread_mutex_lock(&mutexListos);
    t_Entrenador* unEntrenador = list_remove(LISTOS, 0);
    log_info(logger,"Se quito el entrenador con ID %d de la cola de listos, por ser el que esta primero, para llevarlo a ejecutar", unEntrenador->id);
    pthread_mutex_unlock(&mutexListos);
    cambiarEstado(unEntrenador, EXEC);
    log_info(logger,"Se paso el entrenador con ID %d a ejecucion segun el algoritmo Round Robin", unEntrenador->id);
    //printf("Posicion de X a donde tiene que ir: %d\n", unEntrenador->objetivoX);
    //printf("Posicion de Y a donde tiene que ir: %d\n", unEntrenador->objetivoY);
    for(int i = 0; i < unTeamConfig->quantum; i++)
    {
        if(entrenadorEjecutando != NULL)
        {
            //printf("ID del entrenador ejecutando: %d\n",entrenadorEjecutando->id);
            pthread_mutex_lock(&mutexSemaforosEntrenador);
            sem_post(list_get(semaforosEntrenador, entrenadorEjecutando->id));
            pthread_mutex_unlock(&mutexSemaforosEntrenador);
            //log_warning(logger, "Hay un entrenador en ejecución");
            sem_wait(semaforoTerminoEjecucion);
            sleep(unTeamConfig->retardoCicloCPU);
        }
        else
        {
            break;
        }
    }
    if(entrenadorEjecutando != NULL)
    {
        cambiarEstado(entrenadorEjecutando, READY);
        log_info(logger,"Se saco al entrenador con ID %d de ejecucion ya que termino su quantum", entrenadorEjecutando->id);
        log_info(logger,"Se agrego al entrenador con ID %d a la cola de listos ya que no termino su objetivo antes de ser cortado por rafaga", entrenadorEjecutando->id);
        pthread_mutex_lock(&mutexEntrenadorEjecutando);
        entrenadorEjecutando = NULL;
        pthread_mutex_unlock(&mutexEntrenadorEjecutando);
        pthread_mutex_lock(&mutexCantidadCambiosDeContexto);
        cantidadCambiosDeContexto ++;
        pthread_mutex_unlock(&mutexCantidadCambiosDeContexto);
    }
}

void planificarSJF()
{
    //printf("Planifique SJF sin desalojo\n");
    int posicionEntre = entrenadorConMenorEstimacion();
    pthread_mutex_lock(&mutexListos);
    t_Entrenador* unEntrenador = list_remove(LISTOS, posicionEntre);
    log_info(logger,"Se quito el entrenador con ID %d de la cola de listos, por ser el que tiene menor estimacion, para llevarlo a ejecutar", unEntrenador->id);
    pthread_mutex_unlock(&mutexListos);
    cambiarEstado(unEntrenador, EXEC);
    log_info(logger,"Se paso el entrenador con ID %d a ejecucion segun el algoritmo SJF sin desalojo", unEntrenador->id);
    //printf("Posicion de X a donde tiene que ir: %d\n", unEntrenador->objetivoX);
    //printf("Posicion de Y a donde tiene que ir: %d\n", unEntrenador->objetivoY);
    while(entrenadorEjecutando != NULL){
        pthread_mutex_lock(&mutexSemaforosEntrenador);
        sem_post(list_get(semaforosEntrenador, entrenadorEjecutando->id));
        pthread_mutex_unlock(&mutexSemaforosEntrenador);
        //log_warning(logger, "Hay un entrenador en ejecución");
        sem_wait(semaforoTerminoEjecucion);
        sleep(unTeamConfig->retardoCicloCPU);
    }
    //log_debug(logger,"Termino de ejecutar\n");
}

void planificarSRT()
{
    //printf("Planifique SJF con desalojo\n");
    int posicionEntre = entrenadorConMenorEstimacion();
    pthread_mutex_lock(&mutexListos);
    t_Entrenador* unEntrenador = list_remove(LISTOS, posicionEntre);
    log_info(logger,"Se quito el entrenador con ID %d de la cola de listos, por ser el que tiene menor estimacion, para llevarlo a ejecutar", unEntrenador->id);
    pthread_mutex_unlock(&mutexListos);
    cambiarEstado(unEntrenador, EXEC);
    log_info(logger,"Se paso el entrenador con ID %d a ejecucion segun el algoritmo SJF con desalojo", unEntrenador->id);
    pthread_mutex_lock(&mutexListos);
    int procesosEnListos = list_size(LISTOS);
    pthread_mutex_unlock(&mutexListos);
    //printf("Posicion de X a donde tiene que ir: %d\n", unEntrenador->objetivoX);
    //printf("Posicion de Y a donde tiene que ir: %d\n", unEntrenador->objetivoY);
    while(entrenadorEjecutando != NULL){
        pthread_mutex_lock(&mutexSemaforosEntrenador);
        sem_post(list_get(semaforosEntrenador, entrenadorEjecutando->id));
        pthread_mutex_unlock(&mutexSemaforosEntrenador);
        //log_warning(logger, "Hay un entrenador en ejecución");
        sem_wait(semaforoTerminoEjecucion);
        sleep(unTeamConfig->retardoCicloCPU);
        pthread_mutex_lock(&mutexListos);
        int nuevaCantidadProcesosEnListos = list_size(LISTOS);
        pthread_mutex_unlock(&mutexListos);
        if(procesosEnListos != nuevaCantidadProcesosEnListos && entrenadorEjecutando != NULL)
        {
            cambiarEstado(entrenadorEjecutando, READY);
            log_info(logger,"Se saco al entrenador con ID %d de ejecucion para volver a planificar segun SJF con desalojo", entrenadorEjecutando->id);
            log_info(logger,"Se agrego al entrenador con ID %d a la cola de listos ya que se detectaron cambios en la cola de listos y hay que replanificar", entrenadorEjecutando->id);
            pthread_mutex_lock(&mutexEntrenadorEjecutando);
            entrenadorEjecutando = NULL;
            pthread_mutex_unlock(&mutexEntrenadorEjecutando);
            pthread_mutex_lock(&mutexCantidadCambiosDeContexto);
            cantidadCambiosDeContexto ++;
            pthread_mutex_unlock(&mutexCantidadCambiosDeContexto);
        }
    }
    //log_debug(logger,"Termino de ejecutar\n");

}

//////////////////////// Funciones auxiliares de entrenador ////////////////////////////////////////////////

bool puedeAtrapar(t_Entrenador* pEntrenador)
{
    if(pEntrenador->cuantosPuedeAtrapar > cantidadTotalDePokemonesEnLista(pEntrenador->pokemones))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool entrenadorCumplioObjetivos(t_Entrenador* pEntrenador)
{
    for(int i = 0; i < list_size(pEntrenador->objetivos); i++)
    {
        t_Pokemon* unPokemon = list_get(pEntrenador->objetivos, i);
        if(unPokemon->cantidad != cantidadDeUnPokemonEnLista(pEntrenador->pokemones, unPokemon->nombre))
        //Ver si un entrenador puede cumplir sus objetivos si tiene mas cantidad de la necesaria
        {
            return false;
        }
    }
    return true;
}

bool estaBloqueadoPorRecursos(t_Entrenador* pEntrenador)
{
    if(!puedeAtrapar(pEntrenador) && pEntrenador->objetivo == Ninguno)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void entrenadorFinalizoSuTarea(t_Entrenador* pEntrenador)
{//Se llama cuando un entrenador termina con su objetivo actual
    estimarRafaga(pEntrenador);
    pEntrenador->objetivo = Ninguno;
    if(entrenadorCumplioObjetivos(pEntrenador))
    {
        finalizarEntrenador(pEntrenador);
    }
    else
    {
        bloquearEntrenador(pEntrenador);
        log_info(logger,"Se agrego al entrenador con ID %d a la cola de bloqueados a la espera de recibir un nuevo objetivo", pEntrenador->id);
    }
}

int posicionEntrenadorEnLista(t_list* pLista, int pId)
{
    for(int i = 0; i < list_size(pLista); i++)
    {
        t_Entrenador* unEntrenador = list_get(pLista,i);
        if(unEntrenador->id == pId)
        {
            return i;
        }  
    }
    return -1; //Si no existe
}

bool puedeDesbloquearse(t_Entrenador* pEntrenador)
{
    if(puedeAtrapar(pEntrenador) && pEntrenador->objetivo == Ninguno)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int cuantosLeFaltan(t_Entrenador* pEntrenador, char* pPokemon)
{//Se puede usar una sola funcion o dividir en 2 entre los q faltan y los que sobran
    int tiene;
    int precisa;
    tiene = cantidadDeUnPokemonEnLista(pEntrenador->pokemones, pPokemon);
    precisa = cantidadDeUnPokemonEnLista(pEntrenador->objetivos, pPokemon);
    return precisa - tiene;
}

char* cualEsElPrimerPokemonQuePrecisa(t_Entrenador* pEntrenador)
{
    for(int i = 0; i < list_size(pEntrenador->objetivos); i++)
    {
        t_Pokemon* unPokemon = list_get(pEntrenador->objetivos, i);
        if(cuantosLeFaltan(pEntrenador, unPokemon->nombre) > 0)
        {
            return unPokemon->nombre;
        }
    }
}

char* cualEsElPrimerPokemonQueLeSobra(t_Entrenador* pEntrenador)
{
    for(int i = 0; i < list_size(pEntrenador->pokemones); i++)
    {
        t_Pokemon* unPokemon = list_get(pEntrenador->pokemones, i);
        if(cuantosLeFaltan(pEntrenador, unPokemon->nombre) < 0)
        {
            return unPokemon->nombre;
        }
    }
}

//////////////////////// Funciones auxiliares de Team /////////////////////////////////////////////////////

bool teamCumplioObjetivos()
{
    pthread_mutex_lock(&mutexListaDeEntrenadores);
    int tamanioListaEntrenadores = list_size(listaDeEntrenadores);
    for(int i = 0; i < tamanioListaEntrenadores; i++)
    {
        t_Entrenador* unEntrenador = list_get(listaDeEntrenadores, i);
        if(unEntrenador->estado != 4)
        {
            pthread_mutex_unlock(&mutexListaDeEntrenadores);
            return false;
        }
    }
    pthread_mutex_unlock(&mutexListaDeEntrenadores);
    return true;
}

int calcularDistancia(int x1, int y1, int x2, int y2)
{	int distancia;
	distancia = abs(x2 - x1) + abs(y2 - y1);
	return distancia;
}

void estimarRafaga(t_Entrenador* pEntrenador)
{
    pEntrenador->rafagaAnterior = pEntrenador->rafagaActual;
    pEntrenador->rafagaActual = 0;
    pEntrenador->estimacionActual = (unTeamConfig->alpha * pEntrenador->estimacionAnterior / 1000) + ((1 - (unTeamConfig->alpha)) * (pEntrenador->rafagaAnterior));
    pEntrenador->estimacionAnterior = pEntrenador->estimacionActual;
}

int entrenadorConMenorEstimacion()
{
    int posicion = 0;
    float estimacionMenor;
    t_Entrenador* unEntrenador;
    pthread_mutex_lock(&mutexListos);
    bool listaVacia = list_is_empty(LISTOS);
    if(!listaVacia)
    {
        unEntrenador = list_get(LISTOS, posicion);
        estimacionMenor = unEntrenador->estimacionActual;
    }
    else
    {
        //log_error(logger,"No hay entrenadores en la cola de listos\n");
    }
    int tamanioLista = list_size(LISTOS);
    for(int i = 1; i < tamanioLista; i++)
    {
        unEntrenador = list_get(LISTOS, i);
        if(unEntrenador->estimacionActual < estimacionMenor)
        {
            estimacionMenor = unEntrenador->estimacionActual;
            posicion = i;
        }
    }
    pthread_mutex_unlock(&mutexListos);
    return posicion;
}

int cuantosPrecisaGlobalmente(char* pPokemon)
{
    pthread_mutex_lock(&mutexPokemonesObjetivos);
    int posicion = posicionPokeEnLista(pokemonesObjetivos, pPokemon);
    pthread_mutex_unlock(&mutexPokemonesObjetivos);
    if(posicion == -1)
    {
        return -1;
    }
    else
    {
        pthread_mutex_lock(&mutexPokemonesObjetivos);
        int objetivos  = cantidadDeUnPokemonEnLista(pokemonesObjetivos, pPokemon);
        pthread_mutex_unlock(&mutexPokemonesObjetivos);
        pthread_mutex_lock(&mutexPokemonesBuscandose);
        int buscandose = cantidadDeUnPokemonEnLista(pokemonesBuscandose, pPokemon);
        pthread_mutex_unlock(&mutexPokemonesBuscandose);
        pthread_mutex_lock(&mutexPokemonesAtrapados);
        int atrapados  = cantidadDeUnPokemonEnLista(pokemonesAtrapados, pPokemon);
        pthread_mutex_unlock(&mutexPokemonesAtrapados);
        int resultado = objetivos - buscandose - atrapados;
        if(resultado < 0)
        {
            //log_error(logger, "El proceso tiene mas pokemones que los que necesita de esa especie");
            return 0;
        }
        else
        {
            return resultado;
        }
    }
}

t_Entrenador* entrenadorMasCercano(int posXpokemon, int posYpokemon, char* pPokemonNombre, int pPokemonCantidad)
{
    int idEntrenadorNew;
    int idEntrenadorBlock;
    int distanciaMenorNew;
    int distanciaMenorBlock;
	int distancia;

    pthread_mutex_lock(&mutexNuevos);
    int tamanioListaNuevos = list_size(NUEVOS);

    if(tamanioListaNuevos > 0)
    {
        t_Entrenador* unEntrenador = list_get(NUEVOS, 0);
        
        distanciaMenorNew = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
        
        idEntrenadorNew = unEntrenador->id;
    }
    else
    {
        distanciaMenorNew = 100000;
    }

	for(int i = 1; i < tamanioListaNuevos; i++)
    {
        t_Entrenador* unEntrenador = list_get(NUEVOS, i);

        distancia = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
        
        if(distancia < distanciaMenorNew)
        {
            distanciaMenorNew = distancia;
            idEntrenadorNew = unEntrenador->id;
        }
	}

    pthread_mutex_lock(&mutexBloqueados);
    int tamanioListaBloqueados = list_size(BLOQUEADOS);

	
    if(list_size(BLOQUEADOS) > 0)
    {
        t_Entrenador* unEntrenador = list_get(BLOQUEADOS, 0);
        if(puedeDesbloquearse(unEntrenador))
        {
            distanciaMenorBlock = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
            idEntrenadorBlock = unEntrenador->id;
        }
        else
        {
            distanciaMenorBlock = 100000;
        }
    }
    else
    {
        distanciaMenorBlock = 100000;
    }

	for(int i = 1; i < list_size(BLOQUEADOS); i++)
    {
        t_Entrenador* unEntrenador = list_get(BLOQUEADOS, i);
        if(puedeDesbloquearse(unEntrenador))
        {
            distancia = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);

            if(distancia < distanciaMenorBlock)
            {
                distanciaMenorBlock = distancia;
                idEntrenadorBlock = unEntrenador->id;
            }
        }
	}

    if(distanciaMenorNew == 100000 && distanciaMenorBlock == 100000) //Se puede sacar
    {
        //log_error(logger, "No hay entrenadores que puedan pasar a ready");
        pthread_mutex_unlock(&mutexNuevos);
        pthread_mutex_unlock(&mutexBloqueados);
        return NULL;
    }
    else
    {
        t_Entrenador* entrenadorRetorno;
        if(distanciaMenorNew < distanciaMenorBlock) //Por teoria deberia tener prioridad el q vuelve de block
        {
            int posicion = posicionEntrenadorEnLista(NUEVOS, idEntrenadorNew);
            entrenadorRetorno = list_remove(NUEVOS, posicion);
            log_info(logger,"Se quito al entrenador con ID %d de la cola de nuevos por ser el mas cercano al objetivo", entrenadorRetorno->id);
        }
        else
        {
            int posicion = posicionEntrenadorEnLista(BLOQUEADOS, idEntrenadorBlock);
            entrenadorRetorno = list_remove(BLOQUEADOS, posicion);
            log_info(logger,"Se quito al entrenador con ID %d de la cola de bloqueados por ser el mas cercano al objetivo", entrenadorRetorno->id);
        }
        entrenadorRetorno->objetivoX = posXpokemon;
        entrenadorRetorno->objetivoY = posYpokemon;
        entrenadorRetorno->objetivo = BuscandoAtrapar;
        t_Pokemon* pokemonRetorno = entrenadorRetorno->objetivoPokemon;
        pokemonRetorno->nombre = string_new();
        string_append(&pokemonRetorno->nombre,pPokemonNombre);
        //pokemonRetorno->nombre = pPokemonNombre;
        pokemonRetorno->cantidad = pPokemonCantidad;
        entrenadorRetorno->objetivoPokemon = pokemonRetorno;
        for(int i = 0; i < entrenadorRetorno->objetivoPokemon->cantidad; i++)
        {
            pthread_mutex_lock(&mutexPokemonesBuscandose);
            agregarPokeALista(pokemonesBuscandose, entrenadorRetorno->objetivoPokemon->nombre);
            pthread_mutex_unlock(&mutexPokemonesBuscandose);
        }
        pthread_mutex_unlock(&mutexNuevos);
        pthread_mutex_unlock(&mutexBloqueados);
        return entrenadorRetorno;
    }
}

t_entrenadoresEnDeadlock* quienesEstanEnDeadlock()
{
    t_entrenadoresEnDeadlock* entrenadores = malloc(sizeof(t_entrenadoresEnDeadlock));
    t_list* listaDeadlock;
    listaDeadlock = list_create();
    pthread_mutex_lock(&mutexBloqueados);
    int tamanioListaBloqueados = list_size(BLOQUEADOS);
    for(int i = 0; i < tamanioListaBloqueados; i++)
    {
        t_Entrenador* unEntrenador = list_get(BLOQUEADOS, i);
        if(estaBloqueadoPorRecursos(unEntrenador))
        {
            list_add(listaDeadlock, unEntrenador);
        }
    }
    pthread_mutex_unlock(&mutexBloqueados);
    if(list_size(listaDeadlock) >= 2)
    {
        t_Entrenador* primerEntrenador = list_remove(listaDeadlock, 0);
        entrenadores->id1 = primerEntrenador->id;
        char* pokemonQuePrecisa = cualEsElPrimerPokemonQuePrecisa(primerEntrenador);
        int tamanioListaDeadlock = list_size(listaDeadlock);
        for(int i = 0; i < tamanioListaDeadlock; i++)
        {
            t_Entrenador* segundoEntrenador = list_remove(listaDeadlock, 0);
            if(cuantosLeFaltan(segundoEntrenador, pokemonQuePrecisa) < 0)
            {
                entrenadores->id2 = segundoEntrenador->id;
                segundoEntrenador->objetivo = EsperandoIntercambio;
                primerEntrenador->objetivo = BuscandoIntercambio;
                primerEntrenador->objetivoX = segundoEntrenador->posicionX;
                primerEntrenador->objetivoY = segundoEntrenador->posicionY;
                t_Pokemon* unPokemonObjetivo = primerEntrenador->objetivoPokemon;
                unPokemonObjetivo->nombre = string_new();
                string_append(&unPokemonObjetivo->nombre, pokemonQuePrecisa);
                unPokemonObjetivo->cantidad = 1;
                primerEntrenador->objetivoPokemon = unPokemonObjetivo;
                primerEntrenador->intercambioEntrenador = segundoEntrenador->id;
                pthread_mutex_lock(&mutexBloqueados);
                list_remove(BLOQUEADOS, posicionEntrenadorEnLista(BLOQUEADOS, primerEntrenador->id));
                log_info(logger,"Se quito al entrenador con ID %d de la cola de bloqueados para que pueda realizar el intercambio", primerEntrenador->id);
                pthread_mutex_unlock(&mutexBloqueados);
                cambiarEstado(primerEntrenador, READY);
                log_info(logger,"Se agrego al entrenador con ID %d a la cola de listos para que pueda realizar un intercambio e intente solucionar deadlock", primerEntrenador->id);
                free(listaDeadlock);
                return entrenadores;
            }
        }
    }
    else
    {
        entrenadores->id1 = 0;
        entrenadores->id2 = 0;
        free(listaDeadlock);
        return entrenadores;
    }
}

/*
bool hayDeadlock()
{
    t_entrenadoresEnDeadlock* entrenadores = quienesEstanEnDeadlock();
    if(entrenadores->id1 == 0 && entrenadores->id2 == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}*/

int entrenadorSiguiente(t_Pokemon* pPokemon)
{
    pthread_mutex_lock(&mutexBloqueados);
    for(int i = 0; i < list_size(BLOQUEADOS); i++)
    {
        t_Entrenador* otroEntrenador = list_get(BLOQUEADOS, i);
        if(estaBloqueadoPorRecursos(otroEntrenador) && (cuantosLeFaltan(otroEntrenador, pPokemon) < 0))
        {
            pthread_mutex_unlock(&mutexBloqueados);
            return otroEntrenador->id;
        }
    }
    pthread_mutex_unlock(&mutexBloqueados);
}

void calcularDeadlock()
{
    log_info(logger,"Inicia el algoritmo de deteccion");
    t_list* listaInter;
    listaInter = list_create();
    pthread_mutex_lock(&mutexBloqueados);
    int tamanioListaBloqueados = list_size(BLOQUEADOS);
    for(int i = 0; i < tamanioListaBloqueados; i++)
    {
        t_Entrenador* unEntrenador = list_get(BLOQUEADOS, i);
        if(estaBloqueadoPorRecursos(unEntrenador))
        {
            list_add(listaInter, unEntrenador);
        }
    }
    pthread_mutex_unlock(&mutexBloqueados);
    if(list_size(listaInter) < 2)
    {
        log_info(logger, "No hay deadlock");
    }
    else
    {
        while(list_size(listaInter) >= 2)
        {
            t_Entrenador* primerEntrenador = list_remove(listaInter, 0);
            int idInicial = primerEntrenador->id;
            char* pokemonQuePrecisa;
            pokemonQuePrecisa = cualEsElPrimerPokemonQuePrecisa(primerEntrenador);
            int proximoId;
            t_Entrenador* proximoEntrenador;
            proximoId = entrenadorSiguiente(pokemonQuePrecisa);
            int proximaPosicion;
            proximaPosicion = posicionEntrenadorEnLista(listaInter, proximoId);
            proximoEntrenador = list_remove(listaInter, proximaPosicion);
            while(idInicial != proximoId)
            {
                pokemonQuePrecisa = cualEsElPrimerPokemonQuePrecisa(proximoEntrenador);
                proximoId = entrenadorSiguiente(pokemonQuePrecisa);
                proximoEntrenador = list_remove(listaInter, posicionEntrenadorEnLista(listaInter, proximoId));
            }
            log_info(logger, "Hay deadlock");
            pthread_mutex_lock(&mutexCantidadDeadlocks);
            cantidadDeadlocks++;
            pthread_mutex_unlock(&mutexCantidadDeadlocks);
            quienesEstanEnDeadlock();
        }
    }
}

void intercambiar()
{
    log_trace(logger,"Se busca intercambiar entre el entrenador %d y el entrenador %d",entrenadorEjecutando->id,entrenadorEjecutando->intercambioEntrenador);
    entrenadorEjecutando->cpuIntercambio = 0;
    pthread_mutex_lock(&mutexBloqueados);
    t_Entrenador* segundoEntrenador = list_remove(BLOQUEADOS, posicionEntrenadorEnLista(BLOQUEADOS, entrenadorEjecutando->intercambioEntrenador));
    log_info(logger,"Se quito al entrenador con ID %d de la cola de bloqueados para que pueda intercambiar con el entrenador que propone el intercambio", segundoEntrenador->id);
    pthread_mutex_unlock(&mutexBloqueados);
    t_Pokemon* pokemonNecesitado = entrenadorEjecutando->objetivoPokemon;
    char* pokemonSobra = cualEsElPrimerPokemonQueLeSobra(entrenadorEjecutando);
    //printf("El pokemon que le sobra al primer entrenador es: %s\n", pokemonSobra);
    //printf("El pokemon que esta buscando el primer entrenador es: %s\n", pokemonNecesitado->nombre);
    agregarPokeALista(entrenadorEjecutando->pokemones, pokemonNecesitado->nombre);
    quitarPokeDeLista(segundoEntrenador->pokemones, pokemonNecesitado->nombre);
    agregarPokeALista(segundoEntrenador->pokemones, pokemonSobra);
    quitarPokeDeLista(entrenadorEjecutando->pokemones, pokemonSobra);
    if(entrenadorCumplioObjetivos(entrenadorEjecutando) && entrenadorCumplioObjetivos(segundoEntrenador)) //TO DO AND o OR???
    {
        //cantidadDeadlocks++;
        pthread_mutex_lock(&mutexCantidadDeadlocksResueltos);
        cantidadDeadlocksResueltos++;
        pthread_mutex_unlock(&mutexCantidadDeadlocksResueltos);
    }
    entrenadorFinalizoSuTarea(segundoEntrenador);
    entrenadorFinalizoSuTarea(entrenadorEjecutando);
    pthread_mutex_lock(&mutexCantidadDeadlocks);
    int cantDead = cantidadDeadlocks;
    pthread_mutex_unlock(&mutexCantidadDeadlocks);
    pthread_mutex_lock(&mutexCantidadDeadlocksResueltos);
    int cantResueltos = cantidadDeadlocksResueltos;
    pthread_mutex_unlock(&mutexCantidadDeadlocksResueltos);
    if(cantDead > cantResueltos){
        quienesEstanEnDeadlock();
    }
}

void atrapar()
{
    log_trace(logger,"Se busca atrapar el pokemon %s en la posicion [%d,%d]",entrenadorEjecutando->objetivoPokemon->nombre,entrenadorEjecutando->objetivoX,entrenadorEjecutando->objetivoY);

    pthread_mutex_lock(&mutexSocketBroker);

    brokerActivo = probarConexionSocket(socketBroker);

    if(brokerActivo > 0){

        t_catchPokemon* unCatchPokemon = malloc(sizeof(t_catchPokemon));

        unCatchPokemon->identificador = 0;
        unCatchPokemon->identificadorCorrelacional = 0;

        unCatchPokemon->nombrePokemon = string_new();
        string_append(&unCatchPokemon->nombrePokemon,entrenadorEjecutando->objetivoPokemon->nombre);

        unCatchPokemon->posicionEnElMapaX = entrenadorEjecutando->objetivoX;
        unCatchPokemon->posicionEnElMapaY = entrenadorEjecutando->objetivoY;

        if(entrenadorEjecutando && entrenadorEjecutando->objetivoPokemon){
        
        //log_debug(logger,"--EntrenadorEjecutando %d ",entrenadorEjecutando->estado);
        //log_debug(logger,"--objetivoCantidad %d ",entrenadorEjecutando->objetivoPokemon->cantidad);

        //log_debug(logger,"--objetivo nombre %s ",entrenadorEjecutando->objetivoPokemon->nombre);
        //log_debug(logger,"--catch pokemon Nombre %s ",unCatchPokemon->nombrePokemon);

        }
        
        
        int tamanioCatchPokemon = 0;

        enviarInt(socketBroker, 4);
        enviarPaquete(socketBroker, tCatchPokemon, unCatchPokemon, tamanioCatchPokemon);

        int resultado;
        int tipoResultado = 0;

        if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

            if(tipoResultado > 0){
                
                log_warning(logger,"Pókemon mandado a atrapar con éxito");

                entrenadorEjecutando->objetivo = EsperandoMensaje;
                cambiarEstado(entrenadorEjecutando,BLOCK);
                log_info(logger,"Se agrega al entrenador con ID %d a la cola de bloqueados mientras espera la respuesta a catchPokemon", entrenadorEjecutando->id);
                entrenadorEjecutando->identificadorCorrelacional = tipoResultado;
                
            }else if(tipoResultado == 0){

                log_warning(logger,"No se pudo intentar atrapar al pókemon");
                entrenadorFinalizoSuTarea(entrenadorEjecutando);

            }

        }else{

            log_error(logger,"Hubo un error al recibir el resultado del catchPokemon desde el Broker. Está desconectado. Se ejecuta el default.");
            
            agregarPokeALista(entrenadorEjecutando->pokemones, entrenadorEjecutando->objetivoPokemon->nombre);
            pthread_mutex_lock(&mutexPokemonesAtrapados);
            agregarPokeALista(pokemonesAtrapados, entrenadorEjecutando->objetivoPokemon->nombre);
            pthread_mutex_unlock(&mutexPokemonesAtrapados);
            pthread_mutex_lock(&mutexPokemonesBuscandose);
            quitarPokeDeLista(pokemonesBuscandose, entrenadorEjecutando->objetivoPokemon->nombre);
            
            pthread_mutex_unlock(&mutexPokemonesBuscandose);

            entrenadorEjecutando->objetivoPokemon->cantidad --;

            if(entrenadorEjecutando->objetivoPokemon->cantidad == 0){

                entrenadorFinalizoSuTarea(entrenadorEjecutando);

            }else{

                cambiarEstado(entrenadorEjecutando,READY);
                log_info(logger,"Se agrego al entrenador con ID %d a la cola de listos porque no termino de atrapar todos los pokemones que fue a buscar", entrenadorEjecutando->id);
                

            }

            brokerActivo = probarConexionSocket(socketBroker);
            
            //ANALIZAR DESCONEXION DEL BROKER Y VER QUÉ SE HACE
        }

        free(unCatchPokemon->nombrePokemon);
        free(unCatchPokemon);

    }else{

        log_error(logger, "Falló envio de catchPokemon al broker, se ejecuta la operación por default.");
        agregarPokeALista(entrenadorEjecutando->pokemones, entrenadorEjecutando->objetivoPokemon->nombre);
        pthread_mutex_lock(&mutexPokemonesAtrapados);
        agregarPokeALista(pokemonesAtrapados, entrenadorEjecutando->objetivoPokemon->nombre);
        pthread_mutex_unlock(&mutexPokemonesAtrapados);
        pthread_mutex_lock(&mutexPokemonesBuscandose);
        quitarPokeDeLista(pokemonesBuscandose, entrenadorEjecutando->objetivoPokemon->nombre);
        pthread_mutex_unlock(&mutexPokemonesBuscandose);
        entrenadorEjecutando->objetivoPokemon->cantidad --;

        if(entrenadorEjecutando->objetivoPokemon->cantidad == 0){

            entrenadorFinalizoSuTarea(entrenadorEjecutando);

        }else{

            cambiarEstado(entrenadorEjecutando,READY);
            log_info(logger,"Se agrego al entrenador con ID %d a la cola de listos porque no termino de atrapar todos los pokemones que fue a buscar", entrenadorEjecutando->id);

        }

    }

    pthread_mutex_unlock(&mutexSocketBroker);

}

void ejecutar(int pId){

    while(1)
    {
        pthread_mutex_lock(&mutexSemaforosEntrenador);
        sem_t* unSem = list_get(semaforosEntrenador, pId);
        pthread_mutex_unlock(&mutexSemaforosEntrenador);

        sem_wait(unSem);

        //log_warning(logger,"Paso el wait el hilo del entrenador: %d", pId);

        //sleep(unTeamConfig->retardoCicloCPU); //Puede que haya que ponerlo adentro del IF de entrenadorEjecutando

        pthread_mutex_lock(&mutexEntrenadorEjecutando);

        if(strcmp(unTeamConfig->algoritmoPlanificacion,"SJF-SD") == 0 || strcmp(unTeamConfig->algoritmoPlanificacion,"SJF-CD") == 0)
        {
            entrenadorEjecutando->rafagaActual++;
            entrenadorEjecutando->estimacionActual--;
        }

        entrenadorEjecutando->rafagasTotales++;

        pthread_mutex_lock(&mutexCantidadCiclosCPU);
        cantidadCiclosCPU ++;
        pthread_mutex_unlock(&mutexCantidadCiclosCPU);

        if(entrenadorEjecutando!=NULL){

            //log_trace(logger,"Hay un entrenador ejecutando");

            if(entrenadorEjecutando->objetivo == BuscandoAtrapar && !puedeAtrapar(entrenadorEjecutando))
            {
                //log_debug(logger,"El entrenador ya tiene su maximo de pokemones");

                pthread_mutex_lock(&mutexPokemonesBuscandose);
                quitarPokeDeLista(pokemonesBuscandose, entrenadorEjecutando->objetivoPokemon->nombre);
                pthread_mutex_unlock(&mutexPokemonesBuscandose);
                
                free(entrenadorEjecutando->objetivoPokemon->nombre);
                log_info(logger,"Se saco al entrenador con ID %d de ejecucion ya que termino su tarea", entrenadorEjecutando->id);
                entrenadorFinalizoSuTarea(entrenadorEjecutando);
                entrenadorEjecutando = NULL;
                pthread_mutex_lock(&mutexCantidadCambiosDeContexto);
                cantidadCambiosDeContexto ++; //PROBAR
                pthread_mutex_unlock(&mutexCantidadCambiosDeContexto);
            }
            else
            {
                //log_warning(logger,"Moviendo al entrenador, posición actual: %d | %d", entrenadorEjecutando->posicionX, entrenadorEjecutando->posicionY);

                if(entrenadorEjecutando->objetivoX != entrenadorEjecutando->posicionX){
                    moverEntrenadorEnX();
                } else if(entrenadorEjecutando->objetivoY != entrenadorEjecutando->posicionY){
                    moverEntrenadorEnY();
                } else {
                    //log_debug(logger,"El entrenador llegó a destino");
                    if(entrenadorEjecutando->objetivo == BuscandoAtrapar)
                    {
                        atrapar();
                        log_info(logger,"Se saco al entrenador con ID %d de ejecucion ya que termino su tarea", entrenadorEjecutando->id);
                        entrenadorEjecutando = NULL;
                        pthread_mutex_lock(&mutexCantidadCambiosDeContexto);
                        cantidadCambiosDeContexto ++; //PROBAR
                        pthread_mutex_unlock(&mutexCantidadCambiosDeContexto);
                    }
                    else
                    {
                        if(entrenadorEjecutando->cpuIntercambio == 4)
                        {
                            intercambiar();
                            log_info(logger,"Se saco al entrenador con ID %d de ejecucion ya que termino su tarea", entrenadorEjecutando->id);
                            entrenadorEjecutando = NULL;
                            pthread_mutex_lock(&mutexCantidadCambiosDeContexto);
                            cantidadCambiosDeContexto ++; //PROBAR
                            pthread_mutex_unlock(&mutexCantidadCambiosDeContexto);
                        }
                        else
                        {
                            log_trace(logger,"Se busca intercambiar entre el entrenador %d y el entrenador %d",entrenadorEjecutando->id,entrenadorEjecutando->intercambioEntrenador);
                            entrenadorEjecutando->cpuIntercambio++;
                        }
                    }
                }
            
            }
        
        }
        pthread_mutex_unlock(&mutexEntrenadorEjecutando);
        sem_post(semaforoTerminoEjecucion);
    }

}

void moverEntrenadorEnX(){

    int xActual = entrenadorEjecutando->posicionX;
    int xObjetivo = entrenadorEjecutando->objetivoX;

    if(xActual > xObjetivo){
        entrenadorEjecutando->posicionX--;
    } else {
        entrenadorEjecutando->posicionX++;
    }

    log_info(logger,"El entrenador %d se movio a la posicion [%d,%d]",entrenadorEjecutando->id,entrenadorEjecutando->posicionX,entrenadorEjecutando->posicionY);

}

void moverEntrenadorEnY(){

    int yActual = entrenadorEjecutando->posicionY;
    int yObjetivo = entrenadorEjecutando->objetivoY;

    if(yActual > yObjetivo){
        entrenadorEjecutando->posicionY--;
    } else {
        entrenadorEjecutando->posicionY++;
    }

    log_info(logger,"El entrenador %d se movio a la posicion [%d,%d]",entrenadorEjecutando->id,entrenadorEjecutando->posicionX,entrenadorEjecutando->posicionY);

}

void actualizarConexionConBroker(){

    int resultado;
    int tipoResultado = 0;
    int tamanioSuscriptor = 0;

    while (1)
    {

        sleep(unTeamConfig->tiempoReconexion);

        //log_debug(logger, "Variable brokerActivo: %d", brokerActivo);

        pthread_mutex_lock(&mutexSocketBroker);

        brokerActivo = probarConexionSocket(socketBroker);

        //log_debug(logger, "Variable brokerActivo luego de testeo de conexión: %d", brokerActivo);

        if (brokerActivo <= 0){

            log_warning(logger, "BROKER DESCONECTADO. INTENTANDO RECONEXIÓN.");

            socketBroker = cliente(unTeamConfig->ipBroker, unTeamConfig->puertoBroker, ID_BROKER);

            if (socketBroker > 0){

                log_trace(logger, "BROKER RECONECTADO");

                t_suscriptor *unSuscriptor = malloc(sizeof(t_suscriptor));

                unSuscriptor->identificador = 0;
                unSuscriptor->identificadorCorrelacional = 0;

                unSuscriptor->colaDeMensajes = tAppearedPokemon;

                unSuscriptor->tiempoDeSuscripcion = 0;
                unSuscriptor->puerto = unTeamConfig->puertoTeam;

                unSuscriptor->ip = string_new();
                string_append(&unSuscriptor->ip, unTeamConfig->ipTeam);

                enviarInt(socketBroker, 4);
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

                log_error(logger, "INTENTO DE RECONEXIÓN FALLIDO. REINTENTO EN %d SEGUNDOS.", unTeamConfig->tiempoReconexion);

            }
        
        }/*else{

            log_debug(logger, "El Broker continúa conectado");

        }
        */
        pthread_mutex_unlock(&mutexSocketBroker);

    }

    return;
}