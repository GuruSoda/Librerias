#if !defined __GURUPROTOCOLOINFORMACION_H__
#define __GURUPROTOCOLOINFORMACION_H__

#include <GuruCompat.h>

#include <GuruInfo.hpp>

#include <string>
#include <algorithm>

class GuruProtocoloInformacion : public GuruProtocoloServidor
{
public:
	GuruProtocoloInformacion(IEnviar& enviar, IRecibir& recibir):GuruProtocoloServidor(enviar, recibir) {};
	virtual ~GuruProtocoloInformacion() {};

	bool init() {
		cargar_msg_bienvenida("Bienvenido al Protocolo de Informacion.");
		cargar_msg_salida("Gracias por utilizar el Protocolo de Informacion.");

		GuruProtocoloServidor::init();

		cargar_comando(this, &GuruProtocoloInformacion::cmd_memoria, "Memoria", "Informacion sobre el uso de memoria.");
		cargar_comando(this, &GuruProtocoloInformacion::cmd_unidades, "Unidades", "Informacion sobre espacios en los discos.");
		cargar_comando(this, &GuruProtocoloInformacion::cmd_procesos, "Procesos", "Informacion sobre los procesos ejecutandose.");
		cargar_comando(this, &GuruProtocoloInformacion::cmd_usuario, "Usuario", "Informacion el usuario que ejecutar el servicio.");
		cargar_comando(this, &GuruProtocoloInformacion::cmd_computadora, "Computadora", "Informacion el nombre de la computadora que ejecuta el servicio.");
		cargar_comando(this, &GuruProtocoloInformacion::cmd_so, "SO", "Informacion sobre el Sistema Operativo.");
		//	cargar_comando(this, &GuruProtocoloInformacion::cmd_procesosWTS, "Procesos-WTS", "Informacion sobre los procesos ejecutandose en un terminal server.");
		cargar_comando(this, &GuruProtocoloInformacion::cmd_compilacion, "Compilacion", "Informacion la version del protocolo.");

		return true;
	}

private:
	void cmd_memoria(const vector<string>& parametros) {

		GuruInfo::memoria memoria;

		GuruInfo::estadomemoria(memoria);

		std::string linea_retorno;

		linea_retorno += "CargaFisica=" + GuruStringAlgo::itos((uint64_t)memoria.cargaMemoria);

		linea_retorno += " Fisica=" + GuruStringAlgo::itos((uint64_t)memoria.totalFisica);
		linea_retorno += " FisicaLibre=" + GuruStringAlgo::itos((uint64_t)memoria.disponibleFisica);

		linea_retorno += " PageFile=" + GuruStringAlgo::itos((uint64_t)memoria.totalPageFile);
		linea_retorno += " PageFileLibre=" + GuruStringAlgo::itos((uint64_t)memoria.disponiblePageFile);

		linea_retorno += " Virtual=" + GuruStringAlgo::itos((uint64_t)memoria.totalVirtual);
		linea_retorno += " VirtualLibre=" + GuruStringAlgo::itos((uint64_t)memoria.disponibleVirtual);

		BIEN(linea_retorno);
	}

	void cmd_unidades(const vector<string>& parametros) {

		vector<GuruInfo::unidad> unidades;

		GuruInfo::estadounidades(unidades);

		std::string linea;

		for (vector<GuruInfo::unidad>::iterator it=unidades.begin(),itend=unidades.end();it!=itend;++it) {
			linea += "Unidad=" + GuruStringAlgo::comillas(it->nombre);
			linea += " Total=" + GuruStringAlgo::itos(it->total);
			linea += " Libre=" + GuruStringAlgo::itos(it->libre);

			BIEN(linea);
		}

		MAL();
	}

	void cmd_procesos(const vector<string>& parametros) {
		/*
		GuruProcesosWin32 procwin32;

		vector<data_proceso> procs;

		procs = procwin32.procesos();

		vector<data_proceso>::iterator it;

		for (it=procs.begin();it!=procs.end();++it) {
		ostringstream linea;

		linea << "Archivo=\"" << it->sacar_nombre_archivo() << "\" ID=" << it->sacar_id_proceso() << " Area_Trabajo=" << it->sacar_area_trabajo() << " Max_Area_Trabajo=" << it->sacar_maximo_area_trabajo() << " Area_Intercambio=" << it->sacar_uso_pagefile() << " Max_Area_Intercambio=" << it->sacar_maximo_uso_pagefile();
		BIEN(linea.str());
		}
		*/
		MAL();
	}

	void cmd_usuario(const vector<string>& parametros) {
		std::string c(GuruInfo::username());

		if (c.empty()) 
			MAL(GuruError().descripcion());
		else 
			BIEN(c);
	}

	void cmd_computadora(const vector<string>& parametros) {
		std::string c(GuruInfo::computername());

		if (c.empty()) 
			MAL(GuruError().descripcion());
		else 
			BIEN(c);
	}

	void cmd_so(const vector<string>& parametros) {
		BIEN(GuruInfo::ver_windows());
	}

	void cmd_compilacion(const vector<string>& parametros) {
		BIEN(__DATE__ " - " __TIME__);
	}
};

#endif

