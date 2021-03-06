#include "FifoEscritura.h"

FifoEscritura::FifoEscritura(const std::string nombre) : Fifo(nombre) {
}

FifoEscritura::FifoEscritura() {

}

FifoEscritura::~FifoEscritura() {
}

void FifoEscritura::abrir() {
	fd = open ( nombre.c_str(),O_WRONLY );

	if (fd == -1) {

		std::string errMsg = std::string(std::string (strerror(errno)));
		throw "Fallo al abrir el fifo escritura: " + errMsg;

	}
}

ssize_t FifoEscritura::escribir(const void* buffer,const ssize_t buffsize) const {
	ssize_t escribio = write ( fd,buffer,buffsize );
	if(escribio < 0 ) {
		std::string errMsg = std::string(std::string (strerror(errno)));
		throw "Fallo al escribir el fifo escritura: " + errMsg;
	}
}


