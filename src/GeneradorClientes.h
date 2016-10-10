
#ifndef TP1_GENERADORDECLIENTES_H
#define TP1_GENERADORDECLIENTES_H

#include "Estructuras/Semaforo.h"
#include "Procesos/Cliente.h"
#include "Estructuras/FifoLectura.h"
#include "Constantes.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdexcept>

class GeneradorClientes {

public:
    GeneradorClientes();
    pid_t cargarClientes(Semaforo sem_entrada, Semaforo sem_recepcion,FifoLectura fifoRecepcionLectura);

};


#endif //TP1_GENERADORDECLIENTES_H
