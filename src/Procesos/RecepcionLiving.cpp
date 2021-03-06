//
// Created by mafv on 10/10/16.
//

#include "RecepcionLiving.h"

RecepcionLiving::RecepcionLiving(int cantidadDeMesas) {

    this->cantidadDeMesas = cantidadDeMesas;
}

void RecepcionLiving::_run(){


    fifoLivingEscritura->obtenerCopia();

    while (sem_living->p() > -1) {

        AdministradorLiving administradorLiving;
        Mesas mesas(cantidadDeMesas);

        if(administradorLiving.hayClientesEnElLiving()){

            int mesaLibre = mesas.obtenerMesaDesocupada();
            fifoLivingEscritura->escribir(&mesaLibre, sizeof(int));

            administradorLiving.sacarClienteDelLiving();

        }

    }

    fifoLivingEscritura->cerrar();

}

void RecepcionLiving::setSem_living(Semaforo *sem_living) {
    RecepcionLiving::sem_living = sem_living;
}

void RecepcionLiving::setFifoLivingEscritura(FifoEscritura *fifoLivingEscritura) {
    RecepcionLiving::fifoLivingEscritura = fifoLivingEscritura;
}
