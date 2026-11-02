#include "GuruConfigLeer.h"

//////////////////////////////////////////////////////////////////////
// Lee una seccion desde un archivo .conf.
// Esta seccion puede o no tener un nombre
// Ejem:
// nombre (entrada1=valor1;entrada2=valor2);
// o
// (entrada1=valor1;entrada2=valor2);
// Las secciones en el mismo archivo .conf pueden o no tener nombre
//////////////////////////////////////////////////////////////////////
string GuruConfigLeer::leer (const string& seccion, const string& entrada, const string& val_default)
{
//	if (seccion.empty() || entrada.empty()) return val_default;
	if (entrada.empty()) return val_default;

	string _valor(val_default);

	enum tipo_caracter token;

	try {
		abrir();

		while ((token = sacar_token()) != fin_archivo) {

			// pregunto si es una seccion con o sin nombre
			if (token != simbolo && token != abre_parentesis) throw _valor;

			if ((seccion.empty() && token == abre_parentesis) || (GuruStringAlgo::stringequals(seccion, buffer) == true)) {

				if (!seccion.empty()) token = sacar_token();

				if (token != abre_parentesis) throw _valor;

				while ((token = sacar_token()) != cierra_parentesis) {
					// es el nombre de una entrada y en token tendria que ser un simbolo
					if (GuruStringAlgo::stringequals(entrada, buffer)) {
						token = sacar_token();

						// esta pregunta es por si en el archivo pusieron Destino=;
						if (buffer.length() == 0) throw _valor;

						switch (token) {
						case valor: throw buffer;
									break;
						case campo_libre: throw buffer;
									break;
						case subseccion: sacar_hasta_fin_seccion();

									token = sacar_token();
									if (token != punto_y_coma)
										throw _valor;
									break;
						default:
									break;
						}
					} else {
						if (!siguiente_simbolo()) throw _valor;
					}
				}
				
				throw string(val_default); // entrada no encontrada
				
			} else {
				if (token == simbolo) token = sacar_token();

				if (token == abre_parentesis) {
					if (sacar_hasta_fin_seccion() == fin_archivo)
						throw _valor;
				} else
					throw _valor;
			}
		}
	} catch (GuruConfigExcepcion&) {
	} catch (string& e) {
		_valor=e;
	};

//	cerrar_conf();

	return _valor;
}

//////////////////////////////////////////////////////////////////////
// total_secciones: retorna la cantidad de secciones encontradas en
//                  el archivo de configuracion
// Nota: NO almancena en ninguna lista, SOLO cuenta las secciones
// Estado: Casi O.K.
//////////////////////////////////////////////////////////////////////
uint32_t GuruConfigLeer::total_secciones()
{
	abrir();

	uint32_t total_secciones = 0;

	enum tipo_caracter token;

	while ((token = sacar_token()) != fin_archivo) {
		if (token == simbolo) {				
			token = sacar_token();
			if (token == abre_parentesis) {
				total_secciones++;
				sacar_hasta_fin_seccion();
			}
		}
	}

	cerrar();

	return total_secciones;
}

string GuruConfigLeer::leer_seccion(map<string, string>& simbolo_valor)
{
	simbolo_valor.clear();
	string nombre_seccion;

	try {
		token = sacar_token();

		if (token == simbolo || token == abre_parentesis) {
			// simbolo == buffer con el nombre de la entrada
			if (token == simbolo) {
				nombre_seccion = buffer;
				token = sacar_token(); // despues del nombre de la seccion viene un abre_parentesis
			}

			if (token == abre_parentesis) {
				while ((token = sacar_token()) != cierra_parentesis) {
					string str_entrada(buffer);
					string str_valor;

					token = sacar_token();

					switch (token) {
					case valor:								
					case campo_libre: 
								str_valor=buffer;
								break;
					case subseccion: sacar_hasta_fin_seccion();
								token = sacar_token();
								if (token != punto_y_coma)
									throw "Error en parser";
								break;
					default:
								break;
					}
					simbolo_valor.insert(make_pair(str_entrada, str_valor));
				}
			}
		}
	} catch (GuruConfigExcepcion&) {
	} catch (string& e) {
		cerr << e << endl;
	};

	return nombre_seccion;
}

map<string, string>& GuruConfigLeer::leer_seccion(const string& seccion, map<string, string>& simbolo_valor) {

	enum tipo_caracter token;

	try {
		simbolo_valor.clear();

		abrir();

		while ((token = sacar_token()) != fin_archivo) {
			// pregunto si es una seccion con o sin nombre
			if (token != simbolo && token != abre_parentesis) throw GuruConfigExcepcion("Error en parser.");

			if ((seccion.empty() && token == abre_parentesis) || (GuruStringAlgo::stringequals(seccion, buffer) == true)) {

				if (!seccion.empty()) token = sacar_token();

				if (token != abre_parentesis) throw GuruConfigExcepcion("Error en parser.");

				while ((token = sacar_token()) != cierra_parentesis) {
					string _llave, _valor;

					if (token == simbolo) _llave = buffer;

					token = sacar_token();

					switch (token) {
					case valor: _valor=buffer;
								break;
					default:
								break;
					}

					simbolo_valor.insert(make_pair(_llave, _valor));
				}
			} else {
				if (token == simbolo) token = sacar_token();

				if (token == abre_parentesis) {
					if (sacar_hasta_fin_seccion() == fin_archivo)
						throw simbolo_valor;
				} else
					throw simbolo_valor;
			}
		}
	} catch (GuruConfigExcepcion& e) {
		cerr << "GuruConfigLeer::leer_seccion:" << e.what() << endl;
	}

	cerrar();

	return simbolo_valor;
}


vector<string>& GuruConfigLeer::secciones(vector<string>& listado)
{
	abrir();

	listado.clear();

	while ((token = sacar_token()) != fin_archivo) {
		if (token == simbolo) {	
			listado.push_back(buffer);
			token = sacar_token();
			if (token == abre_parentesis) {
				sacar_hasta_fin_seccion();
			}
		}
	}

	cerrar();

	return listado;
}
