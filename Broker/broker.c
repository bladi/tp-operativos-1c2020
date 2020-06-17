#include "broker.h"

uint32_t main() {
	
	signal(SIGUSR1, manejarSeniales);

	inicializarBroker();

	finalizarBroker();
	
	return 0;

}