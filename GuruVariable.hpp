#ifndef __GURUVARIABLES_H__
#define __GURUVARIABLES_H__

#include <string>
#include <map>
#include <GuruCompat.h>
#include <GuruBuffer.hpp>
#include <GuruString_algo.hpp>

template <typename T>
class GuruVariable
{
	typedef string (T::*Metodo)(const string& variable);
public:
	GuruVariable(T* laclase, Metodo elmetodo, const char caracter_inicio = '%', const char caracter_fin = '%'):Clase(laclase),metodo(elmetodo), caracter_inicio(caracter_inicio), caracter_fin(caracter_fin) {};
	virtual ~GuruVariable(void) {};

	std::string reemplazar(const std::string& strConVar) {
		std::string variable;
		std::string cambiar(strConVar);

		std::string::size_type inicio,fin,indice=0;

		do{
			inicio = cambiar.find(caracter_inicio, indice);
			if (inicio==std::string::npos) break;

			fin = cambiar.find(caracter_fin, inicio+1);
			if (fin==std::string::npos) break;

			if (fin == inicio+1) {
				cambiar.erase(inicio,1);
				indice = fin+1;
				continue;
			}

			variable = cambiar.substr(inicio+1, (fin-inicio)-1);

			string entrada((Clase->*metodo)(variable));

			if (entrada.empty())
				indice = inicio + (fin - inicio) + 1;
			else {
				cambiar.replace(inicio, (fin - inicio) + 1, entrada.c_str()); // entrada debe ser c_str() por que sino no funciona.
				indice = inicio + entrada.length();
			}

//			indice=inicio+entrada.length();
		}while (8);

		return cambiar;
	}

	string operator() (const string& strVar) { return reemplazar(strVar); }

	T* obj() { return Clase;}

protected:
	T*		Clase;
	Metodo	metodo;
	char	caracter_inicio;
	char	caracter_fin;
};

class GuruVariableMap {
public:
	GuruVariableMap(const char caracter_inicio = '%', const char caracter_fin = '%'):cambio(this, &GuruVariableMap::reemplazo, caracter_inicio, caracter_fin) {
	}
	virtual ~GuruVariableMap() {}

	void insertar(std::map<std::string, std::string>& var_map) {

		for (std::map<std::string, std::string>::iterator it = var_map.begin(), itend = var_map.end(); it != itend; ++it) {
			variables.insert(make_pair(it->first, it->second));
		}
	}

	inline void insertar(const string& clave, const string& valor) {
		variables.insert(make_pair(clave, valor));
	}

	string reemplazar(const string& str) {
		return cambio.reemplazar(str);
	}

	inline std::map<std::string, std::string>& variableMap() { return variables; }

	inline void clear() { variables.clear(); }

	inline void reemplazar() {
		for (std::map<std::string, std::string>::iterator it = variables.begin(), end=variables.end(); it != end; ++it) {
			it->second = reemplazar(it->second);
		}
	}

protected:
	string reemplazo(const string& cadena) {
		std::map<std::string, std::string>::iterator it = variables.find(cadena);

		return (it != variables.end()) ? it->second : string();
	}

	GuruVariable<GuruVariableMap>		cambio;
	std::map<std::string, std::string>	variables;
};

class GuruVariableEntorno {
public:
	GuruVariableEntorno(const char caracter_inicio = '{', const char caracter_fin = '}'):cambio(this, &GuruVariableEntorno::func_reemplazo, caracter_inicio, caracter_fin) {
	}

	string reemplazar(const string& str) {
		return cambio.reemplazar(str);
	}

	virtual ~GuruVariableEntorno() {}
protected:

	string func_reemplazo(const string& str) {
#ifdef __GURUWIN32__
	#if defined __GURUUNICODE__
		GuruBufferWChar buffer(32767); //Limit according to http://msdn.microsoft.com/en-us/library/ms683188.aspx
		DWORD tot;
		if ((tot = GetEnvironmentVariable(GuruStringAlgo::ansi_unicode(str).c_str(), buffer.buf(), 32767)) != 0) {
			buffer.asignar(TCHAR('\0'), tot + 1);
			return GuruStringAlgo::unicode_ansi(wstring(buffer.buf()));
		}
		else
			return string();
	#else
		GuruBufferChar buffer(32767); //Limit according to http://msdn.microsoft.com/en-us/library/ms683188.aspx
		DWORD tot;
		if ((tot = GetEnvironmentVariable(str.c_str(), buffer.buf(), 32767)) != 0) {
			buffer.asignar('\0', tot + 1);
			return string(buffer.buf());
		} else
			return string();
	#endif
#elif defined __GURULINUX__
		char* varent;

		varent = getenv(str.c_str());
		if (varent != NULL)
			return string(varent);
		else
			return string();
#endif
	}

	GuruVariable<GuruVariableEntorno>		cambio;
};

class GuruVariableEx {
public:
	GuruVariableEx():var(NULL) {
		var = new GuruVariable<GuruVariableEx>(this, &GuruVariableEx::reemplazo);
	}
	string reemplazar(const string& cadena) {
		return var->reemplazar(cadena);
	}

protected:
	virtual string reemplazo(const string& cadena) = 0;

	GuruVariable<GuruVariableEx>*	var;
};

#endif
