#ifndef __GURUHTTPLOG_H__
#define __GURUHTTPLOG_H__

#include <GuruCompat.h>

#if defined __GURUWIN32__
	#include <process.h>
#elif defined __GURULINUX__
//////////////////////////////////////////////////////////////////////
//	Link: pthread
//////////////////////////////////////////////////////////////////////
	#include <pthread.h>
	#include <semaphore.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <queue>

#include <GuruAcceso.hpp>
#include <GuruVariable.hpp>
#include <GuruAlgo.hpp>

#include "GuruhttpRequest.hpp"

using namespace std;

class GuruhttpLog
{
	struct data_archivo_log {
		ofstream*	archivo;
		string		nombre;
	};

	struct data_linea_log {
		ofstream*	archivo;
		string		log;
	};

	queue<data_linea_log> linea_log;
	map<uint16_t, vector<data_archivo_log> > archivo_log;

	enum accesos	{acceso_cola_linea_log=0, acceso_ejecutando, acceso_total};
	GuruAcceso		acceso;

	#ifdef __GURUWIN32__
	HANDLE	hescribir;
	#elif defined __GURULINUX__
	sem_t	hescribir;
	#endif

	bool ejecucion;
public:
	GuruhttpLog():ejecucion(false) {acceso.inicializar(acceso_total);}

	virtual ~GuruhttpLog(void) {
		terminar();
	};

	void terminar() {
		
		if (!ejecutando()) return ;
			
		ejecutando(false);

		#ifdef __GURUWIN32__
		ReleaseSemaphore(hescribir, 1, NULL);
		#elif defined __GURULINUX__
		sem_post(&hescribir);
		#endif

		bool esperar=true; 
		do {
			acceso.esperar_acceso(acceso_cola_linea_log);
			if (linea_log.empty()) esperar=false;
			acceso.permitir_acceso(acceso_cola_linea_log);
		} while (esperar);

		for (map<uint16_t, vector<data_archivo_log> >::iterator it=archivo_log.begin();it!=archivo_log.end();++it) {
			for (vector<data_archivo_log>::iterator it2=it->second.begin();it2!=it->second.end();++it2) {
				if (it2->nombre.compare("+") != 0 && it2->nombre.compare("-") != 0)
					if (it2->archivo->is_open()) it2->archivo->close();
			}
		}

		#ifdef __GURUWIN32__
		CloseHandle(hescribir);
		#elif defined __GURULINUX__
		sem_destroy(&hescribir);
		#endif
	};

	virtual void escribirLog(GuruhttpRequest& request, GuruhttpResponse& response) {

		if (request.config() == NULL) return ;

		if (request.config()->sacarLog().empty()) return ;

		map <uint16_t, vector<data_archivo_log> >::iterator it(archivo_log.find(request.config()->id()));

		if (it == archivo_log.end()) { 
			init(request, response);
			it = archivo_log.find(request.config()->id());

			if (it == archivo_log.end()) return ;
			else if (it->second.empty()) return ;
		} else if (it->second.empty()) return ;

		for (vector<data_archivo_log>::iterator it2=it->second.begin();it2!=it->second.end();++it2) {
			for (map<string, string>::iterator it3=request.config()->sacarLog().begin();it3!=request.config()->sacarLog().end();++it3) {
				if (GuruStringAlgo::stringequals(it2->nombre, it3->first)) {
					data_linea_log data;
					data.archivo = it2->archivo;

					GuruhttpVar a(request, response);
					data.log=GuruVariable<GuruhttpVar>(&a, &GuruhttpVar::reemplazar).reemplazar(it3->second);

					acceso.esperar_acceso(acceso_cola_linea_log);
					linea_log.push(data);
					acceso.permitir_acceso(acceso_cola_linea_log);

					#ifdef __GURUWIN32__
					ReleaseSemaphore(hescribir, 1, NULL);
					#elif defined __GURULINUX__
					sem_post(&hescribir);
					#endif
				}
			}
		}
	}

