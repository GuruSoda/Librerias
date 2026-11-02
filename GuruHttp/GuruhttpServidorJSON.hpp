#if !defined GURUHTTPSERVIDORJSON_H__
#define GURUHTTPSERVIDORJSON_H__

#include <string>

#if defined GURUHTTP_JSON
#include <jsoncpp/json.h>
#endif

#include <GuruCompat.h>
#include <GuruDirectorio.hpp>

#include "GuruhttpInterface.hpp"

using namespace std;

class GuruhttpServidorJSON : public SitioWeb
{
public:
	GuruhttpServidorJSON() {};
	virtual ~GuruhttpServidorJSON() {};

	void procesarGet() {
		if (!recurso().info().Existe()) throw GuruhttpExcepcion(404);

		if (recurso().info().EsArchivo()) return response()->enviar_archivo();

	#if defined GURUHTTP_JSON
		if (recurso().info().EsDirectorio()) {
			listado_json(recurso().ruta_absoluta());
		} else throw GuruhttpExcepcion(404);
	#else
		throw GuruhttpExcepcion(400, "JSON no Implementado.");
	#endif
	}

#if defined GURUHTTP_JSON
	void listado_json(const std::string& directorio) {

		Json::Value root;

		Json::Value archivos;
		Json::Value directorios;

		std::string tmpdir(directorio);

		GuruDirectorio::GuruArchivo ar;

		tmpdir = GuruDirectorio::comoDirectorio(tmpdir);

		ar = GuruDirectorio::propiedades(tmpdir);

		// pregunto si navego el directorio o descargo el archivo
		if (GuruDirectorio::esDirectorio(tmpdir)) {

			for (GuruDirectorio::iterator it(tmpdir), end; it != end; ++it) {
				Json::Value item;

				string dir(recurso().subdirectorio() + recurso().archivo());

				if (!dir.empty())
					if (dir.at(dir.length() - 1) != '/')
						dir += '/';

				if (it->EsDirectorio()) {

					item["Nombre"] = it->nombre();
					item["directorio"] = "http://" + atributo("Host") + recurso().directorio() + dir + it->nombre() + "/";

					root["Directorios"].append(item);
				} else if (it->EsArchivo()) {
					item["Nombre"] = it->nombre();
					item["Bytes"] = static_cast<Json::Value::UInt64>(it->tamanio());
					item["Fecha"] = GuruAlgo::fechaHoraRFC822(it->fecha());
					item["archivo"] = "http://" + atributo("Host") + recurso().directorio() + dir + it->nombre();

					root["Archivos"].append(item);
				}

				item.clear();
			}
		} else if (ar.EsArchivo()) {
			response()->content_type(GuruhttpAlgo::mime(ar.nombre()));
			recurso().info(ar);
			response()->enviar_archivo();

			return;
		} else throw GuruhttpExcepcion(404);

		//			Json::StyledWriter writer;
		//			Json::FastWriter writer;
		//			oss << writer.write(vector);

		if (root.empty()) throw GuruhttpExcepcion(404);

		enviar_buffer(root.toStyledString(), "application/json"); 
	}
#endif

	void procesarHead() {

		if (!recurso().info().Existe()) {
				throw GuruhttpExcepcion(404);
		}

		if (recurso().info().EsArchivo()) {
			return response()->enviar_header_archivo();

		} else if (recurso().info().EsDirectorio()) {
		} else throw GuruhttpExcepcion(404);
	}
};

#endif
