#ifndef __GURUPROTOCOLOSERVIDOR_H__
#define __GURUPROTOCOLOSERVIDOR_H__

#include <GuruProtocolo.hpp>
#include <GuruInterCom.hpp>
#include <GuruSocket/GuruInterComSocket.hpp>

class GuruProtocoloServidor: public GuruInterComSocket
{
	std::string txt_enviar;

public:
	GuruProtocoloServidor(IEnviar& enviar, IRecibir& recibir):GuruInterComSocket(enviar,recibir) {};
	virtual ~GuruProtocoloServidor(void) {};

	virtual bool init() {

		agregar(make_comando<GuruProtocoloServidor>(this, &GuruProtocoloServidor::cmd_ayuda, "?", "Lista los comandos validos.", false));
		agregar(make_comando<GuruProtocoloServidor>(this, &GuruProtocoloServidor::cmd_ayuda, "ayuda", "Lista los comandos validos.", false));
		agregar(make_comando<GuruProtocoloServidor>(this, &GuruProtocoloServidor::cmd_ayuda, "help", "Lista los comandos validos.", false));

		cargar_salida("salir");

		return true;
	}

	inline void BIEN(const string& str_salida = "") {
		txt_enviar = GuruProtocolo::str_bien() + std::string(' ' + str_salida) + "\r\n";

		GuruInterCom::enviar << txt_enviar;

		salida_comando(txt_enviar);
	}

	inline void MAL(const string& str_salida="") {
		txt_enviar = GuruProtocolo::str_mal() + std::string(' ' + str_salida) + "\r\n";

		GuruInterCom::enviar << txt_enviar;

		salida_comando(txt_enviar);
	}

	inline void PROHIBIDO(const string& str_salida="") {
		txt_enviar = GuruProtocolo::str_prohibido() + std::string(' ' + str_salida) + "\r\n";

		GuruInterCom::enviar << txt_enviar;

		salida_comando(txt_enviar);
	}

	inline void NOEXISTE(const string& str_salida="") {
		txt_enviar = GuruProtocolo::str_noexiste() + std::string(' ' + str_salida) + "\r\n";

		GuruInterCom::enviar << txt_enviar;

		salida_comando(txt_enviar);
	}

	virtual void comando_desconocido (const vector<std::string>& parametros) {
		txt_enviar = GuruProtocolo::str_noexiste() + std::string(" Comando Desconocido") + "\r\n";

		GuruInterCom::enviar << txt_enviar;

		salida_comando(txt_enviar);
	};

	virtual void comando_prohibido (const vector<string>& parametros) {
		txt_enviar = GuruProtocolo::str_prohibido() + std::string(" Comando Prohibido") + "\r\n";

		GuruInterCom::enviar << txt_enviar;

		salida_comando(txt_enviar);
	};

	virtual void enviar_msg_bienvenida() {
		txt_enviar = sacar_msg_bienvenida();

		BIEN(txt_enviar);

		salida_comando(txt_enviar);
	};

	virtual void enviar_msg_salida() {
		txt_enviar = sacar_msg_salida();

		BIEN(txt_enviar);

		salida_comando(txt_enviar);
	};

	virtual void salida_comando(const std::string&) {

	}

	virtual void cmd_ayuda (const vector<string>& parametros) {
		size_t ancho=0;
		for (vector<IGuruComando*>::iterator it=comandos.begin();it!=comandos.end();++it){
			if (!(*it)->oculto())
				if ((*it)->nombre().size() > ancho) ancho=(*it)->nombre().size();
		}

		for (vector<IGuruComando*>::iterator i=comandos.begin();i!=comandos.end();++i){
			if (!(*i)->oculto()){
				txt_enviar = (*i)->nombre();

				for (unsigned largo=0;largo!=((ancho+2)-((*i)->nombre().size()));largo++) txt_enviar += ' ';

				txt_enviar += std::string(' ' + (*i)->descripcion());

				BIEN(txt_enviar);
				
				salida_comando(txt_enviar);
			}
		}

		MAL();
	}
};

#endif
