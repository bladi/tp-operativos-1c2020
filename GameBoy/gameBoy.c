#include "gameBoy.h"

uint32_t main() {

    /*pthread_t hiloConsola;
	pthread_t hiloServidor;
	pthread_t hiloDump;
	pthread_t hiloCompactaciones;*/
	pthread_t hiloArchivoConfig;
	
	inicializarGameBoy();
	
	/*pthread_create(&hiloCompactaciones, NULL, (void*)fsIniciarCompactacionesTablasExistentes, (void*)pathTablas);
	pthread_create(&hiloDump, NULL, (void*)fsDump, NULL);
    pthread_create(&hiloConsola, NULL, (void*)ConsolaMain, NULL);*/
	pthread_create(&hiloArchivoConfig, NULL, (void*)actualizarConfiguracionGameBoy, NULL);
	
	//pthread_create(&hiloServidor,NULL,(void*)servidor_inicializar,(void*)unaInfoServidor);
	pthread_join(hiloArchivoConfig, NULL);

	finalizarGameBoy();

	return 0;

}