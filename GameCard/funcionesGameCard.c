#include "gameCard.h"

void configurarLoggerGameCard(){

	logger = log_create(unGameCardConfig->logFile, "GAME CARD", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");

}

void cargarConfiguracionGameCard(){

    unGameCardConfig = malloc(sizeof(gameCardConfig_t));

    unGameCardArchivoConfig = malloc(sizeof(t_config));
	unGameCardArchivoConfig = config_create(PATH_CONFIG_GAMECARD);
	       
	if (unGameCardArchivoConfig == NULL) {

        printf("\n=============================================================================================\n");
	    printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL GAME CARD");

	}else{

        printf("\n=======================================================================================\n");
        printf("\nCONFIGURACION DEL GAME CARD IMPORTADA CON EXITO");

        unGameCardConfig->tiempoReintentoConexion = config_get_int_value(unGameCardArchivoConfig, TIEMPO_DE_REINTENTO_CONEXION);
        unGameCardConfig->tiempoReintentoOperacion = config_get_int_value(unGameCardArchivoConfig, TIEMPO_DE_REINTENTO_OPERACION);
        unGameCardConfig->puntoMontajeTallGrass = config_get_string_value(unGameCardArchivoConfig, PUNTO_MONTAJE_TALLGRASS);
        unGameCardConfig->ipBroker = config_get_string_value(unGameCardArchivoConfig, IP_BROKER);
        unGameCardConfig->puertoBroker = config_get_string_value(unGameCardArchivoConfig, PUERTO_BROKER);
        unGameCardConfig->ipGameCard = config_get_string_value(unGameCardArchivoConfig, IP_GAMECARD);
        unGameCardConfig->puertoGameCard = config_get_string_value(unGameCardArchivoConfig, PUERTO_GAMECARD);
        unGameCardConfig->logFile = config_get_string_value(unGameCardArchivoConfig, LOG_FILE);

        printf("\n\n· Tiempo de Reintento de Conexion = %d\n", unGameCardConfig->tiempoReintentoConexion);
        printf("· Tiempo de Reintento de Operacion = %d\n", unGameCardConfig->tiempoReintentoOperacion);
        printf("· Punto de Montaje de Tall Grass = %s\n", unGameCardConfig->puntoMontajeTallGrass);
        printf("· IP del Broker = %s\n", unGameCardConfig->ipBroker);
        printf("· Puerto del Broker = %s\n", unGameCardConfig->puertoBroker);
        printf("· IP del Game Card = %s\n", unGameCardConfig->ipGameCard);
        printf("· Puerto del Game Card = %s\n", unGameCardConfig->puertoGameCard);
        printf("· Ruta del Archivo Log del Game Card = %s\n\n", unGameCardConfig->logFile);

        char* pathMetadata = string_new();
        string_append_with_format(&pathMetadata,"%sMetadata/Metadata.bin", unGameCardConfig->puntoMontajeTallGrass);

        printf("· pathMetadata = %s\n\n", pathMetadata);

        configMetadata = config_create(pathMetadata);

        if(configMetadata == NULL) {
            //log_error(logger,"- NO SE PUDO IMPORTAR LA METADATA");
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

void inicializarGameCard(){

    cargarConfiguracionGameCard();

	configurarLoggerGameCard();
    
    inicializarBitMap();

    crearBloquesFileSystem();

    casoDePrueba();

    inicializarHilosYVariablesGameCard();


}

void finalizarGameCard(){

    free(unGameCardConfig);
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

void actualizarConfiguracionGameCard(){

    FILE *archivoConfigFp;

	while(1){

		sleep(10);

		archivoConfigFp = fopen(PATH_CONFIG_GAMECARD,"rb");

		nuevoIdConfigGameCard = 0;

		while (!feof(archivoConfigFp) && !ferror(archivoConfigFp)) {

   			nuevoIdConfigGameCard ^= fgetc(archivoConfigFp);

		}

        fclose(archivoConfigFp);


        if(cantidadDeActualizacionesConfigGameCard == 0){

        cantidadDeActualizacionesConfigGameCard += 1;

        }else{

            if (nuevoIdConfigGameCard != idConfigGameCard) {

                log_info(logger,"El archivo de configuración del Game Card cambió. Se procederá a actualizar.");
                cargarConfiguracionGameCard();
                cantidadDeActualizacionesConfigGameCard += 1;
                
            }

        }

        idConfigGameCard = nuevoIdConfigGameCard;

	}

}

void inicializarHilosYVariablesGameCard(){

    cantidadDeActualizacionesConfigGameCard = 0;

    socketBroker = cliente(unGameCardConfig->ipBroker, unGameCardConfig->puertoBroker, ID_BROKER);

    infoServidor_t* unaInfoServidorGameCard;

    unaInfoServidorGameCard = malloc(sizeof(infoServidor_t));

    unaInfoServidorGameCard->puerto = unGameCardConfig->puertoGameCard;
    unaInfoServidorGameCard->ip = string_new();
    //string_append(&unaInfoServidorGameCard->ip,unGameCardConfig->ipGameCard); PUEDE QUE HAYA QUE HACER ESTO CUANDO LO PROBEMOS EN LABORATORIO
    string_append(&unaInfoServidorGameCard->ip,"0");

    pthread_create(&hiloActualizadorConfigGameCard, NULL, (void*)actualizarConfiguracionGameCard, NULL);
    pthread_create(&hiloServidorGameCard,NULL,(void*)servidor_inicializar,(void*)unaInfoServidorGameCard);

    pthread_join(hiloActualizadorConfigGameCard, NULL);

}

void manejarRespuestaAGameBoy(int socketCliente,int idCliente){

    int* tipoMensaje = malloc(sizeof(int));
	int* tamanioMensaje = malloc(sizeof(int));

	void* buffer = recibirPaquete(socket, tipoMensaje, tamanioMensaje);

    switch(*tipoMensaje){

        case tNewPokemon: {

            /*
            
            Casteo de estructura (ejemplo): 
            
            t_newPokemon* unNewPokemon = (t_newPokemon*) buffer;

            */

            /*
            
            Logueo de lo recibido (ejemplo):

            log_info(logger,"El nombre del Pokemón es: %s",unNewPokemon->nombre);
            log_info(logger,"La posicion del Pokémon es: %d %d", unNewPokemon->posicion[0], unNewPokemon->posicion[1]);
            
            */

           /*
        
           Funciones que se invocan luego de recibir un NEW_POKEMON (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           darAGameCardPosicionesDePokemon(unNewPokemon->nombrePokemon, unNewPokemon->cantidad,unNewPokemon->posicion, idNuevoMensaje);
           recibirRespuestaGameCard(idNuevoMensaje);
           avisarATeamPokemonAparecido(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */
            
            break;
        }

        case tGetPokemon: {

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

           Funciones que se invocan luego de recibir un GET_POKEMON (ejemplo):

           int idNuevoMensaje = generarNuevoIdMensajeBroker();

           pedirAGameCardPosicionesDePokemon(unGetPokemon->nombrePokemon, idNuevoMensaje);
           recibirRespuestaGameCard(idNuevoMensaje);
           avisarATeamPokemonLocalizado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

            break;

        }

        case tCatchPokemon: {

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

            Funciones que se invocan luego de recibir un CATCH_POKEMON (ejemplo):

            int idNuevoMensaje = generarNuevoIdMensajeBroker();

            darAGameCardPokemonAtrapado(unCatchPokemon->nombrePokemon,unCatchPokemon->posicion, idNuevoMensaje);
            recibirRespuestaGameCard(idNuevoMensaje);
            avisarATeamPokemonAtrapado(unGetPokemon->nombrePokemon,idNuevoMensaje);

            */

            break;

        }

        default:{

            log_error(logger,"Recibimos algo del Game Boy que no sabemos manejar: %d",*tipoMensaje);
            abort();
            break;

        }

    }

    free(tipoMensaje);
    free(tamanioMensaje);
	free(buffer);

    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////BITMAP///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void inicializarBitMap(){


    char* pathBitmap = string_new();
    string_append_with_format(&pathBitmap,"%sMetadata/Bitmap.bin",unGameCardConfig->puntoMontajeTallGrass);
         
    if (validarArchivo(pathBitmap)){
        log_info(logger,"BITMAP EXISTENTE ENCONTRADO, SE CARGA EN MEMORIA");
        verificarBitmapPrevio(pathBitmap);
    } else {
        log_info(logger,"NO EXISTE UN BITMAP PREVIO, CREO UNO NUEVO");
        crearArchivoBitmap(pathBitmap);
        verificarBitmapPrevio(pathBitmap);
        
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void verificarBitmapPrevio(char* pathBitmap){
    
    int tamBitmap = unGameCardConfig->cantidadDeBloques/8;

	if(unGameCardConfig->cantidadDeBloques % 8 != 0){
		tamBitmap++;
	}

	int fd = open(pathBitmap, O_RDWR);
	ftruncate(fd, tamBitmap);
    
    struct stat mystat;

    if (fstat(fd, &mystat) < 0) {
		log_error(logger, "ERROR AL ESTABLECER FSTAT PARA BITMAP. SE CREARÁ UNO NUEVO.");
		close(fd);
        exit(-1);
	} else {
        char *bitmap = (char *) mmap(NULL, mystat.st_size, PROT_READ | PROT_WRITE,	MAP_SHARED, fd, 0);
        if (bitmap == MAP_FAILED) {
            log_error(logger, "ERROR AL MAPEAR BITMAP A MEMORIA. SE CREARÁ UNO NUEVO.");
            close(fd);
            //crearArchivoBitmap(pathBitmap);
        } else{
            
            bitarray = bitarray_create_with_mode(bitmap, tamBitmap, LSB_FIRST);
            close(fd);
        }
    }

    //free(pathBitmap);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void crearArchivoBitmap(char* pathBitmap){
    
    int tamBitarray = unGameCardConfig->cantidadDeBloques/8;
    int bit = 0;

    if(unGameCardConfig->cantidadDeBloques % 8 != 0){
        tamBitarray++;
    }

    char* data = malloc(tamBitarray);
    t_bitarray* bitarrayInicial = bitarray_create_with_mode(data,tamBitarray,MSB_FIRST);

    while(bit < unGameCardConfig->cantidadDeBloques){
        bitarray_clean_bit(bitarrayInicial, bit);
        bit ++;
    }

	FILE *bitmap;
	bitmap = fopen(pathBitmap, "wb+");

	//fseek(bitmap, 0, SEEK_SET);
	fwrite(bitarrayInicial->bitarray, 1, bitarrayInicial->size, bitmap);

    log_info(logger,"ARCHIVO BITMAP CREADO EN: %s", pathBitmap);

    bitarray_destroy(bitarrayInicial);
	fclose(bitmap);
	//free(pathBitmap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int validarArchivo(char *path) {
	struct stat buffer;
	int status;
	status = stat(path, &buffer);
	if (status < 0) {
		return 0;
	} else {
		return 1;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int buscarBloqueLibreYOcupar(){
    for(int i = 0; i< unGameCardConfig->cantidadDeBloques; i++){
        if(bitarray_test_bit(bitarray, i)==0){
            bitarray_set_bit(bitarray,i);
            return i;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int cantidadBloquesLibres(){

	int posicion = 0;
	int libres = 0;

    //pthread_mutex_lock(&mutexMemtable);
	while(posicion < unGameCardConfig->cantidadDeBloques){
		if(bitarray_test_bit(bitarray, posicion) == 0){
			libres ++;
		}
		posicion ++;
	}
    //pthread_mutex_unlock(&mutexMemtable);

	return libres;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mostrarEstadoBitmap(){
	int posicion = 0;
	bool a;

    //pthread_mutex_lock(&mutexMemtable);
    while(posicion < unGameCardConfig->cantidadDeBloques){
        if((posicion%10) == 0) printf ("\n");
		a = bitarray_test_bit(bitarray,posicion);
		printf("%i", a);
        posicion ++;
	}
    //pthread_mutex_unlock(&mutexMemtable);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////MANEJO BLOQUES/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void crearBloquesFileSystem(){

    char* pathBloques = string_new();
    string_append_with_format(&pathBloques, "%sBlocks/",unGameCardConfig->puntoMontajeTallGrass);

    //printf("\nPath bloques: %s ", pathBloques);
    //printf("\nCantidad de bloques a crear: %d ", unGameCardConfig->cantidadDeBloques);
    //printf("\n!(pathBloques,0777): %d ", !(pathBloques,0777));

    if (!mkdir(pathBloques,0777)){
       int i = 0;
       
        while(i<unGameCardConfig->cantidadDeBloques){
            FILE* bloque;
            char* pathBloquei = string_new();
            string_append_with_format(&pathBloquei, "%s%d.bin",pathBloques,i);
            bloque = fopen(pathBloquei, "wb+");
            fclose(bloque);
            free(pathBloquei);
            i++;
            printf("\nCreado bloque %d",i);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////MANEJO BLOQUES/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int existePokemon(char* pokemon){

	struct stat infoArchivo;
	char* path = string_new();
    string_append_with_format(&path,"%sFiles/%s",unGameCardConfig->puntoMontajeTallGrass, pokemon);
    
	if(stat(path,&infoArchivo) == 0 && S_ISDIR(infoArchivo.st_mode))
	{
		return 1; 
	}
	return 0; 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
int crearPokemon(char* pokemon, uint32_t posicionX, uint32_t posicionY, uint32_t cantidad){

    FILE* f;
    char* formatoUbicacion = string_new();
    char* pathPokemon      = string_new();
    char* pathMetadata     = string_new();
    
    string_append_with_format(&formatoUbicacion,"%d-%d=%d",posicionX, posicionY, cantidad);
    int tamanioPokemon = string_length(formatoUbicacion);

    string_append_with_format(&pathPokemon, "%sFiles/%s",unGameCardConfig->puntoMontajeTallGrass, pokemon);

    mkdir(pathPokemon,0777);

    string_append_with_format(&pathMetadata,"%s/Metadata.bin",pathPokemon);
    
    f = fopen(pathMetadata,"w");

    if(f == NULL){
    
        log_error(logger,"NO SE PUDO CREAR EL ARCHIVO METADATA PARA EL POKEMON %s", pokemon);
	
    }else{ 

        char* formatoDirectory = string_new();
        char* formatoSize      = string_new();
        char* formatoBlocks    = string_new();
        char* formatoOpen      = string_new();

        int cantBloquesAOcupar = cantBloquesParaSize(tamanioPokemon);
        int bloquesAEscribir[cantBloquesAOcupar];
        
        if(cantBloquesAOcupar <= cantidadBloquesLibres()){

            for(int j=0; j<cantBloquesAOcupar; j++ ){
                bloquesAEscribir[j] = buscarBloqueLibreYOcupar();
            }

            formatoBlocks = generarStringBlocks(cantBloquesAOcupar, bloquesAEscribir);

        } else{
            log_error(logger, "NO HAY BLOQUES SUFICIENTES PARA CREAR AL POKEMON");
            free(formatoDirectory);
            free(formatoSize);
            free(formatoBlocks);
            free(formatoOpen);
            return 0;
        }

        string_append(&formatoDirectory, "DIRECTORY=N");
        string_append_with_format(&formatoSize,"\nSIZE=%d\n", tamanioPokemon);
        string_append(&formatoDirectory, "\nOPEN=N");

        fputs(formatoDirectory,f);
        fputs(formatoSize,f);
        fputs(formatoBlocks,f);
        fputs(formatoOpen,f);
        fseek(f, 0, SEEK_SET);
        fclose(f);

        log_info(logger,"ARCHIVO METADATA DEL POKEMON %s CREADO CORRECTAMENTE", pokemon); 

        escribirEnBloques(formatoUbicacion,bloquesAEscribir,cantBloquesAOcupar);

        free(formatoDirectory);
        free(formatoSize);
        free(formatoBlocks);
        free(formatoOpen);
        return 1;
        
    }
}
*/
int crearPokemon(char* pokemon, uint32_t posicionX, uint32_t posicionY, uint32_t cantidad){

    char* pathPokemon = string_new();
    char* ubicacion   = string_new();

    string_append_with_format(&pathPokemon, "%sFiles/%s",unGameCardConfig->puntoMontajeTallGrass, pokemon);

    mkdir(pathPokemon,0777);

    string_append_with_format(&ubicacion,"%d-%d=%d\n",posicionX, posicionY, cantidad);
    int tamanioPokemon = string_length(ubicacion);

    return actualizarPokemon(pokemon, ubicacion, tamanioPokemon);

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int actualizarPokemon(char* pokemon, char* stringUbicaciones, int sizeUbicaciones){
    
    FILE* f;
    char* pathMetadata = string_new();

    string_append_with_format(&pathMetadata,"%sFiles/%s/Metadata.bin",unGameCardConfig->puntoMontajeTallGrass,pokemon);
    
    f = fopen(pathMetadata,"w");

    if(f == NULL){
    
        log_error(logger,"NO SE PUDO CREAR EL ARCHIVO METADATA PARA EL POKEMON %s", pokemon);
        return 0;
	
    }else{ 

        char* formatoDirectory = string_new();
        char* formatoSize      = string_new();
        char* formatoBlocks    = string_new();
        char* formatoOpen      = string_new();

        int cantBloquesAOcupar = cantBloquesParaSize(sizeUbicaciones);
        int bloquesAEscribir[cantBloquesAOcupar];
        
        if(cantBloquesAOcupar <= cantidadBloquesLibres()){

            for(int j=0; j<cantBloquesAOcupar; j++ ){
                bloquesAEscribir[j] = buscarBloqueLibreYOcupar();
            }

            formatoBlocks = generarStringBlocks(cantBloquesAOcupar, bloquesAEscribir);

        } else{
            log_error(logger, "NO HAY BLOQUES SUFICIENTES PARA CREAR AL POKEMON");
            free(formatoDirectory);
            free(formatoSize);
            free(formatoBlocks);
            free(formatoOpen);
            return 0;
        }

        string_append(&formatoDirectory, "DIRECTORY=N");
        string_append_with_format(&formatoSize,"\nSIZE=%d\n", sizeUbicaciones);
        string_append(&formatoDirectory, "\nOPEN=N");

        fputs(formatoDirectory,f);
        fputs(formatoSize,f);
        fputs(formatoBlocks,f);
        fputs(formatoOpen,f);
        fseek(f, 0, SEEK_SET);
        fclose(f);

        log_info(logger,"ARCHIVO METADATA DEL POKEMON %s CREADO CORRECTAMENTE", pokemon); 
        
        escribirEnBloques(stringUbicaciones,bloquesAEscribir,cantBloquesAOcupar);

        free(formatoDirectory);
        free(formatoSize);
        free(formatoBlocks);
        free(formatoOpen);

        return 1;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* leerUbicacionesPokemon(char* pokemon){


    // while(true){

    //     if(leerEstadoPokemon(pokemon) == 1){
        
    //         sleep(unGameCardConfig->tiempoReintentoOperacion);
        
    //     } else {

    //        cambiarEstadoPokemon(pokemon,1);

            char* pathMetadata = string_new();
            string_append_with_format(&pathMetadata, "%s/Files/%s/Metadata.bin",unGameCardConfig->puntoMontajeTallGrass,pokemon);

            t_config* configMetadata = config_create(pathMetadata);
            char** bloques = config_get_array_value(configMetadata, "BLOCKS");

            free(configMetadata);
            free(pathMetadata);

            int cantBloques;
            char* todasLasUbicaciones = string_new();
            
            for(cantBloques = 0; bloques[cantBloques] != NULL; cantBloques++){
                
                char* pathBloque = string_new();
                char* ubicacion = string_new();
                size_t tamLinea = 0;

                string_append_with_format(&pathBloque, "%s/Blocks/%s.bin",unGameCardConfig->puntoMontajeTallGrass,bloques[cantBloques]);
                FILE* f;
                f = fopen(pathBloque,"r");

                while(getline(&ubicacion,&tamLinea,f)>0){
                    string_append(&todasLasUbicaciones,ubicacion);
                }

                free(ubicacion);
                free(pathBloque);
                fclose(f);
            }

    //        cambiarEstadoPokemon(pokemon,0);

            free(bloques);

            return todasLasUbicaciones;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

t_list* generarListaUbicaciones(char* pokemon){

    t_list* listaUbicaciones = list_create();
    
    char* ubicacionesEnString = leerUbicacionesPokemon(pokemon);
    char** arregloUbicaciones = string_split(ubicacionesEnString,"\n");

    int cantUbicaciones;

    for(cantUbicaciones = 0; arregloUbicaciones[cantUbicaciones] != NULL; cantUbicaciones++){
        uint32_t posX;
        uint32_t posY;
        uint32_t cant;

        char** igualdad = string_split(arregloUbicaciones[cantUbicaciones],"=");
        char** posiciones = string_split(igualdad[0],"-");

        posX = (uint32_t) atoi(posiciones[0]);
        posY = (uint32_t) atoi(posiciones[1]);
        cant = (uint32_t) atoi(igualdad[1]);
        
        datosPokemon* ubicacion = malloc(sizeof(datosPokemon));
        ubicacion->posicionEnElMapaX = posX;
        ubicacion->posicionEnElMapaY = posY;
        ubicacion->cantidad  = cant;

        list_add(listaUbicaciones,ubicacion);
        
        free(igualdad);
        free(posiciones);

    }

    free(ubicacionesEnString);
    free(arregloUbicaciones);

    return listaUbicaciones;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int actualizarUbicacionPokemon(char* pokemon, uint32_t posX, uint32_t posY, int cant){
   
    busquedaX = posX;
    busquedaY = posY;

    t_list* listaUbicaciones = generarListaUbicaciones(pokemon);

    datosPokemon* ubicacionEncontrada = list_find(listaUbicaciones, (void*)mismaUbicacion);

    if(ubicacionEncontrada == NULL){

        list_destroy_and_destroy_elements(listaUbicaciones, eliminarNodoDatosPokemon);
        
        if(cant > 0){
            // añadir ubicacion a los bloques
            char* ubicacionesPokemon = leerUbicacionesPokemon(pokemon);
            char* nuevaUbicacion = string_new();
            string_append_with_format(&nuevaUbicacion,"%d-%d=%d\n",posX,posY,cant);
            string_append(&ubicacionesPokemon,nuevaUbicacion);
            int sizeUbicaciones = string_length(ubicacionesPokemon);
            liberarBloquesDelPokemon(pokemon);

            return actualizarPokemon(pokemon,ubicacionesPokemon,sizeUbicaciones);

        } else {
            log_error(logger,"NO SE ENCUENTRAN %s EN LA UBICACION %d-%d", pokemon,posX,posY);
            return 0;
        }

    } else {
        //actualizar ubicacion, reescribir bloques y actualizar metadata.

        ubicacionEncontrada->cantidad = ubicacionEncontrada->cantidad + cant; //probar bien esto, por ahí hay que castear
        char* nuevoStringUbicaciones = generarStringUbicacionesSegunLista(listaUbicaciones);
        int sizeUbicaciones = string_length(nuevoStringUbicaciones);
        liberarBloquesDelPokemon(pokemon);

        return actualizarPokemon(pokemon,nuevoStringUbicaciones,sizeUbicaciones);

    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* generarStringUbicacionesSegunLista(t_list* listaUbicaciones){

    //por ahi no tome la lista como una lista de datosPokemon de una, maybe castear
    int tamLista = list_size(listaUbicaciones);
    char* stringUbicaciones = string_new();

    for(int i = 0; i < tamLista; i++){
        datosPokemon* nodo = list_get(listaUbicaciones,i);
        string_append_with_format(&stringUbicaciones,"%d-%d=%d\n", nodo->posicionEnElMapaX, nodo->posicionEnElMapaY, nodo->cantidad);
    }

    return stringUbicaciones;


}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Devuelve 0 si ya existe el archivo ó si hubo algún error, 1 si salió bien
int crearDirectorio(char* path, char* nombreDirectorio){

    struct stat infoDirectorio;
    char* pathDirectorio = string_new();
    string_append_with_format(&pathDirectorio,"%s/%s",path,nombreDirectorio);

    if(stat(path,&infoDirectorio) == 0 && S_ISDIR(infoDirectorio.st_mode)){
        
        free(pathDirectorio);
        log_warning(logger,"YA EXISTE EL DIRECTORIO %s", pathDirectorio);
		return 0; 
	
    } else {

        mkdir(pathDirectorio,0777);

        FILE* f;
        
        char* pathMetadata = string_new();
        string_append_with_format(&pathMetadata,"%s/Metadata.bin",pathDirectorio);
    
        f = fopen(pathMetadata,"w");

        if(f == NULL){
            
            free(pathMetadata);
            log_error(logger,"NO SE PUDO CREAR EL ARCHIVO METADATA PARA EL DIRECTORIO %s", nombreDirectorio);
            return 0;
        
        }else{ 
            fputs("DIRECTORY=Y\0",f);
            fseek(f, 0, SEEK_SET);

            fclose(f);
            free(pathMetadata);
            log_info(logger,"SE CREÓ EL DIRECTORIO %s EN %s", nombreDirectorio, path);
            return 1;
        }
    
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int cantBloquesParaSize(int size){
    int cantBloques = size / unGameCardConfig->tamanioBloques;
    int resto = size % unGameCardConfig->tamanioBloques;

    if(resto > 0){
        cantBloques = cantBloques + 1 ;
    }

    return cantBloques;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* generarStringBlocks(int cantBloques, int bloquesAEscribir[]){
    
    char* stringBlocks = string_new();
    string_append(&stringBlocks,"BLOCKS=[");

    for(int j=0; j<cantBloques; j++ ){

        //pthread_mutex_lock(&mutexBitmap);
        //bloquesAEscribir[j] = buscarBloqueLibreYOcupar();
        //pthread_mutex_unlock(&mutexBitmap);
        if (j==(cantBloques-1)){
            string_append_with_format(&stringBlocks,"%d]",bloquesAEscribir[j]);
        }else{
            string_append_with_format(&stringBlocks,"%d,",bloquesAEscribir[j]);
        }
    }

    return stringBlocks;
      
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int escribirEnBloques(char* ubicaciones, int arregloBloques[], int cantBloques){
    
    int totalEscrito = 0;

    for (int i = 0; i<cantBloques-1;i++){
        char* pathBloque = string_new();
        string_append_with_format(&pathBloque,"%sBlocks/%d.bin",unGameCardConfig->puntoMontajeTallGrass,arregloBloques[i]);
        FILE* f;
        //fopen(pathBloque,f);
        char* escrituraPorBloque = string_substring(ubicaciones,totalEscrito,unGameCardConfig->tamanioBloques);
        f = fopen(pathBloque,"wb+");
        fputs(escrituraPorBloque,f);
        fclose(f);
        totalEscrito += string_length(escrituraPorBloque);
        free(pathBloque);
    }
    
    int restante = string_length(ubicaciones) - totalEscrito;
    
    char* ultimaPorcionAEscribir = string_substring(ubicaciones,totalEscrito,restante);
    char* pathUltimoBloque = string_new();

    string_append_with_format(&pathUltimoBloque,"%sBlocks/%d.bin",unGameCardConfig->puntoMontajeTallGrass,arregloBloques[cantBloques-1]);
    FILE* g;
    g = fopen(pathUltimoBloque,"wb+");
    fputs(ultimaPorcionAEscribir,g);
    fclose(g);

    free(pathUltimoBloque);
    free(ultimaPorcionAEscribir);
    free(ubicaciones);
    
    return 1;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void liberarBloquesDelPokemon(char* pokemon){
    
    char* pathMetadata = string_new();
    t_config* metadata;

    string_append_with_format(&pathMetadata,"%sFiles/%s/Metadata.bin" ,unGameCardConfig->puntoMontajeTallGrass, pokemon);

    log_info(logger, "INGRESANDO A LA METADATA DE PATH %s", pathMetadata);
    metadata = config_create(pathMetadata);
    
    free(pathMetadata);
    
    if (metadata == NULL) {

        free(metadata);
        log_error(logger,"ERROR: NO SE PUDO LEER LA INFORMACION DEL ARCHIVO");
        exit(1);

    }else{
        
        char** bloques = config_get_array_value(configMetadata, "BLOCKS");

        for(int i = 0; bloques[i] != NULL; i++){
            int bloqueALiberar = atoi(bloques[i]);
            //pthread_mutex_lock(&mutexBitmap);
            bitarray_clean_bit(bitarray,bloqueALiberar);
            //pthread_mutex_unlock(&mutexBitmap);
        }

        free(bloques);
        free(metadata);

    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//DEVUELVE 0 si OPEN=N, 1 si OPEN=Y, 2 si no existe
int leerEstadoPokemon(char* pokemon){

    if(existePokemon(pokemon)){

        char* pathMetadata = string_new();
        string_append_with_format(&pathMetadata,"%sFiles/%s/Metadata.bin" ,unGameCardConfig->puntoMontajeTallGrass, pokemon);
        //FILE* f;
        //f = fopen(pathMetadata,"r");//podría ser rb

        t_config* metadata;
        log_info(logger, "INGRESANDO A LA METADATA DE PATH %s", pathMetadata);
        metadata = config_create(pathMetadata);
        
        free(pathMetadata);
        
        if (metadata == NULL) {

            free(metadata);
            log_error(logger,"ERROR: NO SE PUDO LEER LA INFORMACION DEL ARCHIVO");
            exit(1);

        }else{
            
            char* estadoPokemon = config_get_string_value(metadata,"OPEN");
            if(!strcmp(estadoPokemon,"N")){
                free(metadata);
                free(estadoPokemon);
                return 0;
            } else {
                free(metadata);
                free(estadoPokemon);
                return 1;
            } 

        }

    } else return 2;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lo del estado se puede hacer dentro de la misma funcion, pero me pareció que iba a ser mejor separarlo así se puede controlar mejor el tema del "semaforo"
int cambiarEstadoPokemon(char* pokemon, int estado){

    char* pathMetadata = string_new();
    string_append_with_format(&pathMetadata,"%s/Files/%s/Metadata.bin");

    char* nuevoEstado = string_new();
    if(estado == 1){
        string_append(&nuevoEstado, "OPEN=Y\0");
    } else {
        string_append(&nuevoEstado, "OPEN=N\0");
    }

    FILE* f;
    f = fopen(pathMetadata,"r+");
    
    if(f = NULL){
        log_error(logger,"NO SE PUDO ABRIR EL ARCHIVO METADATA DEL POKEMON %s", pokemon);
        free(pathMetadata);
        free(nuevoEstado);
        exit(1);
    }else{ 
        char* linea     = string_new();
        char* contenido = string_new();
        size_t tamLinea = 0;

        for(int i = 0; i < 3; i++){
            getline(&linea,&tamLinea,f);
            string_append(&contenido,linea);
        }

        fclose(f);
        f = fopen(pathMetadata,"w");
        fputs(contenido,f);
        fputs(nuevoEstado,f);

        fclose(f);
        free(pathMetadata);
        free(linea);
        free(contenido);
        free(nuevoEstado);

        return 1;

    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void casoDePrueba(){
    if(crearPokemon("AlvaritoGUEI", 15, 24, 51)){
        printf("\nSo un cra");
    } else{

        printf("\nfalló, boludazo");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool mismaUbicacion(datosPokemon* ubicacion){
    if(ubicacion->posicionEnElMapaX == busquedaX && ubicacion->posicionEnElMapaY==busquedaY){
        return true;
    }else{
        return false;
    }
}