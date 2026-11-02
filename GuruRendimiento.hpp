#ifndef __GURURENDIMIENTO_H__
#define __GURURENDIMIENTO_H__

//////////////////////////////////////////////////////////////////////
// Link: Pdh.lib
//////////////////////////////////////////////////////////////////////
#include <Pdh.h>
#include <pdhmsg.h>
#include <iostream>
#include <string>
#include <map>

#include <GuruAlgo.hpp>
#include <GuruError.hpp>
#include <GuruAcceso.hpp>

using namespace std;

class GuruContador
{
	friend class GuruRendimiento;
public:
	GuruContador() {};

	double valor () { return _valor;};
	string contador() { return _contador;};
	string alias() { return _alias;};

protected:
	string			_contador;
	string			_alias;
	double			_valor;

	PDH_HCOUNTER	hcontador;
	DWORD_PTR		userData;
};

class GuruRendimiento
{
public:
	GuruRendimiento():hrendimiento(NULL),ultimoUserData(1),eventoColectar(NULL) {};
	virtual ~GuruRendimiento() {
		if (hrendimiento != NULL) 
			PdhCloseQuery(hrendimiento);
	}

	void conectar () {
		PDH_STATUS salida = PdhOpenQuery(NULL, 0, &hrendimiento);
		if (salida != ERROR_SUCCESS) return ;
	}

	unsigned long agregar(const string& alias_contador, const string& ruta_contador){

			if (hrendimiento == NULL) conectar();

			unsigned long tmp=-1;

			acceso.bloquear();

			if (alias.find(alias_contador) == alias.end()) { 

				PDH_HCOUNTER c;
				PDH_STATUS salida = PdhAddCounter(hrendimiento, ruta_contador.c_str(), ultimoUserData, &c);

				if (salida == ERROR_SUCCESS) {

					GuruContador conta;

					conta._contador = ruta_contador;
					conta._alias = alias_contador;
					conta.hcontador = c;
					conta.userData  = ultimoUserData++;

					std::list<GuruContador>::iterator it;

					it = contadores.insert(contadores.end(), conta);

					alias.insert(make_pair(alias_contador, it));
					handles.insert(make_pair(c, it));

					tmp=conta.userData;
				}

			}

			acceso.desbloquear();

			return tmp;
	}

	void borrar (const string& alias_contador) {
		map<std::string, std::list<GuruContador>::iterator>::iterator it;

		acceso.bloquear();

		if ((it=alias.find(alias_contador)) != alias.end()) {
			
			std::map<PDH_HCOUNTER, std::list<GuruContador>::iterator>::iterator it2;

			it2=handles.find(it->second->hcontador);

			handles.erase(it2);

			contadores.erase(it->second);

			alias.erase(it);
		}

		acceso.desbloquear();
	}

	vector<string>& sacar_contadores(vector<string>& listado){
		listado.clear();
/*
		for (map<string, GuruContador>::iterator it=_contadores.begin();it!=_contadores.end();++it) {
			listado.push_back(it->second.contador);
		}
*/
	};

	void capturar () {
		PdhCollectQueryData(hrendimiento);

		eventoColectar = CreateEvent(NULL, FALSE, FALSE, NULL);

		PDH_STATUS salida = PdhCollectQueryDataEx(hrendimiento, 2,  eventoColectar);

		if (salida != ERROR_SUCCESS) 
			cerr << "Error al iniciar la captura:" << GuruError().descripcion(salida, "pdh.dll") << endl;

		HANDLE ht=(HANDLE)_beginthreadex(NULL, 0, colectar, this, 0, NULL);
		CloseHandle(ht);
	}

