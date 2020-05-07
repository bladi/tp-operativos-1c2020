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
        //unTeamConfig->ipTeam = config_get_string_value(unTeamArchivoConfig, IP_TEAM);
        //unTeamConfig->puertoTeam = config_get_int_value(unTeamArchivoConfig, PUERTO_TEAM);
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
        //printf("· IP del Team = %s\n", unTeamConfig->ipTeam);
        //printf("· Puerto del Team = %d\n", unTeamConfig->puertoTeam);
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

                //manejarRespuestaAGameBoy(unaInfoAdmin->socketCliente,idCliente);
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

///////////////////////// Funciones Auxiliares para Estados ///////////////////////////////////////////////

void cambiarEstado(t_Entrenador *unEntrenador, Estado unEstado)
{
    switch(unEstado)
	{
		case READY:
		{
			unEntrenador->estado = READY;
			break;
		}
		
		case BLOCK:
		{
			unEntrenador->estado = BLOCK;
			break;
		}

		case EXEC:
		{
			unEntrenador->estado = EXEC;
			break;
		}

        case EXIT:
		{
			unEntrenador->estado = EXIT;
			break;
		}

		default:
		{
			break;
		}
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
    cantidadDeActualizacionesConfigTeam = 0;
    
    listaDeEntrenadores = list_create();
    NUEVOS = list_create();
    LISTOS = list_create();
    BLOQUEADOS = list_create();
    EJECUTANDO = list_create();
    FINALIZADOS = list_create();
    pokemonesAtrapados = list_create();
    pokemonesObjetivos = list_create();
    mapa = list_create();
    
    cargarEntrenadoresYListasGlobales();
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
    planificar();
    if(teamCumplioObjetivos())
    {
        printf("Se cumplieron todos los objetivos \n");
    }
    else
    {
        printf("Todavia no se cumplieron todos los objetivos \n");
    }
    //socketBroker = cliente(unTeamConfig->ipBroker, unTeamConfig->puertoBroker, ID_BROKER);

    // unaInfoServidorTeam = malloc(sizeof(infoServidor_t));

    // unaInfoServidorTeam->puerto = unTeamConfig->puertoTeam;
    // unaInfoServidorTeam->ip = string_new();
    // //string_append(&unaInfoServidorTeam->ip,unTeamConfig->ipTeam); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    // string_append(&unaInfoServidorTeam->ip,"0");

    //pthread_create(&hiloActualizadorConfigTeam, NULL, (void*)actualizarConfiguracionTeam, NULL);
    // pthread_create(&hiloServidorTeam,NULL,(void*)servidor_inicializar,(void*)unaInfoServidorTeam);

    //pthread_join(hiloActualizadorConfigTeam, NULL);

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
        list_add(listaDeEntrenadores, unEntrenador);
        list_add(NUEVOS, unEntrenador);
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
            return i;
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
    t_Pokemon* unPokemon = list_get(pLista, posicionPokeEnLista(pLista, pPokemon));
    return unPokemon->cantidad;
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

//////////////////////// Funciones auxiliares de Team /////////////////////////////////////////////////////

bool teamCumplioObjetivos()
{
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

//////////////////////// Cosas Comentadas /////////////////////////////////////////////////////////////////

/*void cargarEntrenadoresVersionAnterior()
{
    int cantidadDeEntrenadores = 0;
    for (int i = 0; unTeamConfig->posicionEntrenadores[i] != NULL; i++)
	{
        if(string_ends_with(unTeamConfig->posicionEntrenadores[i],"]"))
            cantidadDeEntrenadores++;
    }
    int idEntrenador = 1;
    int auxPosiciones = 0;
    int auxPokemones = 0;
    int auxObjetivos = 0;
    while(idEntrenador <= cantidadDeEntrenadores)
    {
        t_Entrenador *unEntrenador = malloc(sizeof(t_Entrenador));
        unEntrenador->pokemones = list_create();
        unEntrenador->objetivos = list_create();
        unEntrenador->id = idEntrenador;
		for(auxPosiciones;unTeamConfig->posicionEntrenadores[auxPosiciones] != NULL;auxPosiciones++)
        {
            if(string_starts_with(unTeamConfig->posicionEntrenadores[auxPosiciones],"["))
            {    
                unEntrenador->posicionX = atoi(string_substring_from(unTeamConfig->posicionEntrenadores[auxPosiciones],1));
            }
            else if(string_ends_with(unTeamConfig->posicionEntrenadores[auxPosiciones],"]"))
            {
                unEntrenador->posicionY = atoi(string_substring_until(unTeamConfig->posicionEntrenadores[auxPosiciones],strlen(unTeamConfig->posicionEntrenadores[auxPosiciones])-1));
                auxPosiciones++;
                break;
            }
        }
        for(auxPokemones; unTeamConfig->pokemonEntrenadores[auxPokemones] != NULL; auxPokemones++)
        {
            if(string_starts_with(unTeamConfig->pokemonEntrenadores[auxPokemones],"["))
            {    
                agregarPokeALista(unEntrenador->pokemones, (string_substring_from(unTeamConfig->pokemonEntrenadores[auxPokemones],1)));
            }
            else
            {
                if(string_ends_with(unTeamConfig->pokemonEntrenadores[auxPokemones],"]"))
                {
                    agregarPokeALista(unEntrenador->pokemones, (string_substring_until(unTeamConfig->pokemonEntrenadores[auxPokemones],strlen(unTeamConfig->pokemonEntrenadores[auxPokemones])-1)));
                    auxPokemones++;
                    break;
                }
                else
                {
                    agregarPokeALista(unEntrenador->pokemones, unTeamConfig->pokemonEntrenadores[auxPokemones]);
                }
            }
        }
        for(auxObjetivos; unTeamConfig->objetivosEntrenadores[auxObjetivos] != NULL; auxObjetivos++)
        {
            if(string_starts_with(unTeamConfig->objetivosEntrenadores[auxObjetivos],"["))
            {    
                agregarPokeALista(unEntrenador->objetivos, (string_substring_from(unTeamConfig->objetivosEntrenadores[auxObjetivos],1)));
            }
            else
            {
                if(string_ends_with(unTeamConfig->objetivosEntrenadores[auxObjetivos],"]"))
                {
                    agregarPokeALista(unEntrenador->objetivos, (string_substring_until(unTeamConfig->objetivosEntrenadores[auxObjetivos],strlen(unTeamConfig->objetivosEntrenadores[auxObjetivos])-1)));
                    auxObjetivos++;
                    break;
                }
                else
                {
                    agregarPokeALista(unEntrenador->objetivos, unTeamConfig->objetivosEntrenadores[auxObjetivos]);
                }
            }
        }
        unEntrenador->estado = NEW;
        unEntrenador->cuantosPuedeAtrapar = auxObjetivos;
		list_add(listaDeEntrenadores, unEntrenador);
        list_add(NUEVOS, unEntrenador);
        idEntrenador++;
    }
}*/

//void manejarRespuestaATeam(int socketCliente,int idCliente)
//{
//     int* tipoMensaje = malloc(sizeof(int));
// 	int* tamanioMensaje = malloc(sizeof(int));

// 	void* buffer = recibirPaquete(socket, tipoMensaje, tamanioMensaje);

//     switch(*tipoMensaje){

//         case tNewPokemon: {

//             /*
            
//             Casteo de estructura (ejemplo): 
            
//             t_newPokemon* unNewPokemon = (t_newPokemon*) buffer;

//             */

//             /*
            
//             Logueo de lo recibido (ejemplo):

//             log_info(logger,"El nombre del Pokemón es: %s",unNewPokemon->nombre);
//             log_info(logger,"La posicion del Pokémon es: %d %d", unNewPokemon->posicion[0], unNewPokemon->posicion[1]);
            
//             */

//            /*
        
//            Funciones que se invocan luego de recibir un NEW_POKEMON (ejemplo):

//            int idNuevoMensaje = generarNuevoIdMensajeBroker();

//            darAGameCardPosicionesDePokemon(unNewPokemon->nombrePokemon, unNewPokemon->cantidad,unNewPokemon->posicion, idNuevoMensaje);
//            recibirRespuestaGameCard(idNuevoMensaje);
//            avisarATeamPokemonAparecido(unGetPokemon->nombrePokemon,idNuevoMensaje);

//             */
            
//             break;
//         }

//         case tGetPokemon: {

//             /*
            
//             Casteo de estructura (ejemplo): 
            
//             t_getPokemon* unGetPokemon = (t_getPokemon*) buffer;

//             */

//             /*
            
//             Logueo de lo recibido (ejemplo):

//             log_info(logger,"El nombre del Pokemón es: %s",unGetPokemon->nombrePokemon);
//             log_info(logger,"Las posiciones del Pokémon son: %d %d", unGetPokemon->posicion[0], unGetPokemon->posicion[1]);
//             log_info(logger,"La cantidad que hay de ese Pokémon es: %d",unGetPokemon->cantidad);
            
//             */

//            /*

//            Funciones que se invocan luego de recibir un GET_POKEMON (ejemplo):

//            int idNuevoMensaje = generarNuevoIdMensajeBroker();

//            pedirAGameCardPosicionesDePokemon(unGetPokemon->nombrePokemon, idNuevoMensaje);
//            recibirRespuestaGameCard(idNuevoMensaje);
//            avisarATeamPokemonLocalizado(unGetPokemon->nombrePokemon,idNuevoMensaje);

//             */

//             break;

//         }

//         case tCatchPokemon: {

//             /*
            
//             Casteo de estructura (ejemplo): 
            
//             t_catchPokemon* unCatchPokemon = (t_catchPokemon*) buffer;

//             */

//             /*
            
//             Logueo de lo recibido (ejemplo):

//             log_info(logger,"El nombre del Pokemón es: %s",unCatchPokemon->nombrePokemon);
//             log_info(logger,"La posicion del Pokémon era: %d %d", unCatchPokemon->posicion[0], unCatchPokemon->posicion[1]);
//             log_info(logger,"El nombre del entrenador es: %s",unCatchPokemon->nombreEntrenador);
            
//             */

//             /*

//             Funciones que se invocan luego de recibir un CATCH_POKEMON (ejemplo):

//             int idNuevoMensaje = generarNuevoIdMensajeBroker();

//             darAGameCardPokemonAtrapado(unCatchPokemon->nombrePokemon,unCatchPokemon->posicion, idNuevoMensaje);
//             recibirRespuestaGameCard(idNuevoMensaje);
//             avisarATeamPokemonAtrapado(unGetPokemon->nombrePokemon,idNuevoMensaje);

//             */

//             break;

//         }

//         default:{

//             log_error(logger,"Recibimos algo del Game Boy que no sabemos manejar: %d",*tipoMensaje);
//             abort();
//             break;

//         }

//     }

//     free(tipoMensaje);
//     free(tamanioMensaje);
// 	free(buffer);

//     return;
//}

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
	/*//muestra los entrenadores que hay en LISTOS
	for(int i = 0; i < list_size(LISTOS); i++){
		t_Entrenador* otroEnt = list_get(LISTOS,i);
		printf("\n\n------------------COLA DE READY-------------------------");
		printf("\nEntrenador n°: %d \n", otroEnt->id);
		printf("Posicion X: %d \n", otroEnt->posicionX);
	    printf("Posicion Y: %d \n", otroEnt->posicionY);
	    printf("Estado:%s ",unEntrenador->estado);
	    printf("-------------------------------------------");
	}*/


}

float calcularDistancia(int x1,int y1,int x2,int y2)
{	float distancia;
	distancia = sqrt(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1)));
	return distancia;
}