#include "GuruParserMame.h"

GuruParserMame::GuruParserMame()
{
	en_memoria				= true;
	archivo					= NULL;
	inicio_archivo			= NULL;
	opciones_notificacion	= GPM_ALL;
	notificar				= NULL;
	notificar_seccion       = NULL;
	parametro_notificacion	= NULL;
	parametro_notificacion_seccion = NULL;
	analizando				= false;
	mame_txt                = NULL;

	inicio_parser			= 0;
	fin_parser				= 0;


	buffer = new char [GPM_TAMANIO_BUFFER + 1];
	*buffer = '\0';
}

GuruParserMame::~GuruParserMame()
{
	cerrar_listinfo();

	if (buffer)
		delete buffer;
}

//////////////////////////////////////////////////////////////////////
// Tarea:	Cargar una notificacion sobre cada carga de entrada del archivo
// Parametro: Direccion de la funcion a llamar
//////////////////////////////////////////////////////////////////////
void GuruParserMame::cargar_notificacion(entrada_listinfo *nueva_notificacion, void *nuevo_extra_parametro)
{
	notificar = nueva_notificacion;

	if (nuevo_extra_parametro)
		parametro_notificacion = nuevo_extra_parametro;
}

void GuruParserMame::cargar_notificacion_seccion(seccion_listinfo *nueva_notificacion, void *extra_parametro_seccion)
{
	notificar_seccion = nueva_notificacion;

	if (extra_parametro_seccion)
		parametro_notificacion_seccion = extra_parametro_seccion;
}

//////////////////////////////////////////////////////////////////////
// Tarea:	cargar en una lista doblemente enlazada la informacion
//			sacada del archivo mame.exe
// Parametros: ruta donde se encuentra el ejecutable del mame
//////////////////////////////////////////////////////////////////////
bool GuruParserMame::cargar_listinfo(const char *ruta_mame, char *dir_temp)
{
	bool salida		= false;
	bool es_game	= false;
	bool es_resource	= false;

	analizando = true;

	if (abrir_listinfo(ruta_mame, dir_temp))
		{
		time(&inicio_parser);

		enum tipo_caracter token;

		juego	info_juego;

		while ((token = sacar_token()) != fin_archivo)
			{
			if (token != simbolo) break;

			if (!analizando) return false;

			es_game = es_resource = false;

			if (stricmp(buffer, "game") == 0)
				es_game = true;
			else
				if (stricmp(buffer, "resource") == 0)
					es_resource = true;

			if (es_game || es_resource)
				{
				if ((token = sacar_token()) != abre_parentesis) continue;

				if (notificar)
					(*notificar) ((es_game) ? game : resource, 0, NULL, parametro_notificacion);

				while ((token = sacar_token()) != cierra_parentesis)
					{

					if (!analizando) return false;

					if (stricmp(buffer, "name") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							char *parametro[] = {buffer};

							info_juego.nombre.poner(buffer);

							if (notificar)
								(*notificar) (name, 1, parametro, parametro_notificacion);

							continue;
							}

					if (stricmp(buffer,"description") == 0)
						if ((token = sacar_token()) == cadena)
							{
							char *parametro[] = {buffer};
							info_juego.descripcion.poner(buffer);

							if (notificar)
								(*notificar) (description, 1, parametro, parametro_notificacion);

							continue;
							}

					if (stricmp(buffer,"manufacturer") == 0)
						if ((token = sacar_token()) == cadena)
							{
							char *parametro[] = {buffer};
							info_juego.fabricante.poner(buffer);

							if (notificar)
								(*notificar) (manufacturer, 1, parametro, parametro_notificacion);

							continue;
							}

					if (stricmp(buffer,"year") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							char *parametro[] = {buffer};
							info_juego.anio.poner(buffer);

							if (notificar)
								(*notificar) (year, 1, parametro, parametro_notificacion);

							continue;
							}

					if (stricmp(buffer,"romof") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							char *parametro[] = {buffer};
							info_juego.rom_de.poner(buffer);							

							if (notificar)
								(*notificar) (romof, 1, parametro, parametro_notificacion);

							continue;
							}

					if (stricmp(buffer,"cloneof") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							char *parametro[] = {buffer};
							info_juego.clon_de.poner(buffer);

							if (notificar)
								(*notificar) (cloneof, 1, parametro, parametro_notificacion);

							continue;
							}

					if (stricmp(buffer,"sampleof") == 0)
						if ((token = sacar_token()) == cadena)
							{
							char *parametro[] = {buffer};

							if (notificar)
								(*notificar) (sampleof, 1, parametro, parametro_notificacion);

							continue;
							}

					if (stricmp(buffer,"rom") == 0)
						{
						armar_registro_de_simbolos(rom);

						continue;
						}

					if (stricmp(buffer,"disk") == 0)
						{
						armar_registro_de_simbolos(disk);

						continue;
						}

					if (stricmp(buffer,"biosset") == 0)
						{
						armar_registro_de_simbolos(biosset);

						continue;
						}

					if (stricmp(buffer,"chip") == 0)
						{
						armar_registro_de_simbolos(chip);

						continue;
						}

					if (stricmp(buffer,"video") == 0)
						{
						armar_registro_de_simbolos(video);

						continue;
						}

					if (stricmp(buffer,"sound") == 0)
						{
						armar_registro_de_simbolos(sound);

						continue;
						}

					if (stricmp(buffer,"input") == 0)
						{
						armar_registro_de_simbolos(input);

						continue;
						}

					if (stricmp(buffer,"dipswitch") == 0)
						{
						armar_registro_de_cadenas(dipswitch);

						continue;
						}

					if (stricmp(buffer,"driver") == 0)
						{
						armar_registro_de_simbolos(driver);

						continue;
						}
//					else
//						armar_registro_de_simbolos(driver);


					} // mientras no se cierra el parentesis de game

				if (notificar)
					(*notificar) ((es_game) ? fin_game : fin_resource, 0, NULL, parametro_notificacion);

				if (notificar_seccion)
					(*notificar_seccion) (&info_juego, parametro_notificacion_seccion);
				}
			}
		}

	time(&fin_parser);

	cerrar_listinfo();

	return salida;
}


