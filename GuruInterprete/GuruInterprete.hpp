#ifndef __GURUINTERPRETE_HPP__
#define __GURUINTERPRETE_HPP__

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <iostream> // para cout

#include <GuruError.hpp>
#include <GuruString_algo.hpp>

using namespace std;

namespace GuruInterprete {

	static const char version[] = "1.2";

	#define CREAR_SENTENCIA(x,y,z) agregar_sentencia(x, new GuruInterprete::GuruSentencia<y>(this, &y::z));
	#define CREAR_FUNCION(x,y,z)   agregar_funcion(x, new GuruInterprete::GuruFuncion<y>(this, &y::z));

	typedef enum {
		fin_archivo=-1, 
		fin_analisis=-2, 
		error=0, 
		fin_instruccion, 
		control, 
		espacio, 
		imprimible, 
		cadena, 
		coma, 
		instruccion, 
		funcion, 
		sentencia,
		variable,
		objeto,
		igual,
		comentario, 
		comillas_dobles,
		comillas_simples,
		abre_parentesis, 
		cierra_parentesis, 
		abre_llave, 
		cierra_llave, 
		abre_corchetes, 
		cierra_corchete, 
		control_analisis
	} tipo_token;

	typedef enum {
		mostrar=1,					// Envia por la salida el resultado del proceso.
		sin_control_fin_analisis	// No ejecuta el control de fin de analisis.
	} tipo_control;

	typedef struct {
		istream*	ptr_entrada;
		ostream*	ptr_salida;

		uint32_t	cantidad_analizar;
		uint32_t	cantidad_analizada;

		tipo_token	token;
		uint16_t	control_interprete;
		int			caracter;
		string		buffer;
		ios::pos_type posicion;
	}estado;

	typedef struct {
		uint32_t	x;
		uint32_t	y;
	}posicion;

	class IGuruFuncion {
	public:
		IGuruFuncion() {};
		virtual ~IGuruFuncion() {};

		virtual string ejecutar(const string& intruccion, const vector<string>& parametros) = 0;
		virtual vector<string>& instrucciones(vector<string>& instruccion) = 0;
	};

	class IGuruSentencia {
	public:
		IGuruSentencia() {};
		virtual ~IGuruSentencia() {};

		virtual void ejecutar() = 0;
	};

	class IGuruControl {
	public:
		IGuruControl() {};
		virtual ~IGuruControl() {};

		virtual void ejecutar(istream& entrada, ostream& salida, estado& estado_interprete) = 0;
	};

//////////////////////////////////////////////////////////////////////
// Interpreta lineas como:
// * variable("HOSTNAME")		// muestra la variable de la aplicacion on sistema operativo
// * atributo("host")			// muestra atributo del request
// * parametro("var")			// muestra el parametro var
// * variable(parametro("var"))	// toma la variable "var" que es un parametro de post/get y se pasa a la funcion variable
// * funcion(parametro("destino"),atributo("host"))
// * iif (parametro("usuario"), login(), error())
// * iif (parametro("usuario"), <>, "")
// * equ (parametro("usuario"), "Fabricio") // equ/less/more/
// * validar(variable(parametro("usuario")), "nombre"))
// * mostrar ("Fecha:", fecha_actual())
//////////////////////////////////////////////////////////////////////
// Funciones internas:
// * ejecutar("parametro("lineacomando")")
// * var("pepe")
// * foreach(inicio(), funcion())
// * while (funcion()) {
//   ...
//   }
// * for (i,)
//////////////////////////////////////////////////////////////////////
	class Interprete {
	public:
		Interprete():ptr_entrada(NULL),ptr_salida(NULL),control_interprete(mostrar), cantidad_analizada(0) {};

		Interprete(const Interprete& copia):ptr_entrada(NULL),ptr_salida(NULL),control_interprete(mostrar), cantidad_analizada(0) {
			ptr_entrada=copia.ptr_entrada;
			ptr_salida=copia.ptr_salida;

			funciones=copia.funciones;
			sentencias=copia.sentencias;
			controles=copia.controles;
		};

		virtual ~Interprete() {
			for (map<string, IGuruSentencia*>::iterator it=sentencias.begin();it!=sentencias.end();++it)
				delete it->second;

//			for (map<string, IGuruFuncion*>::iterator it2=funciones.begin();it2!=funciones.end();++it2) delete it2->second;
		};

