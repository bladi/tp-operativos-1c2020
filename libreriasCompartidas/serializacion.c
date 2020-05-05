#include "serializacion.h"

void* serializar(int tipoMensaje, void* mensaje, int* tamanio){
	void* buffer;

	switch(tipoMensaje){

		case tNewPokemon: {
			buffer = serializarNewPokemon(mensaje, tamanio);
			break;
		}

		case tGetPokemon: {
			buffer = serializarGetPokemon(mensaje, tamanio);
			break;
		}

		case tCatchPokemon: {
			buffer = serializarCatchPokemon(mensaje, tamanio);
			break;
		}

		case tCaughtPokemon: {
			buffer = serializarCaughtPokemon(mensaje,tamanio);
			break;
		}

		case tLocalizedPokemon: {
			buffer = serializarLocalizedPokemon(mensaje,tamanio);
			break;

		}

		case tAppearedPokemon: {
			buffer = serializarAppearedPokemon(mensaje, tamanio);
			break;

		}
		case tSuscriptor: {
			buffer = serializarSuscriptor(mensaje, tamanio);
			break;

		}

		case 0: {
			//printf("Desconexion. \n");
			break;
		}

		default: {
			//perror("Se recibio un mensaje que no esta en el protocolo.");
			abort();
			break;
		}
	}
	return buffer;
}

void* deserializar(uint16_t tipoMensaje, void* mensaje){

	void* buffer;

		switch(tipoMensaje){

			case tNewPokemon: {
				buffer = deserializarNewPokemon(mensaje);
				break;
			}

			case tGetPokemon: {
				buffer = deserializarGetPokemon(mensaje);
				break;
			}

			case tCatchPokemon: {
				buffer = deserializarCatchPokemon(mensaje);
				break;
			}

			case tCaughtPokemon: {
				buffer = deserializarCaughtPokemon(mensaje);
				break;
			}

			case tLocalizedPokemon: {
				buffer = deserializarLocalizedPokemon(mensaje);
				break;

			}

			case tAppearedPokemon: {
				buffer = deserializarAppearedPokemon(mensaje);
				break;

			}

			case tSuscriptor: {
				buffer = deserializarSuscriptor(mensaje);
				break;

			}

			case 0: {
				//printf("Desconexion. \n");
				break;
			}


		default:{
				//perror("Se recibio un mensaje que no esta en el protocolo.");
				abort();
				break;
			}
		}
		return buffer;
}

void enviarPaquete(int fdCliente, int tipoMensaje, void * mensaje, int tamanioMensaje){

	int desplazamiento = 0;

	void* mensajeSerializado = serializar(tipoMensaje,mensaje,&tamanioMensaje);
	int tamanioTotal = sizeof(int) * 2 + tamanioMensaje;

	void* buffer = malloc(tamanioTotal);
	memcpy(buffer + desplazamiento, &tipoMensaje, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento, &tamanioMensaje, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento, mensajeSerializado, tamanioMensaje);

	enviarPorSocket(fdCliente, buffer, tamanioTotal);

	free(buffer);
	free(mensajeSerializado);

}

void* recibirPaquete(int fdCliente, int* tipoMensaje, int* tamanioMensaje){

	int recibido = recibirPorSocket(fdCliente, tipoMensaje, sizeof(int));

	log_info(logger,"Tipo de mensaje recibido: %d", *tipoMensaje);

	if (*tipoMensaje < 1 || *tipoMensaje > tFinDeProtocolo || recibido <= 0){
		return NULL;
	}

	recibido = recibirPorSocket(fdCliente, tamanioMensaje, sizeof(int));
	void* mensaje = malloc(*tamanioMensaje);

	recibido = recibirPorSocket(fdCliente, mensaje, *tamanioMensaje);

	void* buffer = deserializar(*tipoMensaje, mensaje);

	free(mensaje);
	return buffer;

} // Recordar castear
 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////SERIALIZACION/DESERIALIZACION DE PAQUETES/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////NEW POKEMON//////////////////////////////////////////////////////

