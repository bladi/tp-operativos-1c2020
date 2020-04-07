#include "broker.h"

void configurarLoggerBroker() {

	logger = log_create(unBrokerConfig->logFile, "BROKER", true, LOG_LEVEL_TRACE);
    log_info(logger, "LOG INICIALIZADO CON EXITO");
}

void cargarConfiguracionBroker() {

    unBrokerConfig = malloc(sizeof(brokerConfig_t));

    unBrokerArchivoConfig = malloc(sizeof(t_config));

	unBrokerArchivoConfig = config_create(PATH_CONFIG_BROKER);
	    
	if (unBrokerArchivoConfig == NULL) {

        printf("\n=============================================================================================\n");
	    printf("\nNO SE PUDO IMPORTAR LA CONFIGURACION DEL BROKER");

	}else{

        printf("\n=======================================================================================\n");
        printf("\nCONFIGURACION DEL BROKER IMPORTADA CON EXITO");

        unBrokerConfig->tamanioMemoria = config_get_int_value(unBrokerArchivoConfig, TAMANO_MEMORIA);
        unBrokerConfig->tamanioMinimoParticion = config_get_int_value(unBrokerArchivoConfig, TAMANO_MINIMO_PARTICION);
        unBrokerConfig->algoritmoMemoria = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_MEMORIA);
        unBrokerConfig->algoritmoReemplazo = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_REEMPLAZO);
        unBrokerConfig->algoritmoParticionLibre = config_get_string_value(unBrokerArchivoConfig, ALGORITMO_PARTICION_LIBRE);
        unBrokerConfig->ipBroker = config_get_string_value(unBrokerArchivoConfig, IP_BROKER);
        unBrokerConfig->puertoBroker = config_get_int_value(unBrokerArchivoConfig, PUERTO_BROKER);
        unBrokerConfig->frecuenciaCompactacion = config_get_int_value(unBrokerArchivoConfig, FRECUENCIA_COMPACTACION);
        unBrokerConfig->logFile = config_get_string_value(unBrokerArchivoConfig, LOG_FILE);

        printf("\n\n· Tamanio de la Memoria = %d\n", unBrokerConfig->tamanioMemoria);
        printf("· Tamanio Minimo de Particion = %d\n", unBrokerConfig->tamanioMinimoParticion);
        printf("· Algoritmo de Memoria = %s\n", unBrokerConfig->algoritmoMemoria);
        printf("· Algoritmo de Reemplazo = %s\n", unBrokerConfig->algoritmoReemplazo);
        printf("· Algoritmo de Particion Libre = %s\n", unBrokerConfig->algoritmoParticionLibre);
        printf("· IP del Broker = %s\n", unBrokerConfig->ipBroker);
        printf("· Puerto del Broker = %d\n", unBrokerConfig->puertoBroker);
        printf("· Frecuencia de Compactacion = %d\n", unBrokerConfig->frecuenciaCompactacion);
        printf("· Ruta del Archivo Log del Broker = %s\n\n", unBrokerConfig->logFile);

        free(unBrokerArchivoConfig);

    }

}

void inicializarBroker() {

    cantidadDeActualizacionesConfigBroker = 0;

    cargarConfiguracionBroker();

    configurarLoggerBroker();

}

void finalizarBroker() {

    free(unBrokerConfig);
    free(logger);
}

void administradorDeConexiones(void* infoAdmin){
    return;
}

void actualizarConfiguracionBroker(){

    FILE *archivoConfigFp;

	while(1){

		sleep(10);

		archivoConfigFp = fopen(PATH_CONFIG_BROKER,"rb");

		nuevoIdConfigBroker = 0;

		while (!feof(archivoConfigFp) && !ferror(archivoConfigFp)) {

   			nuevoIdConfigBroker ^= fgetc(archivoConfigFp);

		}

        fclose(archivoConfigFp);


        if(cantidadDeActualizacionesConfigBroker == 0){

        cantidadDeActualizacionesConfigBroker += 1;

        }else{

            if (nuevoIdConfigBroker != idConfigBroker) {

                log_info(logger,"El archivo de configuración del Broker cambió. Se procederá a actualizar.");
                cargarConfiguracionBroker();
                cantidadDeActualizacionesConfigBroker += 1;
                
            }

        }

        idConfigBroker = nuevoIdConfigBroker;

	}

}