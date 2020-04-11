#ifndef TEAM_H_
#define TEAM_H_
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

#define POSICIONES_ENTRENADORES "POSICIONES_ENTRENADORES"
#define POKEMON_ENTRENADORES "POKEMON_ENTRENADORES"
#define OBJETIVOS_ENTRENADORES "OBJETIVOS_ENTRENADORES"
#define TIEMPO_RECONEXION "TIEMPO_RECONEXION"
#define RETARDO_CICLO_CPU "RETARDO_CICLO_CPU"
#define ALGORITMO_PLANIFICACION "ALGORITMO_PLANIFICACION"
#define QUANTUM "QUANTUM"
#define IP_BROKER "IP_BROKER"
#define ESTIMACION_INICIAL "ESTIMACION_INICIAL"
#define PUERTO_BROKER "PUERTO_BROKER"
//#define IP_TEAM "IP_BROKER"
//#define PUERTO_TEAM "PUERTO_BROKER"
#define LOG_FILE "LOG_FILE"

//////////////////////////////////////////////////////////////////////VARIABLES ESTATICAS/////////////////////////////////////////////////////////////////////////////////

#define PATH_CONFIG_TEAM "configs/configTeam.config"

#define ID_BROKER 1

///////////////////////////////////////////////////////////////////////////ESTRUCTURAS////////////////////////////////////////////////////////////////////////////////////

typedef struct teamConfig_s{

	char** posicionEntrenadores; //Por Commons se reciben las listas de esta forma. Recordar despues recorrer el char** y pasarlo a formato t_list*
	char** pokemonEntrenadores; //Por Commons se reciben las listas de esta forma. Recordar despues recorrer el char** y pasarlo a formato t_list*
	char** objetivosEntrenadores; //Por Commons se reciben las listas de esta forma. Recordar despues recorrer el char** y pasarlo a formato t_list*
	uint32_t tiempoReconexion;
	uint32_t retardoCicloCPU;
	char* algoritmoPlanificacion;
    uint32_t quantum;
	char* ipBroker;
	uint32_t estimacionInicial;
    uint32_t puertoBroker;
	//char* ipTeam;
	//uint32_t puertoTeam;
    char* logFile;

} teamConfig_t;

///////////////////////////////////////////////////////////////////////VARIABLES GLOBALES/////////////////////////////////////////////////////////////////////////////////

t_config* unTeamArchivoConfig;
teamConfig_t* unTeamConfig;
t_log* logger;

unsigned char idConfigTeam;
unsigned char nuevoIdConfigTeam;

uint32_t cantidadDeActualizacionesConfigTeam;

//pthread_t hiloServidorTeam;
pthread_t hiloActualizadorConfigTeam;

//infoServidor_t* unaInfoServidorTeam;

int socketBroker;

///////////////////////////////////////////////////////////////////////////FUNCIONES//////////////////////////////////////////////////////////////////////////////////////

void configurarLoggerTeam();

void cargarConfiguracionTeam();
void actualizarConfiguracionTeam();

void inicializarTeam();
void inicializarHilosYVariablesTeam();
void finalizarTeam();

void administradorDeConexiones(void* infoAdmin);
//void manejarRespuestaAGameBoy(int socketCliente,int idCliente);

#endif