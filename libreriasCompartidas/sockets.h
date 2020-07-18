#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serializacion.h"
#include "../GameBoy/gameBoy.h"
#include "../GameCard/gameCard.h"
#include "../Team/team.h"
#include "../Broker/broker.h"

#define FAIL -1
#define TOPE_CLIENTES_ACTIVOS 100

int32_t escucharSocket(int32_t puertoObjetivo);
int32_t aceptarConexion(int32_t socketEscucha);
int32_t conectarseA(char *ip, int32_t puertoObjetivo);
int32_t enviarCadena(int32_t socketDestino, char *mensaje);
int32_t recibirCadena(int32_t socketOrigen, char *mensaje);
int32_t enviarInt(int32_t socketDestino, int32_t num);
int32_t recibirInt(int32_t socketDestino, int32_t *i);
int32_t cliente(char *ipCliente, int32_t puertoCliente, int32_t idCliente);
char *devuelveNombreProceso(int32_t idProceso);
int32_t enviarPorSocket(int32_t fdCliente, const void *mensaje, int32_t totalAEnviar);
int32_t recibirPorSocket(int32_t fdCliente, void *buffer, int32_t totalARecibir);
void selectRead(int32_t descriptor, fd_set *bag, struct timeval *timeout);
void selectWrite(int32_t descriptor, fd_set *bag, struct timeval *timeout);
void selectException(int32_t descriptor, fd_set *bag, struct timeval *timeout);
int probarConexionSocket(int socketDestino);

void servidor_inicializar(void* unaInfoServidor);

#endif
