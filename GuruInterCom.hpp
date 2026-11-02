#if !defined __GURUINTERCOM_H__
#define __GURUINTERCOM_H__

#include <GuruCompat.h>
#include <GuruToken.hpp>
#include <GuruString_algo.hpp>

#include <algorithm>
#include <vector>
#include <string>

#include <iostream> // for cout

using namespace std;

const char INTERCOM_NO_COMANDO_DEFAULT[] = " -> Comando desconocido.\r\n";

/*
Comentarios:
		Migracion:
			Eliminar: 
				GuruInterCom::init();
			Agregar:
				cargar_salida("salir");
				y/o
				ayuda();

			Eliminar: (si el compilador se queja)
				cargar_comando(this, &GuruProtocoloInformacion::cmd_memoria, "Memoria", "Informacion sobre el uso de memoria.");
			Agregar:
				GuruInterCom::GuruSetComando<XXX> cmd(this);
				cmd.agregar(&XXX::dummy, "Memoria", "Informacion sobre el uso de memoria.");
				cmd.agregar(&XXX::dummy, "nop", "No Operation.");
				agregar(cmd.data());
				o
				agregar(make_comando<XXX>(this, &XXX::dummy, "Ver", "Version de la aplicacion."));
				o
				cargar_comando<XXX>(this, &XXX::nada, "nop", "No Operation.");
			
		Herencia:
			La herencia puede ser de GuruInterComConsola/GuruProtocoloServidor/GuruInterComSocket

Ejemplo de Uso:		
class testcmd: public GuruInterComConsola {
public:
	testcmd() {};
	virtual ~testcmd() {};

	bool init() {
/////////////////[agregar comandos]///////////////////////////////////
		// Metodo 1 para agregar comandos:
		GuruInterCom::GuruSetComando<testcmd> cmd(this);

		cmd.agregar(&testcmd::dummy, "Version", "Version de la aplicacion.");
		cmd.agregar(&testcmd::dummy, "nop", "No hace nada.");

		agregar(cmd.data());
		
		// Metodo 2 para agregar comandos:
		agregar(make_comando<testcmd>(this, &testcmd::dummy, "Ver", "Version de la aplicacion."));
		agregar(make_comando<testcmd>(this, &testcmd::sacar, "sacar", "remueve un comando."));

		// Metodo 3 para agregar comandos:
		cargar_comando<testcmd>(this, &testcmd::nada, "nada");

/////////////////[agregar comandos internos]//////////////////////////
		// Metodo 1 para agregar comandos internos
		cargar_pre_comando(this, &testcmd::revision_comandos);
		
		// Metodo 2 para agregar comandos internos
		cargar_pre_comando(make_comando<testcmd>(this, &testcmd::revision_comandos));

/////////////////[configuracion]//////////////////////////////////////
		// Configuracion del interprete de comandos
		cargar_salida("salir");
		cargar_prompt("Guru->");
		ayuda();

		return true;
	}

	void nada (const vector<string>& parametros) {
		salida("Nada\n");
	}

	void dummy (const vector<string>& parametros) {
	}

	void sacar (const vector<string>& parametros) {
		descargar_comando("Ver");
	}
};

*/

class GuruInterComException:public std::exception {
public:
	GuruInterComException () {};
	virtual ~GuruInterComException() throw() {};

	virtual const char* what() const throw() { return "";};
};

class IGuruComando {
	friend class GuruInterCom;
public:
	IGuruComando() {};
	virtual ~IGuruComando() {};

	const	string& nombre()		{return _nombre;};
	const	string& descripcion()	{return _descripcion;};
	bool	oculto()				{return _oculto;};

	virtual void ejecutar(const vector<string>& parametros) {};

protected:
	string		_nombre;
	string		_descripcion;
	bool		_oculto;
};

template <typename T>
class GuruComando: public IGuruComando {
public:
	GuruComando(T* clase):clase(clase) {};
	GuruComando(T* clase, void (T::*metodo) (const vector<string>&), const string& nombreComando, const string& descripcionComando="", bool mostrar=true) {
		this->clase=clase;
		comando(metodo, nombreComando, descripcionComando, mostrar);
	}

