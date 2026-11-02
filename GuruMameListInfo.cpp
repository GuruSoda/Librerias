#include "GuruMameListInfo.h"

GuruMameListInfo::GuruMameListInfo(const string& ruta_listinfo, bool cargar_a_memoria)
{
	_nombre_archivo_listinfo = ruta_listinfo;
	en_memoria = cargar_a_memoria;
	archivo = NULL;
	inicio_archivo = NULL;
	token = GuruMameListInfo::simbolo;
}

GuruMameListInfo::~GuruMameListInfo(void)
{
}

vector<juego>& GuruMameListInfo::listinfo2list()
{
	abrir();

	for (GuruMameListInfo::iterator it=begin();it!=end();++it)
		lista_juegos.push_back(*it);

	cerrar();

	return lista_juegos;
}

map<string, juego>& GuruMameListInfo::listinfo2map()
{
	abrir();

	for (GuruMameListInfo::iterator it=begin();it!=end();++it)
		mapa_juegos.insert(make_pair(it->nombre, *it));

	cerrar();

	return mapa_juegos;
}

map<string, vector<rom> >& GuruMameListInfo::listinfo2rommap()
{
	abrir();

	for (GuruMameListInfo::iterator it=begin();it!=end();++it)
		mapa_roms.insert(make_pair(it->nombre, it->lista_roms));

	cerrar();

	return mapa_roms;
}

juego GuruMameListInfo::sacar_juego()
{
/*
	if (!en_memoria){
		if (!_archivo_listinfo.is_open())
			if (!abrir()) return juego();
	}else if (archivo == NULL)
			if (!abrir()) return juego();
*/

	bool es_game		= false;
	bool es_resource	= false;

	juego	info_juego;

	if ((token = sacar_token()) == fin_archivo) {
//		cerrar();
		return juego();
	} else {
		es_game = es_resource = false;

		if (buffer.compare("game") == 0)
			es_game = true;
		else
			if (buffer.compare("resource") == 0)
				es_resource = true;

		if (es_game || es_resource)
			{
			while ((token = sacar_token()) != cierra_parentesis)
				{
				if (token == abre_parentesis)
					continue;

				if (buffer.compare("name") == 0)
					if ((token = sacar_token()) == simbolo)
						{
						info_juego.nombre = buffer;
//							cout << info_juego.nombre << endl;

						continue;
						}

				if (buffer.compare("description") == 0)
					if ((token = sacar_token()) == cadena)
						{
						info_juego.descripcion = buffer;

						continue;
						}

				if (buffer.compare("manufacturer") == 0)
					if ((token = sacar_token()) == cadena)
						{
						info_juego.fabricante = buffer;

						continue;
						}

				if (buffer.compare("year") == 0)
					if ((token = sacar_token()) == simbolo)
						{
						info_juego.anio = buffer;

						continue;
						}

				if (buffer.compare("romof") == 0)
					if ((token = sacar_token()) == simbolo)
						{
						info_juego.rom_de = buffer;

						continue;
						}

				if (buffer.compare("cloneof") == 0)
					if ((token = sacar_token()) == simbolo)
						{
						info_juego.clon_de = buffer;

						continue;
						}

				if (buffer.compare("sampleof") == 0)
					if ((token = sacar_token()) == cadena)
						{
						continue;
						}

				if (buffer.compare("sourcefile") == 0)
					if ((token = sacar_token()) == simbolo)
						{
						continue;
						}

				if (buffer.compare("rom") == 0)
					{
					enum tipo_caracter	token;
					rom		info_rom;

					while ((token = sacar_token()) != cierra_parentesis)
						{
						if (token == abre_parentesis) continue;

						if (token == simbolo)
							{
							if (buffer.compare("name") == 0)
								{
								token = sacar_token();
								if (token == simbolo || token == cadena)
									info_rom.nombre = buffer;
								}
							else if (buffer.compare("size") == 0)
								{
								if ((token = sacar_token()) == simbolo)
									info_rom.tamanio = atol(buffer.c_str());
								}
							else if (buffer.compare("crc") == 0)
								{
								if ((token = sacar_token()) == simbolo)
									info_rom.crc = buffer;
								}
							else if (buffer.compare("sha1") == 0)
								{
								if ((token = sacar_token()) == simbolo)
									info_rom.sha1 = buffer;
								}
							else if (buffer.compare("merge") == 0)
								{
								token = sacar_token();
								if (token == simbolo || token == cadena)
									info_rom.unido = buffer;
								}
							else continue;

							continue;
							}
						}

					info_juego.lista_roms.push_back(info_rom);

//					info_rom.clear();
					continue;
					}

				if (buffer.compare("disk") == 0)
					{
					ignorar_seccion();
			//		armar_registro_de_simbolos(disk);

					continue;
					}

				if (buffer.compare("biosset") == 0)
					{
			//		armar_registro_de_simbolos(biosset);
					ignorar_seccion();

					continue;
					}

				if (buffer.compare("chip") == 0)
					{
			//		armar_registro_de_simbolos(chip);
					ignorar_seccion();

					continue;
					}

				if (buffer.compare("video") == 0)
					{
			//		armar_registro_de_simbolos(video);
					ignorar_seccion();

					continue;
					}

				if (buffer.compare("sound") == 0)
					{
			//		armar_registro_de_simbolos(sound);
					ignorar_seccion();

					continue;
					}

				if (buffer.compare("input") == 0)
					{
			//		armar_registro_de_simbolos(input);
					ignorar_seccion();

					continue;
					}

				if (buffer.compare("dipswitch") == 0)
					{
			//		armar_registro_de_cadenas(dipswitch);
					ignorar_seccion();

					continue;
					}

				if (buffer.compare("driver") == 0)
					{
			//		armar_registro_de_simbolos(driver);
					ignorar_seccion();

					continue;
					}

				} // mientras no se cierra el parentesis de game
			}
		}

	return juego(info_juego);
}

