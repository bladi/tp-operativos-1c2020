#ifndef GAMEBOY_H_
#define GAMEBOY_H_
//////////////////////////////////////////////////////////////////////////COMMONS///////////////////////////////////////////////////////////////////////////////////////

#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <commons/log.h>

//////////////////////////////////////////////////////////////////////////LIBS "C"//////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

////////////////////////////////////////////////////////////////////////LIBS COMPARTIDAS/////////////////////////////////////////////////////////////////////////////////

#include "../libreriasCompartidas/sockets.h"
#include "../libreriasCompartidas/serializacion.h"

///////////////////////////////////////////////////////////////////////VARIABLES CONFIG//////////////////////////////////////////////////////////////////////////////////

#define IP_BROKER "IP_BROKER"
#define IP_TEAM "IP_TEAM"
#define IP_GAMECARD "IP_GAMECARD"
#define IP_GAMEBOY "IP_GAMEBOY"
#define PUERTO_BROKER "PUERTO_BROKER"
#define PUERTO_TEAM "PUERTO_TEAM"
#define PUERTO_GAMECARD "PUERTO_GAMECARD"
#define PUERTO_GAMEBOY "PUERTO_GAMEBOY"
#define LOG_FILE "LOG_FILE"

//////////////////////////////////////////////////////////////////////VARIABLES ESTATICAS/////////////////////////////////////////////////////////////////////////////////

#define PATH_CONFIG_GAMEBOY "configs/configGameBoy.config"

#define ID_BROKER 1
#define ID_GAMECARD 3
#define ID_TEAM 4

#define MAX_CANTIDAD_ARGUMENTOS 8
#define MIN_CANTIDAD_ARGUMENTOS 4

#define CATCH_POKEMON "CATCH_POKEMON"
#define CAUGHT_POKEMON "CAUGHT_POKEMON"
#define GET_POKEMON "GET_POKEMON"
#define APPEARED_POKEMON "APPEARED_POKEMON"
#define NEW_POKEMON "NEW_POKEMON"
#define LOCALIZED_POKEMON "LOCALIZED_POKEMON"

///////////////////////////////////////////////////////////////////////////ESTRUCTURAS////////////////////////////////////////////////////////////////////////////////////

typedef struct gameBoyConfig_s{

	char* ipBroker;
	char* ipTeam;
	char* ipGameCard;
	char* ipGameBoy;
    uint32_t puertoBroker;
    uint32_t puertoTeam;
    uint32_t puertoGameCard;
	uint32_t puertoGameBoy;
	char* logFile;

} gameBoyConfig_t;

///////////////////////////////////////////////////////////////////////VARIABLES GLOBALES/////////////////////////////////////////////////////////////////////////////////

t_config* unGameBoyArchivoConfig;
gameBoyConfig_t* unGameBoyConfig;
t_log* logger;

unsigned char idConfigGameBoy;
unsigned char nuevoIdConfigGameBoy;

uint32_t cantidadDeActualizacionesConfigGameBoy;

int socketBroker;
int socketGameCard;
int socketTeam;

pthread_t hiloActualizadorConfigGameBoy;
pthread_t hiloServidorGameBoy;

///////////////////////////////////////////////////////////////////////////FUNCIONES//////////////////////////////////////////////////////////////////////////////////////

void configurarLoggerGameBoy();

void cargarConfiguracionGameBoy();
void actualizarConfiguracionGameBoy();

void inicializarGameBoy();
void inicializarHilosYVariablesGameBoy();
void finalizarGameBoy();

void administradorDeConexiones(void* infoAdmin);
void manejarRespuestaABroker(int socketCliente,int idCliente);

void enviarNewPokemonABroker(char* nombrePokemon,char* posicionEnX,char* posicionEnY,char* cantidad);
void enviarAppearedPokemonABroker(char* nombrePokemon,char* posicionEnX,char* posicionEnY,char* idMensaje);
void enviarCatchPokemonABroker(char* nombrePokemon,char* posicionEnX,char* posicionEnY);
void enviarCaughtPokemonABroker(char* nombrePokemon,char* resultado);
void enviarGetPokemonABroker(char* nombrePokemon);
void enviarAppearedPokemonATeam(char* nombrePokemon,char* posicionEnX,char* posicionEnY);
void enviarNewPokemonAGameCard(char* nombrePokemon,char* posicionEnX,char* posicionEnY,char* cantidad, char* idMensaje);
void enviarCatchPokemonAGameCard(char* nombrePokemon,char* posicionEnX,char* posicionEnY, char* idMensaje);
void enviarGetPokemonAGameCard(char* nombrePokemon);
void enviarSuscriptorABroker(char* colaDeMensajes,char* tiempoDeSuscripcion);
void sacarBarraNFinal(int posicion, char **argv);

#endif