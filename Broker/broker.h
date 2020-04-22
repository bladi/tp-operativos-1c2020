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

//typedef enum tipoMensaje {M_NEW, M_APPEARED, M_CATCH, M_CAUGHT, M_GET, M_LOCALIZED} tipoMensaje_t;

typedef struct {

	uint32_t idMensaje;
	uint32_t idMensajeCorrelacional;
	uint32_t tipoMensaje;
	char* posicionEnMemoria;
	t_list* suscriptoresEnviados; // uint32_t
	t_list* acknowledgement; // uint32_t
	
} tMensaje;// ??* revisar

typedef struct {

	uint32_t idSuscriptor;
	char* ipServer;
	uint32_t PuertoEschucha;
	uint32_t socketSuscriptor;
		
} tSuscriptor;

typedef struct {

	uint32_t idSuscriptor;
	uint32_t startTime;
	uint32_t timeToLive;
	
		
} tSuscriptorEnCola;

typedef struct {

	char* posicion;
	uint32_t tamanio;
	bool free;
			
} tParticion;

t_list* METADATA_MEMORIA; //tparticion

t_list* SUSCRIPTORES; // tSuscriptor

t_list* NEW_POKEMON; //tSuscriptorEnCola

t_list* APPEARED_POKEMON; //tSuscriptorEnCola

t_list* CATCH_POKEMON; //tSuscriptorEnCola

t_list* CAUGHT_POKEMON; //tSuscriptorEnCola

t_list* GET_POKEMON; //tSuscriptorEnCola

t_list* LOCALIZED_POKEMON; //tSuscriptorEnCola

t_list* MENSAJES; //tMensaje




///////////////////////////////////////////////////////////////////////VARIABLES GLOBALES/////////////////////////////////////////////////////////////////////////////////


brokerConfig_t* CONFIG_BROKER;
t_log* logger;

unsigned char idConfigBroker;
unsigned char nuevoIdConfigBroker;

uint32_t cantidadDeActualizacionesConfigBroker;

pthread_t hiloServidorBroker;
pthread_t hiloActualizadorConfigBroker;


char *MEMORIA_PRINCIPAL;
uint32_t NUM_SUSCRIPTOR;
pthread_mutex_t mutex_NUM_SUSCRIPTOR;//??*preguntar si va
uint32_t ID_MENSAJE;
pthread_mutex_t mutex_ID_MENSAJE;//??*preguntar si va

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

void inicializarMemoria();
uint32_t generarNuevoIdMensajeBroker();
uint32_t generarNuevoIdSuscriptor();
char* getDireccionMemoriaLibre(uint32_t tamanio);

void ingresarNuevoSuscriber(t_suscribeQueue* nuevaSuscripcion);

void  enviarMensajeNewPokemon(tMensaje* unMensaje,tSuscriptor* unSuscriptor);
void enviarMensajeAppearedPokemon(tMensaje *unMensaje, tSuscriptor *unSuscriptor);
void enviarMensajeCatchPokemon(tMensaje *unMensaje, tSuscriptor *unSuscriptor);
void enviarMensajeCaughtPokemon(tMensaje *unMensaje, tSuscriptor *unSuscriptor);
void enviarMensajeGetPokemon(tMensaje *unMensaje, tSuscriptor *unSuscriptor);
void enviarMensajeLocalizedPokemon(tMensaje *unMensaje, tSuscriptor *unSuscriptor);

void ejecutarColaNewPokemon();
void ejecutarColaAppearedPokemon();
void ejecutarColaCatchPokemon();
void ejecutarColaCaughtPokemon();
void ejecutarColaGetPokemon();
void ejecutarColaLocalizedPokemon();

//////////////////////////////////////////////////FUNCIONES LISTAS//////////////////////////////////////////////////////////////////////////////////////

bool existeNuevoSuscriber(void *tSuscriptor);
char* ipServerABuscar;
uint32_t PuertoEschuchaABuscar;
pthread_mutex_t mutex_ipServerABuscar;
pthread_mutex_t mutex_PuertoEschuchaABuscar;


bool existeIdSuscriberEnCola(void *suscriptorEnCola);
uint32_t idSuscriberABuscar;
pthread_mutex_t mutex_idSuscriberABuscar;


bool existeIdSuscriptor(void *suscriptor);
uint32_t idSuscriptorABuscar;
pthread_mutex_t mutex_idSuscriptorABuscar;


bool existeTipoMensaje(void *mensaje);
uint32_t tipoMensajeABuscar;
pthread_mutex_t mutex_tipoMensajeABuscar;


bool existeAck(void *mensaje);
uint32_t ackABuscar;
pthread_mutex_t mutex_ackABuscar;


 // 1 NEW_POKEMON 2 APPEARED_POKEMON 3 CATCH_POKEMON 4 CAUGHT_POKEMON 5 GET_POKEMON 6 LOCALIZED_POKEMON

#endif