	virtual ~GuruComando() {};

	virtual void ejecutar(const vector<string>& parametros) {(clase->*metodo)(parametros);};

	GuruComando<T>& comando(void (T::*metodo) (const vector<string>&), const string& nombreComando, const string& descripcionComando="", bool mostrar=true) {
		this->metodo=metodo;
		this->_nombre=nombreComando;
		this->_descripcion=descripcionComando;
		this->_oculto=!mostrar;

		return *this;
	};

private:
	T*				clase;
	void (T::*metodo) (const vector<string>&);
};

class GuruInterCom
{
protected:
	string			prompt;				// prompt del interprete
	string			cmd_salida;			// comando de salida del interprete
	string			msg_bienvenida;		// mensaje de bienvenida
	string			msg_despedida;		// mensaje de fin de ejecucion del interprete
	string			msg_no_comando;		// mensaje cuando no se reconoce el comando
	string			linea_comando;		// comando introducido al interprete de comandos.
	string			salida_comando;		// respuesta enviada por el interprete de comandos.

	IGuruComando* no_comando;			// puntero a objeto para cuando no existe el comando
	IGuruComando* comando_salir;		// puntero a objeto que se ejecuta cuando se realice el comando de salida del interprete
	IGuruComando* comando_bienvenida;	// puntero a objeto que se ejecuta cuando se ingresa al interprete
	IGuruComando* pre_comando;			// puntero a objeto que se ejecutara antes de ejecutar el comando
	IGuruComando* post_comando;			// puntero a objeto que se ejecutara despues de ejecutar el comando

	template <typename T>
	class GuruSetComando {
	public:
		GuruSetComando(T* clase):clase(clase) {};
		virtual ~GuruSetComando() {
//			for (vector <IGuruComando*>::iterator it=comandos.begin();it!=comandos.end();++it) delete *it;
		};

		void agregar(void (T::*metodo) (const vector<string>&), const string& nombreComando, const string& descripcionComando="", bool mostrar=true) {
			GuruComando<T>* cmd = new GuruComando<T>(clase);

			cmd->comando(metodo, nombreComando, descripcionComando, mostrar);

			comandos.push_back(cmd);
		}

		vector <IGuruComando*>& data() {return comandos;};

	private:
		T*		clase;
		vector <IGuruComando*> comandos;
	};

// private:
	vector <IGuruComando*> comandos;
public:
	GuruInterCom() {
		limpiar_variables();
		enviar.esto=this;
		recibir.esto=this;
	}

	virtual ~GuruInterCom() {
		if (no_comando) delete no_comando;
		if (comando_salir) delete comando_salir;
		if (comando_bienvenida) delete comando_bienvenida;
		if (pre_comando) delete pre_comando;
		if (post_comando) delete post_comando;

		for (vector <IGuruComando*>::iterator it=comandos.begin();it!=comandos.end();++it) delete *it;
	};

	template<typename Y> inline
	IGuruComando* make_comando(Y* clase, void (Y::*metodo) (const vector<string>&), const string& nombreComando="", const string& descripcionComando="", bool mostrar=true) {
		return new GuruComando<Y>(clase, metodo, nombreComando, descripcionComando, mostrar);
	}

	class GuruInterComEnviar{
	public:
		GuruInterComEnviar(){}
		GuruInterComEnviar& operator << (const string& cadena){
				if (cadena.empty()) return *this;

				esto->salida_comando = cadena;

				esto->salida(cadena);

				return *this;
		}
        private:
                friend class GuruInterCom;
                GuruInterCom* esto;
        };

	class GuruInterComRecibir{
	public:
		GuruInterComRecibir(){}
		GuruInterComRecibir& operator >> (string& cadena){
				cadena = esto->entrada();
				return *this;
		}
	private:
		friend class GuruInterCom;
		GuruInterCom* esto;
	};

	GuruInterComEnviar	enviar;
	GuruInterComRecibir recibir;

