#include "team.h"

void configurarLoggerTeam() {

	logger = log_create(unTeamConfig->logFile, "TEAM", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");
}

void cargarConfiguracionTeam() {

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

void inicializarTeam() {

    cargarConfiguracionTeam();

	configurarLoggerTeam();

    inicializarHilosYVariablesTeam();

}

void finalizarTeam() {

    free(unTeamConfig);
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

void actualizarConfiguracionTeam(){

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

void inicializarHilosYVariablesTeam(){

    cantidadDeActualizacionesConfigTeam = 0;

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