////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Tarea:	cargar en una lista doblemente enlazada la informacion
//			sacada del archivo mame.exe
// Parametros: ruta donde se encuentra el ejecutable del mame
//////////////////////////////////////////////////////////////////////
bool GuruParserMame::cargar_listinfo_V2(const char *ruta_mame, char *dir_temp)
{
	bool salida			= false;
	bool es_game		= false;
	bool es_resource	= false;

	analizando = true;

	if (abrir_listinfo(ruta_mame, dir_temp))
		{
		time(&inicio_parser);

		enum tipo_caracter token;

		juego	info_juego;
		info_juego.rom.autoincrementar(false);

		while ((token = sacar_token()) != fin_archivo)
			{
			if (token != simbolo) break;

			if (!analizando) return false;

			es_game = es_resource = false;

			if (stricmp(buffer, "game") == 0)
				es_game = true;
			else
				if (stricmp(buffer, "resource") == 0)
					es_resource = true;

			if (es_game || es_resource)
				{
				if ((token = sacar_token()) != abre_parentesis) continue;

				while ((token = sacar_token()) != cierra_parentesis)
					{

					if (!analizando) return false;

					if (stricmp(buffer, "name") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							info_juego.nombre = buffer;

							continue;
							}

					if (stricmp(buffer,"description") == 0)
						if ((token = sacar_token()) == cadena)
							{
							info_juego.descripcion = buffer;

							continue;
							}

					if (stricmp(buffer,"manufacturer") == 0)
						if ((token = sacar_token()) == cadena)
							{
							info_juego.fabricante = buffer;

							continue;
							}

					if (stricmp(buffer,"year") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							info_juego.anio = buffer;

							continue;
							}

					if (stricmp(buffer,"romof") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							info_juego.rom_de = buffer;

							continue;
							}

					if (stricmp(buffer,"cloneof") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							info_juego.clon_de = buffer;

							continue;
							}

					if (stricmp(buffer,"sampleof") == 0)
						if ((token = sacar_token()) == cadena)
							{
							continue;
							}

					if (stricmp(buffer,"rom") == 0)
						{						
						enum tipo_caracter	token;
						data_rom			tmp_data_rom;


						while ((token = sacar_token()) != cierra_parentesis)
							{
							if (token == abre_parentesis) continue;

							if (token == simbolo)
								{
								if (stricmp(buffer, "name") == 0)
									{
									if ((token = sacar_token()) == simbolo)
										tmp_data_rom.nombre = buffer;
									}
								else if (stricmp(buffer, "size") == 0)
									{
									if ((token = sacar_token()) == simbolo)
										tmp_data_rom.tamanio = buffer;
									}
								else if (stricmp(buffer, "crc") == 0)
									{
									if ((token = sacar_token()) == simbolo)
										tmp_data_rom.crc = buffer;
									}
								else if (stricmp(buffer, "sha1") == 0)
									{
									if ((token = sacar_token()) == simbolo)
										tmp_data_rom.sha1 = buffer;
									}
								else if (stricmp(buffer, "merge") == 0)
									{
									if ((token = sacar_token()) == simbolo)
										tmp_data_rom.unido = buffer;
									}
								else continue;

								continue;
								}
							}

						if (!tmp_data_rom.unido.contenido())
							info_juego.rom.agregar(&tmp_data_rom);

						tmp_data_rom.nombre.liberar();
						tmp_data_rom.tamanio.liberar();
						tmp_data_rom.crc.liberar();
						tmp_data_rom.sha1.liberar();
						tmp_data_rom.unido.liberar();

						continue;
						}

					if (stricmp(buffer,"disk") == 0)
						{
						armar_registro_de_simbolos(disk);

						continue;
						}

					if (stricmp(buffer,"biosset") == 0)
						{
						armar_registro_de_simbolos(biosset);

						continue;
						}

					if (stricmp(buffer,"chip") == 0)
						{
						armar_registro_de_simbolos(chip);

						continue;
						}

					if (stricmp(buffer,"video") == 0)
						{
						armar_registro_de_simbolos(video);

						continue;
						}

					if (stricmp(buffer,"sound") == 0)
						{
						armar_registro_de_simbolos(sound);

						continue;
						}

					if (stricmp(buffer,"input") == 0)
						{
						armar_registro_de_simbolos(input);

						continue;
						}

					if (stricmp(buffer,"dipswitch") == 0)
						{
						armar_registro_de_cadenas(dipswitch);

						continue;
						}

					if (stricmp(buffer,"driver") == 0)
						{
						armar_registro_de_simbolos(driver);

						continue;
						}

					} // mientras no se cierra el parentesis de game

				if (notificar_seccion)
					(*notificar_seccion) (&info_juego, parametro_notificacion_seccion);

				info_juego.nombre.liberar();
				info_juego.descripcion.liberar();
				info_juego.anio.liberar();
				info_juego.fabricante.liberar();
				info_juego.rom_de.liberar();
				info_juego.clon_de.liberar();

				info_juego.rom.al_inicio();

				while (!info_juego.rom.fin())
					{
					info_juego.rom.item()->nombre.liberar();
					info_juego.rom.item()->tamanio.liberar();
					info_juego.rom.item()->crc.liberar();
					info_juego.rom.item()->sha1.liberar();
					info_juego.rom.item()->unido.liberar();

					info_juego.rom++;
					}

				info_juego.rom.al_inicio();
				info_juego.rom.vaciar_lista();				
				}
			}
		}

	time(&fin_parser);

	cerrar_listinfo();

	return salida;
}

