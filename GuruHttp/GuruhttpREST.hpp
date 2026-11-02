#ifndef __GURUHTTPREST_HPP__
#define __GURUHTTPREST_HPP__

#include <jsoncpp/json.h>

#include "GuruhttpInterface.hpp"
#include <GuruToken.hpp>

class GuruHttpREST: public SitioWeb {
public:
	GuruHttpREST() {};
	virtual ~GuruHttpREST() {};

	inline string subdir(size_t pos) { return t[pos]; }

	inline bool equal(size_t pos, const string& subdir) { return (t[pos].compare(subdir) == 0) ? true : false; }

	// para verificar si es /aaaa/bbbb/cccc o /aaa/{var1} /aaa/{var1}/{var2}
	bool es(const string& subdir) {
		return uri(subdir);
	}

	std::string var(const std::string nombre) {
		return urivar(nombre);
	}

	inline string dir() { return request()->recurso().subdirectorio() + request()->recurso().archivo(); }

	inline size_t totalsubdir() { return subdirectorios; }

	void enviar_json(const string& buffer) {
		if (buffer.empty()) throw GuruhttpExcepcion(404);

		response()->content_type("application/json");
		response()->enviar(buffer);
	}

	void enviar_json(Json::Value& root) {
		response()->content_type("application/json");

		//  Json::StyledWriter writer;
		//	Json::FastWriter writer;

	#if defined _DEBUG
		response()->enviar(root.toStyledString());
	#else
		response()->enviar(Json::FastWriter().write(root));
	#endif

//		
/*
// opcion a reemplazar, escribe el json a archivo y luego envia el archivo.
		std::string tempFile(GuruDirectorio::archivo_temporal());

		cout << "tempFile:" << tempFile << endl;

		std::ofstream jsonFile;
		jsonFile.open(tempFile);

		Json::StyledWriter styledWriter;
		jsonFile << styledWriter.write(root);

		jsonFile.close();

		enviar_archivo(tempFile, "application/json");

//		enviar_archivo("d:\\Desarrollo\\Librerias\\jsoncpp\\jsoncpp.cpp", "application/json");

		GuruDirectorio::borrarArchivo(tempFile);
*/
	}
};

#endif

// distintas maneras de verificar los parametros en una peticion HTTP

/*
// metodo 1
	if (request()->recurso().archivo().compare("games") == 0) {
		if (!request()->parametro("game").empty())
			buscar(oss, request()->parametro("game"));
		else
			resumen_games(oss);
	} else if (request()->recurso().archivo().compare("manufacturers") == 0) {
		manufacturers(oss);
	} else if (request()->recurso().subdirectorio().compare("games/") == 0 && !request()->recurso().archivo().empty()) {
		info_juego(oss, request()->recurso().archivo());
	} else {
	}


// metodo 2
	GuruTokenEx t("/");

	size_t total = t.extraer(request()->recurso().subdirectorio() + request()->recurso().archivo());

	if (t[0].compare("games") == 0) {
		switch (total) {
		case 1: // games | games?o=name|descripcion|namedescripcion
				// http://localhost/armamews/games?o=namedescription&f=bios|games|devices|mechanical|
			games(str_games(request()->parametro("o")), str_filtro_games(request()->parametro("f")));
			break;
		case 2: // games/gng|rygar|pacmam
			std::async(std::launch::async, [&]() {info_juego(t[1], str_game(request()->parametro("o"))); }).get();
			break;
		case 3: // games/gng/roms|download|title|snap|chip|display
			std::async(std::launch::async, [&]() {info_detallada_juego(t[1], t[2]); }).get();
			break;
		default:
			break;
		}
	} else if (t[0].compare("manufacturers") == 0) {
		switch (total) {
		case 1: // manufacturer
				manufacturers();
				break;
		case 2: // manufacturer/capcom
				throw GuruhttpExcepcion(501);
				break;
		case 3: // manufacturer/capcom/games
				throw GuruhttpExcepcion(501);
				break;
		default:
			throw GuruhttpExcepcion(404);
		break;
		}
	}

// metodo 3
	init();

	// http://<host>/<sitio>/usuarios/<usuario>/desbloquear|sid|grupos|bloqueado|info
	// http://<host>/<sitio>/grupos/<grupo>/sid|usuarios|info

	if (equal(0, "usuarios")) {
		switch (totalsubdir()) {
			case 1:
					usuarios();
					break;
			case 3:
					if (equal(2, "desbloquear")) desbloquear(subdir(1));
					else if (equal(2, "clavenoexpira")) clave_nunca_expira(subdir(1));
					else throw GuruhttpExcepcion(501);
					break;
			default:
					throw GuruhttpExcepcion(404);
					break;
		}
	} else if (equal(0, "grupos")) {
		switch (totalsubdir()) {
			case 1: // listado
					grupos();
					break;
			case 3:
					if (equal(2, "sid")) sid_grupo(subdir(1));
					else if (equal(2, "usuarios")) usuarios_grupo(subdir(1));
					else if (equal(2, "info")) info_grupo(subdir(1));
					else throw GuruhttpExcepcion(501);
					break;
			default:
					throw GuruhttpExcepcion(404);
					break;
		}
	} else throw GuruhttpExcepcion(400);

// metodo 4
	init();

// http://<host>/<sitio>/usuarios/<usuario>/desbloquear|sid|grupos|bloqueado|info
// http://<host>/<sitio>/grupos/<grupo>/sid|usuarios|info

	if (es("/usuarios/{user}/clavenuncaexpira")) clave_nunca_expira(varReq["user"]);
	else if (es("/usuarios/frossi/desbloquear")) desbloquear(subdir(1));
	else if (es("/usuarios/frossi/clavenoexpira")) clave_nunca_expira(subdir(1));
	else if (es("/usuarios/frossi/sid")) sid_usuario(subdir(1));
	else if (es("/usuarios/frossi/grupos")) grupos_usuario(subdir(1));
	else if (es("/usuarios/frossi/bloqueado")) bloqueado(subdir(1));
	else if (es("/usuarios/frossi/info")) info_usuario(subdir(1));
	else throw GuruhttpExcepcion(400);

*/