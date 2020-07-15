#include "team.h"

///////////////////////// Manejo de Configuracion y Logger ////////////////////////////////////////////////

void configurarLoggerTeam()
{

	logger = log_create(unTeamConfig->logFile, "TEAM", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");
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

                log_info(logger,"El archivo de configuración del Team cambió. Se procederá a actualizar.");
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

                log_info(logger, "RECIBIMOS UNA PRUEBA DE CONEXION");
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

        log_info(logger, "ME HICIERON UN PING\n\n\n");
        
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

        case tAppearedPokemon:
        {

            log_trace(logger, "Llegó un APPEARD_POKEMON del Game Boy");

                t_appearedPokemon *unAppearedPokemon = (t_appearedPokemon *)buffer;

                enviarInt(socketCliente, 1); //Le avisamos al Broker que recibimos bien la solicitud.

                //SEMAFORO (POSIBLE DENTRO DE FUNCION)

                int posicion = posicionPokeEnListaMapaSinPosicion(unAppearedPokemon->nombrePokemon);

                //FIN SEMAFORO (POSIBLE DENTRO DE FUNCION)

                if(posicion != -1){

                    log_info(logger, "El pokémon ya existía en el mapa. Nombre del Pokemón: %s - Posición en X: %d - Posición en Y: %d.", unAppearedPokemon->nombrePokemon, unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);
                    //t_posicionPokemon* unaPosicionPokemon = list_get(mapa, posicion);
                    //unaPosicionPokemon->pokemon->cantidad ++;

                }else{

                    log_info(logger, "El pokémon no existía en el mapa, procedemos a agregarlo. Nombre del Pokemón: %s - Posición en X: %d - Posición en Y: %d.", unAppearedPokemon->nombrePokemon, unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);
                    
                    t_posicionPokemon* unaPosicionPokemon = malloc(sizeof(t_posicionPokemon));

                    t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
                    unPokemon->nombre = string_new();

                    string_append(&unPokemon->nombre,unAppearedPokemon->nombrePokemon);
                    unPokemon->cantidad = 1;

                    unaPosicionPokemon->pokemon = unPokemon;
                    unaPosicionPokemon->posicionX = unAppearedPokemon->posicionEnElMapaX;
                    unaPosicionPokemon->posicionY = unAppearedPokemon->posicionEnElMapaY;

                    list_add(mapa, unaPosicionPokemon);

                    planificarReady(unAppearedPokemon->posicionEnElMapaX,unAppearedPokemon->posicionEnElMapaY,unAppearedPokemon->nombrePokemon,1);

                }

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
            
            log_trace(logger, "Llegó un APPEARD_POKEMON del Broker");

            t_appearedPokemon *unAppearedPokemon = (t_appearedPokemon *)buffer;

            enviarInt(socketCliente, 1); //Le avisamos al Broker que recibimos bien la solicitud.

            //SEMAFORO (POSIBLE DENTRO DE FUNCION)

            int posicion = posicionPokeEnListaMapaSinPosicion(unAppearedPokemon->nombrePokemon);

            //FIN SEMAFORO (POSIBLE DENTRO DE FUNCION)

            if(posicion != -1){

                log_info(logger, "El pokémon ya existía en el mapa. Nombre del Pokemón: %s - Posición en X: %d - Posición en Y: %d.", unAppearedPokemon->nombrePokemon, unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);
                //t_posicionPokemon* unaPosicionPokemon = list_get(mapa, posicion);
                //unaPosicionPokemon->pokemon->cantidad ++;

            }else{

                log_info(logger, "El pokémon no existía en el mapa, procedemos a agregarlo. Nombre del Pokemón: %s - Posición en X: %d - Posición en Y: %d.", unAppearedPokemon->nombrePokemon, unAppearedPokemon->posicionEnElMapaX, unAppearedPokemon->posicionEnElMapaY);
                
                t_posicionPokemon* unaPosicionPokemon = malloc(sizeof(t_posicionPokemon));

                t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
                unPokemon->nombre = string_new();

                string_append(&unPokemon->nombre,unAppearedPokemon->nombrePokemon);
                unPokemon->cantidad = 1;
                unaPosicionPokemon->pokemon = unPokemon;
                unaPosicionPokemon->posicionX = unAppearedPokemon->posicionEnElMapaX;
                unaPosicionPokemon->posicionY = unAppearedPokemon->posicionEnElMapaY;

                list_add(mapa, unaPosicionPokemon);

                planificarReady(unAppearedPokemon->posicionEnElMapaX,unAppearedPokemon->posicionEnElMapaY,unAppearedPokemon->nombrePokemon,1);

            }

            break;

        }

        case tCaughtPokemon:
        {
            log_trace(logger, "Llegó un CAUGHT_POKEMON del Broker");

            t_caughtPokemon *unCaughtPokemon = (t_caughtPokemon *)buffer;

            enviarInt(socketCliente, 4); //Le avisamos al Broker que recibimos bien la solicitud.

            int* contador = malloc(sizeof(int));

            *contador = 0;

            t_Entrenador* unEntrenador;

            log_warning(logger,"Antes del if");

            if(!list_is_empty(BLOQUEADOS)){

                log_warning(logger,"dentro if");

                unEntrenador = list_get(BLOQUEADOS,*contador);

                while(unEntrenador != NULL && unEntrenador->identificadorCorrelacional != unCaughtPokemon->identificadorCorrelacional){

                    *contador ++;
                    unEntrenador = list_get(BLOQUEADOS,*contador);

                }

                //log_warning(logger,"valor de entrenador id %d", unEntrenador->id);
                //log_warning(logger,"valor de entrenador id %d", unEntrenador->identificadorCorrelacional);


                if(unEntrenador && (unEntrenador->identificadorCorrelacional == unCaughtPokemon->identificadorCorrelacional)){

                    log_info("El entrenador: %d pudo atrapar correctamente al pokémon que fue a buscar.", unEntrenador->id);
                    agregarPokeALista(unEntrenador->pokemones, unCaughtPokemon->nombrePokemon);
                    agregarPokeALista(pokemonesAtrapados, unEntrenador->objetivoPokemon->nombre);
                    unEntrenador->objetivoPokemon->cantidad --;

                    if(unEntrenador->objetivoPokemon->cantidad == 0){

                        entrenadorFinalizoSuTarea(unEntrenador);

                    }else{

                        cambiarEstado(unEntrenador,READY);

                    }

                }else{

                    log_error(logger,"No se encontró al entrenador que estaba esperando el resultado del CATCH_POKEMON");

                }

            }else{

                log_warning(logger,"La lista de entrenadores esta vacía");

            }

            free(contador);

            break;

        }

        case tLocalizedPokemon:
        {
            log_trace(logger, "Llegó un LOCALIZED_POKEMON del Broker.");

            t_localizedPokemon *unLocalizedPokemon = (t_localizedPokemon *)buffer;

            enviarInt(socketCliente, 4);

            log_info(logger, "El nombre del Pokemón es: %s", unLocalizedPokemon->nombrePokemon);

            int cantidadListaDatosPokemon = list_size(unLocalizedPokemon->listaDatosPokemon);

            int contador = 0;

            int posicionEnLista = 0;

            log_info(logger,"Identificador: %d", unLocalizedPokemon->identificador);
            log_info(logger,"Identificador Correlacional: %d", unLocalizedPokemon->identificadorCorrelacional);
            log_info(logger,"Nombre del Pokemón: %s", unLocalizedPokemon->nombrePokemon);

            posicionEnLista = posicionPokeEnListaMapaSinPosicion(unLocalizedPokemon->nombrePokemon);

            if(posicionEnLista != -1){

                log_info(logger, "Ya existe el pokémon: %s en el mapa", unLocalizedPokemon->nombrePokemon);

            }else{

                log_info(logger, "El pokémon no existe en el mapa, se procederá a agregarlo", unLocalizedPokemon->nombrePokemon);

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

                    list_add(mapa, unaPosicionPokemon);

                    planificarReady(nodoDatosPokemon->posicionEnElMapaX,nodoDatosPokemon->posicionEnElMapaY,unLocalizedPokemon->nombrePokemon,nodoDatosPokemon->cantidad);

                    contador += 1;

                    log_info(logger,"Se agregaron %d %s en la posición: [%d,%d].", nodoDatosPokemon->cantidad, unLocalizedPokemon->nombrePokemon, nodoDatosPokemon->posicionEnElMapaX,nodoDatosPokemon->posicionEnElMapaY);

                }

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

///////////////////////// Funciones Auxiliares para Estados ///////////////////////////////////////////////

void cambiarEstado(t_Entrenador *pEntrenador, Estado pEstado)
{

    pEntrenador->estado = pEstado;
    switch(pEstado)
	{
		case READY:
		{
			list_add(LISTOS, pEntrenador);
            sem_post(semaforoPlanificador);
			break;
		}

		case BLOCK:
		{
			list_add(BLOQUEADOS, pEntrenador);
			break;
		}

        case EXEC:
		{
            cantidadCambiosDeContexto ++; //PROBAR
            pthread_mutex_lock(&mutexEntrenadorEjecutando);
			entrenadorEjecutando = pEntrenador;
            pthread_mutex_unlock(&mutexEntrenadorEjecutando);
			break;
		}

        case EXIT:
		{
			list_add(FINALIZADOS, pEntrenador);
			break;
		}

        default:
        {
            log_error(logger, "No se reconocio el estado indicado");
            break;
        }
    }
}

void bloquearEntrenador(t_Entrenador* pEntrenador)
{
    printf("El estado anterior era: %d\n", pEntrenador->estado);
    cambiarEstado(pEntrenador, BLOCK);
    printf("El estado nuevo es: %d\n", pEntrenador->estado);
}

void finalizarEntrenador(t_Entrenador* pEntrenador)
{
    cambiarEstado(pEntrenador, EXIT);
    if(teamCumplioObjetivos())
    {
        printf("Se cumplieron todos los objetivos \n");
    }
    else
    {
        printf("Todavia no se cumplieron todos los objetivos \n");
    }
}

///////////////////////// Inicio y Fin de Team ////////////////////////////////////////////////////////////

void inicializarTeam() {

    cargarConfiguracionTeam();

	configurarLoggerTeam();

    inicializarHilosYVariablesTeam();

}

void finalizarTeam() {

    free(unTeamConfig);
    free(logger);

}

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
    EJECUTANDO = list_create();
    FINALIZADOS = list_create();
    entrenadorEjecutando = NULL;
    pokemonesAtrapados = list_create();
    pokemonesObjetivos = list_create();
    mapa = list_create();

    cantidadEntrenadores = 0;
    cantidadCiclosCPU = 0;
    cantidadCambiosDeContexto = 0;
    cantidadDeadlocks = 0;
    cantidadDeadlocksResueltos = 0;

    cantidadEntrenadores = list_size(listaDeEntrenadores);

    pthread_create(&hiloCPU,NULL,(void*)ejecutar,NULL);
    semaforosEntrenador = list_create();
    
    cargarEntrenadoresYListasGlobales();

    semaforoPlanificador = malloc(sizeof(sem_t));
    sem_init(semaforoPlanificador, 0, 0);

    semaforoTerminoEjecucion = malloc(sizeof(sem_t));
    sem_init(semaforoTerminoEjecucion, 0, 0);

    pthread_create(&hiloPlanificador, NULL, (void*)planificarExec, NULL);
    
    //pthread_create(&hiloCPU,NULL,(void*)ejecutar,NULL);
    
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

            log_info(logger, "Pedido de suscripción realizado con éxito");

        }else if (tipoResultado == 0){

            log_info(logger, "No se pudo realizar el pedido de suscripción");

        }

    }else{

        log_error(logger, "Hubo un error al recibir el resultado de la operación desde el Broker");

    }

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
            agregarPokeALista(pokemonesAtrapados, pokemones[j]);
        }
        string_iterate_lines(pokemones, (void*) free);
        free(pokemones);
        char** objetivos = string_split(unTeamConfig->objetivosEntrenadores[i], "|");
        for (int j = 0; objetivos[j] != NULL; j++)
        {
            agregarPokeALista(unEntrenador->objetivos, objetivos[j]);
            agregarPokeALista(pokemonesObjetivos, objetivos[j]);
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
        
        sem_t* unSemaforo = malloc(sizeof(sem_t));
        sem_init(unSemaforo, 0, 0);

        pthread_mutex_lock(&mutexSemaforosEntrenador);
        list_add(semaforosEntrenador, unSemaforo);
        pthread_mutex_unlock(&mutexSemaforosEntrenador);

        char *hiloEntrenador = string_new();
        string_append_with_format(&hiloEntrenador, ",%d", unEntrenador->id);
        pthread_create(&hiloEntrenador, NULL, (void*)ejecutar, unEntrenador->id);

        list_add(listaDeEntrenadores, unEntrenador);
        list_add(NUEVOS, unEntrenador);

    }
}

