#if !defined __GURUHTTPSITIOSTATS_HPP__
#define __GURUHTTPSITIOSTATS_HPP__

#include <string>
#include <sstream>
#include <GuruAlgo.hpp>
#include <GuruSocket/GuruServidorTCP.hpp>
#include <GuruHttp/GuruhttpInterface.hpp>
#include <GuruHttp/Guruhttp.hpp>
#include <GuruHttp/GuruhttpResponse.hpp>

class stats: public SitioWeb {
public:
	void procesarGet() {
		GuruServidorTCP<Guruhttp>* puerto = static_cast<GuruServidorTCP<Guruhttp>*>(request()->config()->extra_ptr());
		GuruServidorTCP<Guruhttp>::estadisticas a;

		if (puerto) puerto->stats(a);

		ostringstream oss;

		oss << "Pool:" << endl;
		oss << "\tCapacidad: " << a.capacidad_pool << endl;
		oss << "\tTamaño actual: " << a.size_pool << endl;
		oss << "\tThreads en estado ocupado: " << a.threads_ejecutando << endl;
		oss << "\tThreads por finalizar: " << a.esperando_terminar << endl;
		oss << "\tThreads en espera: " << a.esperas << endl;
		oss << "\tServicios en Ejecucion: " << a.servicios_ejecutando << endl;
		oss << endl;
		oss << "Servidor: " << endl;
		oss << "\tPedidos: " << a.pedidos << endl;
		oss << "\tCola de Sockets: " << a.cola_socket << endl;
		oss << "\tErrores de socket listen: " << a.errores_socket_listen << endl;
		oss << "\tErrores de socket accept: " << a.errores_socket_accept << endl;
		oss << "\tInicio Ejecucion: " << GuruAlgo::fechaHoraRFC822(a.inicio_ejecucion) << endl;
		oss << "\tTiempo de Ejecucion: " << GuruAlgo::segundos_a_legible_dias(a.tiempo_ejecucion) << endl;

		enviar_buffer(oss.str(), "text/plain");
	};
	void procesarPost() {};
	void procesarPut() {};
	void procesarHead() {};
	void procesarDelete() {};
	void procesarTrace() {};
	void procesarOptions() {};
};

#endif