/////////////////////////////////////////////////////////////////////////
/*
void GuruMameListInfo::armar_registro_de_simbolos(enum entrada tipo_entrada)
{
	char **parametro	= NULL;
	int cant_items		= 0;
	enum tipo_caracter token;

	parametro = new char * [1000]; // 500 items?

	while ((token = sacar_token()) != cierra_parentesis)
		{
		if (token == abre_parentesis) continue;

		if (token == simbolo)
			{
			parametro[cant_items] = new char [strlen(buffer) + 1];
			strcpy(parametro[cant_items++], buffer);

			if ((token = sacar_token()) == simbolo)
				{
				parametro[cant_items] = new char [strlen(buffer) + 1];
				strcpy(parametro[cant_items++], buffer);

				continue;
				}
			}
		}

	parametro[cant_items] = NULL;
	parametro[cant_items+1] = NULL;

	for (int i = 0; i< cant_items; i++)
		delete parametro [i];

	delete [] parametro;
}
*/

void GuruMameListInfo::ignorar_seccion()
{
	while (sacar_token() != cierra_parentesis)
		;
}

//////////////////////////////////////////////////////////////////////
// Tarea:	Abrir el archivo correspondiente a la salida del mame con
//			la opcion -listinfo.
// Parametros: Ruta donde se encuentra el mame y el temporario de
//				dejar la salida.
//////////////////////////////////////////////////////////////////////
bool GuruMameListInfo::abrir()
{
	cerrar(); // en caso que ya se encuentre abierto

	_archivo_listinfo.open(_nombre_archivo_listinfo.c_str(), std::ios_base::in | std::ios_base::binary);

	if (_archivo_listinfo)
		{
		if (en_memoria)
			{
			if (cargar_a_memoria())
				{
				_archivo_listinfo.close();
				return true;
				}
			else
				return false;
			}
		return true;
		}
	else
		{
		token = GuruMameListInfo::fin_archivo;
		return false;
		}
}

void GuruMameListInfo::cerrar()
{
	if (inicio_archivo)
		{
		delete [] inicio_archivo;
		inicio_archivo = NULL;
		}

	if (_archivo_listinfo.is_open())
		_archivo_listinfo.close();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar a memoria el archivo de salida del mame.
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
bool GuruMameListInfo::cargar_a_memoria()
{
	unsigned long tamanio;

	_archivo_listinfo.seekg(0, ios::end); // fseek(mame_txt, 0, SEEK_END);
	tamanio = _archivo_listinfo.tellg(); // tamanio = ftell(mame_txt);
	_archivo_listinfo.seekg(0, ios::beg); // fseek(mame_txt, 0, SEEK_SET);

	archivo = new char [tamanio + 4];

	if (!archivo) return false;

	inicio_archivo = archivo;

	_archivo_listinfo.read(archivo, tamanio); // fread(archivo, tamanio, 1, mame_txt);

	archivo[_archivo_listinfo.gcount()] = EOF;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Extrae un caracter del archivo o de memoria
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
int GuruMameListInfo::sacar_caracter()
{
	if (en_memoria)
		{
		if (inicio_archivo == NULL)
			return EOF;
		else
			return *archivo++;
		}
	else
		{
		char elchar;
		if (_archivo_listinfo.get(elchar).eof()) elchar = EOF;
		return (int)elchar;
		}
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar un Token para luego su analisis sintactico
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruMameListInfo::tipo_caracter GuruMameListInfo::sacar_token()
{
	caracter = sacar_caracter();

	buffer.clear();

	while (caracter != EOF && isspace(caracter))
		caracter = sacar_caracter();

	switch (caracter)
		{
	case EOF: return fin_archivo;
	case '(': return abre_parentesis;
	case ')': return cierra_parentesis;
	case '\"': return sacar_cadena();
	default: return sacar_simbolo();
		}
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar una cadena desde el archivo o memoria
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruMameListInfo::tipo_caracter GuruMameListInfo::sacar_cadena()
{
	caracter = sacar_caracter();

	buffer.clear();

	while (caracter != EOF && caracter != '\"')
		{
		agregar_caracter(caracter);

		caracter = sacar_caracter();
		}

	return cadena;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar un simbolo desde memoria o desde el archivo
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum GuruMameListInfo::tipo_caracter GuruMameListInfo::sacar_simbolo()
{
	while (caracter != '(' && caracter != ')' && caracter != EOF && !isspace(caracter) && caracter != '\"')
		{
		agregar_caracter(caracter);

		caracter = sacar_caracter();
		}

	return simbolo;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Agrega un caracter al buffer que contiene datos sobre
//				el analisis de la salida
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
void GuruMameListInfo::agregar_caracter(int caracter)
{
	buffer.append(1, caracter);
/*
	dword tamanio = (dword)strlen(buffer);

	buffer[tamanio] = caracter;
	buffer[tamanio+1] = '\0';
*/
}