void* serializarNewPokemon(t_newPokemon* newPokemon, int* tamanio){

	t_newPokemon* unNewPokemon = (t_newPokemon*) newPokemon;

	int desplazamiento = 0;
	int tamanioNombrePokemon = string_length(unNewPokemon->nombrePokemon);

	*tamanio = sizeof(int) + tamanioNombrePokemon + 5 * sizeof(uint32_t);

	void* newPokemonSerializado = malloc(*tamanio);

	memcpy(newPokemonSerializado + desplazamiento, &unNewPokemon->identificador, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(newPokemonSerializado + desplazamiento, &unNewPokemon->identificadorCorrelacional, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(newPokemonSerializado + desplazamiento, &tamanioNombrePokemon, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(newPokemonSerializado + desplazamiento, unNewPokemon->nombrePokemon, tamanioNombrePokemon);
	desplazamiento += tamanioNombrePokemon;

	memcpy(newPokemonSerializado + desplazamiento, &unNewPokemon->posicionEnElMapaX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(newPokemonSerializado + desplazamiento, &unNewPokemon->posicionEnElMapaY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(newPokemonSerializado + desplazamiento, &unNewPokemon->cantidadDePokemon, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	///////////////////////////////////////CASO DE PRUEBA SERIALIZACION NEW_POKEMON///////////////////////////////////////////////
	
	// printf("\n\nNEW_POKEMON A SERIALIZAR: \n");
	// printf("\nIdentificador: %d", unNewPokemon->identificador);
	// printf("\nIdentificador Correlacional: %d", unNewPokemon->identificadorCorrelacional);
	// printf("\nNombre del Pokemón: %s", unNewPokemon->nombrePokemon);
	// printf("\nPosicion en el mapa en el eje 'x': %d", unNewPokemon->posicionEnElMapaX);
	// printf("\nPosicion en el mapa en el eje 'y': %d", unNewPokemon->posicionEnElMapaY);
	// printf("\nTamaño del NEW_POKEMON: %d", *tamanio);
	// printf("\nCantidad del pokémon: %d", unNewPokemon->cantidadDePokemon);
	
	// t_newPokemon* newPokemonDeserializado = deserializarNewPokemon(newPokemonSerializado);

	// printf("\n\nNEW_POKEMON DESERIALIZADO: \n");
	// printf("\nIdentificador: %d", newPokemonDeserializado->identificador);
	// printf("\nIdentificador Correlacional: %d", newPokemonDeserializado->identificadorCorrelacional);
	// printf("\nNombre del Pokemón: %s", newPokemonDeserializado->nombrePokemon);
	// printf("\nPosicion en el mapa en el eje 'x': %d", newPokemonDeserializado->posicionEnElMapaX);
	// printf("\nCantidad del pokémon: %d", newPokemonDeserializado->cantidadDePokemon);
	// printf("\nPosicion en el mapa en el eje 'y': %d", newPokemonDeserializado->posicionEnElMapaY);

	// free(newPokemonDeserializado);
	
	///////////////////////////////////////FIN CASO DE PRUEBA SERIALIZACION NEW_POKEMON////////////////////////////////////////////

	return newPokemonSerializado;

}

t_newPokemon* deserializarNewPokemon(void* buffer){

	t_newPokemon* unNewPokemon = malloc(sizeof(t_newPokemon));

	int desplazamiento = 0;
	int tamanioNombrePokemon = 0;

	memcpy(&unNewPokemon->identificador, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unNewPokemon->identificadorCorrelacional, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&tamanioNombrePokemon, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char* bufferNombrePokemon = malloc(tamanioNombrePokemon+1);
	memcpy(bufferNombrePokemon, buffer + desplazamiento, tamanioNombrePokemon);
	bufferNombrePokemon[tamanioNombrePokemon] = '\0';
	desplazamiento += tamanioNombrePokemon;

	unNewPokemon->nombrePokemon = string_new();

	string_append(&unNewPokemon->nombrePokemon, bufferNombrePokemon);

	memcpy(&unNewPokemon->posicionEnElMapaX, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unNewPokemon->posicionEnElMapaY, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unNewPokemon->cantidadDePokemon, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	free(bufferNombrePokemon);

	return unNewPokemon;

}

/////////////////////////////////////////////////////GET POKEMON////////////////////////////////////////////////////////

void* serializarGetPokemon(t_getPokemon* getPokemon, int* tamanio){

	t_getPokemon* unGetPokemon = (t_getPokemon*) getPokemon;

	int desplazamiento = 0;
	int tamanioNombrePokemon = string_length(unGetPokemon->nombrePokemon);

	*tamanio = sizeof(int) + tamanioNombrePokemon + 2 * sizeof(uint32_t);

	void* getPokemonSerializado = malloc(*tamanio);

	memcpy(getPokemonSerializado + desplazamiento, &unGetPokemon->identificador, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(getPokemonSerializado + desplazamiento, &unGetPokemon->identificadorCorrelacional, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(getPokemonSerializado + desplazamiento, &tamanioNombrePokemon, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(getPokemonSerializado + desplazamiento, unGetPokemon->nombrePokemon, tamanioNombrePokemon);
	desplazamiento += tamanioNombrePokemon;

	///////////////////////////////////////CASO DE PRUEBA SERIALIZACION GET_POKEMON///////////////////////////////////////////////
	
	// printf("\n\nGET_POKEMON A SERIALIZAR: \n");
	// printf("\nIdentificador: %d", unGetPokemon->identificador);
	// printf("\nIdentificador Correlacional: %d", unGetPokemon->identificadorCorrelacional);
	// printf("\nNombre del Pokemón: %s", unGetPokemon->nombrePokemon);
	// printf("\nTamaño del GET_POKEMON: %d", *tamanio);
	
	// t_getPokemon* getPokemonDeserializado = deserializarGetPokemon(getPokemonSerializado);

	// printf("\n\nGET_POKEMON DESERIALIZADO: \n");
	// printf("\nIdentificador: %d", getPokemonDeserializado->identificador);
	// printf("\nIdentificador Correlacional: %d", getPokemonDeserializado->identificadorCorrelacional);
	// printf("\nNombre del Pokemón: %s", getPokemonDeserializado->nombrePokemon);

	// free(getPokemonDeserializado);
	
	///////////////////////////////////////FIN CASO DE PRUEBA SERIALIZACION GET_POKEMON////////////////////////////////////////////

	return getPokemonSerializado;

}

t_getPokemon* deserializarGetPokemon(void* buffer){
	
	t_getPokemon* unGetPokemon = malloc(sizeof(t_getPokemon));

	int desplazamiento = 0;
	int tamanioNombrePokemon = 0;

	memcpy(&unGetPokemon->identificador, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unGetPokemon->identificadorCorrelacional, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&tamanioNombrePokemon, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char* bufferNombrePokemon = malloc(tamanioNombrePokemon+1);
	memcpy(bufferNombrePokemon, buffer + desplazamiento, tamanioNombrePokemon);
	bufferNombrePokemon[tamanioNombrePokemon] = '\0';
	desplazamiento += tamanioNombrePokemon;

	unGetPokemon->nombrePokemon = string_new();

	string_append(&unGetPokemon->nombrePokemon, bufferNombrePokemon);

	free(bufferNombrePokemon);

	return unGetPokemon;

	return;

}

/////////////////////////////////////////////////////SUSCRIPTOR////////////////////////////////////////////////////////

void* serializarSuscriptor(t_suscriptor* suscriptor, int* tamanio){

	t_suscriptor* unSuscriptor = (t_suscriptor*) suscriptor;

	int desplazamiento = 0;
	int tamanioIp = string_length(unSuscriptor->ip);

	*tamanio = sizeof(int) + tamanioIp + 5 * sizeof(uint32_t);

	void* suscriptorSerializado = malloc(*tamanio);

	memcpy(suscriptorSerializado + desplazamiento, &unSuscriptor->identificador, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(suscriptorSerializado + desplazamiento, &unSuscriptor->identificadorCorrelacional, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(suscriptorSerializado + desplazamiento, &unSuscriptor->colaDeMensajes, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(suscriptorSerializado + desplazamiento, &unSuscriptor->tiempoDeSuscripcion, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(suscriptorSerializado + desplazamiento, &unSuscriptor->puerto, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(suscriptorSerializado + desplazamiento, &tamanioIp, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(suscriptorSerializado + desplazamiento, unSuscriptor->ip, tamanioIp);
	desplazamiento += tamanioIp;

	///////////////////////////////////////CASO DE PRUEBA SERIALIZACION SUSCRIPTOR///////////////////////////////////////////////
	
	// printf("\n\nSUSCRIPTOR A SERIALIZAR: \n");
	// printf("\nIdentificador: %d", unSuscriptor->identificador);
	// printf("\nIdentificador Correlacional: %d", unSuscriptor->identificadorCorrelacional);
	// printf("\nCola de mensajes a suscribirse: %d", unSuscriptor->colaDeMensajes);
	// printf("\nTiempo de suscripción: %d", unSuscriptor->tiempoDeSuscripcion);
	// printf("\nTiempo de suscripción: %s", unSuscriptor->ip);
	// printf("\nTiempo de suscripción: %d", unSuscriptor->puerto);
	// printf("\nTamaño del SUSCRIPTOR: %d", *tamanio);
	
	// t_suscriptor* suscriptorDeserializado = deserializarSuscriptor(suscriptorSerializado);

	// printf("\n\nSUSCRIPTOR DESERIALIZADO: \n");
	// printf("\nIdentificador: %d", suscriptorDeserializado->identificador);
	// printf("\nIdentificador Correlacional: %d", suscriptorDeserializado->identificadorCorrelacional);
	// printf("\nCola de mensajes a suscribirse: %d", suscriptorDeserializado->colaDeMensajes);
	// printf("\nTiempo de suscripción: %d", suscriptorDeserializado->tiempoDeSuscripcion);	
	// printf("\nTiempo de suscripción: %s", suscriptorDeserializado->ip);
	// printf("\nTiempo de suscripción: %d", suscriptorDeserializado->puerto);

	// free(suscriptorDeserializado);
	
	///////////////////////////////////////FIN CASO DE PRUEBA SERIALIZACION SUSCRIPTOR////////////////////////////////////////////

	return suscriptorSerializado;

}

t_suscriptor* deserializarSuscriptor(void* buffer){
	
	t_suscriptor* unSuscriptor = malloc(sizeof(t_suscriptor));

	int desplazamiento = 0;
	int tamanioIp = 0;

	memcpy(&unSuscriptor->identificador, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unSuscriptor->identificadorCorrelacional, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unSuscriptor->colaDeMensajes, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unSuscriptor->tiempoDeSuscripcion, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unSuscriptor->puerto, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&tamanioIp, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char* bufferIp = malloc(tamanioIp+1);
	memcpy(bufferIp, buffer + desplazamiento, tamanioIp);
	bufferIp[tamanioIp] = '\0';
	desplazamiento += tamanioIp;

	unSuscriptor->ip = string_new();

	string_append(&unSuscriptor->ip, bufferIp);

	free(bufferIp);

	return unSuscriptor;

}

///////////////////////////////////////////////////CATCH POKEMON//////////////////////////////////////////////////////

void* serializarCatchPokemon(t_catchPokemon* catchPokemon, int* tamanio){

	t_catchPokemon* unCatchPokemon = (t_catchPokemon*) catchPokemon;

	int desplazamiento = 0;
	int tamanioNombrePokemon = string_length(unCatchPokemon->nombrePokemon);

	*tamanio = sizeof(int) + tamanioNombrePokemon + 4 * sizeof(uint32_t);

	void* catchPokemonSerializado = malloc(*tamanio);

	memcpy(catchPokemonSerializado + desplazamiento, &unCatchPokemon->identificador, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(catchPokemonSerializado + desplazamiento, &unCatchPokemon->identificadorCorrelacional, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(catchPokemonSerializado + desplazamiento, &tamanioNombrePokemon, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(catchPokemonSerializado + desplazamiento, unCatchPokemon->nombrePokemon, tamanioNombrePokemon);
	desplazamiento += tamanioNombrePokemon;

	memcpy(catchPokemonSerializado + desplazamiento, &unCatchPokemon->posicionEnElMapaX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(catchPokemonSerializado + desplazamiento, &unCatchPokemon->posicionEnElMapaY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	///////////////////////////////////////CASO DE PRUEBA SERIALIZACION CATCH_POKEMON///////////////////////////////////////////////
	
	// printf("\n\nCATCH_POKEMON A SERIALIZAR: \n");
	// printf("\nIdentificador: %d", unCatchPokemon->identificador);
	// printf("\nIdentificador Correlacional: %d", unCatchPokemon->identificadorCorrelacional);
	// printf("\nNombre del Pokemón: %s", unCatchPokemon->nombrePokemon);
	// printf("\nPosicion en el mapa en el eje 'x': %d", unCatchPokemon->posicionEnElMapaX);
	// printf("\nPosicion en el mapa en el eje 'y': %d", unCatchPokemon->posicionEnElMapaY);
	// printf("\nTamaño del CATCH_POKEMON: %d", *tamanio);
	
	// t_catchPokemon* catchPokemonDeserializado = deserializarCatchPokemon(catchPokemonSerializado);

	// printf("\n\nCATCH_POKEMON DESERIALIZADO: \n");
	// printf("\nIdentificador: %d", catchPokemonDeserializado->identificador);
	// printf("\nIdentificador Correlacional: %d", catchPokemonDeserializado->identificadorCorrelacional);
	// printf("\nNombre del Pokemón: %s", catchPokemonDeserializado->nombrePokemon);
	// printf("\nPosicion en el mapa en el eje 'x': %d", catchPokemonDeserializado->posicionEnElMapaX);
	// printf("\nPosicion en el mapa en el eje 'y': %d", catchPokemonDeserializado->posicionEnElMapaY);

	// free(catchPokemonDeserializado);
	
	///////////////////////////////////////FIN CASO DE PRUEBA SERIALIZACION CATCH_POKEMON////////////////////////////////////////////

	return catchPokemonSerializado;

}

t_catchPokemon* deserializarCatchPokemon(void* buffer){

	t_catchPokemon* unCatchPokemon = malloc(sizeof(t_catchPokemon));

	int desplazamiento = 0;
	int tamanioNombrePokemon = 0;

	memcpy(&unCatchPokemon->identificador, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unCatchPokemon->identificadorCorrelacional, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&tamanioNombrePokemon, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char* bufferNombrePokemon = malloc(tamanioNombrePokemon+1);
	memcpy(bufferNombrePokemon, buffer + desplazamiento, tamanioNombrePokemon);
	bufferNombrePokemon[tamanioNombrePokemon] = '\0';
	desplazamiento += tamanioNombrePokemon;

	memcpy(&unCatchPokemon->posicionEnElMapaX, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unCatchPokemon->posicionEnElMapaY, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	unCatchPokemon->nombrePokemon = string_new();

	string_append(&unCatchPokemon->nombrePokemon, bufferNombrePokemon);

	free(bufferNombrePokemon);

	return unCatchPokemon;

}

///////////////////////////////////////////////////CAUGHT POKEMON//////////////////////////////////////////////////////

void* serializarCaughtPokemon(t_caughtPokemon* caughtPokemon, int* tamanio){

	t_caughtPokemon* unCaughtPokemon = (t_caughtPokemon*) caughtPokemon;

	int desplazamiento = 0;
	int tamanioNombrePokemon = string_length(unCaughtPokemon->nombrePokemon);

	*tamanio = sizeof(int) + tamanioNombrePokemon + 3 * sizeof(uint32_t);

	void* caughtPokemonSerializado = malloc(*tamanio);

	memcpy(caughtPokemonSerializado + desplazamiento, &unCaughtPokemon->identificador, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(caughtPokemonSerializado + desplazamiento, &unCaughtPokemon->identificadorCorrelacional, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(caughtPokemonSerializado + desplazamiento, &tamanioNombrePokemon, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(caughtPokemonSerializado + desplazamiento, unCaughtPokemon->nombrePokemon, tamanioNombrePokemon);
	desplazamiento += tamanioNombrePokemon;

	memcpy(caughtPokemonSerializado + desplazamiento, &unCaughtPokemon->resultado, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	///////////////////////////////////////CASO DE PRUEBA SERIALIZACION CAUGHT_POKEMON///////////////////////////////////////////////
	
	// printf("\n\nCAUGHT_POKEMON A SERIALIZAR: \n");
	// printf("\nIdentificador: %d", unCaughtPokemon->identificador);
	// printf("\nIdentificador Correlacional: %d", unCaughtPokemon->identificadorCorrelacional);
	// printf("\nNombre del Pokemón: %s", unCaughtPokemon->nombrePokemon);
	// printf("\nResultado de la atrapada: %s", unCaughtPokemon->resultado ? "true" : "false");
	// printf("\nTamaño del CAUGHT_POKEMON: %d", *tamanio);
	
	// t_caughtPokemon* caughtPokemonDeserializado = deserializarCaughtPokemon(caughtPokemonSerializado);

	// printf("\n\nCAUGHT_POKEMON DESERIALIZADO: \n");
	// printf("\nIdentificador: %d", caughtPokemonDeserializado->identificador);
	// printf("\nIdentificador Correlacional: %d", caughtPokemonDeserializado->identificadorCorrelacional);
	// printf("\nNombre del Pokemón: %s", caughtPokemonDeserializado->nombrePokemon);
	// printf("\nResultado de la atrapada: %s", caughtPokemonDeserializado->resultado ? "true" : "false");

	// free(caughtPokemonDeserializado);
	
	///////////////////////////////////////FIN CASO DE PRUEBA SERIALIZACION CAUGHT_POKEMON////////////////////////////////////////////

	return caughtPokemonSerializado;

}

t_caughtPokemon* deserializarCaughtPokemon(void* buffer){

	t_caughtPokemon* unCaughtPokemon = malloc(sizeof(t_caughtPokemon));

	int desplazamiento = 0;
	int tamanioNombrePokemon = 0;

	memcpy(&unCaughtPokemon->identificador, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unCaughtPokemon->identificadorCorrelacional, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&tamanioNombrePokemon, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char* bufferNombrePokemon = malloc(tamanioNombrePokemon+1);
	memcpy(bufferNombrePokemon, buffer + desplazamiento, tamanioNombrePokemon);
	bufferNombrePokemon[tamanioNombrePokemon] = '\0';
	desplazamiento += tamanioNombrePokemon;

	memcpy(&unCaughtPokemon->resultado, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	unCaughtPokemon->nombrePokemon = string_new();

	string_append(&unCaughtPokemon->nombrePokemon, bufferNombrePokemon);

	free(bufferNombrePokemon);

	return unCaughtPokemon;

}

//////////////////////////////////////////////////LOCALIZED POKEMON////////////////////////////////////////////////////

void* serializarLocalizedPokemon(t_localizedPokemon* localizedPokemon, int* tamanio){

	t_localizedPokemon* unLocalizedPokemon = (t_localizedPokemon*) localizedPokemon;

	int cantidadListaDatosPokemon = list_size(unLocalizedPokemon->listaDatosPokemon);
	//int tamanioDatosPokemon = 0; 
	int contador = 0;

	int desplazamiento = 0;
	int tamanioNombrePokemon = string_length(unLocalizedPokemon->nombrePokemon);

	*tamanio = 3 * sizeof(int) + tamanioNombrePokemon + 2 * sizeof(uint32_t) + sizeof(datosPokemon) * cantidadListaDatosPokemon;

	//printf("\nTamaño datosPokemon: %d", sizeof(datosPokemon) * cantidadListaDatosPokemon);
	
	printf("\nTamaño del paquete a serializar: %d", *tamanio); //51

	void* localizedPokemonSerializado = malloc(*tamanio);

	memcpy(localizedPokemonSerializado + desplazamiento, tamanio, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(localizedPokemonSerializado + desplazamiento, &unLocalizedPokemon->identificador, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(localizedPokemonSerializado + desplazamiento, &unLocalizedPokemon->identificadorCorrelacional, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(localizedPokemonSerializado + desplazamiento, &tamanioNombrePokemon, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(localizedPokemonSerializado + desplazamiento, unLocalizedPokemon->nombrePokemon, tamanioNombrePokemon);
	desplazamiento += tamanioNombrePokemon;

	memcpy(localizedPokemonSerializado + desplazamiento, &cantidadListaDatosPokemon, sizeof(int));
	desplazamiento += sizeof(int);

	while(contador < cantidadListaDatosPokemon){

		datosPokemon* nodoDatosPokemon = list_get(unLocalizedPokemon->listaDatosPokemon,contador);

		memcpy(localizedPokemonSerializado + desplazamiento, &nodoDatosPokemon->cantidad, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		memcpy(localizedPokemonSerializado + desplazamiento, &nodoDatosPokemon->posicionEnElMapaX, sizeof(uint32_t));
	 	desplazamiento += sizeof(uint32_t);

	 	memcpy(localizedPokemonSerializado + desplazamiento, &nodoDatosPokemon->posicionEnElMapaY, sizeof(uint32_t));
	 	desplazamiento += sizeof(uint32_t);

		contador+=1;
	}
	/*
	///////////////////////////////////////CASO DE PRUEBA SERIALIZACION LOCALIZED_POKEMON///////////////////////////////////////////////
	
	printf("\n\nLOCALIZED_POKEMON A SERIALIZAR: \n");
	printf("\nIdentificador: %d", unLocalizedPokemon->identificador);
	printf("\nIdentificador Correlacional: %d", unLocalizedPokemon->identificadorCorrelacional);
	printf("\nNombre del Pokemón: %s", unLocalizedPokemon->nombrePokemon);

	contador = 0;
	datosPokemon* nodoDatosPokemon;

	while(contador < cantidadListaDatosPokemon){

		nodoDatosPokemon = list_get(unLocalizedPokemon->listaDatosPokemon,contador); 

		printf("\nCantidad de pokemón en %d° ubicación: %d", contador, nodoDatosPokemon->cantidad);
		printf("\nUbicacion en 'x': %d", nodoDatosPokemon->posicionEnElMapaX);
		printf("\nUbicacion en 'y': %d\n", nodoDatosPokemon->posicionEnElMapaY);

		contador+=1;

	}

	printf("\nTamaño del LOCALIZED_POKEMON: %d", *tamanio);//tiene que dar 47

	list_destroy_and_destroy_elements(unLocalizedPokemon->listaDatosPokemon,eliminarNodoDatosPokemon);
	
	t_localizedPokemon* localizedPokemonDeserializado = deserializarLocalizedPokemon(localizedPokemonSerializado);

	printf("\n\nLOCALIZED_POKEMON DESERIALIZADO: \n");
	printf("\nIdentificador: %d", localizedPokemonDeserializado->identificador);
	printf("\nIdentificador Correlacional: %d", localizedPokemonDeserializado->identificadorCorrelacional);
	printf("\nNombre del Pokemón: %s", localizedPokemonDeserializado->nombrePokemon);

	contador = 0;

	while(contador < cantidadListaDatosPokemon){

		nodoDatosPokemon = list_get(localizedPokemonDeserializado->listaDatosPokemon,contador);
		
		printf("\nCantidad de pokemón en %d° ubicación: %d", contador, nodoDatosPokemon->cantidad);
		printf("\nUbicacion en 'x': %d", nodoDatosPokemon->posicionEnElMapaX);
		printf("\nUbicacion en 'y': %d\n", nodoDatosPokemon->posicionEnElMapaY);

		contador+=1;

	}

	free(localizedPokemonDeserializado);
	
	///////////////////////////////////////FIN CASO DE PRUEBA SERIALIZACION LOCALIZED_POKEMON////////////////////////////////////////////
    */
	return localizedPokemonSerializado;

}

t_localizedPokemon* deserializarLocalizedPokemon(void* buffer){

	int desplazamiento = 0;
	int tamanioNombrePokemon = 0;
	int tamanioBuffer = 0;

	memcpy(&tamanioBuffer, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	t_localizedPokemon* unLocalizedPokemon = malloc(tamanioBuffer - 2 * sizeof(int)); //porque el buffer contiene el tamaño total del paquete y el tamaño del string, que no deben ser considerados al mallocear
	

	memcpy(&unLocalizedPokemon->identificador, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unLocalizedPokemon->identificadorCorrelacional, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&tamanioNombrePokemon, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char* bufferNombrePokemon = malloc(tamanioNombrePokemon+1);
	memcpy(bufferNombrePokemon, buffer + desplazamiento, tamanioNombrePokemon);
	bufferNombrePokemon[tamanioNombrePokemon] = '\0';
	desplazamiento += tamanioNombrePokemon;

	unLocalizedPokemon->nombrePokemon = string_new();
	string_append(&unLocalizedPokemon->nombrePokemon, bufferNombrePokemon);

	free(bufferNombrePokemon);

	int cantidadListaDatosPokemon = 0;

	memcpy(&cantidadListaDatosPokemon, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	t_list* unaListaDatosPokemon = list_create();
	int contador = 0;

	while(contador < cantidadListaDatosPokemon){

		datosPokemon* nodoDatosPokemon = malloc(sizeof(datosPokemon));

		memcpy(&nodoDatosPokemon->cantidad, buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		memcpy(&nodoDatosPokemon->posicionEnElMapaX, buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		memcpy(&nodoDatosPokemon->posicionEnElMapaY, buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		contador +=1;

		list_add(unaListaDatosPokemon,nodoDatosPokemon);

	}

	unLocalizedPokemon->listaDatosPokemon = unaListaDatosPokemon;

	return unLocalizedPokemon;

}

//////////////////////////////////////////////////APPEARED POKEMON////////////////////////////////////////////////////

void* serializarAppearedPokemon(t_appearedPokemon* appearedPokemon, int* tamanio){

	t_appearedPokemon* unAppearedPokemon = (t_appearedPokemon*) appearedPokemon;

	int desplazamiento = 0;
	int tamanioNombrePokemon = string_length(unAppearedPokemon->nombrePokemon);

	*tamanio = sizeof(int) + tamanioNombrePokemon + 4 * sizeof(uint32_t);

	void* appearedPokemonSerializado = malloc(*tamanio);

	memcpy(appearedPokemonSerializado + desplazamiento, &unAppearedPokemon->identificador, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(appearedPokemonSerializado + desplazamiento, &unAppearedPokemon->identificadorCorrelacional, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(appearedPokemonSerializado + desplazamiento, &tamanioNombrePokemon, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(appearedPokemonSerializado + desplazamiento, unAppearedPokemon->nombrePokemon, tamanioNombrePokemon);
	desplazamiento += tamanioNombrePokemon;

	memcpy(appearedPokemonSerializado + desplazamiento, &unAppearedPokemon->posicionEnElMapaX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(appearedPokemonSerializado + desplazamiento, &unAppearedPokemon->posicionEnElMapaY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

/*	///////////////////////////////////////CASO DE PRUEBA SERIALIZACION APPEARED_POKEMON///////////////////////////////////////////////
	
	printf("\n\nAPPEARED_POKEMON A SERIALIZAR: \n");
	printf("\nIdentificador: %d", unAppearedPokemon->identificador);
	printf("\nIdentificador Correlacional: %d", unAppearedPokemon->identificadorCorrelacional);
	printf("\nNombre del Pokemón: %s", unAppearedPokemon->nombrePokemon);
	printf("\nPosicion en el mapa en el eje 'x': %d", unAppearedPokemon->posicionEnElMapaX);
	printf("\nPosicion en el mapa en el eje 'y': %d", unAppearedPokemon->posicionEnElMapaY);
	printf("\nTamaño del APPEARED_POKEMON: %d", *tamanio);
	
	t_appearedPokemon* appearedPokemonDeserializado = deserializarAppearedPokemon(appearedPokemonSerializado);

	printf("\n\nAPPEARED_POKEMON DESERIALIZADO: \n");
	printf("\nIdentificador: %d", appearedPokemonDeserializado->identificador);
	printf("\nIdentificador Correlacional: %d", appearedPokemonDeserializado->identificadorCorrelacional);
	printf("\nNombre del Pokemón: %s", appearedPokemonDeserializado->nombrePokemon);
	printf("\nPosicion en el mapa en el eje 'x': %d", appearedPokemonDeserializado->posicionEnElMapaX);
	printf("\nPosicion en el mapa en el eje 'y': %d", appearedPokemonDeserializado->posicionEnElMapaY);

	free(appearedPokemonDeserializado);
	
	///////////////////////////////////////FIN CASO DE PRUEBA SERIALIZACION APPEARED_POKEMON////////////////////////////////////////////
*/
	return appearedPokemonSerializado;

}

t_appearedPokemon* deserializarAppearedPokemon(void* buffer){

	t_appearedPokemon* unAppearedPokemon = malloc(sizeof(t_appearedPokemon));

	int desplazamiento = 0;
	int tamanioNombrePokemon = 0;

	memcpy(&unAppearedPokemon->identificador, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unAppearedPokemon->identificadorCorrelacional, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&tamanioNombrePokemon, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char* bufferNombrePokemon = malloc(tamanioNombrePokemon+1);
	memcpy(bufferNombrePokemon, buffer + desplazamiento, tamanioNombrePokemon);
	bufferNombrePokemon[tamanioNombrePokemon] = '\0';
	desplazamiento += tamanioNombrePokemon;

	memcpy(&unAppearedPokemon->posicionEnElMapaX, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&unAppearedPokemon->posicionEnElMapaY, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	unAppearedPokemon->nombrePokemon = string_new();

	string_append(&unAppearedPokemon->nombrePokemon, bufferNombrePokemon);

	free(bufferNombrePokemon);

	return unAppearedPokemon;

}

//////////////////////////////////////////FUNCIONES PARA SERIALIZACION DE LISTAS///////////////////////////////////////

void eliminarNodoDatosPokemon(datosPokemon* unNodoDatosPokemon){

    free(unNodoDatosPokemon);
	return;
}