		/*
		Excepciones en el Interprete

								std::exception
										|
						GuruInterprete::InterpreteException
							/                 \
		InterpreteExcepcionSintaxis    InterpreteExcepcionIdentificador
		*/

		class InterpreteException :public std::exception {
		public:
			InterpreteException(const string& msg, Interprete& esto) :msg(msg), esto(esto) {
				this->msg += "(" + GuruStringAlgo::int64astr(esto.posicionxy.y) + "," + GuruStringAlgo::int64astr(esto.posicionxy.x) + ")";
			};
			InterpreteException(const char* msg, Interprete& esto) :msg(msg), esto(esto) {};
			virtual ~InterpreteException() throw() {};

			virtual const char* what() const throw() { return msg.c_str(); };
		private:
			string msg;
			Interprete& esto;
		};

		class InterpreteExceptionSintaxis :public InterpreteException {
		public:
			InterpreteExceptionSintaxis(const string& msg, Interprete& esto) :InterpreteException(msg, esto) {};
			virtual ~InterpreteExceptionSintaxis() throw() {};
		};

		class InterpreteExcepcionIdentificador : public InterpreteException {
			friend class Interprete;
		public:
			InterpreteExcepcionIdentificador(const string& msg, Interprete& esto) :InterpreteException(msg, esto) {

			}
			virtual ~InterpreteExcepcionIdentificador() throw() {};
		};

		class InterpreteExceptionParametro :public InterpreteException {
		public:
			InterpreteExceptionParametro(const string& msg, Interprete& esto) :InterpreteException(msg, esto) {};
			virtual ~InterpreteExceptionParametro() throw() {};
		};

		class FuncionException :public std::exception {
		public:
			FuncionException(const string& msg):msg(msg) {};
			FuncionException(const char* msg):msg(msg) {};
			virtual ~FuncionException() throw() {};

			virtual const char* what() const throw() { return msg.c_str(); };
		private:
			string msg;
		};

		class FuncionExceptionParametro :public FuncionException {
		public:
			FuncionExceptionParametro(const string& msg):FuncionException(msg) {};
			virtual ~FuncionExceptionParametro() throw() {};
		};


		void agregar_funciones(IGuruFuncion& obj) {
			vector<string> instru;
			if (obj.instrucciones(instru).size() != 0)
				for (vector<string>::iterator it=instru.begin();it!=instru.end();++it)
					funciones.insert(make_pair(*it, static_cast<IGuruFuncion*> (&obj)));
		};

		void agregar_funciones(IGuruFuncion* obj) {
			vector<string> instru;
			if (obj->instrucciones(instru).size() != 0)
				for (vector<string>::iterator it=instru.begin();it!=instru.end();++it)
					funciones.insert(make_pair(*it, obj));
		};

		void borrar_funciones(IGuruFuncion& obj) {
			vector<string> instru;
			if (obj.instrucciones(instru).size() != 0)
				for (vector<string>::iterator it=instru.begin();it!=instru.end();++it)
					funciones.erase(*it);
		};

		void borrar_funciones(IGuruFuncion* obj) {
			vector<string> instru;
			if (obj->instrucciones(instru).size() != 0)
				for (vector<string>::iterator it=instru.begin();it!=instru.end();++it)
					funciones.erase(*it);
		};

		void agregar_funcion(const string& nombre, IGuruFuncion* funcion) {
			funciones.insert(make_pair(nombre, funcion));
		}

		void agregar_sentencia(const string& nombre, IGuruSentencia* lasentencia) {
			sentencias.insert(make_pair(nombre, lasentencia));
		}

		void agregar_control(const tipo_token& control, IGuruControl* elcontrol) {
			controles.insert(make_pair(control, elcontrol));
		}

		void agregar_variable(const string& nombre, const string& valor_inicial) {

			map<string, string>::iterator it(variables.find(nombre));

			if (it == variables.end())
				variables.insert(make_pair(nombre, valor_inicial));
			else
				it->second = valor_inicial;
		}

		void agregar_objeto(const string& nombre, const string& nombre_objeto) {
			objetos.insert(make_pair(nombre, nombre_objeto));
		}

