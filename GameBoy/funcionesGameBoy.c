#include "gameBoy.h"

void configurarLoggerGameBoy() {

	logger = log_create(unGameBoyConfig->logFile, "GAME BOY", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");
}

void cargarConfiguracionGameBoy() {

    unGameBoyConfig = malloc(sizeof(gameBoyConfig_t));

    unGameBoyArchivoConfig = malloc(sizeof(t_config));
    unGameBoyArchivoConfig = config_create(PATH_CONFIG_GAMEBOY);

	if (unGameBoyArchivoConfig == NULL) {

        printf("\n=============================================================================================\n");
	    printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL GAME BOY");

	}else{

        printf("\n=======================================================================================\n");
        printf("\nCONFIGURACION DEL GAME BOY IMPORTADA CON EXITO");

        unGameBoyConfig->ipBroker = config_get_string_value(unGameBoyArchivoConfig, IP_BROKER);
        unGameBoyConfig->ipTeam = config_get_string_value(unGameBoyArchivoConfig, IP_TEAM);
        unGameBoyConfig->ipGameCard = config_get_string_value(unGameBoyArchivoConfig, IP_GAMECARD);
        unGameBoyConfig->puertoBroker = config_get_int_value(unGameBoyArchivoConfig, PUERTO_BROKER);
        unGameBoyConfig->puertoTeam = config_get_int_value(unGameBoyArchivoConfig, PUERTO_TEAM);
        unGameBoyConfig->puertoGameCard = config_get_int_value(unGameBoyArchivoConfig, PUERTO_GAMECARD);
        unGameBoyConfig->logFile = config_get_string_value(unGameBoyArchivoConfig, LOG_FILE);

        printf("\n\n· IP del Broker = %s\n", unGameBoyConfig->ipBroker);
        printf("· IP del Team = %s\n", unGameBoyConfig->ipTeam);
        printf("· IP del Game Card = %s\n", unGameBoyConfig->ipGameCard);
        printf("· Puerto del Broker = %d\n", unGameBoyConfig->puertoBroker);
        printf("· Puerto del Team = %d\n", unGameBoyConfig->puertoTeam);
        printf("· Puerto del Game Card = %d\n", unGameBoyConfig->puertoGameCard);
        printf("· Ruta del Archivo Log del Game Boy = %s\n\n", unGameBoyConfig->logFile);

        free(unGameBoyArchivoConfig);

    }

}

void inicializarGameBoy() {

    cantidadDeActualizacionesConfigGameBoy = 0;

    cargarConfiguracionGameBoy();

    configurarLoggerGameBoy();

}

void finalizarGameBoy() {

    free(unGameBoyConfig);
    free(logger);
}

void administradorDeConexiones(void* infoAdmin){
    return;
}

void actualizarConfiguracionGameBoy(){

    FILE *archivoConfigFp;

	while(1){

		sleep(10);

		archivoConfigFp = fopen(PATH_CONFIG_GAMEBOY,"rb");

		nuevoIdConfigGameBoy = 0;

		while (!feof(archivoConfigFp) && !ferror(archivoConfigFp)) {

   			nuevoIdConfigGameBoy ^= fgetc(archivoConfigFp);

		}

        fclose(archivoConfigFp);


        if(cantidadDeActualizacionesConfigGameBoy == 0){

        cantidadDeActualizacionesConfigGameBoy += 1;

        }else{

            if (nuevoIdConfigGameBoy != idConfigGameBoy) {

                log_info(logger,"El archivo de configuración del Game Boy cambió. Se procederá a actualizar.");
                cargarConfiguracionGameBoy();
                cantidadDeActualizacionesConfigGameBoy += 1;
                
            }

        }

        idConfigGameBoy = nuevoIdConfigGameBoy;

	}

}