	virtual void salida(const string& texto) = 0;
	virtual string entrada(void) = 0;

//////////////////////////////////////////////////////////////////////
	void inline agregar(vector <IGuruComando*>& cmd) {
		comandos.insert(comandos.end(), cmd.begin(), cmd.end());
	}

	void inline agregar(IGuruComando* cmd) {
		comandos.push_back(cmd);
	}

	template<typename W> inline
	void agregar(W* clase, void (W::*metodo) (const vector<string>&), const string& nombreComando, const string& descripcionComando="", bool mostrar=true) {
		comandos.push_back(static_cast<IGuruComando*>(new GuruComando<W>(clase, metodo, nombreComando, descripcionComando, mostrar)));
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Limpiar las variables a utilizar en la clase
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
	void limpiar_variables() {
		no_comando=NULL;
		comando_salir=NULL;
		comando_bienvenida=NULL;
		pre_comando=NULL;
		post_comando=NULL;
	};

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar un nuevo prompr al interprete de comandos
// Parametros:	El nuevo prompt.
//////////////////////////////////////////////////////////////////////
	virtual inline void cargar_prompt(const string& nuevo_prompt) {
		prompt = nuevo_prompt;
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Retornar el actual prompt del interprete
// Parametros:	Ninguno
//////////////////////////////////////////////////////////////////////
	virtual inline const string& actual_prompt() {
		return prompt;
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar el comando para terminar el interprete de comandos
// Parametros:	El nuevo comando de salida
//////////////////////////////////////////////////////////////////////
	virtual inline void cargar_salida(const string& comando_salida) {
		cmd_salida = comando_salida;
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Retornar el actual comando de salida del interprete de comandos
// Parametros:	Ninguno
//////////////////////////////////////////////////////////////////////
	virtual inline const string& actual_salida() {
		return cmd_salida;
	}
//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar un nuevo comando a la lista de comandos validos
// Parametros:
//				nombre_comando = Nombre por el cual sera llamado el comando
//				nuevo_comando  = Funcion definida por el usuario al cual
//								sera llama cuando se invoque al comando
//				decripcion_comando = Descripcion del comando
//////////////////////////////////////////////////////////////////////
	template<typename W> inline
	void cargar_comando(W* clase, void (W::*metodo) (const vector<string>&), const string& nombreComando, const string& descripcionComando="", bool mostrar=true) {
		comandos.push_back(static_cast<IGuruComando*>(new GuruComando<W>(clase, metodo, nombreComando, descripcionComando, mostrar)));
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Eliminar un comando de la lista de comandos
// Parametros:
//				nombre_comando = Nombre del comando a sacar de la lista.
//////////////////////////////////////////////////////////////////////
	virtual void descargar_comando(const string& nombre_comando) {
		for (vector <IGuruComando*>::iterator it=comandos.begin();it!=comandos.end();++it) {
			if (GuruStringAlgo::stringequals((*it)->nombre(), nombre_comando)) {
				delete *it;
				comandos.erase(it);
				break;
			}
		}
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Dividir el comando y llamar la funcion que lo ejecuta
// Parametros:	La linea con el comando y los parametros
// Nota:
//				Ejem: copy *.* c:
//////////////////////////////////////////////////////////////////////
	virtual bool interpretar(const string& comando_usuario) {
		GuruToken tokens;
		vector<string> linea_comandos;

		tokens.extraer(comando_usuario, linea_comandos);

		if (linea_comandos.empty()) return false;

		ejecutar_comando(linea_comandos);

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Verificar si es un comando valido y/o llamar a la
//				funcion que lo ejecuta.
// Parametros:
//				cant_comandos = cantidad de parametros
//				origen = puntero a las cadenas que poseen el comando
// Nota:
//				Llamada desde el metodo interpretar
//////////////////////////////////////////////////////////////////////
	virtual void ejecutar_comando(const vector<string>& orden) {
		vector <IGuruComando*>::iterator it;

	//	it = find(comandos.begin(), comandos.end(), orden.at(0));

		for (it=comandos.begin();it!=comandos.end();++it)
			if (GuruStringAlgo::stringequals((*it)->nombre(), orden.at(0))) break;

		try {
			if (it != comandos.end()){				
				if (pre_comando)
					pre_comando->ejecutar(orden); // puede tirar una excepcion para cancelar la ejecucion!!!

				(*it)->ejecutar(orden);

				if (post_comando)
					post_comando->ejecutar(orden);
			}else{
				if (no_comando) // si tiene cargada una funcion para cuando no existe un comando
					no_comando->ejecutar(orden);
				else {
					if (!msg_no_comando.empty())
						enviar << msg_no_comando;
					else
						enviar << orden.at(0) << INTERCOM_NO_COMANDO_DEFAULT;
				}
			}
		} catch (GuruInterComException&) {};
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar una funcion que sera llamada cuando no se
//				reconozca un comando(que un comando no se encuentre
//				en la lista de comando validos).
// Parametros:	Un puntero a esa funcion.
//////////////////////////////////////////////////////////////////////
	inline void cargar_pre_comando(IGuruComando* nuevo_pre_comando) {
		if (pre_comando) delete pre_comando;

		pre_comando = nuevo_pre_comando;

		pre_comando->_oculto = true;
	}

	template<typename W> inline
	void cargar_pre_comando(W* clase, void (W::*metodo) (const vector<string>&)) {
		if (pre_comando) delete pre_comando;

		pre_comando = static_cast<IGuruComando*>(new GuruComando<W>(clase, metodo, "", "", false));
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar una funcion que sera llamada cuando no se
//				reconozca un comando(que un comando no se encuentre
//				en la lista de comando validos).
// Parametros:	Un puntero a esa funcion.
//////////////////////////////////////////////////////////////////////
	void cargar_post_comando(IGuruComando* nuevo_post_comando) {
		if (post_comando) delete post_comando;

		post_comando = nuevo_post_comando;
	}

	template<typename W> inline
	void cargar_post_comando(W* clase, void (W::*metodo) (const vector<string>&)) {
		if (post_comando) delete pre_comando;

		post_comando = static_cast<IGuruComando*>(new GuruComando<W>(clase, metodo, "", "", false));
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar una funcion que sera llamada cuando no se
//				reconozca un comando(que un comando no se encuentre
//				en la lista de comando validos).
// Parametros:	Un puntero a esa funcion.
//////////////////////////////////////////////////////////////////////
	void cargar_no_comando(IGuruComando* nuevo_no_comando) {
		if (no_comando) delete no_comando;

		no_comando = nuevo_no_comando;
	}

	template<typename W> inline
	void cargar_no_comando(W* clase, void (W::*metodo) (const vector<string>&)) {
		if (no_comando) delete pre_comando;

		no_comando = static_cast<IGuruComando*>(new GuruComando<W>(clase, metodo, "", "", false));
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar un mensaje que sera llamada cuando no se
//				reconozca un comando(que un comando no se encuentre
//				en la lista de comando validos).
// Parametros:	EL mensaje de comando no reconocido.
//////////////////////////////////////////////////////////////////////
	virtual void cargar_msg_no_comando(const string& cadena_msg_no_comando) {
		msg_no_comando = cadena_msg_no_comando;
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar una funcion que sera llamada cuando se ejecute
//				el comando de terminacion o de salida del interprete
// Parametros:	Un puntero a esa funcion.
//////////////////////////////////////////////////////////////////////
	virtual void cargar_comando_salida(IGuruComando* nuevo_metodo_salir) {
		if (comando_salir) delete comando_salir;

		comando_salir = nuevo_metodo_salir;
	}

	template<typename W> inline
	void cargar_comando_salida(W* clase, void (W::*metodo) (const vector<string>&)) {
		if (comando_salir) delete pre_comando;

		comando_salir = static_cast<IGuruComando*>(new GuruComando<W>(clase, metodo, "", "", false));
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar una funcion que sera llamada cuando se ejecute
//				el comando de terminacion o de salida del interprete
// Parametros:	Un puntero a esa funcion.
//////////////////////////////////////////////////////////////////////
	virtual void cargar_msg_salida(const string& cadena_msg_despedida) {
		msg_despedida = cadena_msg_despedida;
	}

	virtual const string& sacar_msg_salida() {
		return msg_despedida;
	}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar una funcion que sera llamada cuando se ejecute
//				por primera vez el interprete.
// Parametros:	Un puntero a esa funcion.
//////////////////////////////////////////////////////////////////////
	virtual void cargar_comando_bienvenida(IGuruComando* nuevo_metodo_bienvenida) {
		if (comando_bienvenida) delete pre_comando;

		comando_bienvenida = nuevo_metodo_bienvenida;
	}

	template<typename W> inline
	void cargar_comando_bienvenida(W* clase, void (W::*metodo) (const vector<string>&)) {
		if (comando_bienvenida) delete pre_comando;

		comando_bienvenida = static_cast<IGuruComando*>(new GuruComando<W>(clase, metodo, "", "", false));
	}
//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar el mensaje de bienvenida que sera mostrado
//				cuando se ejecute por primera vez el interprete.
// Parametros:	El mensaje de bienvenida.
//////////////////////////////////////////////////////////////////////
	virtual void cargar_msg_bienvenida(const string& cadena_bienvenida) {
		msg_bienvenida = cadena_bienvenida;
	}

	virtual const string& sacar_msg_bienvenida() {
		return msg_bienvenida;
	}

	virtual void enviar_msg_bienvenida() {
		enviar << msg_bienvenida;
	}

	virtual void enviar_msg_salida() {
		enviar << msg_despedida;
	}

	virtual void cmd_intercom_comandos (const vector<string>& parametros) {
		if (parametros.size() == 1) {  // sin parametros
			size_t ancho=0;
			for (vector<IGuruComando*>::iterator it=comandos.begin();it!=comandos.end();++it) {
				if (!(*it)->oculto())
					if ((*it)->nombre().size() > ancho) ancho=(*it)->nombre().size();
			}

			for (vector<IGuruComando*>::iterator i=comandos.begin();i!=comandos.end();++i){
				if (!(*i)->oculto()){
					enviar << (*i)->nombre();
					for (unsigned largo=0;largo!=((ancho+2)-((*i)->nombre().size()));largo++) enviar << " ";
					enviar << (*i)->descripcion() << "\r\n";
				}
			}
		} else if (parametros.size() == 2) {
//			vector<IGuruComando>::iterator it=find(comandos.begin(), comandos.end(), parametros[1]);
			for (vector<IGuruComando*>::iterator it=comandos.begin();it!=comandos.end();++it) {
				if (GuruStringAlgo::stringequals((*it)->nombre(), parametros[1]))
					enviar << (*it)->descripcion() << "\r\n";
			}
		}
	}

	virtual void ayuda() {
		agregar(make_comando<GuruInterCom>(this, &GuruInterCom::cmd_intercom_comandos, "?", "Lista los comandos y sus descripciones.", false));
		agregar(make_comando<GuruInterCom>(this, &GuruInterCom::cmd_intercom_comandos, "ayuda", "Lista los comandos y sus descripciones.", false));
		agregar(make_comando<GuruInterCom>(this, &GuruInterCom::cmd_intercom_comandos, "help", "Lista los comandos y sus descripciones.", false));
	}

	virtual bool init() {
//		cargar_salida(GURUINTERCOM_SALIDA);

		return true;
	}

	int main (void) {
		if (!init()) return -1;

		if (!msg_bienvenida.empty()) enviar_msg_bienvenida();

		do {
			enviar << actual_prompt();
			recibir >> linea_comando;

			GuruStringAlgo::trim(linea_comando);

//			cout << linea_comando << endl;

			if (linea_comando.empty()) continue;

			if (GuruStringAlgo::stringequals(linea_comando, actual_salida())) break;

			interpretar(linea_comando);
		}while (1);

		if (!msg_despedida.empty()) enviar_msg_salida();

		return 0;
	}

};

#endif 
