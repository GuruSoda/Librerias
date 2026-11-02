#include "GuruConfigV2.h"

//////////////////////////////////////////////////////////////////////
// Tarea:		Abrir el archivo .conf
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
void GuruConfig::abrir(const string& nombre)
{
	nombre_archivo=nombre;

	cerrar();
	abrir();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Abrir el archivo .conf
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
void GuruConfig::abrir()
{
	if (handle_archivo.is_open()) {
		handle_archivo.seekg(ios::beg);
	} else { 
		handle_archivo.open(nombre_archivo.c_str(), ios::in | ios::binary);
	}

	if (!handle_archivo.good()) throw GuruConfigExcepcion(GuruError().descripcion());
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cerrar el archivo .conf
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
void GuruConfig::cerrar()
{
	handle_archivo.close();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Extrae un caracter del archivo o de memoria
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
int GuruConfig::sacar_caracter()
{
	caracter = handle_archivo.get();

	switch (caracter) {
	case EOF:   break;
	case '\n': 
	case '\r':
				columna = 0;
				fila++;
				break;
	default:
				columna++;
				break;
	}

	lugar_actual++;

	return caracter;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Pone un caracter del archivo o de memoria
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
int GuruConfig::poner_caracter()
{
	handle_archivo.putback(caracter);

	lugar_actual--;

	return caracter;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar un Token para luego su analisis sintactico
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruConfig::tipo_caracter GuruConfig::sacar_token()
{
	caracter = sacar_caracter();

	buffer.clear();

	while (caracter != EOF && isspace(caracter))
		caracter = sacar_caracter();

	switch (caracter) {
	case EOF: return fin_archivo;
	case '(': return abre_parentesis;
	case ')': return cierra_parentesis;
	case '\"': return sacar_cadena();
	case '#':
				token = sacar_hasta_enter();

				if (token == fin_archivo) 
					return token;

				return sacar_token();
	case '=':   // despues del igual puede venir un valor o una subseccion o un campo libre, los campos libre empiezan con '{'.

				do {
					caracter = sacar_caracter();
				}while (caracter != EOF && isspace(caracter));

				inicio_valor = lugar_actual;

				switch (caracter) {
		#if defined GURUCONFIG_CAMPOLIBRE
				case '{': 
						token=sacar_campo_libre();
						return campo_libre;						
						break;
		#endif
				case '"':
						token = sacar_cadena(); // debe retornar valor
						if (token == punto_y_coma) return valor;
						else return token;
						break;
				case '(':  
						// es una subseccion
						return subseccion;
						break;
				default:
						poner_caracter(); // devuelvo el caracter que saque para saber si es un campo libre o no.

						token = sacar_hasta_punto_y_coma();

						if (token == punto_y_coma)
							return valor;
						else
							if (token == error || token == fin_archivo || token == enter)
								return token;
							else
								return error;

								break;
				}
#if defined GURUCONFIG_CAMPOLIBRE
	case '{': return abre_llave;
	case '}': return cierra_llave;
#endif
	case ';': return punto_y_coma;
	default: inicio_simbolo = lugar_actual; 
			return sacar_simbolo();
	}
}

bool GuruConfig::siguiente_simbolo()
{
	token = sacar_token();
	
	switch (token) {
	case valor: // token = sacar_token();
				break;
	case campo_libre: 
				// ignorar_campo_libre();
				break;
	case subseccion:
		//		sacar_hasta_fin_seccion();
		//		if ((token = sacar_token()) != punto_y_coma) return false;
				break;
	default:
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar una cadena desde el archivo o memoria
// Parametros:	Ninguno.
// NOTA: Se tendria que llamar descartar_hasta_enter (al igual que otras)
//////////////////////////////////////////////////////////////////////
enum GuruConfig::tipo_caracter GuruConfig::sacar_hasta_enter()
{
	caracter = sacar_caracter();

	buffer.clear();

	while (caracter != EOF && caracter != '\n')
		caracter = sacar_caracter();

	if (caracter == EOF)
		return fin_archivo;
	else
		return comentario;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar una cadena desde el archivo o memoria
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruConfig::tipo_caracter GuruConfig::sacar_hasta_punto_y_coma()
{
	caracter = sacar_caracter();

	buffer.clear();

	while (caracter != EOF && isspace(caracter))
		caracter = sacar_caracter();

	while (caracter != EOF && caracter != ';') {
		agregar_caracter(caracter);

		caracter = sacar_caracter();
	}

	switch (caracter) {
	case EOF: return fin_archivo;
	case ';': // poner_caracter();
			  // return valor;
				return punto_y_coma;
	}

	return error;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar una cadena desde el archivo o memoria
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruConfig::tipo_caracter GuruConfig::sacar_hasta_cierra_parenteris()
{
	uint32_t cant_parentesis = 0;

	buffer.clear();

	do{
		caracter = sacar_caracter();

		if (caracter == '(')
			cant_parentesis++;

		if (caracter == ')' && cant_parentesis != 0) {
			cant_parentesis--;
			caracter = sacar_caracter();
		}

	}while (caracter != EOF && caracter != ')');

	if (caracter == EOF)
		return fin_archivo;
	else
		return cierra_parentesis;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar toda la seccion (hasta cierra_parentesis)
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruConfig::tipo_caracter GuruConfig::sacar_hasta_fin_seccion()
{
	enum tipo_caracter token;

	while ((token = sacar_token()) != cierra_parentesis) {
		if (token == simbolo) continue;

		if (token == fin_archivo) break;

		if (token == valor) continue;

		if (token == campo_libre) continue;

		if (token == subseccion) 
			if (sacar_hasta_fin_seccion() == cierra_parentesis)
				if ((token = sacar_token()) == punto_y_coma) 
					continue;
	}

	if (token == fin_archivo)
		return fin_archivo;
	else
		return cierra_parentesis;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar una cadena desde el archivo o memoria
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruConfig::tipo_caracter GuruConfig::sacar_cadena()
{
	caracter = sacar_caracter();

	buffer.clear();

	while (caracter != EOF && caracter != '\"') {
		agregar_caracter(caracter);

		caracter = sacar_caracter();
	}

	switch (caracter) {
	case EOF: return fin_archivo;
	case '\"':	
				while (caracter != EOF && caracter != ';')
					caracter = sacar_caracter();

				switch (caracter) {
				case ';': return punto_y_coma;
				default:
						return error;
				}
	default:
			return error;
	}
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar un simbolo desde memoria o desde el archivo
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruConfig::tipo_caracter GuruConfig::sacar_simbolo()
{
	while (caracter != '(' && caracter != ')' && caracter != EOF && !isspace(caracter) && caracter != '\"'
		&& caracter != '=' && caracter != ';') {
		agregar_caracter(caracter);

		caracter = sacar_caracter();
	}

	if (caracter == EOF) return fin_archivo;

	poner_caracter();

	return simbolo;
}

#if defined GURUCONFIG_CAMPOLIBRE
enum GuruConfig::tipo_caracter GuruConfig::sacar_campo_libre()
{
	caracter = sacar_caracter();

	while (caracter != '}' && caracter != EOF) {
		agregar_caracter(caracter);

		if (caracter == '{') {
			if (sacar_bloque('{','}') == fin_archivo) break;
			else agregar_caracter(sacar_caracter()); // saco el caracter de fin de bloque
		}

		caracter = sacar_caracter();
	}

	if (caracter == EOF) return fin_archivo;

	return campo_libre;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Pasar por alto un campo libre, sin cargarlo en 
//				ningun buffer interno
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruConfig::tipo_caracter GuruConfig::ignorar_campo_libre()
{
	uint32_t tamanio = 0;

	caracter = sacar_caracter();

	switch (caracter) {
	case '(':  // entre parentesis se encuetran la cantidad de bytes del campo
		if ((token = sacar_token()) == simbolo) {
			tamanio = atol(buffer.c_str());

			if ((token = sacar_token()) != cierra_parentesis)
				return error;
		} else
			return error;

		while (tamanio--) {
			sacar_caracter();
			// if (sacar_caracter() == EOF) 
			//	return fin_archivo;
		}

		//							if ((token = sacar_token()) != cierra_llave)
		//								return error;

		break;
	case '[':   // entre corchetes se encuetra un tag de fin de campo
		break;
	default: // si no es nada, en campo termina con una llave cerrada
		while (caracter != '}') caracter = sacar_caracter();
		break;
	}

	if ((token = sacar_token()) != punto_y_coma) return error;
	else return token;

	/*
	if (token == cierra_llave)
	{
	// if ((token = sacar_token()) != punto_y_coma) return error;
	// else
	return campo_libre;
	}
	else
	return token;
	*/
}

#endif

enum GuruConfig::tipo_caracter GuruConfig::sacar_bloque(int iniciobloque, int finbloque)
{
	caracter = sacar_caracter();

	while (caracter != finbloque && caracter != EOF) {
		agregar_caracter(caracter);

		if (caracter == iniciobloque)
			sacar_bloque(iniciobloque, finbloque);

		caracter = sacar_caracter();
	}

	if (caracter == EOF) return fin_archivo;
	else if (caracter == finbloque) poner_caracter();

	return campo_libre;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Agrega un caracter al buffer que contiene datos sobre
//				el analisis de la salida
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
void GuruConfig::agregar_caracter(int caracter)
{
	if (caracter == '\n') {
		fila = 0;
		columna++;
	} else
		fila++;

	buffer += caracter;
}
