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
#include <math.h>
#include <semaphore.h>

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
#define ALPHA "ALPHA"
#define IP_BROKER "IP_BROKER"
#define ESTIMACION_INICIAL "ESTIMACION_INICIAL"
#define PUERTO_BROKER "PUERTO_BROKER"
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
	float alpha;
	char* ipBroker;
	uint32_t estimacionInicial;
    uint32_t puertoBroker;
	char* ipTeam;
	uint32_t puertoTeam;
    char* logFile;

} teamConfig_t;

typedef enum {
	NEW,
	READY,
	BLOCK,
	EXEC,
	EXIT
} Estado;

typedef t_list *EstadoProceso;
EstadoProceso NUEVOS, LISTOS, BLOQUEADOS, FINALIZADOS;

typedef enum Objetivo{
	Ninguno,
	BuscandoAtrapar,
	BuscandoIntercambio,
	EsperandoMensaje,
	EsperandoIntercambio
} Objetivo;

typedef struct
{
	char* nombre;
	int cantidad;
} t_Pokemon;

typedef struct entrenador
{
	int id;
	int posicionX;
	int posicionY;
	t_list* pokemones;
	t_list* objetivos;
	Estado estado;
	int cuantosPuedeAtrapar;
	int objetivoX;
	int objetivoY;
	Objetivo objetivo;
	t_Pokemon* objetivoPokemon;
	int intercambioEntrenador;
	uint32_t identificadorCorrelacional;
	float estimacionAnterior;
	int rafagaAnterior;
	float estimacionActual;
	int rafagaActual;
	int cpuIntercambio;
	int rafagasTotales;
} t_Entrenador;

typedef struct
{
	t_Pokemon* pokemon;
	int posicionX;
	int posicionY;
} t_posicionPokemon;

typedef struct
{
	int id1;
	int id2;
} t_entrenadoresEnDeadlock;

///////////////////////////////////////////////////////////////////////VARIABLES GLOBALES/////////////////////////////////////////////////////////////////////////////////

t_config* unTeamArchivoConfig;
teamConfig_t* unTeamConfig;
t_log* logger;
t_list* listaDeEntrenadores;
t_list* mapa;
t_list* mapaPendientes;
t_list* pokemonesAtrapados;
t_list* pokemonesObjetivos;
t_list* pokemonesBuscandose;
t_list* identificadoresGet;
t_list* semaforosEntrenador;
//t_list* pidHilos;

t_Entrenador* entrenadorEjecutando;

uint32_t cantidadEntrenadores;
uint32_t cantidadCiclosCPU;
uint32_t cantidadCambiosDeContexto;
uint32_t cantidadDeadlocks;
uint32_t cantidadDeadlocksResueltos;

unsigned char idConfigTeam;
unsigned char nuevoIdConfigTeam;

uint32_t cantidadDeActualizacionesConfigTeam;

pthread_t hiloServidorTeam;
pthread_t hiloActualizadorConfigTeam;
pthread_t hiloCPU;
pthread_t hiloPlanificador;

pthread_t hiloActualizadorSocketBrocker;

int socketBroker;
int brokerActivo;

pthread_mutex_t mutexEntrenadorEjecutando;
pthread_mutex_t mutexSemaforosEntrenador;
pthread_mutex_t mutexPokemonesBuscandose;
pthread_mutex_t mutexPokemonesObjetivos;
pthread_mutex_t mutexPokemonesAtrapados;
pthread_mutex_t mutexMapaPendientes;
pthread_mutex_t mutexMapa;
pthread_mutex_t mutexListaDeEntrenadores;
pthread_mutex_t mutexSocketBroker;
pthread_mutex_t mutexIdentificadoresGet;
pthread_mutex_t mutexNuevos;
pthread_mutex_t mutexListos;
pthread_mutex_t mutexBloqueados;
pthread_mutex_t mutexFinalizados;
pthread_mutex_t mutexCantidadCiclosCPU;
pthread_mutex_t mutexCantidadCambiosDeContexto;
pthread_mutex_t mutexCantidadDeadlocks;
pthread_mutex_t mutexCantidadDeadlocksResueltos;

sem_t* semaforoPlanificador;
sem_t* semaforoTerminoEjecucion;

///////////////////////////////////////////////////////////////////////////FUNCIONES//////////////////////////////////////////////////////////////////////////////////////

void configurarLoggerTeam();

void cargarConfiguracionTeam();
void actualizarConfiguracionTeam();
void actualizarConexionConBroker();
void mostrarMetricas();

void administradorDeConexiones(void* infoAdmin);
void manejarRespuestaABroker(int socketCliente, int idCliente);
void manejarRespuestaAGameBoy(int socketCliente, int idCliente);
bool estaIdEnLista(int pIdCorrelacional);

void cambiarEstado(t_Entrenador *unEntrenador, Estado unEstado);
void bloquearEntrenador(t_Entrenador* pEntrenador);
void finalizarEntrenador(t_Entrenador* pEntrenador);

void inicializarTeam();
void finalizarTeam();
//void eliminarHilos(pthread_t* pHilo);
void inicializarHilosYVariablesTeam();
void cargarEntrenadoresYListasGlobales();

int posicionPokeEnLista(t_list* pLista, char* pPokemon);
int posicionPokeEnListaMapa(char* nombrePokemon, int posicionEnX, int posicionEnY);
int posicionPokeEnListaMapaSinPosicion(char* nombrePokemon);
void agregarPokeALista(t_list* pLista, char* pPokemon);
void quitarPokeDeLista(t_list* pLista, char* pPokemon);
int cantidadDeUnPokemonEnLista(t_list* pLista, char* pPokemon);
int cantidadTotalDePokemonesEnLista(t_list* pLista);

void planificarReady(int posXpokemon,int posYpokemon, char* pPokemonNombre, int pPokemonCantidad);
void guardarPokemonABuscar(int posXpokemon,int posYpokemon, char* pPokemonNombre, int pPokemonCantidad);
void planificarPokemonPendiente();
bool puedoPlanificarReady();
void planificarExec();
void planificarFIFO();
void planificarRR();
void planificarSJF();
void planificarSRT();

bool puedeAtrapar(t_Entrenador* pEntrenador);
bool entrenadorCumplioObjetivos(t_Entrenador* pEntrenador);
bool estaBloqueadoPorRecursos(t_Entrenador* pEntrenador);
void entrenadorFinalizoSuTarea(t_Entrenador* pEntrenador);
int posicionEntrenadorEnLista(t_list* pLista, int pId);
bool puedeDesbloquearse(t_Entrenador* pEntrenador);
int cuantosLeFaltan(t_Entrenador* pEntrenador, char* pPokemon);
char* cualEsElPrimerPokemonQuePrecisa(t_Entrenador* pEntrenador);
char* cualEsElPrimerPokemonQueLeSobra(t_Entrenador* pEntrenador);

bool teamCumplioObjetivos();
int calcularDistancia(int x1, int y1, int x2, int y2);
t_Entrenador* entrenadorMasCercano(int posXpokemon, int posYpokemon, char* pPokemonNombre, int pPokemonCantidad);
t_entrenadoresEnDeadlock* quienesEstanEnDeadlock();
bool hayDeadlock();
void intercambiar();
void atrapar();
void ejecutar(int pId);
void moverEntrenadorEnX();
void moverEntrenadorEnY();

void calcularDeadlock();
int entrenadorSiguiente(t_Pokemon* pPokemon);

#endif