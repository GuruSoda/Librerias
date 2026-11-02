#ifndef __GURUSOCKET_SERVIDOR_PROTOCOLO_H__
#define __GURUSOCKET_SERVIDOR_PROTOCOLO_H__

#include <GuruSocketServidor.h>

template <class T>
class GuruServidorProtocolo: public GuruConexionSocket
{
public:
	GuruServidorProtocolo(void) {};
	~GuruServidorProtocolo(void) {};

	void main (void);
};

template <class T>
void GuruServidorProtocolo<T>::main (void){
	T protocolo(sacar_socket());
	protocolo.main();
}

#endif
