#ifndef GAMECARD_H_
#define GAMECARD_H_
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
#include <commons/bitarray.h>
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

#define TIEMPO_DE_REINTENTO_CONEXION "TIEMPO_DE_REINTENTO_CONEXION"
#define TIEMPO_DE_REINTENTO_OPERACION "TIEMPO_DE_REINTENTO_OPERACION"
#define PUNTO_MONTAJE_TALLGRASS "PUNTO_MONTAJE_TALLGRASS"
#define IP_BROKER "IP_BROKER"
#define PUERTO_BROKER "PUERTO_BROKER"
#define PUERTO_GAMECARD "PUERTO_GAMECARD"
#define IP_BROKER "IP_GAMECARD"
#define LOG_FILE "LOG_FILE"

///////////////// CONFIG METADATA
#define BLOCKS "BLOCKS"
#define BLOCK_SIZE "BLOCK_SIZE"
#define MAGIC_NUMBER "MAGIC_NUMBER"

//////////////////////////////////////////////////////////////////////VARIABLES ESTATICAS/////////////////////////////////////////////////////////////////////////////////

#define PATH_CONFIG_GAMECARD "configs/configGameCard.config"
#define ID_BROKER 1

///////////////////////////////////////////////////////////////////////////ESTRUCTURAS////////////////////////////////////////////////////////////////////////////////////

typedef struct gameCardConfig_s{

	uint32_t tiempoReintentoConexion;
	uint32_t tiempoReintentoOperacion;
	char* puntoMontajeTallGrass;
	char* ipBroker;
    uint32_t puertoBroker;
	char* ipGameCard;
    uint32_t puertoGameCard;
	char* logFile;
	int32_t cantidadDeBloques;
	size_t tamanioBloques;
	char* magicNumber;

} gameCardConfig_t;

///////////////////////////////////////////////////////////////////////VARIABLES GLOBALES/////////////////////////////////////////////////////////////////////////////////

t_config* unGameCardArchivoConfig;
t_config* configMetadata;
gameCardConfig_t* unGameCardConfig;
t_log* logger;

t_bitarray* bitarray;

unsigned char idConfigGameCard;
unsigned char nuevoIdConfigGameCard;

uint32_t cantidadDeActualizacionesConfigGameCard;

int socketBroker;

pthread_t hiloServidorGameCard;
pthread_t hiloActualizadorConfigGameCard;

///////////////////////////////////////////////////////////////////////////FUNCIONES//////////////////////////////////////////////////////////////////////////////////////

void configurarLoggerGameCard();

void cargarConfiguracionGameCard();
void actualizarConfiguracionGameCard();

void inicializarGameCard();
void inicializarHilosYVariablesGameCard();
void finalizarGameCard();

void administradorDeConexiones(void* infoAdmin);
void manejarRespuestaAGameBoy(int socketCliente,int idCliente);

int existePokemon(char* pokemon);
int crearPokemon(char* pokemon, uint32_t posicionX, uint32_t posicionY, uint32_t cantidad);
int leerEstadoPokemon(char* nombrePokemon);
int cambiarEstadoPokemon(char* pokemon, int estado);

int crearDirectorio(char* path, char* nombreDirectorio);

///////////////////////////////////////////////////////////////////////////CONEXIONES/////////////////////////////////////////////////////////////////////////////////////

void inicializarBitMap();
void crearArchivoBitmap(char* pathBitmap);
void verificarBitmapPrevio(char* pathBitmap);
int validarArchivo(char* path);
int cantidadBloquesLibres();
int buscarBloqueLibreYOcupar();
void mostrarEstadoBitmap();

///////////////////////////////////////////////////////////////////////MANEJO BLOQUES/////////////////////////////////////////////////////////////////////////////////////

void crearBloquesFileSystem();
int cantBloquesParaSize(int size);
char* generarStringBlocks(int cantBloques, int bloquesAEscribir[]);
int escribirEnBloques(char* ubicaciones, int arregloBloques[], int cantBloques);


void casoDePrueba();

#endif