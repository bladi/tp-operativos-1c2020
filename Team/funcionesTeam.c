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

            case 2: {

                manejarRespuestaAGameBoy(unaInfoAdmin->socketCliente,idCliente);
                break;

            }

            case 3: {

                manejarRespuestaABroker(unaInfoAdmin->socketCliente,idCliente);
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

        t_appearedPokemon *unAppeardPokemon = (t_appearedPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unAppeardPokemon->nombrePokemon);
        log_info(logger, "La posicion del Pokémon es: %d %d", unAppeardPokemon->posicionEnElMapaX, unAppeardPokemon->posicionEnElMapaY);

        enviarInt(socketCliente, 1); //Le avisamos al GameBoy que recibimos bien la solicitud.

        //Lógica de lo que hay que hacer con la info recibida:

        //EJ: intentar atrapar el pokemon, agregar el pokemon a lista de pokemons aparecidos, etc, etc.

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

    switch (*tipoMensaje)
    {

    case tAppearedPokemon:
    {
        log_trace(logger, "Llegó un APPEARD_POKEMON del Broker");

        t_appearedPokemon *unAppeardPokemon = (t_appearedPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unAppeardPokemon->nombrePokemon);
        log_info(logger, "La posicion del Pokémon es: %d %d", unAppeardPokemon->posicionEnElMapaX, unAppeardPokemon->posicionEnElMapaY);

        enviarInt(socketCliente, 1); //Le avisamos al Broker que recibimos bien la solicitud.

        //Lógica de lo que hay que hacer con la info recibida:

        //EJ: intentar atrapar el pokemon, agregar el pokemon a lista de pokemons aparecidos, etc, etc.

        break;
    }

    case tCaughtPokemon:
    {
        log_trace(logger, "Llegó un CAUGHT_POKEMON del Broker");

        t_caughtPokemon *unCaughtPokemon = (t_caughtPokemon *)buffer;

        enviarInt(socketCliente, 4); //Le avisamos al Broker que recibimos bien la solicitud.

        //Lógica de lo que hay que hacer con la info recibida:

        //EJ: Agregar el pokemon a lista de pokemons atrapados, etc, etc.

        break;
    }

    case tLocalizedPokemon:
    {
        log_trace(logger, "Llegó un LOCALIZED_POKEMON del Broker.");

        t_localizedPokemon *unLocalizedPokemon = (t_localizedPokemon *)buffer;

        log_info(logger, "El nombre del Pokemón es: %s", unLocalizedPokemon->nombrePokemon);

        int cantidadListaDatosPokemon = list_size(unLocalizedPokemon->listaDatosPokemon);

        int contadorito = 0;

        printf("\nIdentificador: %d", unLocalizedPokemon->identificador);
        printf("\nIdentificador Correlacional: %d", unLocalizedPokemon->identificadorCorrelacional);
        printf("\nNombre del Pokemón: %s", unLocalizedPokemon->nombrePokemon);

        datosPokemon *nodoDatosPokemon;

        while (contadorito < cantidadListaDatosPokemon)
        {

            nodoDatosPokemon = list_get(unLocalizedPokemon->listaDatosPokemon, contadorito);

            //entrenadorMasCercano(nodoDatosPokemon->posicionEnElMapaX,nodoDatosPokemon->posicionEnElMapaY); Tener en cuenta a futuro.

            printf("\nCantidad de pokemón en %d° ubicación: %d", contadorito, nodoDatosPokemon->cantidad);
            printf("\nUbicacion en 'x': %d", nodoDatosPokemon->posicionEnElMapaX);
            printf("\nUbicacion en 'y': %d\n", nodoDatosPokemon->posicionEnElMapaY);

            contadorito += 1;
        }

        enviarInt(socketCliente, 4);

        //Le avisamos al Broker que recibimos bien la solicitud.

        //Lógica de lo que hay que hacer con la info recibida:

        //EJ: Agregar todos los pokemon a la lista de pokemon aparecidos, etc, etc.

        break;
    }

    default:
    {

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
			break;
		}

		case BLOCK:
		{
			list_add(BLOQUEADOS, pEntrenador);
			break;
		}

        case EXEC:
		{
			entrenadorEjecutando = pEntrenador;
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

void pasarEntrenadorAReady(int posXpokemon,int posYpokemon)
{
    t_Entrenador* unEntrenador = entrenadorMasCercano(posXpokemon, posYpokemon);
    cambiarEstado(unEntrenador, READY);
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
    
    cargarEntrenadoresYListasGlobales();
    pruebasSanty();

    socketBroker = cliente(unTeamConfig->ipBroker, unTeamConfig->puertoBroker, ID_BROKER);

    infoServidor_t* unaInfoServidorTeam = malloc(sizeof(infoServidor_t));

    unaInfoServidorTeam->puerto = unTeamConfig->puertoTeam;
    unaInfoServidorTeam->ip = string_new();
    //string_append(&unaInfoServidorTeam->ip,unTeamConfig->ipTeam); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    string_append(&unaInfoServidorTeam->ip,"0");

    pthread_create(&hiloActualizadorConfigTeam, NULL, (void*)actualizarConfiguracionTeam, NULL);
    pthread_create(&hiloServidorTeam,NULL,(void*)servidor_inicializar,(void*)unaInfoServidorTeam);

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
        /*
        printf("Entrenador n°: %d \n", unEntrenador->id);
        printf("Posicion X: %d \n", unEntrenador->posicionX);
        printf("Posicion Y: %d \n", unEntrenador->posicionY);
        t_Pokemon* unPokemon = malloc(sizeof(t_Pokemon));
        unPokemon = list_get(unEntrenador->pokemones,0);
        printf("Primer pokemon que tiene: %s \n", unPokemon->nombre);
        printf("Cantidad de ese primer pokemon que tiene: %d \n", cantidadDeUnPokemonEnLista(unEntrenador->pokemones, unPokemon->nombre));
        t_Pokemon* otroPokemon = malloc(sizeof(t_Pokemon));
        otroPokemon = list_get(unEntrenador->objetivos,0);
        printf("Cantidad de objetivos: %d \n", unEntrenador->cuantosPuedeAtrapar);
        printf("Primer pokemon que necesita: %s \n", otroPokemon->nombre);
        printf("Cantidad de ese primer pokemon que necesita: %d \n", cantidadDeUnPokemonEnLista(unEntrenador->objetivos, otroPokemon->nombre));
        free(unPokemon);
        free(otroPokemon);
        */
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

    pasarEntrenadorAReady(6,6); //Tendria que cargar el 3er entrenador

    
    t_Entrenador* unEntrenador = list_get(LISTOS, 0);
    if(unEntrenador->id = 2)
    {
        printf("Funciono bien\n");
    }
    

    printf("Cantidad de entrenadores bloqueados: %d\n", list_size(BLOQUEADOS));
    planificar();
    printf("Cantidad de entrenadores bloqueados: %d\n", list_size(BLOQUEADOS));

    pasarEntrenadorAReady(6,6); //Tendria que cargar el 2do entrenador

    
    unEntrenador = list_get(LISTOS, 0);
    if(unEntrenador->id = 1)
    {
        printf("Funciono bien\n");
    }
    

    printf("Cantidad de entrenadores bloqueados: %d\n", list_size(BLOQUEADOS));
    planificar();
    printf("Cantidad de entrenadores bloqueados: %d\n", list_size(BLOQUEADOS));

    quienesEstanEnDeadlock();
    
    if(teamCumplioObjetivos())
    {
        printf("Se cumplieron todos los objetivos \n");
    }
    else
    {
        printf("Todavia no se cumplieron todos los objetivos \n");
    }
}

////////////////////////// Metricas ///////////////////////////////////////////////////////////////////////



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

void planificar()
{
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
}

void planificarFIFO()
{
    printf("Planifique FIFO\n");
    printf("Cantidad de entrenadores en ready: %d\n", list_size(LISTOS));
    t_Entrenador* unEntrenador = list_remove(LISTOS, 0);
    printf("Cantidad de entrenadores en ready: %d\n", list_size(LISTOS));
    cambiarEstado(unEntrenador, EXEC);
    //Hago lo que tenga que hacer el entrenador
    printf("Termino de ejecutar\n");
    entrenadorEjecutando = NULL;
    entrenadorFinalizoSuTarea(unEntrenador);
}

void planificarRR()
{
    printf("Planifique Round Robin\n");
}

void planificarSJF()
{
    printf("Planifique SJF sin desalojo\n");
}

void planificarSRT()
{
    printf("Planifique SJF con desalojo\n");
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
    if(!puedeAtrapar(pEntrenador) && !estaEsperando(pEntrenador))
    {
        return true;
    }
    else
    {
        false;
    }
}

bool estaEsperando(t_Entrenador* pEntrenador)
{//TO DO
    return false;
}

void entrenadorFinalizoSuTarea(t_Entrenador* pEntrenador)
{//Se llama cuando un entrenador termina con su objetivo actual
    if(entrenadorCumplioObjetivos(pEntrenador))
    {
        finalizarEntrenador(pEntrenador);
    }
    else
    {
        printf("Id del entrenador a bloquear: %d\n", pEntrenador->id);
        bloquearEntrenador(pEntrenador);
    }
}

int posicionEntrenadorEnLista(int pId, t_list* pLista)
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
    if(puedeAtrapar(pEntrenador) && !estaEsperando(pEntrenador))
    {
        return true;
    }
    else
    {
        return false;
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

t_Entrenador* entrenadorMasCercano(int posXpokemon,int posYpokemon)
{
    int idEntrenadorNew;
    int idEntrenadorBlock;
    int distanciaMenorNew;
    int distanciaMenorBlock;
	int distancia;

    if(list_size(NUEVOS) > 0)
    {
        t_Entrenador* unEntrenador = list_get(NUEVOS, 0);
        printf("----------------COLA DE NEW---------------------------");
        printf("\nEntrenador n°: %d \n", unEntrenador->id);
        printf("Posicion X: %d \n", unEntrenador->posicionX);
        printf("Posicion Y: %d \n", unEntrenador->posicionY);
        printf("-------------------------------------------");
        distanciaMenorNew = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
        printf("\nLa distancia al pokemon es: %d\n", distanciaMenorNew);
        idEntrenadorNew = unEntrenador->id;
    }
    else
    {
        distanciaMenorNew = 100000;
    }

	for(int i = 1; i < list_size(NUEVOS); i++)
    {
        t_Entrenador* unEntrenador = list_get(NUEVOS, i);

        printf("----------------COLA DE NEW---------------------------");
        printf("\nEntrenador n°: %d \n", unEntrenador->id);
        printf("Posicion X: %d \n", unEntrenador->posicionX);
        printf("Posicion Y: %d \n", unEntrenador->posicionY);
        printf("-------------------------------------------");
        distancia = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
        printf("\nLa distancia al pokemon es: %d\n", distancia);
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
            printf("----------------COLA DE BLOQUEADOS---------------------------");
            printf("\nEntrenador n°: %d \n", unEntrenador->id);
            printf("Posicion X: %d \n", unEntrenador->posicionX);
            printf("Posicion Y: %d \n", unEntrenador->posicionY);
            printf("-------------------------------------------");
            distanciaMenorBlock = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
            printf("\nLa distancia al pokemon es: %d\n", distanciaMenorBlock);
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
            printf("----------------COLA DE BLOQUEADOS---------------------------");
            printf("\nEntrenador n°: %d \n", unEntrenador->id);
            printf("Posicion X: %d \n", unEntrenador->posicionX);
            printf("Posicion Y: %d \n", unEntrenador->posicionY);
            printf("-------------------------------------------");
            distancia = calcularDistancia(unEntrenador->posicionX, unEntrenador->posicionY, posXpokemon, posYpokemon);
            printf("\nLa distancia al pokemon es: %d\n", distancia);
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
            int posicion = posicionEntrenadorEnLista(idEntrenadorNew, NUEVOS);
            entrenadorRetorno = list_remove(NUEVOS, posicion);
        }
        else
        {
            int posicion = posicionEntrenadorEnLista(idEntrenadorBlock, BLOQUEADOS);
            entrenadorRetorno = list_remove(BLOQUEADOS, posicion);
        }
        return entrenadorRetorno;
    }
}

t_entrenadoresEnDeadlock* quienesEstanEnDeadlock()
{
    t_entrenadoresEnDeadlock* entrenadores = malloc(sizeof(t_entrenadoresEnDeadlock));
    int cantidadDeadlock = 0;
    for(int i = 0; i < list_size(BLOQUEADOS); i++)
    {
        t_Entrenador* unEntrenador = list_get(BLOQUEADOS, i);
        if(estaBloqueadoPorRecursos(unEntrenador))
        {
            cantidadDeadlock++;
        }
    }    
    if(cantidadDeadlock >= 2)
    {

        //entrenadores->id1 = 0;
        //entrenadores->id2 = 0;
        printf("Hay deadlock\n");
        //for() recorro el primero con deadlock
        //for() recorro el segundo con deadlock y veo si tiene alguno de sobra que le sirva al primero
        return entrenadores;
    }
    else
    {
        entrenadores->id1 = 0;
        entrenadores->id2 = 0;
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


/*
int entrenadorMasCercano(int posXpokemon,int posYpokemon){//recibir por parametro los parametros del pokemon

	float distancia;
	
	float distanciaMenor=1000;
	int posEntrenador;
	for(int i = 0; i < list_size(NUEVOS); i++){
	           	t_Entrenador* unEntrenador = list_get(NUEVOS,i);

	           	printf("----------------COLA DE NEW---------------------------");
	           	printf("\nEntrenador n°: %d \n", unEntrenador->id);
	           	printf("Posicion X: %d \n", unEntrenador->posicionX);
	           	printf("Posicion Y: %d \n", unEntrenador->posicionY);
	           	printf("-------------------------------------------");
	           	distancia = calcularDistancia(unEntrenador->posicionX,unEntrenador->posicionY,posXpokemon,posYpokemon);
	        	printf("\nla distancia al pokemon es:%.2f\n",distancia);
	        	if(distancia<distanciaMenor){
	        		distanciaMenor=distancia;
	        		posEntrenador=unEntrenador->id;
	        	}
	 }
	//t_Entrenador* entrenadorCercano = list_get(NUEVOS,posEntrenador);
	//printf("\n\n --- El entrenador mas cercano es: %d y su distancia es de : %.2f---",entrenadorCercano->id, distanciaMenor);


	//list_add(LISTOS,entrenadorCercano); //agrego a la cola de LISTOS(Ready)
	return posEntrenador;
	/*
    //muestra los entrenadores que hay en LISTOS
	for(int i = 0; i < list_size(LISTOS); i++){
		t_Entrenador* otroEnt = list_get(LISTOS,i);
		printf("\n\n------------------COLA DE READY-------------------------");
		printf("\nEntrenador n°: %d \n", otroEnt->id);
		printf("Posicion X: %d \n", otroEnt->posicionX);
	    printf("Posicion Y: %d \n", otroEnt->posicionY);
	    printf("Estado:%s ",unEntrenador->estado);
	    printf("-------------------------------------------");
	}


}
*/