void GuruParserMame::armar_registro_de_simbolos(enum entrada tipo_entrada)
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

	if (notificar)
		(*notificar) (tipo_entrada, cant_items, parametro, parametro_notificacion);

	for (int i = 0; i< cant_items; i++)
		delete parametro [i];

	delete [] parametro;
}

/////////////////////////////////////////////////////////////////////////
void GuruParserMame::armar_registro_de_simbolos_V2(enum entrada tipo_entrada)
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


void GuruParserMame::armar_registro_de_cadenas(enum	entrada tipo_entrada)
{
	char **parametro	= NULL;
	int cant_items		= 0;
	enum tipo_caracter	token;

	parametro = new char * [1000]; // 500 items?

	while ((token = sacar_token()) != cierra_parentesis)
		{
		if (token == abre_parentesis) continue;

		if (token == simbolo)
			{
			parametro[cant_items] = new char [strlen(buffer) + 1];
			strcpy(parametro[cant_items++], buffer);

			if ((token = sacar_token()) == cadena)
				{
				parametro[cant_items] = new char [strlen(buffer) + 1];
				strcpy(parametro[cant_items++], buffer);

				continue;
				}
			}
		}

	parametro[cant_items] = NULL;
	parametro[cant_items+1] = NULL;

	if (notificar)
		(*notificar) (tipo_entrada, cant_items, parametro, parametro_notificacion);

	for (int i = 0; i< cant_items; i++)
		delete parametro [i];

	delete [] parametro;
}

