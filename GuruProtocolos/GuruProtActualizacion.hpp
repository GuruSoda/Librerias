#if !defined __GURUPROTACTUALIZACION_HPP__
#define __GURUPROTACTUALIZACION_HPP__

#include <GuruCompat.h>
#include <GuruSocket/GuruProtocoloServidor.hpp>
#include <GuruEjecutar.hpp>
#include <GuruEtiquetaLeer.hpp>

#include <string>

class GuruProtActualizacion: public GuruProtocoloServidor
{
public:
	GuruProtActualizacion(IEnviar& enviar, IRecibir& recibir):GuruProtocoloServidor(enviar, recibir) {};
	virtual ~GuruProtActualizacion() {};

	bool init() {
		cargar_msg_bienvenida("Bienvenido al Protocolo de Actualizacion.");
		cargar_msg_salida("Gracias por utilizar el Protocolo de Actualizacion.");

		GuruProtocoloServidor::init();

		cargar_comando(this, &GuruProtActualizacion::cmd_actualizar, "Actualizar", "Actualiza esta misma aplicacion.");

		cargar_comando(this, &GuruProtActualizacion::cmd_compilacion, "compilacion", "Fecha y hora de compilacion.");

		return true;
	}

private:
	void cmd_actualizar(const vector<string>& parametros) {

		GuruEtiquetaLeerEx	tag(parametros);

		std::string origen, destino, inicio, fin, actualizador;

		actualizador = tag["actualizador"];
		origen = tag["origen"];
		destino = tag["destino"];
		inicio = tag["inicio"];
		fin = tag["fin"];

		string cmd;

		if (!actualizador.empty()) cmd += actualizador;
		else cmd = "Actualizador.exe";

		if (!GuruDirectorio::existe(cmd)) { MAL("No existe " + cmd + ", Se cancela la actualizacion."); return; }

		if (!origen.empty()) cmd += " --origen=" + GuruStringAlgo::comillas(origen);
		if (!destino.empty()) cmd += " --destino=" + GuruStringAlgo::comillas(destino);
		if (!inicio.empty()) cmd += " --inicio=" + GuruStringAlgo::comillas(inicio);
		if (!fin.empty()) cmd += " --fin=" + GuruStringAlgo::comillas(fin);

		GuruEjecutar::opciones opt;
		opt.esperar(false);

		if (GuruEjecutar::ejecutar(cmd, opt)) {
			BIEN("Todo Bien?");
		} else MAL(opt.msg());

	}

	void cmd_compilacion(const vector<string>& parametros) {
		BIEN(__DATE__ " - " __TIME__);
	}
};

#endif