		// batolome 1131 6 D
		ostream& interpretar(istream& entrada, ostream& salida, uint32_t cantidad_analizar=static_cast<uint32_t>(-1)) {
			this->cantidad_analizar=cantidad_analizar;
			this->ptr_entrada = static_cast<istream*>(&entrada);
			this->ptr_salida = static_cast<ostream*>(&salida);
			this->posicion = entrada.tellg();
			this->cantidad_analizada=0, fila=1, columna=1;
			
			try {
				sacar_token();
				while (token != fin_analisis && token != fin_archivo && token != fin_instruccion) {
					analizar_expresion();
					sacar_token();
				}
			} catch (string& e) {
				control_interprete = mostrar;
				salida << e;
				return salida;
			}

			return salida;
		}

		void limpiar () {
//			control_interprete=mostrar;
			controles.clear();
			buffer.clear();
			cantidad_analizada=0;
			ultimo_resultado.clear();
			variables.clear();

			while (!estados.empty()) estados.pop();
		}

	protected:
		void analizar_expresion() {
			switch (static_cast<int>(token)) {
			case funcion: interpretar_funcion();
						break;
			case sentencia: ejecutar_sentencia(buffer);
						break;
			case variable: procesar_variable(buffer);
						break;
			case comentario: sacar_hasta_fin_linea();
						break;
			default:
					throw InterpreteExcepcionIdentificador("Error en analisis, se esperaba una funcion/sentencia/variable.", *this);
			}
		}

		void omitir_expresion() {
			switch (static_cast<int>(token)) {
			case funcion: 
			case sentencia:
						if (sacar_token() != abre_parentesis) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis.", *this);
						sacar_hasta_cierra_parentesis();
						break;
			case variable: {
							if (sacar_token() != igual) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un igual/parentesis.", *this);
							
							sacar_token();
							switch (static_cast<int>(token)) {
							case abre_parentesis:
											sacar_hasta_cierra_parentesis(); 
											break;
							case funcion: 
							case sentencia: 
										omitir_expresion();
										break;
							default: // si es una variable no hago nada.
										break;
							}
						   }
						break;
			default:
					throw InterpreteExcepcionIdentificador("Error en analisis, se esperaba una funcion/sentencia/variable.", *this);
			}
		}

		string valor_expresion() {
			string tmp;

			switch (static_cast<int>(token)) {
			case funcion:	tmp=interpretar_funcion();
							break;
			case cadena:	tmp=buffer;
							break;
			case variable:	tmp=valor_variable(buffer);
							break;
			default:
					throw InterpreteExcepcionIdentificador("Error en analisis, se esperaba una funcion/cadena/variable.", *this);
			}

			return tmp;
		}

		string interpretar_funcion() {

			string funcion_encontrada(buffer);

			vector<string>				parametros;

			while (sacar_token() != cierra_parentesis) {

				if (token != abre_parentesis && token != coma) throw InterpreteExceptionSintaxis("Error en analisis, se esperaba un parentesis o una coma (buffer=" + buffer  + ").", *this);

				sacar_token();

				switch (static_cast<int>(token)) {
				case funcion:	parametros.push_back(interpretar_funcion());
								break;
				case cadena:	parametros.push_back(buffer);
								break;
				case variable:	parametros.push_back(valor_variable(buffer));
								break;
				case cierra_parentesis: // es una funcion que no tiene un parametro cadena ejem: hostname()
								poner_caracter();
								break;
				}
			}

			if (token == cierra_parentesis) 
					return ultimo_resultado=ejecutar_funcion(funcion_encontrada, parametros);
		
			return string();
		}

		void procesar_variable(const string& buffer) {
			string nombre(buffer);

			if (sacar_token() != igual) throw InterpreteExceptionSintaxis("Error de analisis, se esperaba un =", *this);

			sacar_token();

			switch (static_cast<int>(token)) {
			case funcion:	valor_variable(nombre, interpretar_funcion());
							break;
			case cadena:	valor_variable(nombre, buffer);
							break;
			case variable:	valor_variable(nombre, valor_variable(buffer));
							break;
			case sentencia: throw InterpreteExcepcionIdentificador("No se permite una sentencia.", *this);
							break;
			case abre_parentesis:
							{
							string tmp;
							while (sacar_token() != cierra_parentesis) {
								switch (static_cast<int>(token)) {
								case funcion:	tmp+=interpretar_funcion();
												break;
								case cadena:	tmp+=buffer;
												break;
								case variable:	tmp+=valor_variable(buffer);
												break;

								}
							}
							valor_variable(nombre, tmp);
							break;
							}

			default:
				throw InterpreteExcepcionIdentificador("Error de analisis, se esperaba una funcion/sentencia/variable.", *this);
			}

		}

