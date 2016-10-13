//
// Created by mariano on 12/10/16.
//

#include "Configurador.h"
#include "Logger.h"

Configurador::Configurador(ConfigLoader *config) {
    this->config = config;
    Logger::getInstance().log("MESAS: "+std::to_string(config->getMesas()));
}

void Configurador::crearEstructuras() {

    sem_entrada = new Semaforo(SEM_ENTRADA, 0, false);
    sem_recepcion = new Semaforo(SEM_RECEPCION, 0, false);
    sem_living = new Semaforo(SEM_LIVING, 0, false);

    for (int i=0;i< config->getMesas();i++) {
        semaforosPedidoDeMesas[i] = new Semaforo(SEM_MESAS+i,0,false);
    }

    fifoRecepcionLectura = new FifoLectura(ARCHIVO_FIFO);
    fifoRecepcionEscritura = new FifoEscritura(ARCHIVO_FIFO);
    fifoLivingLectura = new FifoLectura(ARCHIVO_FIFO_LIVING);
    fifoLivingEscritura = new FifoEscritura(ARCHIVO_FIFO_LIVING);
    recepcionistas = new GeneradorRecepcionistas(config->getRecepcionistas());
    clientes = new GeneradorClientes();

    this->cagarGeneradorDeRecepcionistas();
    this->cargarGeneradorDeClientes();

    mesas = new Mesas(config->getMesas());
    administradorLiving = new AdministradorLiving();

}

void Configurador::simular() {

    mesas->armarMesas();
    administradorLiving->armarLiving();

    pid_recepcionistas = recepcionistas->cargarRecepcionistas(config->getMesas());

    pid_clientes = clientes->cargarClientes();

    waitpid(pid_clientes,NULL,0);

    // eliminar semaforos
    sem_entrada->eliminar();
    sem_recepcion->eliminar();
    sem_living->eliminar();

    waitpid(pid_recepcionistas,NULL,0);

}


void Configurador::destruirEstructuras() {

    fifoRecepcionEscritura->eliminar();
    fifoRecepcionLectura->eliminar();
    fifoLivingEscritura->eliminar();
    fifoLivingLectura->eliminar();
    mesas->desarmarMesas();
    administradorLiving->desarmarLiving();

    delete sem_entrada;
    delete sem_recepcion;
    delete sem_living;
    delete fifoLivingEscritura;
    delete fifoLivingLectura;
    delete fifoRecepcionEscritura;
    delete fifoRecepcionLectura;
    delete recepcionistas;
    delete clientes;
    delete mesas;
    delete administradorLiving;

}

void Configurador::cargarGeneradorDeClientes() {

    clientes->setFifoLivingLectura(fifoLivingLectura);
    clientes->setFifoRecepcionLectura(fifoRecepcionLectura);
    clientes->setSem_entrada(sem_entrada);
    clientes->setSem_living(sem_living);
    clientes->setSem_recepcion(sem_recepcion);

}

void Configurador::cagarGeneradorDeRecepcionistas() {

    recepcionistas->setFifoLivingEscritura(fifoLivingEscritura);
    recepcionistas->setFifoRecepcionEscritura(fifoRecepcionEscritura);
    recepcionistas->setSem_entrada(sem_entrada);
    recepcionistas->setSem_recepcion(sem_recepcion);
    recepcionistas->setSem_living(sem_living);

}