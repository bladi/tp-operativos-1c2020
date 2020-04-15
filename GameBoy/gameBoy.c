#include "gameBoy.h"

uint32_t main(int argc, char **argv) {

	printf("\n\n Cantidad de argumentos: %d", argc);

	for(int i=0;i<argc;i++){

		printf("\n\n %d° argumento ingresado: '%s'", i+1, argv[i]);

	}

	if(argc > MAX_CANTIDAD_ARGUMENTOS || MIN_CANTIDAD_ARGUMENTOS > argc){

		printf("\n\n Se ingresaron %d argumentos. El máximo son %d y el mínimo son %d. Intente nuevamente.\n",argc, MAX_CANTIDAD_ARGUMENTOS, MIN_CANTIDAD_ARGUMENTOS);
		return 0;

	}else{

		if(strcmp(argv[1], "BROKER") == 0){

			if(strcmp(argv[2], "NEW_POKEMON") == 0){

				if(argv[3] == NULL){
					
					printf("\n\n No se ingresó un nombre de pokemón en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[4] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'x' en el 5° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[5] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'y' en el 6° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[6] == NULL){

					printf("\n\n No se ingresó una cantidad de pokemón en el 7° argumento. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarNewPokemonABroker(argv[3], argv[4], argv[5], argv[6]);
					finalizarGameBoy();
					return 0;

				}

			}else if(strcmp(argv[2], "APPEARED_POKEMON") == 0){

				if(argv[3] == NULL){
					
					printf("\n\n No se ingresó un nombre de pokemón en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[4] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'x' en el 5° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[5] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'y' en el 6° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[6] == NULL){

					printf("\n\n No se ingresó un ID del mensaje en el 7° argumento. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarAppearedPokemonABroker(argv[3], argv[4], argv[5], argv[6]);
					finalizarGameBoy();
					return 0;

				}

			}else if(strcmp(argv[2], "CATCH_POKEMON") == 0){

				if(argv[3] == NULL){
					
					printf("\n\n No se ingresó un nombre de pokemón en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[4] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'x' en el 5° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[5] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'y' en el 6° argumento. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarCatchPokemonABroker(argv[3], argv[4], argv[5]);
					finalizarGameBoy();
					return 0;

				}

			}else if(strcmp(argv[2], "CAUGHT_POKEMON") == 0){

				if(argv[3] == NULL){
					
					printf("\n\n No se ingresó un ID del mensaje en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[4] == NULL){

					printf("\n\n No se ingresó un resultado de atrapada en el 5° argumento. Intente nuevamente.\n");
					return 0;

				}else if(!(strcmp(argv[4], "OK") == 0) || !(strcmp(argv[4], "FAIL") == 0)){

					printf("\n\n No se ingresó un resultado de atrapada válido. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarCaughtPokemonABroker(argv[3], argv[4]);
					finalizarGameBoy();
					return 0;

				}

			}else if(strcmp(argv[2], "GET_POKEMON") == 0 ){

				if(argv[3] == NULL){
					
					printf("\n\n No se ingresó un nombre de pokemón en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarGetPokemonABroker(argv[3]);
					finalizarGameBoy();
					return 0;

				}

			}else{

				printf("\n\n Se ingresó un tipo de mensaje para el Broker no válido o nulo: '%s'. Intente nuevamente.\n",argv[2]);
				return 0;

			}

		}else if(strcmp(argv[1], "TEAM") == 0){

			if(strcmp(argv[2], "APPEARED_POKEMON") == 0){

				if(argv[3] == NULL){
					
					printf("\n\n No se ingresó un nombre de pokemón en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[4] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'x' en el 5° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[5] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'y' en el 6° argumento. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarAppearedPokemonATeam(argv[3], argv[4], argv[5]);
					finalizarGameBoy();
					return 0;

				}

			}else{

				printf("\n\n Se ingresó un tipo de mensaje para el Team no válido o nulo: '%s'. Intente nuevamente.\n",argv[2]);
				return 0;

			}

		}else if(strcmp(argv[1], "GAMECARD") == 0){

			if(strcmp(argv[2], "NEW_POKEMON") == 0){

				if(argv[3] == NULL){
					
					printf("\n\n No se ingresó un nombre de pokemón en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[4] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'x' en el 5° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[5] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'y' en el 6° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[6] == NULL){

					printf("\n\n No se ingresó una cantidad de pokemón en el 7° argumento. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarNewPokemonAGameCard(argv[3], argv[4], argv[5], argv[6]);
					finalizarGameBoy();
					return 0;

				}
			
			}else if(strcmp(argv[2], "CATCH_POKEMON") == 0){

				if(argv[3] == NULL){
					
					printf("\n\n No se ingresó un nombre de pokemón en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[4] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'x' en el 5° argumento. Intente nuevamente.\n");
					return 0;

				}else if(argv[5] == NULL){

					printf("\n\n No se ingresó una posición del mapa en 'y' en el 6° argumento. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarCatchPokemonAGameCard(argv[3], argv[4], argv[5]);
					finalizarGameBoy();
					return 0;

				}

			}else if(strcmp(argv[2], "GET_POKEMON") == 0 ){

				if(argv[3] == NULL){
					
					printf("\n\n No se ingresó un nombre de pokemón en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarGetPokemonAGameCard(argv[3]);
					finalizarGameBoy();
					return 0;

				}

			}else{

				printf("\n\n Se ingresó un tipo de mensaje para el Game Card no válido o nulo: '%s'. Intente nuevamente.\n",argv[2]);
				return 0;

			}

		}else if(strcmp(argv[1], "SUSCRIPTOR") == 0){

			if(strcmp(argv[2], NEW_POKEMON) == 0 || strcmp(argv[2], CATCH_POKEMON) == 0 || strcmp(argv[2], CAUGHT_POKEMON) == 0 || strcmp(argv[2], GET_POKEMON) == 0 || strcmp(argv[2], APPEARED_POKEMON) == 0){

				if(argv[3] == NULL){

					printf("\n\n No se ingresó un tiempo de suscripción en el 4° argumento. Intente nuevamente.\n");
					return 0;

				}else{

					inicializarGameBoy();
					enviarSuscriptorABroker(argv[2], argv[3]);
					finalizarGameBoy();
					return 0;

				}

			}else{

				printf("\n\n Se ingresó una cola de mensajes para suscribirse al Broker no válida o nula: '%s'. Intente nuevamente.\n",argv[2]);
				return 0;

			}

		}else{

			printf("\n\n Se ingresó un modo no válido: '%s'. Intente nuevamente.\n", argv[1]);
			return 0;

		}

	}

}