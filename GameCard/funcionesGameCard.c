#include "gameCard.h"

void configurarLoggerGameCard() {

	logger = log_create(unGameCardConfig->logFile, "GAME CARD", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");

}

void cargarConfiguracionGameCard() {

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
        unGameCardConfig->logFile = config_get_string_value(unGameCardArchivoConfig, LOG_FILE);

        printf("\n\n· Tiempo de Reintento de Conexion = %d\n", unGameCardConfig->tiempoReintentoConexion);
        printf("· Tiempo de Reintento de Operacion = %d\n", unGameCardConfig->tiempoReintentoOperacion);
        printf("· Punto de Montaje de Tall Grass = %s\n", unGameCardConfig->puntoMontajeTallGrass);
        printf("· IP del Broker = %s\n", unGameCardConfig->ipBroker);
        printf("· Puerto del Broker = %s\n", unGameCardConfig->puertoBroker);
        printf("· Ruta del Archivo Log del Game Card = %s\n\n", unGameCardConfig->logFile);

        free(unGameCardArchivoConfig);

    }

}

void inicializarGameCard() {

    cantidadDeActualizacionesConfigGameCard = 0;

    cargarConfiguracionGameCard();

	configurarLoggerGameCard();

}

void finalizarGameCard() {

    free(unGameCardConfig);
    free(logger);
}

void administradorDeConexiones(void* infoAdmin){
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