		tipo_token sacar_token() {

			buffer.clear();

			do {
				sacar_caracter();
			}while (token == espacio || token == control);

			switch (static_cast<int>(token)) {
			case imprimible:
			case espacio:
			case control: break;
			case fin_analisis:
			case fin_instruccion:
			case fin_archivo:
						return token;
						break;
			}

			posicionxy.x = columna-1;
			posicionxy.y = fila;

			switch (caracter) {
			case '(': return token=abre_parentesis;
						break;
			case ')': return token=cierra_parentesis;
						break;
			case '{': return token=abre_llave;
						break;
			case '}': return token=cierra_llave;
						break;
			case '\'': return token=comillas_simples;
						break;
			case ',': return token=coma;
						break;
			case '#': return token=comentario;
						break;
			case '=': return token=igual;
						break;
			case '\"': return sacar_cadena();
						break;
			default: {
				if (sacar_instruccion() == instruccion) {
					if (existe_funcion(buffer)) return token = funcion;
					else if (existe_sentencia(buffer)) return token = sentencia;
					else if (existe_variable(buffer)) return token = variable;
					else if (existe_objeto(buffer)) return token = objeto;
					else throw InterpreteExcepcionIdentificador("La instruccion " + buffer + " no existe.", *this);

				} else return token = error;
			}
			}
		}

		tipo_token sacar_instruccion() {
			while (token == imprimible && 
					caracter != '(' && caracter != ')' && 
					caracter != '\"' && caracter != ',' && caracter != '=') {
				buffer+=caracter;

				sacar_caracter();
			}

			if (token == fin_instruccion || token == fin_analisis || token == fin_archivo)  return token;

			poner_caracter();

			return token=instruccion;
		}

		tipo_token sacar_cadena() {
			sacar_caracter();
			while ((token == imprimible || token == espacio) && caracter != '\"') {

				if (caracter == '\\') { // el siguiente caracter lo dejo tal cual esta.
					sacar_caracter();
				}

				buffer+=caracter;

				sacar_caracter();
			}

			if (token != imprimible && token != espacio) return token=fin_instruccion;
		
			return token=cadena;
		}

		tipo_token sacar_hasta_cierra_parentesis() {
			sacar_caracter();
			int pila=0;
			while ((token == imprimible || token == espacio) && (caracter != ')' || pila>0)) {

				if (caracter == '(') pila++;
				else if (caracter == ')') pila--;

				sacar_caracter();
			}

			if (caracter == ')') return token=cierra_parentesis;
			else return token=error;
		}

		tipo_token sacar_hasta_fin_linea() {

			string tmp;
			do {
				tmp += sacar_caracter();
			} while ((token == imprimible || token == espacio) && caracter != '\n');

			if (caracter == '\n') return token=comentario;
			else return token=error;
		}

		int sacar_caracter() {

			if (cantidad_analizada >= cantidad_analizar) {
				if (existe_control(fin_analisis)) {
					int tmp_caracter=caracter;
					ejecutar_control(fin_analisis);
					if (cantidad_analizada != 0) {
						token=fin_analisis;
						caracter=tmp_caracter;
					}
				} else {
					token=fin_analisis;
					return caracter=0;						
				}
			}

			if (ptr_entrada->peek() != EOF) {
				caracter = ptr_entrada->get();
				posicion = ptr_entrada->tellg();

				if (isspace(caracter)) token=espacio;
				else if (isprint(caracter)) token=imprimible;
				else if (iscntrl(caracter)) token=control;

				cantidad_analizada++;

				if (caracter == '\n') { fila++; columna = 1; }
				else columna++;
			} else { // fin de archivo encontrado

				if (ptr_entrada->eof()) 
					int a=0;

				caracter=static_cast<int>(fin_archivo);
				token=fin_archivo;
			}
			
			return caracter;
		};

		int poner_caracter() {
			ptr_entrada->unget();
			ptr_entrada->unget();

			cantidad_analizada--;
			cantidad_analizada--;

			if (columna > 1) columna--;
			else if (columna == 1) fila--;

			return sacar_caracter();
		};

		void cargar_estado(estado& estado_actual) {
//			estado_actual.ptr_entrada->rdbuf(ptr_entrada->rdbuf());
//			estado_actual.ptr_salida->rdbuf(ptr_salida->rdbuf());

//			estado_actual.ptr_entrada = ptr_entrada;
//			estado_actual.ptr_salida = ptr_salida;

			estado_actual.cantidad_analizar=cantidad_analizar;
			estado_actual.cantidad_analizada=cantidad_analizada;

			estado_actual.token=token;
			estado_actual.control_interprete=control_interprete;
			estado_actual.caracter=caracter;
			estado_actual.buffer=buffer;
			estado_actual.posicion=posicion;
		}