	static unsigned int __stdcall colectar(void* a) {
		GuruRendimiento* esto=static_cast<GuruRendimiento*>(a);

		ULONG WaitResult;
		ULONG CounterType;
		PDH_FMT_COUNTERVALUE DisplayValue;

		while ((WaitResult = WaitForSingleObject(esto->eventoColectar, INFINITE)) == WAIT_OBJECT_0) {

			esto->acceso.bloquear();

			for (std::list<GuruContador>::iterator it=esto->contadores.begin();it!=esto->contadores.end();++it) {
				PDH_STATUS salida;
				if ((salida=PdhGetFormattedCounterValue(it->hcontador, PDH_FMT_DOUBLE, &CounterType, &DisplayValue)) == ERROR_SUCCESS) {
					it->_valor = DisplayValue.doubleValue;
					cout << it->alias() << ":" << DisplayValue.doubleValue << endl;
				} else 
					cerr << GuruError().descripcion(salida, "pdh.dll") << endl;
			}

			esto->acceso.desbloquear();
		}
		
		_endthreadex(0);
		return 0;
	}

	double valor (const string& alias_contador) {
		std::map<std::string, std::list<GuruContador>::iterator>::iterator it;
		
		acceso.bloquear();
		double tmp=-1;
		if ((it = alias.find(alias_contador)) != alias.end()) 
			tmp=it->second->valor();
		acceso.desbloquear();

		return tmp;
	}

	vector<string>& objetos (vector<string>& listado) {
		listado.clear();

		DWORD largo=0;
		PDH_STATUS salida=PdhEnumObjects(NULL, NULL, NULL, &largo, PERF_DETAIL_NOVICE , false);

		if (salida == PDH_MORE_DATA) {
			char* buffer=new char [largo];

			salida=PdhEnumObjects(NULL, NULL, buffer, &largo, PERF_DETAIL_NOVICE , false);

			if (salida == ERROR_SUCCESS) {
				char* inicio=buffer;
				while (*buffer) {
					listado.push_back(buffer);
					buffer += strlen(buffer)+1;
				}

				delete [] inicio;
			} else 
				cerr << GuruError().descripcion(salida, "pdh.dll") << endl;
		} else 
			cerr << GuruError().descripcion(salida, "pdh.dll") << endl;

		return listado;
	}

	vector<string>& sacar_contadores (vector<string>& vcontadores, vector<string>& vinstancias, const string& objeto) {
		vcontadores.clear();
		vinstancias.clear();

		DWORD largoItems=0;
		DWORD largoInstancias=0;
		PDH_STATUS salida=PdhEnumObjectItems(NULL, NULL, objeto.c_str(), NULL, &largoItems, NULL, &largoInstancias, PERF_DETAIL_EXPERT, 0);

		char* bufferItems = new char [largoItems];
		char* bufferInstancias = new char [largoInstancias];

		if (salida == PDH_MORE_DATA) {
			salida=PdhEnumObjectItems(NULL, NULL, objeto.c_str(), bufferItems, &largoItems, bufferInstancias, &largoInstancias, PERF_DETAIL_EXPERT, 0);

			if (salida == ERROR_SUCCESS) {
				vinstancias.clear();
				char* inicioInstancias=bufferInstancias;
				while (*bufferInstancias) {
					vinstancias.push_back(bufferInstancias);
					bufferInstancias+=strlen(bufferInstancias)+1;
				}
				delete [] inicioInstancias;

				char* iniciocontadores=bufferItems;
				while (*bufferItems) {
					vcontadores.push_back(bufferItems);
					bufferItems += strlen(bufferItems)+1;
				}
				delete [] iniciocontadores;
			} else 
				cerr << GuruError().descripcion(salida, "pdh.dll") << endl;
		} else 
			cerr << GuruError().descripcion(salida, "pdh.dll") << endl;

		return vcontadores;
	}

protected:
	PDH_HQUERY				hrendimiento;
	DWORD_PTR				ultimoUserData;
	HANDLE					eventoColectar;
	GuruBloqueo				acceso;

	std::list<GuruContador>		contadores;
	std::map<PDH_HCOUNTER, std::list<GuruContador>::iterator>	handles;
	std::map<std::string, std::list<GuruContador>::iterator>	alias;
};

#endif
