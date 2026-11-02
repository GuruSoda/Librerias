#if !defined __GURUPROTEJECUTAR_HPP__
#define __GURUPROTEJECUTAR_HPP__

#include <GuruCompat.h>
#include <GuruSocket/GuruProtocoloServidor.hpp>
#include <GuruEjecutar.hpp>
#include <GuruEtiquetaLeer.hpp>

#include <string>

class GuruProtEjecutar: public GuruProtocoloServidor
{
public:
	GuruProtEjecutar(IEnviar& enviar, IRecibir& recibir):GuruProtocoloServidor(enviar, recibir) {};
	virtual ~GuruProtEjecutar() {};

	bool init() {
		cargar_msg_bienvenida("Bienvenido al Protocolo de Ejecucion.");
		cargar_msg_salida("Gracias por utilizar el Protocolo de Ejecucion.");

		GuruProtocoloServidor::init();

		cargar_comando(this, &GuruProtEjecutar::cmd_ejecutar, "Ejecutar", "Ejecuta una aplicacion.");

		cargar_comando(this, &GuruProtEjecutar::cmd_compilacion, "compilacion", "Fecha y hora de compilacion.");

		return true;
	}

private:
	void cmd_ejecutar(const vector<string>& parametros) {
		if (parametros.size() == 1) { MAL("Error en parametros."); return; }

		GuruEtiquetaLeerEx	tag(parametros);

		std::string comando;
		bool esperar;

		comando = tag["comando"];
		esperar = ((tag["esperar"].empty()) || (tag["esperar"].compare("0") == 0)) ? false : true;

		if (comando.empty()) { MAL("Se necesita el parametro comando."); return; }

		GuruEjecutar::opciones opt;
		opt.esperar(esperar);

		if (GuruEjecutar::ejecutar(comando, opt)) {
			BIEN("Todo Bien?");
		} else MAL(opt.msg());
	}

	void cmd_compilacion(const vector<string>& parametros) {
		BIEN(__DATE__ " - " __TIME__);
	}
};

#endif
