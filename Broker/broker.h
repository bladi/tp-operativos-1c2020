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
#define PING -2

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

/*Estructura de mensajes entrantes*/
typedef struct {

	uint32_t idMensaje;
	uint32_t idMensajeCorrelacional;
	uint32_t tipoMensaje;
	char* posicionEnMemoria;
	t_list* suscriptoresEnviados; // uint32_t
	t_list* acknowledgement; // uint32_t
	
} tMensaje;// ??* revisar creo que falta variable si esta en memoria el mensaje

/*estructura de Suscriptor*/
typedef struct {

	uint32_t identificador;
	uint32_t startTime;
	uint32_t timeToLive;
	
		
} tSuscriptorEnCola;

/*Estructura de particion en memoria*/
typedef struct {
	uint32_t idParticion;
	char* posicion;
	uint32_t tamanio;
	bool free;
	uint32_t idMensaje;
	uint32_t lru;
				
} tParticion;

t_list* METADATA_MEMORIA; //tparticion

t_list* SUSCRIPTORES_LISTA; // tSuscriptor

t_list* NEW_POKEMON_LISTA; //tSuscriptorEnCola

t_list* APPEARED_POKEMON_LISTA; //tSuscriptorEnCola

t_list* CATCH_POKEMON_LISTA; //tSuscriptorEnCola

t_list* CAUGHT_POKEMON_LISTA; //tSuscriptorEnCola

t_list* GET_POKEMON_LISTA; //tSuscriptorEnCola

t_list* LOCALIZED_POKEMON_LISTA; //tSuscriptorEnCola

t_list* MENSAJES_LISTA; //tMensaje


///////////////////////////////////////////////////////////////////////VARIABLES GLOBALES/////////////////////////////////////////////////////////////////////////////////


brokerConfig_t* CONFIG_BROKER;
t_log* logger;

unsigned char idConfigBroker;
unsigned char nuevoIdConfigBroker;

uint32_t cantidadDeActualizacionesConfigBroker;  // 1 NEW_POKEMON_LISTA 2 APPEARED_POKEMON_LISTA 3 CATCH_POKEMON_LISTA 4 CAUGHT_POKEMON_LISTA 5 GET_POKEMON_LISTA 6 LOCALIZED_POKEMON_LISTA

pthread_t hiloServidorBroker;
pthread_t hiloActualizadorConfigBroker;
pthread_t hiloNew;
pthread_t hiloAppeared;
pthread_t hiloCatch;
pthread_t hiloCaught;
pthread_t hiloGet;
pthread_t hiloLocalized;


char *MEMORIA_PRINCIPAL;
uint32_t NUM_SUSCRIPTOR;
pthread_mutex_t mutex_NUM_SUSCRIPTOR;
uint32_t ID_MENSAJE;
pthread_mutex_t mutex_ID_MENSAJE;//??*preguntar si va
uint32_t ID_PARTICION;
pthread_mutex_t mutex_ID_PARTICION;
uint32_t CANTIDAD_BUSQUEDAS_FALLIDAS;

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
uint32_t generarNuevoIdParticion();
char* getDireccionMemoriaLibre(uint32_t idMensaje, uint32_t tamanio);
tParticion * splitParticion(tParticion * unaParticion, uint32_t tamanio);//?* no se si lo va a tomar de ultima pasar a void* y castear
void  ejecutarCompactacion();
void compactarMemoria();
void ejecutarEliminarParticion();
void eliminarMensaje(uint32_t unIdMensaje);

void ingresarNuevoSuscriber(void* nuevaSuscripcion);
void reconectarSuscriptor(void *unaNuevaSuscripcion);

void enviarMensajeNewPokemon(tMensaje* unMensaje,void* unSuscriptor);
void enviarMensajeAppearedPokemon(tMensaje *unMensaje, void* unSuscriptor);
void enviarMensajeCatchPokemon(tMensaje *unMensaje, void* unSuscriptor);
void enviarMensajeCaughtPokemon(tMensaje *unMensaje, void* unSuscriptor);
void enviarMensajeGetPokemon(tMensaje *unMensaje, void* unSuscriptor);
void enviarMensajeLocalizedPokemon(tMensaje *unMensaje, void* unSuscriptor);

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

bool existeIdMensaje(void *mensaje);
uint32_t idMensajeABuscar;
pthread_mutex_t mutex_idMensajeABuscar;

bool existeAck(void *mensaje);
uint32_t ackABuscar;
pthread_mutex_t mutex_ackABuscar;

bool  existeParticionLibre(void *particion);
uint32_t tamanioParticionABuscar;
pthread_mutex_t mutex_tamanioParticionABuscar;

bool sortParticionMenor(tParticion *p, tParticion *q);
bool sortPidMenor(tParticion *p, tParticion *q);
bool sortTimeMenor(tParticion *p, tParticion *q);
bool esParticionLibre(void *unaParticion);
bool esParticionOcupada(void *unaParticion);

tParticion *buscarParticionLibreEnMemoria(uint32_t tamanio);
t_list *buscarListaDeParticionesLibresEnMemoriaOrdenadas(uint32_t tamanio);


 // 1 NEW_POKEMON_LISTA 2 APPEARED_POKEMON_LISTA 3 CATCH_POKEMON_LISTA 4 CAUGHT_POKEMON_LISTA 5 GET_POKEMON_LISTA 6 LOCALIZED_POKEMON_LISTA

 // EL NUMERO DE IDENTIFICACION DEL PROCESO 1 BROKER 2 GAMEBOY 3 GAMECARD 4 TEAM

#endif