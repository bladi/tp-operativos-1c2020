#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "sockets.h"

///////////////////////////////////////////////////ESTRUCTURAS SERIALIZACION/DESEREALIZACION///////////////////////////////////////

typedef enum t_protocolo {
	tNewPokemon = 1,
	tGetPokemon,
	tCatchPokemon,
	tCaughtPokemon,
	tLocalizedPokemon,
	tAppearedPokemon,
	tSuscriptor,
	tFinDeProtocolo //NO SACAR Y DEJAR A LO ULTIMO!!!
} t_protocolo;

typedef struct infoAdminConexiones_s{
	int socketCliente;
} __attribute__((packed)) infoAdminConexiones_t;

typedef struct infoServidor_s{
	int puerto;
	char* ip;
} __attribute__((packed)) infoServidor_t;

typedef struct t_newPokemon{

	uint32_t identificador;
	uint32_t identificadorCorrelacional;
	char* nombrePokemon;
	uint32_t posicionEnElMapaX;
	uint32_t posicionEnElMapaY;
	uint32_t cantidadDePokemon;

} __attribute__((packed)) t_newPokemon;

typedef struct t_getPokemon{

	uint32_t identificador;
	uint32_t identificadorCorrelacional;
	char* nombrePokemon;

} __attribute__((packed)) t_getPokemon;

typedef struct t_caughtPokemon{

	uint32_t identificador;
	uint32_t identificadorCorrelacional;
	char* nombrePokemon;
	bool resultado;

} __attribute__((packed)) t_caughtPokemon;

typedef struct t_catchPokemon{

	uint32_t identificador;
	uint32_t identificadorCorrelacional;
	char* nombrePokemon;
	uint32_t posicionEnElMapaX;
	uint32_t posicionEnElMapaY;

} __attribute__((packed)) t_catchPokemon;

typedef struct t_localizedPokemon{

	uint32_t identificador;
	uint32_t identificadorCorrelacional;
	char* nombrePokemon;
	t_list* listaDatosPokemon;

} __attribute__((packed)) t_localizedPokemon;

typedef struct t_appearedPokemon{

	uint32_t identificador;
	uint32_t identificadorCorrelacional;
	char* nombrePokemon;
	uint32_t posicionEnElMapaX;
	uint32_t posicionEnElMapaY;

} __attribute__((packed)) t_appearedPokemon;

typedef struct t_suscriptor{

	uint32_t identificador;
	uint32_t identificadorCorrelacional;
	char* colaDeMensajes;
	uint32_t tiempoDeSuscripcion;

} __attribute__((packed)) t_suscriptor;

typedef struct datosPokemon{

	uint32_t cantidad;
	uint32_t posicionEnElMapaX;
	uint32_t posicionEnElMapaY;

} __attribute__((packed)) datosPokemon;

/////////////////////////////////////////////////FUNCIONES SERIALIZACION/DESEREALIZACION///////////////////////////////////////////

void* recibirPaquete(int fdCliente, int* tipoMensaje, int* tamanioMensaje);
void  enviarPaquete(int fdCliente, int tipoMensaje, void * mensaje, int tamanioMensaje);

void* serializar(int tipoMensaje, void* mensaje, int* tamanio);
void* deserializar(uint16_t tipoMensaje, void* mensaje);

void* serializarLocalizedPokemon(t_localizedPokemon* localizedPokemon, int* tamanio);
t_localizedPokemon* deserializarLocalizedPokemon(void* buffer);

void* serializarAppearedPokemon(t_appearedPokemon* appearedPokemon, int* tamanio);
t_appearedPokemon* deserializarAppearedPokemon(void* buffer);

void* serializarCaughtPokemon(t_caughtPokemon* caughtPokemon, int* tamanio);
t_caughtPokemon* deserializarCaughtPokemon(void* buffer);

void* serializarCatchPokemon(t_catchPokemon* catchPokemon, int* tamanio);
t_catchPokemon* deserializarCatchPokemon(void* buffer);

void* serializarGetPokemon(t_getPokemon* getPokemon, int* tamanio);
t_getPokemon* deserializarGetPokemon(void* buffer);

void* serializarNewPokemon(t_newPokemon* newPokemon, int* tamanio);
t_newPokemon* deserializarNewPokemon(void* buffer);

//////////////////////////////////////////FUNCIONES PARA SERIALIZACION DE LISTAS///////////////////////////////////////

void eliminarNodoDatosPokemon(datosPokemon* unNodoDatosPokemon);

#endif /* SERIALIZACION_H_ */