	virtual void escribirLogError(GuruhttpRequest& request) {
	}

private:
	void init(GuruhttpRequest& request, GuruhttpResponse& response) {

		vector<data_archivo_log> info;
		for (map<string, string>::iterator it=request.config()->sacarLog().begin();it!=request.config()->sacarLog().end();++it) {
				data_archivo_log data;
				data.nombre=it->first;

				if (it->first.compare("+") == 0)
					data.archivo = static_cast<std::ofstream*>(&std::cout);
				else if (it->first.compare("-") == 0)
					data.archivo = static_cast<std::ofstream*>(&std::cerr);
				else
					data.archivo = new ofstream(it->first.c_str(), ios::app | ios::ate | ios::out);

				info.push_back(data);
		}

		archivo_log.insert(make_pair(request.config()->id(), info));

		if (info.empty()) return ;

		if (!ejecutando()) {
			#ifdef __GURUWIN32__
			hescribir=CreateSemaphore(NULL, 0, 50, NULL);

			unsigned	tid=0;
			uintptr_t salida_bt = _beginthreadex(NULL, 0, escrituraLog, static_cast<void*>(this), 0, &tid);
			CloseHandle((HANDLE)salida_bt);
			#elif defined __GURULINUX__
			sem_init(&hescribir, 0, 0);

			pthread_t	tid=0;
			pthread_create(&tid, NULL, escrituraLog, (void*)this);
			#endif

			ejecutando(true);
		}
	}

	#ifdef __GURUWIN32__
	static unsigned int __stdcall escrituraLog(void* llamador) {
	#elif defined __GURULINUX__
	static void* escrituraLog(void* llamador) {
	#endif
		GuruhttpLog*		esto = static_cast<GuruhttpLog*>(llamador);
		data_linea_log		nueva_linea_log;
		bool				terminar_ciclo=false;

		do {
			#ifdef __GURUWIN32__
			WaitForSingleObject(esto->hescribir, INFINITE);
			#elif defined __GURULINUX__
			sem_wait(&esto->hescribir);
			#endif

			esto->acceso.esperar_acceso(esto->acceso_cola_linea_log);
			if (!esto->linea_log.empty()) {
				nueva_linea_log = esto->linea_log.front();
				esto->linea_log.pop();
			}
			esto->acceso.permitir_acceso(esto->acceso_cola_linea_log);

			if (!nueva_linea_log.log.empty())
				*nueva_linea_log.archivo << nueva_linea_log.log << endl;

			nueva_linea_log.archivo->flush();
			nueva_linea_log.log.clear();

			if (!esto->ejecutando()) terminar_ciclo=true;
		} while(!terminar_ciclo);

		esto->acceso.esperar_acceso(esto->acceso_cola_linea_log);
		while (!esto->linea_log.empty()) {
			nueva_linea_log = esto->linea_log.front();
			esto->linea_log.pop();

			if (!nueva_linea_log.log.empty())
				if (nueva_linea_log.archivo->is_open()) {
					*nueva_linea_log.archivo << nueva_linea_log.log << endl;
					nueva_linea_log.archivo->flush();
				}
		}
		esto->acceso.permitir_acceso(esto->acceso_cola_linea_log);

		#ifdef __GURUWIN32__
		_endthreadex(0);
		#elif defined __GURULINUX__
		pthread_exit(0);
		#endif

		return 0;
	}

	bool ejecutando() {
		bool estado_actual;

		acceso.esperar_acceso(acceso_ejecutando);
		estado_actual =  ejecucion;
		acceso.permitir_acceso(acceso_ejecutando);

		return estado_actual;
	};

	void ejecutando(bool estado) {
		acceso.esperar_acceso(acceso_ejecutando);
		ejecucion=estado;
		acceso.permitir_acceso(acceso_ejecutando);
	};

	class GuruhttpVar {
	public:
		GuruhttpVar (GuruhttpRequest& request, GuruhttpResponse& response):request(request),response(response) {};

		string reemplazar(const string& variable) {
			std::string var;

		if (variable.compare("epoc") == 0)
			var = GuruStringAlgo::int64astr(time(NULL));
		else if (variable.compare("fecha822") == 0)
			var = GuruAlgo::fechaHoraRFC822(time(NULL));
		else if (variable.compare("ip_request") == 0)
			var = request.recibir().interfaz()->sacar_socket().interfaz_remota();
		else if (variable.compare("request") == 0)
			var = request.requerimiento().requerimiento();
		else if (variable.compare("status") == 0)
			var = GuruStringAlgo::itos(response.codigo_estado());
		else if (variable.compare("recurso") == 0)
			var = request.requerimiento().recurso();
		else if (variable.find("request.", 0, strlen("request.")) != string::npos) 
			var = request.atributo(variable.substr(strlen("request.")));

		return var;
		}

	protected:
		GuruhttpRequest& request;
		GuruhttpResponse& response;
	};

};

#endif