////////////////////////////// Pruebas ////////////////////////////////////////////////////////////////////

void pruebasSanty()
{
    /*
    t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
    unPokemon = list_get(pokemonesAtrapados,0);
    printf("El primer pokemon atrapado global: %s \n", unPokemon->nombre);
    printf("La cantidad del primer pokemon atrapado global: %d \n", unPokemon->cantidad);
    free(unPokemon);
    unPokemon = malloc(sizeof(t_Pokemon));
    unPokemon = list_get(pokemonesObjetivos,0);
    printf("El primer pokemon objetivo global: %s \n", unPokemon->nombre);
    printf("La cantidad del primer pokemon objetivo global: %d \n", unPokemon->cantidad);
    free(unPokemon);
    */

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
    }

    planificarReady(2,2,"Charizard",1); //Tendria que cargar el 3er entrenador
    
    printf("SLEEEEEEEEEEEEEEEEEEEP\n");

    sleep(30);

    printf("SLEEEEEEEEEEEEEEEEEEEP\n");

    //planificarExec();
    
    //planificarExec();

    planificarReady(0,0,"Charizard",1); //Tendria que cargar el 1er entrenador

    sleep(3);

    planificarReady(1,0,"Charizard",1); //Tendria que cargar el 1er entrenador

    planificarReady(1,0,"Charizard",1);

    //planificarExec();

    //planificarExec();

    sleep(10);

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
    }

    printf("Termine la ejecucion del programa\n");
}

