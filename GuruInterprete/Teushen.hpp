#ifndef __GURULENGUAJE_HPP__
#define __GURULENGUAJE_HPP__

#include <GuruInterprete/GuruInterprete.hpp>
#include <GuruString_algo.hpp>

#include <string>

/*
Nombres basados en el conjunto de intruccinoes del 8086 de intel
EQ: Igual a.
NE: Distinto de.
LT: Menor que.
GT: Mayor que.
LE: Menor o igual a.
GE: Mayor o igual a.
*/
class teushen: public GuruInterprete::Interprete {
public:
	teushen() {
		// operaciones de comparacion
		CREAR_FUNCION("eq", teushen, igual);
		CREAR_FUNCION("ne", teushen, distinto);
		CREAR_FUNCION("lt", teushen, menor);
		CREAR_FUNCION("gt", teushen, mayor);
		CREAR_FUNCION("le", teushen, menorigual);
		CREAR_FUNCION("ge", teushen, mayorigual);
		CREAR_FUNCION("not", teushen, negacion);

		// operaciones matematicas basicas
		CREAR_FUNCION("inc", teushen, inc);
		CREAR_FUNCION("sum", teushen, inc);
		CREAR_FUNCION("dec", teushen, dec);
		CREAR_FUNCION("res", teushen, dec);
		CREAR_FUNCION("mul", teushen, mul);
		CREAR_FUNCION("div", teushen, div);
		CREAR_FUNCION("mod", teushen, mod);

		// sentencias del lenguaje
		CREAR_SENTENCIA("print", teushen, sentencia_print);
		CREAR_SENTENCIA("println", teushen, sentencia_println);
		CREAR_SENTENCIA("mostrar", teushen, sentencia_print);
		CREAR_SENTENCIA("ejecutar", teushen, sentencia_ejecutar);
		CREAR_SENTENCIA("foreach", teushen, sentencia_foreach);
		CREAR_SENTENCIA("while", teushen, sentencia_while);
		CREAR_SENTENCIA("do", teushen, sentencia_dowhile);
		CREAR_SENTENCIA("mientras", teushen, sentencia_while);
		CREAR_SENTENCIA("if", teushen, sentencia_if);
		CREAR_SENTENCIA("si", teushen, sentencia_if);
		CREAR_SENTENCIA("else", teushen, sentencia_if);
		CREAR_SENTENCIA("sino", teushen, sentencia_if);
		CREAR_SENTENCIA("ur", teushen, sentencia_ur);
		CREAR_SENTENCIA("var", teushen, sentencia_var);
		CREAR_SENTENCIA("fatal", teushen, sentencia_fatal);

		CREAR_FUNCION("defvar", teushen, sentencia_defvar);
	}

	virtual ~teushen() {}

	/*
	// Operaciones de comparacion
	*/
	string igual(const vector<string>& param) {
		if (param.size() != 2) throw FuncionExceptionParametro("Error en analisis, se espera dos parametros.");

		return GuruStringAlgo::stringequals(param.at(0), param.at(1)) ? "1" : "0";
	}

	string distinto(const vector<string>& param) {
		if (param.size() != 2) throw Interprete::FuncionExceptionParametro("Error en analisis, se espera dos parametros.");

		return GuruStringAlgo::stringequals(param.at(0), param.at(1)) ? "0" : "1";
	}

	string menor(const vector<string>& param) {
		if (param.size() != 2) throw Interprete::FuncionExceptionParametro("Error en analisis, se espera dos parametros.");

		if (GuruStringAlgo::numerico(param.at(0)) && GuruStringAlgo::numerico(param.at(1)))
			return (_atoi64(param.at(0).c_str()) < _atoi64(param.at(1).c_str())) ? "1" : "0";
		else 
			throw Interprete::FuncionExceptionParametro("Uno de los dos parametros NO es numerico");
	}

	string mayor(const vector<string>& param) {
		if (param.size() != 2) throw Interprete::FuncionExceptionParametro("Error en analisis, se espera dos parametros.");

		if (GuruStringAlgo::numerico(param.at(0)) && GuruStringAlgo::numerico(param.at(1)))
			return (_atoi64(param.at(0).c_str()) > _atoi64(param.at(1).c_str())) ? "1" : "0";
		else 
			throw Interprete::FuncionExceptionParametro("Uno de los dos parametros NO es numerico");
	}