		void poner_estado(estado& estado_actual) {
//			ptr_entrada->rdbuf(estado_actual.ptr_entrada->rdbuf());
//			ptr_salida->rdbuf(estado_actual.ptr_salida->rdbuf());

//			ptr_entrada=ptr_entrada;
//			ptr_salida=ptr_salida;

			cantidad_analizar=estado_actual.cantidad_analizar;
			cantidad_analizada=estado_actual.cantidad_analizada;

			token=estado_actual.token;
			control_interprete=estado_actual.control_interprete;
			caracter=estado_actual.caracter;
			buffer=estado_actual.buffer;
			posicion=estado_actual.posicion;
		}

		void salvar_estado() {
			estado estado_actual;

			cargar_estado(estado_actual);

			estados.push(estado_actual);
		}

		void restaurar_estado() {
			estado estado_actual;

			estado_actual=estados.top();

			poner_estado(estado_actual);

			ios::pos_type actual_posicion=ptr_entrada->tellg();

			if (actual_posicion>posicion) {
				int32_t total=static_cast<int32_t>(actual_posicion)-static_cast<int32_t>(posicion);
				ptr_entrada->seekg(static_cast<std::ios::off_type>(static_cast<int32_t>(0)-static_cast<int32_t>(total)), std::ios_base::cur);
			} else if (actual_posicion<posicion) {
				int32_t total=static_cast<int32_t>(posicion)-static_cast<int32_t>(actual_posicion);
				ptr_entrada->seekg(static_cast<std::ios::off_type>(total), std::ios_base::cur);
			}

			estados.pop();
		}

		void eliminar_estado() {
			estados.pop();
		}

		bool existe_funcion(const string& nombre){
			map<string, IGuruFuncion*>::iterator it(funciones.find(nombre));

			if (it == funciones.end()) return false;
			else return true;
		}

		string ejecutar_funcion(const string& nombre, const vector<string> parametros) {
			map<string, IGuruFuncion*>::iterator it(funciones.find(nombre));

			if (it == funciones.end()) return string();

			return it->second->ejecutar(nombre, parametros);
		}

		bool existe_sentencia(const string& sentencia) {
			map<string, IGuruSentencia*>::iterator it(sentencias.find(sentencia));

			if (it == sentencias.end()) return false;
			else return true;
		}

		void ejecutar_sentencia(const string& nombre) {
			map<string, IGuruSentencia*>::iterator it(sentencias.find(nombre));

			if (it == sentencias.end()) return ;

			(it->second->ejecutar)();
		}

		bool existe_control(const tipo_token& control) {
			map<tipo_token, IGuruControl*>::iterator it(controles.find(control));

			if (it == controles.end()) return false;
			else return true;
		}

		void ejecutar_control(const tipo_token& control) {
			map<tipo_token, IGuruControl*>::iterator it(controles.find(control));

			if (it == controles.end()) return ;

			estado estado_actual;

			cargar_estado(estado_actual);
		
			(it->second->ejecutar)(static_cast<istream&>(*ptr_entrada), static_cast<ostream&>(*ptr_salida), estado_actual);

			poner_estado(estado_actual);
		}

		bool existe_variable(const string& nombre) {
			map<string, string>::iterator it(variables.find(nombre));

			if (it == variables.end()) return false;
			else return true;
		}

		string valor_variable(const string& nombre) {
			map<string, string>::iterator it(variables.find(nombre));

			if (it == variables.end()) return string();
			else return it->second;
		}

		void valor_variable(const string& nombre, const string& valor) {
			map<string, string>::iterator it(variables.find(nombre));

			if (it == variables.end()) return ;
			else it->second=valor;
		}

		bool existe_objeto(const string& nombre) {
			map<string, string>::iterator it(objetos.find(nombre));

			if (it == objetos.end()) return false;
			else return true;
		}

	public:
		class iterator_funciones {
		public:
			iterator_funciones(Interprete& esto, map<string, IGuruFuncion*>::iterator it):esto(esto),it(it) {};
			virtual ~iterator_funciones() {};

			string nombre () {return it->first; };