////////////////////////// Metricas ///////////////////////////////////////////////////////////////////////

void mostrarMetricas(){

    log_info(logger, "----------------------------------------MÉTRICAS----------------------------------------");
    log_info(logger, "Cantidad de entrenadores procesados: %d", cantidadEntrenadores);
    log_info(logger, "Cantidad de ciclos de CPU: %d", cantidadCiclosCPU);
    log_info(logger, "Cantidad de cambios de contexto: %d", cantidadCambiosDeContexto);
    log_info(logger, "Cantidad de cambios de deadlocks: %d",cantidadDeadlocks);
    log_info(logger, "Cantidad de cambios de deadlocks resueltos: %d",cantidadDeadlocksResueltos);
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

int posicionPokeEnListaMapa(char* nombrePokemon, int posicionEnX, int posicionEnY){


    for(int i = 0; i < list_size(mapa); i++){
        
        t_posicionPokemon* unaPosicionPokemon = list_get(mapa, i);

        if(!strcmp(unaPosicionPokemon->pokemon->nombre, nombrePokemon) && (unaPosicionPokemon->posicionX == posicionEnX) && (unaPosicionPokemon->posicionY == posicionEnY)){

            return i;

        }
        
    }

    return -1;

}

int posicionPokeEnListaMapaSinPosicion(char* nombrePokemon){

    for(int i = 0; i < list_size(mapa); i++){
        
        t_posicionPokemon* unaPosicionPokemon = list_get(mapa, i);

        if(!strcmp(unaPosicionPokemon->pokemon->nombre, nombrePokemon)){

            return i;

        }
        
    }

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
            list_remove(pLista, pPokemon);
        }
    }
    else
    {
        log_error(logger, "El entrenador no tiene el pokemon que iba a intercambiar por otro");
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
    if(list_size(NUEVOS) == 0 && list_size(BLOQUEADOS) == 0)
    {
        log_error(logger,"No hay entrenadores para planificar");
        return;
    }else{

        t_Entrenador* unEntrenador = entrenadorMasCercano(posXpokemon, posYpokemon, pPokemonNombre, pPokemonCantidad);
        cambiarEstado(unEntrenador, READY);
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
                log_error(logger, "No existe el algoritmo de planificacion especificado\n");
            }
            if(hayDeadlock())
            {
                t_entrenadoresEnDeadlock* entrenadoresEnDeadlock = quienesEstanEnDeadlock();
                printf("Hay deadlock entre el entrenador %d y el entrenador %d\n", entrenadoresEnDeadlock->id1, entrenadoresEnDeadlock->id2);
            }
    }
}

