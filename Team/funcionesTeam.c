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
    //printf(unTeamConfig->posicionEntrenadores);
    cargarEntrenadores();
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

void cargarEntrenadores()
{
    int cantidadDeEntrenadores = 0;
    //cantidadDeEntrenadores = 0;
    printf("\n%d\n",cantidadDeEntrenadores);
    for (int i = 0; unTeamConfig->posicionEntrenadores[i] != NULL; i++)
	{
        if(string_ends_with(unTeamConfig->posicionEntrenadores[i],"]"))
            cantidadDeEntrenadores++;
    }
    int auxPosiciones = 1;
    printf("\n%d\n",cantidadDeEntrenadores);
    /*
    while(auxPosiciones <= cantidadDeEntrenadores)
    {
        t_Entrenador *unEntrenador = malloc(sizeof(t_Entrenador));
        unEntrenador->id = auxPosiciones;
        bool mismoEntrenador = true;
		for(int i=0;i<2;i++)
        {
            printf("\n%s\n",unTeamConfig->posicionEntrenadores[0]);
            if(string_starts_with(unTeamConfig->posicionEntrenadores[0],"["))
            {    
                unEntrenador->posicionX = atoi(string_substring_from(unTeamConfig->posicionEntrenadores[0],1));
                list_remove(unTeamConfig->posicionEntrenadores,0);
            }
            else if(string_ends_with(unTeamConfig->posicionEntrenadores[0],"]"))
            {
                unEntrenador->posicionY = atoi(string_substring_until(unTeamConfig->posicionEntrenadores[0],strlen(unTeamConfig->posicionEntrenadores[0])));
                list_remove(unTeamConfig->posicionEntrenadores,0);
                mismoEntrenador = false;
            }
        }
        unEntrenador->estado = NEW;
        //printf(unEntrenador->posicionX);
        //printf(unEntrenador->posicionY);
		list_add(listaDeEntrenadores, unEntrenador);
        auxPosiciones++;
    }
    */
        /*
		t_Entrenador *unEntrenador = malloc(sizeof(t_Entrenador));
		unEntrenador->id = i;
		unEntrenador->posicionX = atoi(unTeamConfig->posicionEntrenadores[i]);
		unEntrenador->posicionY = atoi(unTeamConfig->posicionEntrenadores[i]);
		unEntrenador->pokemons = list_create();
        unEntrenador->pokemons = list_create();
        unEntrenador->objetivos = list_create();
        unEntrenador->objetivos = list_create();
        unEntrenador->estado = NEW;
		list_add(listaDeEntrenadores, unEntrenador);
        */
}

//////////////////////// Cosas Comentadas /////////////////////////////////////////////////////////////////

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