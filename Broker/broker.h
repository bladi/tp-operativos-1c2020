#ifndef BROKER_H_
#define BROKER_H_
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

#define TAMANO_MEMORIA "TAMANO_MEMORIA"
#define TAMANO_MINIMO_PARTICION "TAMANO_MINIMO_PARTICION"
#define ALGORITMO_MEMORIA "ALGORITMO_MEMORIA"
#define ALGORITMO_REEMPLAZO "ALGORITMO_REEMPLAZO"
#define ALGORITMO_PARTICION_LIBRE "ALGORITMO_PARTICION_LIBRE"
#define IP_BROKER "IP_BROKER"
#define PUERTO_BROKER "PUERTO_BROKER"
#define FRECUENCIA_COMPACTACION "FRECUENCIA_COMPACTACION"
#define LOG_FILE "LOG_FILE"

//////////////////////////////////////////////////////////////////////VARIABLES ESTATICAS/////////////////////////////////////////////////////////////////////////////////

#define PATH_CONFIG_BROKER "configs/configBroker.config"

///////////////////////////////////////////////////////////////////////////ESTRUCTURAS////////////////////////////////////////////////////////////////////////////////////

typedef struct brokerConfig_s{

	uint32_t tamanioMemoria;
	uint32_t tamanioMinimoParticion;
	char* algoritmoMemoria;
	char* algoritmoReemplazo;
	char* algoritmoParticionLibre;
	char* ipBroker;
    uint32_t puertoBroker;
	uint32_t frecuenciaCompactacion;
	char* logFile;

} brokerConfig_t;

///////////////////////////////////////////////////////////////////////VARIABLES GLOBALES/////////////////////////////////////////////////////////////////////////////////

t_config* unBrokerArchivoConfig;
brokerConfig_t* unBrokerConfig;
t_log* logger;

unsigned char idConfigBroker;
unsigned char nuevoIdConfigBroker;

uint32_t cantidadDeActualizacionesConfigBroker;

pthread_t hiloServidorBroker;
pthread_t hiloActualizadorConfigBroker;

///////////////////////////////////////////////////////////////////////////FUNCIONES//////////////////////////////////////////////////////////////////////////////////////

void configurarLoggerBroker();

void cargarConfiguracionBroker();
void actualizarConfiguracionBroker();

void inicializarBroker();
void inicializarHilosYVariablesBroker();
void finalizarBroker();

void administradorDeConexiones(void* infoAdmin);
void manejarRespuestaAGameBoy(int socketCliente,int idCliente);
void manejarRespuestaAGameCard(int socketCliente,int idCliente);
void manejarRespuestaATeam(int socketCliente,int idCliente);

#endif