	string menorigual(const vector<string>& param) {
		if (param.size() != 2) throw Interprete::FuncionExceptionParametro("Error en analisis, se espera dos parametros.");

		if (GuruStringAlgo::numerico(param.at(0)) && GuruStringAlgo::numerico(param.at(1)))
			return (_atoi64(param.at(0).c_str()) <= _atoi64(param.at(1).c_str())) ? "1" : "0";
		else 
			throw Interprete::FuncionExceptionParametro("Uno de los dos parametros NO es numerico");
	}

	string mayorigual(const vector<string>& param) {
		if (param.size() != 2) throw Interprete::FuncionExceptionParametro("Error en analisis, se espera dos parametros.");

		if (GuruStringAlgo::numerico(param.at(0)) && GuruStringAlgo::numerico(param.at(1)))
			return (_atoi64(param.at(0).c_str()) >= _atoi64(param.at(1).c_str())) ? "1" : "0";
		else 
			throw Interprete::FuncionExceptionParametro("Uno de los dos parametros NO es numerico");
	}

	string negacion(const vector<string>& param) {
		if (param.at(0).compare("1") == 0) return string(1, '0');
		else if (param.at(0).compare("0") == 0) return string(1, '1');
		else return param.at(0);
	}

	/*
	// Operaciones matematicas basicas
	*/
	string inc(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string variable,nro;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: variable=param.at(i); break;
			case 1: nro=param.at(i); break;
			}
		}

		int64_t int_nro = (nro.empty()) ? 1 : static_cast<int64_t>(_atoi64(nro.c_str()));

		variable=GuruStringAlgo::itos(static_cast<int64_t>(_atoi64(variable.c_str())+int_nro));

		return variable;
	}

	string dec(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string variable,nro;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: variable=param.at(i); break;
			case 1: nro=param.at(i); break;
			}
		}

		int64_t int_nro = (nro.empty()) ? 1 : static_cast<int64_t>(_atoi64(nro.c_str()));

		variable=GuruStringAlgo::itos(static_cast<int64_t>(_atoi64(variable.c_str())-int_nro));

		return variable;
	}

	string mul(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string variable,nro;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: variable=param.at(i); break;
			case 1: nro=param.at(i); break;
			}
		}

		int64_t int_nro = (nro.empty()) ? 1 : static_cast<int64_t>(_atoi64(nro.c_str()));

		variable=GuruStringAlgo::itos(static_cast<int64_t>(_atoi64(variable.c_str())*int_nro));

		return variable;
	}

	string div(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string variable,nro;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: variable=param.at(i); break;
			case 1: nro=param.at(i); break;
			}
		}

		int64_t int_nro = (nro.empty()) ? 1 : static_cast<int64_t>(_atoi64(nro.c_str()));

		if (int_nro > 0)
			variable=GuruStringAlgo::itos(static_cast<int64_t>(_atoi64(variable.c_str())/int_nro));

		return variable;
	}

	string mod(const vector<string>& param) {
		if (param.at(0).empty()) return string();

		string variable,nro;

		for (unsigned int i=0;i<param.size();i++) {
			switch (i) {
			case 0: variable=param.at(i); break;
			case 1: nro=param.at(i); break;
			}
		}

		int64_t int_nro = (nro.empty()) ? 1 : static_cast<int64_t>(_atoi64(nro.c_str()));

		if (int_nro > 0)
			variable=GuruStringAlgo::itos(static_cast<int64_t>(_atoi64(variable.c_str())%int_nro));

		return variable;
	}

	string sentencia_defvar(const vector<string>& param) {

		if (param.at(0).empty()) return string();

		return (existe_variable(param.at(0))) ? string(1, '1') : string(1, '0');
	}

	void sentencia_print(void) {

		while (sacar_token() != GuruInterprete::cierra_parentesis) {

			if (token != GuruInterprete::abre_parentesis && token != GuruInterprete::coma)
				throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis o una coma (buffer=" + buffer + ").", *this);

			sacar_token();

			switch (static_cast<int>(token)) {
			case GuruInterprete::funcion:	*ptr_salida << interpretar_funcion();
				break;
			case GuruInterprete::cadena:	*ptr_salida << buffer;
				break;
			case GuruInterprete::variable: *ptr_salida << valor_variable(buffer);
				break;
			}
		}
	}

	void sentencia_println(void) {
		sentencia_print();
		*ptr_salida << "\n";
	}

	void sentencia_ejecutar(void) {
/*
		string tmp;

		if (sacar_token() != GuruInterprete::abre_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);

		token = sacar_token();

		tmp = valor_expresion();

		if (!tmp.empty()) {
			GuruLenguaje lang(*this);
			istringstream iss(tmp);

			lang.interpretar(iss, *ptr_salida, static_cast<uint32_t>(iss.str().length()));
		}

		sacar_token();

		if (token != GuruInterprete::cierra_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);
*/
	}

	/////////////////////////////////////////////////////////////////////
	// foreach (inicio(), funcion(ur()))
	/////////////////////////////////////////////////////////////////////
	void sentencia_foreach(void) {
		string tmp;

		while (sacar_token() != GuruInterprete::cierra_parentesis) {

			if (token != GuruInterprete::abre_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);

			ios::pos_type anterior_posicion = posicion;

			sacar_token();

			string salida_funcion;
			switch (static_cast<int>(token)) {
			case GuruInterprete::funcion: salida_funcion = interpretar_funcion();
				break;
			case GuruInterprete::cadena: salida_funcion = interpretar_funcion();
				break;
			}

			if (salida_funcion.empty()) {
				sacar_hasta_cierra_parentesis();
				break;
			}

			if (sacar_token() != GuruInterprete::coma) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una coma.", *this);

			sacar_token();

			switch (static_cast<int>(token)) {
			case GuruInterprete::funcion: tmp += interpretar_funcion();
				break;
			case GuruInterprete::cadena: tmp += interpretar_funcion();
				break;
			}

			ptr_entrada->seekg(posicion - anterior_posicion, ios::cur);
			//			for (ios::pos_type i=posicion-anterior_posicion;i>=0;i--) poner_caracter();
		}
	}

	/////////////////////////////////////////////////////////////////////
	// foreach (inicio(), funcion(ur()))
	/////////////////////////////////////////////////////////////////////
	void sentencia_foreachV2(void) {
		string parametro1;
		string parametro2;

		// saco el parentesis abierto
		if (sacar_token() != GuruInterprete::abre_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis abierto.", *this);

		// la funcion/cadena/variable
		sacar_token();

		// la proceso
		parametro1 = valor_expresion();

		// saco la coma como separador de las funciones
		if (sacar_token() != GuruInterprete::coma) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una coma.", *this);

		// la funcion/cadena/variable
		sacar_token();

		// la proceso
		parametro2 = valor_expresion();

		// saco el parentesis cerrado
		if (sacar_token() != GuruInterprete::cierra_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis cerrado.", *this);

		// realizo la comparacion
		GuruStringAlgo::stringequals(parametro1, parametro2) ? (*ptr_salida << "1") : (*ptr_salida << "0");
	}

	void sentencia_while(void) {

		if (sacar_token() != GuruInterprete::abre_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);

		string salida_funcion;

		do {
			salvar_estado();

			sacar_token();

			salida_funcion = valor_expresion();

			// se cierra el while
			if (sacar_token() != GuruInterprete::cierra_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);

			sacar_token();

			// el valor devuelto por la funcion es positivo y entro en el bucle
			if (salida_funcion.compare("0") != 0 && !salida_funcion.empty()) {
				if (token == GuruInterprete::abre_llave) {
					while (sacar_token() != GuruInterprete::cierra_llave) analizar_expresion();
					if (token != GuruInterprete::cierra_llave) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una llave.", *this);
				} else { // while de una sola expresion
					analizar_expresion();
				}
			} else { // el resultado es falso
				if (token == GuruInterprete::abre_llave) {
					sacar_hasta_cierra_llave();
					eliminar_estado();
					if (token != GuruInterprete::cierra_llave) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una llave.", *this);
				} else { // while de una sola expresion
					omitir_expresion();
					eliminar_estado();
				}

				break;
			}

			restaurar_estado();
		} while (8);
	}

	void sentencia_dowhile(void) {
		if (sacar_token() != GuruInterprete::abre_llave) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una llave de apertura.", *this);

		string salida_funcion;
		do {
			salvar_estado();

			while (sacar_token() != GuruInterprete::cierra_llave) analizar_expresion();

			if (sacar_token() != GuruInterprete::sentencia) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una sentencia while.", *this);

			if (buffer.compare("while") == 0 || buffer.compare("mientras") == 0) {
				if (sacar_token() != GuruInterprete::abre_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);
				sacar_token();
				salida_funcion = valor_expresion();
				if (sacar_token() != GuruInterprete::cierra_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);

				if (salida_funcion.compare("0") == 0 || salida_funcion.empty()) {
					eliminar_estado();
					break;
				}
			}
			restaurar_estado();
		} while (8);
	}

	void sentencia_if(void) {

		if (sacar_token() != GuruInterprete::abre_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);

		sacar_token(); // saco el nombre de la funcion/variable/etc

		string salida_funcion(valor_expresion());

		if (sacar_token() != GuruInterprete::cierra_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);

		sacar_token();

		// el valor devuelto por la funcion es positivo y entro
		if (salida_funcion.compare("0") != 0 && !salida_funcion.empty()) {
			if (token == GuruInterprete::abre_llave) {
				while (sacar_token() != GuruInterprete::cierra_llave) analizar_expresion();
				if (token != GuruInterprete::cierra_llave) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una llave.", *this);
			} else {
				analizar_expresion();
			}

			salvar_estado();
			control_interprete = !GuruInterprete::mostrar;
			sacar_token();
			if (token == GuruInterprete::sentencia && (buffer.compare("else") == 0 || buffer.compare("sino") == 0)) {
				control_interprete = GuruInterprete::mostrar;
				sacar_token();
				if (token == GuruInterprete::abre_llave) {
					sacar_hasta_cierra_llave();
				} else {
					omitir_expresion();
				}

				eliminar_estado();
			} else {
				restaurar_estado();
			}
		} else { // el valor de la funcion/variable retorno falso
			if (token == GuruInterprete::abre_llave) {
				sacar_hasta_cierra_llave();
				if (token != GuruInterprete::cierra_llave) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una llave de cierre.", *this);
			} else {
				omitir_expresion();
			}

			// en caso que me venga un else/sino, ya que me dio falso
			salvar_estado();
			control_interprete = !GuruInterprete::mostrar;
			sacar_token();
			if (token == GuruInterprete::sentencia && (buffer.compare("else") == 0 || buffer.compare("sino") == 0)) {
				control_interprete = GuruInterprete::mostrar;
				sacar_token();
				if (token == GuruInterprete::abre_llave) {
					while (sacar_token() != GuruInterprete::cierra_llave)
						analizar_expresion();
					if (token != GuruInterprete::cierra_llave) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una llave de cierre.", *this);
				} else {
					analizar_expresion();
				}

				eliminar_estado();
			} else {
				restaurar_estado();
			}
		}
	}

	void sentencia_ur(void) {

		if (sacar_token() != GuruInterprete::abre_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);
		if (sacar_token() != GuruInterprete::cierra_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);

		//		return ultimo_resultado;
	}

	void sentencia_var(void) {

		if (sacar_token() != GuruInterprete::abre_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);

		if (sacar_token() != GuruInterprete::cadena) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una cadena.", *this);

		string nombre_variable(buffer);
		string valor_inicial("");

		sacar_token();

		if (token != GuruInterprete::coma && token != GuruInterprete::cierra_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba una coma o un parentesis.", *this);

		if (token == GuruInterprete::coma) {
			sacar_token();

			valor_inicial = valor_expresion();

			if (sacar_token() != GuruInterprete::cierra_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);
		}

		agregar_variable(nombre_variable, valor_inicial);
	}

	void sentencia_fatal(void) {

	}

	GuruInterprete::tipo_token sacar_hasta_cierra_llave() {
		control_interprete = static_cast<GuruInterprete::tipo_control>(!GuruInterprete::mostrar);
		sacar_caracter();
		int pila = 1;
		while ((token == GuruInterprete::imprimible || token == GuruInterprete::espacio) && (caracter != '}' || pila>0)) {

			sacar_caracter();

			if (caracter == '{')
				pila++;
			else if (caracter == '}')
				pila--;
		}

		control_interprete = GuruInterprete::mostrar;
		if (caracter == '}') return token = GuruInterprete::cierra_llave;
		else return token = GuruInterprete::error;
	}
};

#endif
