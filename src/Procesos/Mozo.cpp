//
// Created by mafv on 12/10/16.
//

#include "Mozo.h"
#include "../Logger.h"
#include "../ClientesPorComer.h"
#include "../Estructuras/FifoEscritura.h"
#include "../Utils.h"
#include "SaldoDeMesa.h"


Mozo::Mozo(int id) {
    this->id = id;
}

void Mozo::setFifoPedidoMozo(FifoLectura *fifoPedidoMozo) {
    Mozo::fifoPedidoMozo = fifoPedidoMozo;
}

void Mozo::setFifoMozosCocineroLectura(FifoLectura * f){
    Mozo::fifoMozosCocineroLectura = f;
}

void Mozo::setSemaforosPedidoDeMesas(const std::map<int, Semaforo> &semaforosPedidoDeMesas) {
    Mozo::semaforosPedidoDeMesas = semaforosPedidoDeMesas;
}

void Mozo::setFifoCocineroEscritura(FifoEscritura* f){
    this->fifoCocineroEscritura = f;
}

Mozo::Mozo() {

}

void Mozo::_run() {

    Logger::getInstance().log("Mozo " + std::to_string(id) + " creado");

    fifoPedidoMozo->obtenerCopia();
    fifoMozosCocineroLectura->obtenerCopia();
    fifoCocineroEscritura->obtenerCopia();

    LockFile lock(LOCK_MOZOS);

    while(true) {

        lock.tomarLock();

        ordenDeComida pedido;

        ssize_t leido = fifoPedidoMozo->leer(&pedido,sizeof(pedido));

        //Se cerro el fifo de los clientes
        if(leido == 0){
            lock.liberarLock();
            break;
        }

        lock.liberarLock();

        if(pedido.pedidoDeCuenta){

            this->buscarSaldoAPagar(pedido);

        } else {

            this->solicitarPedidoAlCocinero(pedido);

        }

    }

    Logger::getInstance().log("Mozo " + std::to_string(id) +" No Hay mas clientes me retiro");

    fifoPedidoMozo->cerrar();
    fifoMozosCocineroLectura->cerrar();
    fifoCocineroEscritura->cerrar();

}

void Mozo::solicitarPedidoAlCocinero(ordenDeComida orden) {

    SaldoDeMesa saldo;

    saldo.agregarSaldo(orden.numeroDeMesa,menu->getPlato(orden.numeroPlato).getPrecio());

    Logger::getInstance().log("Mozo " + std::to_string(id) + " recibio un pedido de la mesa: " + std::to_string(orden.numeroDeMesa));

    sleep(3);
    fifoCocineroEscritura->escribir(&orden,sizeof(orden));

    Logger::getInstance().log("Mozo " + std::to_string(id) + " pidio plato " +  menu->getPlato(orden.numeroPlato).getNombre()
                              +" de la mesa " + std::to_string(orden.numeroDeMesa) + " al cocinero");

    //todo: lock aca ?
    ordenDeComida comidaParaEntregar;
    fifoMozosCocineroLectura->leer(&comidaParaEntregar,sizeof(comidaParaEntregar));

    Logger::getInstance().log("Mozo " + std::to_string(id) + " recibio plato "+ menu->getPlato(comidaParaEntregar.numeroPlato).getNombre() +" del cocinero para la mesa: " + std::to_string(comidaParaEntregar.numeroDeMesa));

    this->entregarPedidoAlCliente(comidaParaEntregar);

}

void Mozo::entregarPedidoAlCliente(ordenDeComida comidaParaEntregar) {

    //Estas dos lineas irian en el metodo entregarPedidoAlCliente

    Logger::getInstance().log("Mozo " + std::to_string(id) + " entrego pedido a la meza: " + std::to_string(comidaParaEntregar.numeroDeMesa));

    //sleep(3);

    semaforosPedidoDeMesas[comidaParaEntregar.numeroDeMesa].v();

}

void Mozo::setMenu(Menu *menu) {
    Mozo::menu = menu;
}

void Mozo::buscarSaldoAPagar(ordenDeComida pedido) {

    Logger::getInstance().log("Soy el Mozo " + std::to_string(id) + " y voy a consultar lo que debe la mesa " + std::to_string(pedido.numeroDeMesa));

    SaldoDeMesa saldo;
    float totalAPagar = saldo.obtenerSaldo(pedido.numeroDeMesa);

    Logger::getInstance().log("Soy el Mozo " + std::to_string(id) + " y el cliente de la mesa " + std::to_string(pedido.numeroDeMesa)+" debe pagar "+std::to_string(totalAPagar));

    semaforosPedidoDeMesas[pedido.numeroDeMesa].v();

    semaforosSaldos[pedido.numeroDeMesa].p();

    Logger::getInstance().log("Soy el Mozo " + std::to_string(id) + " y recibi el pago de la mesa " + std::to_string(pedido.numeroDeMesa));

    saldo.reiniciar(pedido.numeroDeMesa);

}

void Mozo::setSemaforosSaldos(const std::map<int, Semaforo> &semaforosSaldos) {
    Mozo::semaforosSaldos = semaforosSaldos;
}