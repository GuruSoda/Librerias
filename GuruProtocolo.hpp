#ifndef __GURUPROTOCOLO_H__
#define __GURUPROTOCOLO_H__

namespace GuruProtocolo {
	const char RESPUESTA_BIEN[] = "BIEN";
	const char RESPUESTA_MAL[] = "MAL";
	const char RESPUESTA_NOEXISTE[] = "!";
	const char RESPUESTA_PROHIBIDO[] = "*";

	enum {desconocido, bien=1, mal, noexiste, prohibido};

	inline static const char* str_bien() {return RESPUESTA_BIEN;}
	inline static const char* str_mal() {return RESPUESTA_MAL;}
	inline static const char* str_noexiste() {return RESPUESTA_NOEXISTE;}
	inline static const char* str_prohibido() {return RESPUESTA_PROHIBIDO;}

	inline static int codigo_bien() {return bien;}
	inline static int codigo_mal() {return mal;}
	inline static int codigo_noexiste() {return noexiste;}
	inline static int codigo_prohibido() {return prohibido;}
	inline static int codigo_desconocido() {return desconocido;}
};

#endif