			bool operator != (iterator_funciones it2) { return (it2.it != it) ? true : false; };

			void operator++ () { ++it; };

			string operator* () { return it->first; };

		protected:
			Interprete& esto;
			map<string, IGuruFuncion*>::iterator it;
		};

		iterator_funciones begin_funciones() {return iterator_funciones(*this, funciones.begin());};
		iterator_funciones end_funciones() {return iterator_funciones(*this, funciones.end());};


	protected:
		istream*	ptr_entrada;
		ostream*	ptr_salida;

		tipo_token	token;
		uint16_t	control_interprete;
		int			caracter;
		string		buffer;
		ios::pos_type posicion;
		uint32_t	cantidad_analizar;
		uint32_t	cantidad_analizada;

		uint32_t	fila;
		uint32_t	columna;
		GuruInterprete::posicion	posicionxy;

		string		ultimo_resultado;

		map<string, IGuruFuncion*>		funciones;
		map<string, IGuruSentencia*>	sentencias;
		map<tipo_token, IGuruControl*>	controles;
		map<string, string>				variables;
		map<string, string>				objetos;

		stack<estado> estados;
	};

	template <typename T>
	class GuruFunciones: public IGuruFuncion {
		typedef string (T::*metodo) (const vector<string>&);
	public:
		GuruFunciones() {};
		virtual ~GuruFunciones() {};

		typedef typename map<string, pair<T*, metodo> >::iterator iterator;

		iterator begin() { return funcion.begin();}
		iterator end() { return funcion.end();}

		virtual string ejecutar(const string& instruccion, const vector<string>& parametros) {
			if (funcion.empty()) return string();

			typename map<string, pair<T*, metodo> >::iterator it(funcion.find(instruccion));

			if (it != funcion.end()) {
				string salida;

				salida =  (it->second.first->*it->second.second)(parametros);

				return salida;
			}

			return string();
		};

		virtual vector<string>& instrucciones(vector<string>& instruccion) {
			instruccion.clear();

			for (typename map<string, pair<T*, metodo> >::iterator it=funcion.begin();it!=funcion.end();++it)
				instruccion.push_back(it->first);

			return instruccion;
		};

		virtual void cargar_funcion(T* clase, string (T::*metodo) (const vector<string>&), const string& nombre) {
			funcion.insert(make_pair(nombre, make_pair(clase, metodo)));
		}

		virtual void borrar_funcion(const string& nombre) {
			funcion.erase(nombre);
		}

	private:
		map<string, pair<T*, metodo> > funcion;
	};

	template <typename T>
	class GuruFuncion: public IGuruFuncion {
		typedef string (T::*metodo) (const vector<string>&);
	public:
		GuruFuncion(T* laclase, metodo elmetodo) :laclase(laclase), elmetodo(elmetodo) {};
		virtual ~GuruFuncion() {};

		virtual string ejecutar(const string& instruccion, const vector<string>& parametros) {
			return (laclase->*elmetodo)(parametros);
		}
		// solo por compatibilidad de la clase GuruFuniones que permite tener muchas funciones en una sola clase.
		vector<string>& instrucciones(vector<string>& instruccion) { return instruccion; };
	protected:
		T*		laclase;
		metodo	elmetodo;
	};

	template <typename T>
	class GuruSentencia: public IGuruSentencia {
		typedef void (T::*metodo) ();
	public:
		GuruSentencia(T* laclase, metodo elmetodo):laclase(laclase),elmetodo(elmetodo) {};
		virtual ~GuruSentencia() {};

		void ejecutar() { (laclase->*elmetodo)();};
	protected:
		T*		laclase;
		metodo	elmetodo;
	};
}
#endif

//////////////////////////////////////////////////////////////////////
// Ejem de uso:
//	GuruInterprete b;
//
//	Cadena cad;
//	Sistema ari;
//	b.agragar_funciones(cad);
//	b.agragar_funciones(ari);
//
//	try {
//		cout << b.interpretar("variable(\"HOSTNAME\")") << endl;
//		cout << b.interpretar("variable(parametro(\"maquina\"))") << endl;
//		cout << b.interpretar("mostrar(\"Hola:\", variable(\"usuario\"), variable(\"USERNAME\"))") << endl;
//		cout << b.interpretar("iif (parametro(\"hostname\", pedir(), hacer())") << endl;
//	} catch (string& e) {
//		cout << e << endl;
//	}
//////////////////////////////////////////////////////////////////////