void planificarFIFO()
{
    printf("Planifique FIFO\n");
    t_Entrenador* unEntrenador = list_remove(LISTOS, 0);
    cambiarEstado(unEntrenador, EXEC);
    printf("Posicion de X a donde tiene que ir: %d\n", unEntrenador->objetivoX);
    printf("Posicion de Y a donde tiene que ir: %d\n", unEntrenador->objetivoY);
    while(entrenadorEjecutando != NULL){
        sem_post(list_get(semaforosEntrenador, entrenadorEjecutando->id));
        log_warning(logger, "Hay un entrenador en ejecución");
        sem_wait(semaforoTerminoEjecucion);
        sleep(unTeamConfig->retardoCicloCPU);
    }
    log_debug(logger,"Termino de ejecutar\n");
    //entrenadorFinalizoSuTarea(unEntrenador);
}

void planificarRR()
{
    printf("Planifique Round Robin\n");
    t_Entrenador* unEntrenador = list_remove(LISTOS, 0);
    cambiarEstado(unEntrenador, EXEC);
    printf("Posicion de X a donde tiene que ir: %d\n", unEntrenador->objetivoX);
    printf("Posicion de Y a donde tiene que ir: %d\n", unEntrenador->objetivoY);
    for(int i = 0; i < unTeamConfig->quantum; i++)
    {
        if(entrenadorEjecutando != NULL)
        {
            printf("ID del entrenador ejecutando: %d\n",entrenadorEjecutando->id);
            sem_post(list_get(semaforosEntrenador, entrenadorEjecutando->id));
            log_warning(logger, "Hay un entrenador en ejecución");
            printf("ID del entrenador ejecutando: %d\n",entrenadorEjecutando->id);
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
        entrenadorEjecutando = NULL;
    }
}

void planificarSJF()
{
    printf("Planifique SJF sin desalojo\n");
    t_Entrenador* unEntrenador = list_remove(LISTOS, entrenadorConMenorEstimacion());
    cambiarEstado(unEntrenador, EXEC);
    printf("Posicion de X a donde tiene que ir: %d\n", unEntrenador->objetivoX);
    printf("Posicion de Y a donde tiene que ir: %d\n", unEntrenador->objetivoY);
    while(entrenadorEjecutando != NULL){
        sem_post(list_get(semaforosEntrenador, entrenadorEjecutando->id));
        log_warning(logger, "Hay un entrenador en ejecución");
        sem_wait(semaforoTerminoEjecucion);
        sleep(unTeamConfig->retardoCicloCPU);
    }
    log_debug(logger,"Termino de ejecutar\n");
    //entrenadorFinalizoSuTarea(unEntrenador);
}

void planificarSRT()
{
    printf("Planifique SJF con desalojo\n");
    t_Entrenador* unEntrenador = list_remove(LISTOS, entrenadorConMenorEstimacion());
    cambiarEstado(unEntrenador, EXEC);
    int procesosEnListos = list_size(LISTOS);
    printf("Posicion de X a donde tiene que ir: %d\n", unEntrenador->objetivoX);
    printf("Posicion de Y a donde tiene que ir: %d\n", unEntrenador->objetivoY);
    while(entrenadorEjecutando != NULL){
        sem_post(list_get(semaforosEntrenador, entrenadorEjecutando->id));
        log_warning(logger, "Hay un entrenador en ejecución");
        sem_wait(semaforoTerminoEjecucion);
        sleep(unTeamConfig->retardoCicloCPU);
        if(procesosEnListos != list_size(LISTOS))
        {
            cambiarEstado(entrenadorEjecutando, READY);
            entrenadorEjecutando = NULL;
            //t_Entrenador* unEntrenador = list_remove(LISTOS, entrenadorConMenorEstimacion());
            //cambiarEstado(unEntrenador, EXEC);
        }
    }
    log_debug(logger,"Termino de ejecutar\n");
    //entrenadorFinalizoSuTarea(unEntrenador);

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
{//Se puede cambiar el funcionamiento para chequear que todos sus entrenadores esten en exit
    for(int i = 0; i < list_size(listaDeEntrenadores); i++)
    {
        t_Entrenador* unEntrenador = list_get(listaDeEntrenadores, i);
        if(!entrenadorCumplioObjetivos(unEntrenador))
        {
            return false;
        }
    }
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
    if(!list_is_empty(LISTOS))
    {
        unEntrenador = list_get(LISTOS, posicion);
        estimacionMenor = unEntrenador->estimacionActual;
    }
    else
    {
        log_error(logger,"No hay entrenadores en la cola de listos\n");
    }
    for(int i = 1; i < list_size(LISTOS); i++)
    {
        unEntrenador = list_get(LISTOS, i);
        if(unEntrenador->estimacionActual < estimacionMenor)
        {
            estimacionMenor = unEntrenador->estimacionActual;
            posicion = i;
        }
    }
    return posicion;
}

t_Entrenador* entrenadorMasCercano(int posXpokemon, int posYpokemon, char* pPokemonNombre, int pPokemonCantidad)
{
    int idEntrenadorNew;
    int idEntrenadorBlock;
    int distanciaMenorNew;
    int distanciaMenorBlock;
	int distancia;

    if(list_size(NUEVOS) > 0)
    {
        t_Entrenador* unEntrenador = list_get(NUEVOS, 0);
        //printf("----------------COLA DE NEW---------------------------");
        //printf("\nEntrenador n°: %d \n", unEntrenador->id);
        //printf("Posicion X: %d \n", unEntrenador->posicionX);
        //printf("Posicion Y: %d \n", unEntrenador->posicionY);
        //printf("-------------------------------------------");
        distanciaMenorNew = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
        //printf("\nLa distancia al pokemon es: %d\n", distanciaMenorNew);
        idEntrenadorNew = unEntrenador->id;
    }
    else
    {
        distanciaMenorNew = 100000;
    }

    //CHEQUEAR POSIBLE ERROR PARA LISTA DE NEW VACIA

	for(int i = 1; i < list_size(NUEVOS); i++)
    {
        t_Entrenador* unEntrenador = list_get(NUEVOS, i);

        //printf("----------------COLA DE NEW---------------------------");
        //printf("\nEntrenador n°: %d \n", unEntrenador->id);
        //printf("Posicion X: %d \n", unEntrenador->posicionX);
        //printf("Posicion Y: %d \n", unEntrenador->posicionY);
        //printf("-------------------------------------------");
        distancia = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
        //printf("\nLa distancia al pokemon es: %d\n", distancia);
        if(distancia < distanciaMenorNew)
        {
            distanciaMenorNew = distancia;
            idEntrenadorNew = unEntrenador->id;
        }
	}
	
    if(list_size(BLOQUEADOS) > 0)
    {
        t_Entrenador* unEntrenador = list_get(BLOQUEADOS, 0);
        if(puedeDesbloquearse(unEntrenador))
        {
            //printf("----------------COLA DE BLOQUEADOS---------------------------");
            //printf("\nEntrenador n°: %d \n", unEntrenador->id);
            //printf("Posicion X: %d \n", unEntrenador->posicionX);
            //printf("Posicion Y: %d \n", unEntrenador->posicionY);
            //printf("-------------------------------------------");
            distanciaMenorBlock = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
            //printf("\nLa distancia al pokemon es: %d\n", distanciaMenorBlock);
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

    //CHEQUEAR POSIBLE ERROR PARA LISTA DE BLOQUEADOS VACIA

	for(int i = 1; i < list_size(BLOQUEADOS); i++)
    {
        t_Entrenador* unEntrenador = list_get(BLOQUEADOS, i);
        if(puedeDesbloquearse(unEntrenador))
        {
            //printf("----------------COLA DE BLOQUEADOS---------------------------");
            //printf("\nEntrenador n°: %d \n", unEntrenador->id);
            //printf("Posicion X: %d \n", unEntrenador->posicionX);
            //printf("Posicion Y: %d \n", unEntrenador->posicionY);
            //printf("-------------------------------------------");
            distancia = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
            //printf("\nLa distancia al pokemon es: %d\n", distancia);
            if(distancia < distanciaMenorBlock)
            {
                distanciaMenorBlock = distancia;
                idEntrenadorBlock = unEntrenador->id;
            }
        }
	}

    if(distanciaMenorNew == 100000 && distanciaMenorBlock == 100000)
    {
        log_error(logger, "No hay entrenadores que puedan pasar a ready");
        return NULL;
    }
    else
    {
        t_Entrenador* entrenadorRetorno;
        if(distanciaMenorNew < distanciaMenorBlock) //Por teoria deberia tener prioridad el q vuelve de block
        {
            int posicion = posicionEntrenadorEnLista(NUEVOS, idEntrenadorNew);
            entrenadorRetorno = list_remove(NUEVOS, posicion);
        }
        else
        {
            int posicion = posicionEntrenadorEnLista(BLOQUEADOS, idEntrenadorBlock);
            entrenadorRetorno = list_remove(BLOQUEADOS, posicion);
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
        return entrenadorRetorno;
    }
}

t_entrenadoresEnDeadlock* quienesEstanEnDeadlock()
{
    t_entrenadoresEnDeadlock* entrenadores = malloc(sizeof(t_entrenadoresEnDeadlock));
    t_list* listaDeadlock;
    listaDeadlock = list_create();
    for(int i = 0; i < list_size(BLOQUEADOS); i++)
    {
        t_Entrenador* unEntrenador = list_get(BLOQUEADOS, i);
        if(estaBloqueadoPorRecursos(unEntrenador))
        {
            list_add(listaDeadlock, unEntrenador);
        }
    }
    printf("Cantidad de entrenadores en deadlock: %d\n", list_size(listaDeadlock));
    if(list_size(listaDeadlock) >= 2)
    {
        printf("Hay deadlock\n");
        t_Entrenador* primerEntrenador = list_remove(listaDeadlock, 0);
        entrenadores->id1 = primerEntrenador->id;
        //char* pokemonQuePrecisa = string_new();
        //pokemonQuePrecisa = cualEsElPrimerPokemonQuePrecisa(primerEntrenador);
        char* pokemonQuePrecisa = cualEsElPrimerPokemonQuePrecisa(primerEntrenador);
        printf("El pokemon que precisa es: %s\n", pokemonQuePrecisa);
        for(int i = 0; i < list_size(listaDeadlock); i++)
        {
            t_Entrenador* segundoEntrenador = list_remove(listaDeadlock, i);
            if(cuantosLeFaltan(segundoEntrenador, pokemonQuePrecisa) < 0)
            {
                entrenadores->id2 = segundoEntrenador->id;
                segundoEntrenador->objetivo = EsperandoIntercambio;
                primerEntrenador->objetivo = BuscandoIntercambio;
                primerEntrenador->objetivoX = segundoEntrenador->posicionX;
                primerEntrenador->objetivoY = segundoEntrenador->posicionY;
                t_Pokemon* unPokemonObjetivo = primerEntrenador->objetivoPokemon;
                //
                unPokemonObjetivo->nombre = string_new();
                string_append(&unPokemonObjetivo->nombre, pokemonQuePrecisa);
                //unPokemonObjetivo->nombre = pokemonQuePrecisa;
                unPokemonObjetivo->cantidad = 1;
                primerEntrenador->objetivoPokemon = unPokemonObjetivo;
                primerEntrenador->intercambioEntrenador = segundoEntrenador->id;
                list_remove(BLOQUEADOS, posicionEntrenadorEnLista(BLOQUEADOS, primerEntrenador->id));
                cambiarEstado(primerEntrenador, READY);
                //free(pokemonQuePrecisa);
                free(listaDeadlock);
                return entrenadores;
            }
        }
    }
    else
    {
        printf("No hay deadlock\n");
        entrenadores->id1 = 0;
        entrenadores->id2 = 0;
        free(listaDeadlock);
        return entrenadores;
    }
}

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
}

void intercambiar()
{
    t_Entrenador* segundoEntrenador = list_remove(BLOQUEADOS, posicionEntrenadorEnLista(BLOQUEADOS, entrenadorEjecutando->intercambioEntrenador));
    t_Pokemon* pokemonNecesitado = entrenadorEjecutando->objetivoPokemon;
    char* pokemonSobra = cualEsElPrimerPokemonQueLeSobra(entrenadorEjecutando);
    printf("El pokemon que le sobra al primer entrenador es: %s\n", pokemonSobra);
    printf("El pokemon que esta buscando el primer entrenador es: %s\n", pokemonNecesitado->nombre);
    for(int i = 0; i < pokemonNecesitado->cantidad; i++)
    {
        agregarPokeALista(entrenadorEjecutando->pokemones, pokemonNecesitado->nombre);
        quitarPokeDeLista(segundoEntrenador->pokemones, pokemonNecesitado->nombre);
    }
    agregarPokeALista(segundoEntrenador->pokemones, pokemonSobra);
    quitarPokeDeLista(entrenadorEjecutando->pokemones, pokemonSobra);
    entrenadorFinalizoSuTarea(segundoEntrenador);
    entrenadorFinalizoSuTarea(entrenadorEjecutando);
}

void atrapar()
{

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
        
        log_debug(logger,"--EntrenadorEjecutando %d ",entrenadorEjecutando->estado);
        log_debug(logger,"--objetivoCantidad %d ",entrenadorEjecutando->objetivoPokemon->cantidad);

        log_debug(logger,"--objetivo nombre %s ",entrenadorEjecutando->objetivoPokemon->nombre);
        log_debug(logger,"--catch pokemon Nombre %s ",unCatchPokemon->nombrePokemon);

        }
        
        
        int tamanioCatchPokemon = 0;

        enviarInt(socketBroker, 4);
        enviarPaquete(socketBroker, tCatchPokemon, unCatchPokemon, tamanioCatchPokemon);

        int resultado;
        int tipoResultado = 0;

        if((resultado = recibirInt(socketBroker,&tipoResultado)) > 0){

            if(tipoResultado > 0){
                
                log_info(logger,"Pókemon mandado a atrapar con éxito");

                //SEMAFORO?
                entrenadorEjecutando->objetivo = EsperandoMensaje;
                cambiarEstado(entrenadorEjecutando,BLOCK);
                entrenadorEjecutando->identificadorCorrelacional = tipoResultado;
                //FIN SEMAFORO?
                
            }else if(tipoResultado == 0){

                log_info(logger,"No se pudo intentar atrapar al pókemon");
                //SEMAFORO?
                entrenadorFinalizoSuTarea(entrenadorEjecutando);
                //FIN SEMAFORO?
                //ANALIZAR DESCONEXION DEL BROKER Y VER QUÉ SE HACE

            }

        }else{

            log_error(logger,"Hubo un error al recibir el resultado de la operación desde el Broker. Está desconectado.");
            //SEMAFORO?
            agregarPokeALista(entrenadorEjecutando->pokemones, entrenadorEjecutando->objetivoPokemon->nombre);
            agregarPokeALista(pokemonesAtrapados, entrenadorEjecutando->objetivoPokemon->nombre);
            entrenadorEjecutando->objetivoPokemon->cantidad --;

            if(entrenadorEjecutando->objetivoPokemon->cantidad == 0){

                entrenadorFinalizoSuTarea(entrenadorEjecutando);

            }else{

                cambiarEstado(entrenadorEjecutando,READY);

            }

            brokerActivo = probarConexionSocket(socketBroker);
            
            //FIN SEMAFORO?
            //ANALIZAR DESCONEXION DEL BROKER Y VER QUÉ SE HACE
        }

        free(unCatchPokemon->nombrePokemon);
        free(unCatchPokemon);

    }else{

        log_error(logger, "FALLÓ ENVÍO DE CATCH_POKEMON AL BROKER");

        agregarPokeALista(entrenadorEjecutando->pokemones, entrenadorEjecutando->objetivoPokemon->nombre);
        agregarPokeALista(pokemonesAtrapados, entrenadorEjecutando->objetivoPokemon->nombre);
        entrenadorEjecutando->objetivoPokemon->cantidad --;

        if(entrenadorEjecutando->objetivoPokemon->cantidad == 0){

            entrenadorFinalizoSuTarea(entrenadorEjecutando);

        }else{

            cambiarEstado(entrenadorEjecutando,READY);

        }

    }

    pthread_mutex_unlock(&mutexSocketBroker);

}

void ejecutar(int pId){

    while(1)
    {
        sem_wait(list_get(semaforosEntrenador, pId));

        printf("Paso el wait el hilo del entrenador: %d\n", pId);

        sleep(unTeamConfig->retardoCicloCPU); //Puede que haya que ponerlo adentro del IF de entrenadorEjecutando

        pthread_mutex_lock(&mutexEntrenadorEjecutando);
        
        if(strcmp(unTeamConfig->algoritmoPlanificacion,"SJF-SD") == 0 || strcmp(unTeamConfig->algoritmoPlanificacion,"SJF-CD") == 0)
        {
            entrenadorEjecutando->rafagaActual++;
            entrenadorEjecutando->estimacionActual--;
        }

        if(entrenadorEjecutando!=NULL){

            log_trace(logger,"Hay un entrenador ejecutando");

            if(entrenadorEjecutando->objetivo == BuscandoAtrapar && !puedeAtrapar(entrenadorEjecutando))
            {
                log_debug(logger,"El entrenador ya tiene su maximo de pokemones");
                //Cambiarle su estado interno
                entrenadorFinalizoSuTarea(entrenadorEjecutando);
                entrenadorEjecutando = NULL;

                cantidadCambiosDeContexto ++; //PROBAR
            }
            else
            {
                log_warning(logger,"Moviendo al entrenador, posición actual: %d | %d", entrenadorEjecutando->posicionX, entrenadorEjecutando->posicionY);

                if(entrenadorEjecutando->objetivoX != entrenadorEjecutando->posicionX){
                    moverEntrenadorEnX();
                } else if(entrenadorEjecutando->objetivoY != entrenadorEjecutando->posicionY){
                    moverEntrenadorEnY();
                } else {
                    log_debug(logger,"El entrenador llegó a destino");
                    if(entrenadorEjecutando->objetivo == BuscandoAtrapar)
                    {
                        atrapar();
                    }
                    else
                    {
                        intercambiar();
                    }
                    entrenadorEjecutando = NULL;
                    cantidadCambiosDeContexto ++; //PROBAR
                }
            
            }
        
        }
        pthread_mutex_unlock(&mutexEntrenadorEjecutando);

        sem_post(semaforoTerminoEjecucion);
    }

    cantidadCiclosCPU ++;

}

void moverEntrenadorEnX(){

    int xActual = entrenadorEjecutando->posicionX;
    int xObjetivo = entrenadorEjecutando->objetivoX;

    if(xActual > xObjetivo){
        entrenadorEjecutando->posicionX--;
    } else {
        entrenadorEjecutando->posicionX++;
    }

}

void moverEntrenadorEnY(){

    int yActual = entrenadorEjecutando->posicionY;
    int yObjetivo = entrenadorEjecutando->objetivoY;

    if(yActual > yObjetivo){
        entrenadorEjecutando->posicionY--;
    } else {
        entrenadorEjecutando->posicionY++;
    }

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