//////////////////////////////////////////////////////////////////////
// Tarea:	Abrir el archivo correspondiente a la salida del mame con
//			la opcion -listinfo.
// Parametros: Ruta donde se encuentra el mame y el temporario de
//				dejar la salida.
//////////////////////////////////////////////////////////////////////
bool GuruParserMame::abrir_listinfo(const char *ruta_mame, char *dir_temp)
{
	char nombre_temp[268];

//	extraer_listinfo(nombre_temp, ruta_mame, dir_temp);
	strcpy(nombre_temp, "listinfo.txt");

	if (!analizando) return false;

	if ((mame_txt = fopen(nombre_temp, "rt")) != NULL)
		{
		if (en_memoria)
			if (cargar_a_memoria())
				{
				fclose(mame_txt);
				// _unlink(nombre_temp);
                // remove(nombre_temp);
				return true;
				}
			else
				return false;

		return true;
		}
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
// Tarea:	extraer del archivo mame.exe la info con la opcion -listinfo.
// Parametros: Ruta donde se encuentra el mame, el temporario de
//				dejar la salida, y el archivo que tiene la salida
//////////////////////////////////////////////////////////////////////
bool GuruParserMame::extraer_listinfo(char *nombre_listinfo, const char *ruta_mame, char *dir_temp)
{
	char comando[268];

	if (dir_temp)
		{
		if (strchr(dir_temp, ' '))
			sprintf(nombre_listinfo, "\"%s\"", dir_temp);
		else
			strcpy(nombre_listinfo, dir_temp);
		}
	else
		strcpy(nombre_listinfo, tempnam(dir_temp, NULL));

	if (strchr(ruta_mame, ' '))
		sprintf(comando, "\"%s\" -listinfo > \"%s%s\"", ruta_mame, nombre_listinfo, LISTAINFO);
	else
		sprintf(comando, "%s -listinfo > \"%s%s\"", ruta_mame, nombre_listinfo, LISTAINFO);

// ejecuto la llamda que saca el -listinfo
//	GuruCorrer	run;

	char nombre_bat[MAX_PATH];

	strcpy(nombre_bat, nombre_listinfo);
	strcat(nombre_bat, "armame.bat");

	FILE *archivo = NULL;

	if ((archivo = fopen(nombre_bat, "wt")) != NULL)
		{
		fputc('@',archivo);
		fputs(comando, archivo);
		fclose(archivo);
		}
	else
		return false;

//	run.ejecutar(nombre_bat, true, SW_HIDE);
	system(nombre_bat);

	// _unlink(nombre_bat);

    remove(nombre_bat);


//	system(comando);

	strcat(nombre_listinfo, LISTAINFO);

//	remove(nombre_listinfo);

	return true;
}


//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar a memoria el archivo de salida del mame.
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
bool GuruParserMame::cargar_a_memoria()
{
	unsigned long tamanio;

	fseek(mame_txt, 0, SEEK_END);
	tamanio = ftell(mame_txt);
	fseek(mame_txt, 0, SEEK_SET);

	archivo = new char [tamanio + 4];

	if (!archivo) return false;

	inicio_archivo = archivo;

	fread(archivo, tamanio, 1, mame_txt);

	archivo[tamanio + 1] = EOF;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Extrae un caracter del archivo o de memoria
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
int GuruParserMame::sacar_caracter()
{
	if (en_memoria)
		return *archivo++;
	else
		return fgetc(mame_txt);
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Sacar un Token para luego su analisis sintactico
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
enum tipo_caracter GuruParserMame::sacar_token()
{
	caracter = sacar_caracter();

	limpiar_cadena();

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
enum tipo_caracter GuruParserMame::sacar_cadena()
{
	caracter = sacar_caracter();

	limpiar_cadena();

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
enum tipo_caracter GuruParserMame::sacar_simbolo()
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
void GuruParserMame::agregar_caracter(int caracter)
{
	dword tamanio = (dword)strlen(buffer);

	buffer[tamanio] = caracter;
	buffer[tamanio+1] = '\0';
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Retornar el buffer.
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
char *GuruParserMame::mostrar_buffer()
{
	return buffer;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Limpiar la cadena con la informacion sobre el analisis
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
void GuruParserMame::limpiar_cadena()
{
	buffer[0] = '\0';
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cambiar el estado de una variable para cancelar el
//				analisis del -listinfo
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
void GuruParserMame::cancelar_carga()
{
	analizando = false;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Retornar el tiempo de analisis del archivo -listinfo,
//				en segundos
// Parametros:	Cantidad de segundos tardados.
//////////////////////////////////////////////////////////////////////
time_t GuruParserMame::tiempo_analisis()
{
	if ((inicio_parser == 0) || (fin_parser == 0))
		return 0;

	return fin_parser - inicio_parser;
}

void GuruParserMame::cerrar_listinfo()
{
	if (en_memoria)
		{
		if (inicio_archivo)
			{
			delete [] inicio_archivo;

			inicio_archivo = NULL;
			mame_txt       = NULL;
			}
		}
	else
		if (mame_txt)
			{
			fclose(mame_txt);
			mame_txt = NULL;
			}
}

/*
bool GuruParserMame::cargar_listinfo(char *path_mame)
{
	bool salida = false;

	if (abrir_listinfo())
		{
		enum tipo_caracter token;

		while ((token = sacar_token()) != eof)
			{
			if (token != simbolo) return false;

			if (stricmp(buffer, "game") == 0)
				{
				if ((token = sacar_token()) != abre_parentesis) continue;

				while ((token = sacar_token()) != cierra_parentesis)
					{

					if (stricmp(buffer, "name") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							printf("Nombre : %s\n", buffer);
							continue;
							}

					if (stricmp(buffer,"description") == 0)
						if ((token = sacar_token()) == cadena)
							{
							printf("Descripcion : %s\n", buffer);
							continue;
							}

					if (stricmp(buffer,"year") == 0)
						if ((token = sacar_token()) == simbolo)
							{
							printf("Año : %s\n", buffer);
							continue;
							}

					if (stricmp(buffer,"manufacturer") == 0)
						if ((token = sacar_token()) == cadena)
							{
							printf("Fabricante : %s\n", buffer);
							continue;
							}

					if (stricmp(buffer,"romof") == 0)
						if ((token = sacar_token()) == cadena)
							{
							printf("rom de : %s\n", buffer);
							continue;
							}

					if (stricmp(buffer,"clonof") == 0)
						if ((token = sacar_token()) == cadena)
							{
							printf("clonde : %s\n", buffer);
							continue;
							}


					if (stricmp(buffer,"rom") == 0)
						{
						while ((token = sacar_token()) != cierra_parentesis)
							{
							if (token == abre_parentesis) continue;

							if (token == simbolo)
								{
								if (stricmp(buffer,"name") == 0)
									if ((token = sacar_token()) == simbolo)
										{
										printf("nombre rom : %s\n", buffer);
										continue;
										}

								if (stricmp(buffer,"merge") == 0)
									if ((token = sacar_token()) == simbolo)
										{
										printf("unir con : %s\n", buffer);
										continue;
										}


								if (stricmp(buffer,"size") == 0)
									if ((token = sacar_token()) == simbolo)
										{
										printf("tamaño rom : %s\n", buffer);
										continue;
										}

								if (stricmp(buffer,"crc") == 0)
									if ((token = sacar_token()) == simbolo)
										{
										printf("crc rom : %s\n", buffer);
										continue;
										}
								}

							}
						}

					if (stricmp(buffer,"chip") == 0)
						{
						while ((token = sacar_token()) != cierra_parentesis)
							;

						continue;
						}

					if (stricmp(buffer,"video") == 0)
						{
						while ((token = sacar_token()) != cierra_parentesis)
							;

						continue;
						}

					if (stricmp(buffer,"sound") == 0)
						{
						while ((token = sacar_token()) != cierra_parentesis)
							;

						continue;
						}

					if (stricmp(buffer,"input") == 0)
						{
						while ((token = sacar_token()) != cierra_parentesis)
							;

						continue;
						}

					if (stricmp(buffer,"dipswitch") == 0)
						{
						while ((token = sacar_token()) != cierra_parentesis)
							;

						continue;
						}

					if (stricmp(buffer,"driver") == 0)
						{
						while ((token = sacar_token()) != cierra_parentesis)
							;

						continue;
						}

					}
				}
			else
				token = token;
			}
		}

	return salida;
}

*/
