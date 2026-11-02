// GuruUnZlib.cpp: implementation of the GuruUnZlib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuruUnZlib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruUnZlib::GuruUnZlib()
{
	handle_archivo = NULL;
	strcpy(nombre_zip,"");

	funcion_usuario_cargando	= NULL;
	extra_parametro				= NULL;
	agregar_a_lista				= true;
}

GuruUnZlib::~GuruUnZlib()
{
	if (handle_archivo)
		cerrar();

	ls_borrar_lista();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar en nuevo notificador de la carga de archivos
// Parametro:	Un puntero a funcion, la cual sera la que reciba
//				las notificaciones
//////////////////////////////////////////////////////////////////////
void GuruUnZlib::cargar_notificacion_cargando(listando_contenido *nuevo_notificador, void *nuevo_extra_parametro)
{
	funcion_usuario_cargando = nuevo_notificador;

	if (nuevo_extra_parametro)
		extra_parametro = nuevo_extra_parametro;
}

void GuruUnZlib::crear_lista(bool crear)
{
	agregar_a_lista = crear;
}

//////////////////////////////////////////////////////////////////////
// Tarea: abrir un archivo zip
// Parametro: nombre del archivo
// Retorna: TRUE si la apertura fue exitosa, FALSE en caso contrario
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::abrir(const char *archivo_zip)
{
	if (handle_archivo) cerrar();

	handle_archivo = unzOpen(archivo_zip);

	if (handle_archivo != NULL)
		{
		strcpy(nombre_zip,archivo_zip);
		return true;
		}
	else
		{
		int num = 0;
		GuruUnZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num));
		return false;
		}
}

//////////////////////////////////////////////////////////////////////
// Tarea: Cerrar el handle del archivo zip
// Retorna: Si se pudo realizar o no.
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::cerrar()
{
	if (unzClose(handle_archivo) == UNZ_OK)
		{
		strcpy(nombre_zip,"");
		handle_archivo = NULL;
		return true;
		}
	else
		{
		int num_error = 0;
		GuruUnZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));
		return false;
		}
}

//////////////////////////////////////////////////////////////////////
// Tarea: Extraer un archivo de un zip a un directorio en especial
// Parametros:
//				archivo_en_zip: Nombre en el archivo
//				directorio:		Directorio al que hay que dejarlo
//				archivo_zip:	Nombre del archivo zip desde sale archivo_en_zip
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::extraer(const char *archivo_en_zip, const char *directorio, const char *archivo_zip)
{
	bool	salida = true;
//	int		num_error=0;

	if (abrir(archivo_zip))
		{
		if (unzLocateFile (handle_archivo, archivo_en_zip, 2) == UNZ_OK)
			{
			if (unzOpenCurrentFile(handle_archivo) == UNZ_OK)
				{
				unz_file_info info;

				if (unzGetCurrentFileInfo(handle_archivo, &info, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK)
					{

					char *buffer = new char [info.uncompressed_size+1];

					if (buffer)
						{
						if (unzReadCurrentFile(handle_archivo, buffer, info.uncompressed_size) != UNZ_ERRNO)
							{
							if (escribir_archivo(archivo_en_zip, buffer, &info, directorio))
								salida = true;
							}
						else
							salida = false;

						delete buffer;
						}
					else // buffer
						GuruUnZlib_Error.descripcion();

					}
				else // unzGetCurrentFileInfo
					salida = false;
				}
			else // unzOpenCurrentFile
				salida = false;

			cerrar();
			}
		else // unzLocateFile
			salida = false;
		}
	else
		salida = false;

//	if (salida == false)
//		GuruUnZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));

	return salida;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Extraer de un archivo zip a memoria un archivo
// Parametros:
//				1) Nombre del archivo que se encuentra en el .zip
//				2) Nombre del archivo .zip
//				3) Buffer en donde dejar el archivo leido
//				4) Estructura que contiene la informacion acerca del archivo
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::extraer_a_memoria(const char *archivo_en_zip, const char *archivo_zip, char **buffer, unz_file_info *file_info)
{
	bool	salida		=	false;
//	int		num_error	=	0;
	bool	existente	=	true;

	if (!handle_archivo || !archivo_zip)
		{
		if (archivo_zip)
			strcpy(nombre_zip, archivo_zip);

		if (!abrir(archivo_zip))
			return false;
		else
			existente = false;
		}


	char nombre_original[MAX_PATH];

	strcpy(nombre_original,archivo_en_zip);

	cambio_separadores(nombre_original);

	if (unzLocateFile (handle_archivo, nombre_original, 2) == UNZ_OK)
		{
		if (unzOpenCurrentFile(handle_archivo) == UNZ_OK)
			{
			unz_file_info info;

			if (unzGetCurrentFileInfo(handle_archivo, &info, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK)
				{

				if (file_info)
					*file_info = info;

				*buffer = new char [info.uncompressed_size+1];

				if (buffer)
					{
					if (unzReadCurrentFile(handle_archivo, *buffer, info.uncompressed_size) != UNZ_ERRNO)
						salida = TRUE;
					else
						GuruUnZlib_Error.cargar_descripcion(ERROR_LEYENDO_DATA_DE_ARCHIVO_EN_ZIP);
					}
				else // buffer
					GuruUnZlib_Error.descripcion();
				}
			else // unzGetCurrentFileInfo
				GuruUnZlib_Error.cargar_descripcion(ERROR_OBTENIENDO_INFO_ARCHIVO_EN_ZIP);
			}
		else // unzOpenCurrentFile
			GuruUnZlib_Error.cargar_descripcion(ERROR_ABRIENDO_ARCHIVO_EN_ZIP);
		}
	else // unzLocateFile
		GuruUnZlib_Error.cargar_descripcion(ARCHIVO_NO_ENCONTRADO_EN_ZIP);

	if (!existente)
		cerrar();

	return salida;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Escribir en disco, el archivo cargado en memoria
// Parametros:
//				nombre: Nombre que tendra en disco.
//				buffer: buffer que contiene todo el archivo
//				info:   structura con la data del archivo a escribir
//				directorio: Directorio destino donde se guardara
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::escribir_archivo(const char *nombre, char *buffer, unz_file_info *info, const char *directorio)
{
	HANDLE		archivo;
	dword		escritos=0;
	FILETIME	ftm,ftLocal;
	char		ruta_completa[MAX_PATH];
	char		nombre_en_disco[MAX_PATH];
	bool		salida = false;

	strcpy(nombre_en_disco, nombre);

	limpiar_nombre(nombre_en_disco);

	strcpy(ruta_completa,directorio);

	if (ruta_completa[strlen(ruta_completa)-1] != '\\')
		strcat(ruta_completa,"\\");

	strcat(ruta_completa, nombre_en_disco);

	// pregunto si es un directorio

	if (ruta_completa[strlen(ruta_completa)-1] == '\\' ||
		ruta_completa[strlen(ruta_completa)-1] == '/')
		if (CreateDirectory(ruta_completa, NULL))
			return true;
		else
			{
			if (GetLastError() == ERROR_ALREADY_EXISTS)
				return true;
			else
				{
				GuruUnZlib_Error.descripcion();
				return false;
				}
			}

	archivo = CreateFile(ruta_completa,GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

	if (archivo != INVALID_HANDLE_VALUE)
		{
		if (WriteFile(archivo, buffer, info->uncompressed_size, &escritos, NULL) != 0)
			{
			DosDateTimeToFileTime((WORD)(info->dosDate>>16),(WORD)info->dosDate,&ftLocal);
			LocalFileTimeToFileTime(&ftLocal,&ftm);
			SetFileTime(archivo,&ftm,&ftm,&ftm);
			salida = true;
			}
		else
			GuruUnZlib_Error.descripcion();
		}
	else
		GuruUnZlib_Error.descripcion();


	CloseHandle(archivo);
	return salida;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar en una lista el contenido de un archivo zip
// Parametros:	Nombre del archivo zip
// Retorna:		Cantidad de items cargados en la lista
//////////////////////////////////////////////////////////////////////
dword GuruUnZlib::ls_lista_archivo_zip(const char *nombre_archivo_zip, bool solo_listar)
{
	dword			cant_archivos = 0;
	char			nombre_archivo_en_zip[MAX_PATH];
	info_en_zip		data;

	if (!abrir(nombre_archivo_zip))
		return 0;

	strcpy(nombre_zip, nombre_archivo_zip);

	if (unzGoToFirstFile(handle_archivo) != UNZ_OK)
		return 0;

	do {
		bool salida = true;

		unzGetCurrentFileInfo(handle_archivo, &data.info_archivo_en_zip, nombre_archivo_en_zip, MAX_PATH, NULL, 0, NULL, 0);

		strcpy(data.nombre_en_archivo_zip,nombre_archivo_en_zip);

//		unz_global_info pglobal_info;
//		unzGetGlobalInfo(handle_archivo, &pglobal_info);
//		unzGetGlobalComment(handle_archivo, nombre_archivo_en_zip, _MAX_PATH);

		// si existe la funcion, notifico de la carga del archivo
		if (funcion_usuario_cargando)
			salida = (*funcion_usuario_cargando)(&data, extra_parametro);

		if (salida && agregar_a_lista)
			if (contenido.agregar(&data))
				cant_archivos++;

	}while (unzGoToNextFile(handle_archivo) == UNZ_OK);

	if (solo_listar)
		cerrar();

	return cant_archivos;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Extrae desde la lista a un directorio
// Parametros:	Directorio donde dejar los archivo descompactados
// Nota:
//				La lista de debe cargar con "ls_lista_archivo_zip"
//////////////////////////////////////////////////////////////////////
dword GuruUnZlib::ls_extraer_a(char *directorio)
{
	dword			cant_extraidos=0;
	unz_file_info	info;

	if (contenido.EstaVacia()) return 0;

	dword cant_archivos = contenido.total_elementos();

	char *buffer;

	ls_autoincrementar(false);
	for (DWORD i = 0; i < cant_archivos;i++)
		{
		if (extraer_a_memoria(ls_nombre_archivo(), nombre_zip, &buffer, &info))
			if (escribir_archivo(ls_nombre_archivo(), buffer, &info, directorio))
				cant_extraidos++;

		ls_incrementar();
		delete buffer;
		}

	return cant_extraidos;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Verificar si existe la ruta completa, incluyendo el archivo
// Parametros:  Ruta completa a buscar
//
//				Nota:
//					Si se desea buscar el nombre de un archivo utilizar
//					el metodo ls_buscar(char *nombre_archivo)
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::ls_existe(char *nombre_archivo)
{
	bool	existe = false;

	contenido.autoincrementar(false);

	dword elemento_actual = contenido.posicion_actual();

	contenido.ir_a(0);

	while (!contenido.fin() && !existe)
		{
		if (contenido.item()->info_archivo_en_zip.uncompressed_size != 0)
			if (stricmp(contenido.item()->nombre_en_archivo_zip, nombre_archivo) == 0)
				{
				existe = true;
				break;
				}

		contenido++;
		}

	contenido.ir_a(elemento_actual);

	return existe;
}


//////////////////////////////////////////////////////////////////////
// Tarea:		Buscar un archivo en toda la lista cargada
// Parametros:	El nombre del archivo a buscar
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::ls_buscar(char *nombre_archivo, char *crc, dword tamanio)
{
//	bool	existe = false;
	int		coincidencias = 0;
	int		total_coincidencias = 0;

	contenido.ir_a(0);

	contenido.autoincrementar(false);

	if (nombre_archivo) total_coincidencias++;
	if (crc)			total_coincidencias++;
	if (tamanio)		total_coincidencias++;

	while (!contenido.fin())
		{

//////////////////////////////////////////////////////////////////////
// Por que no ...
/*
		dword largo_buscado = strlen(nombre_archivo);
		dword largo_actual = strlen(contenido.item()->nombre_en_archivo_zip);

		if (largo_actual > largo_buscado)
			if (stricmp(nombre_archivo, &contenido.item()->nombre_en_archivo_zip[largo_actual - largo_buscado]) == 0)
				return true;
*/
//////////////////////////////////////////////////////////////////////

		if (nombre_archivo)
			{
			char nombre_solo[_MAX_PATH];

			strcpy(nombre_solo, contenido.item()->nombre_en_archivo_zip);
			limpiar_nombre(nombre_solo);

			if (stricmp(nombre_solo, nombre_archivo) == 0)
				coincidencias++;
			}

		if (crc)
			{
			char temp_crc[10];

			sprintf(temp_crc,"%08x", contenido.item()->info_archivo_en_zip.crc);
			if (stricmp(temp_crc, crc) == 0)
				coincidencias++;
			}

		if (tamanio)
			{
			if (contenido.item()->info_archivo_en_zip.uncompressed_size == tamanio)
				coincidencias++;
			}

		if (coincidencias == total_coincidencias) return true;

		coincidencias = 0;
		contenido++;
		}

	return false;
}

bool GuruUnZlib::ls_buscar_proximo(char *nombre_archivo)
{
	contenido.autoincrementar(false);

	contenido++;

	while (!contenido.fin())
		{
		char nombre_solo[_MAX_PATH];

		strcpy(nombre_solo, contenido.item()->nombre_en_archivo_zip);
		limpiar_nombre(nombre_solo);

		if (stricmp(nombre_solo, nombre_archivo) == 0)
			return true;

		contenido++;
		}

	return false;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Permitir que se autoincremente por cada acceso a un item de la lista
// Parametros:
//				1) TRUE  = Se imcremente en forma automatica
//				2) FALSE = No se imcrementa automaticamente
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::ls_autoincrementar(bool estado)
{
	return contenido.autoincrementar(estado);
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Incrementar en 1, la posicion actual de la lista
// Parametros:	Ninguno
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::ls_incrementar()
{
	return contenido++;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Se posiciona al inicio de la lista y opcionalmente 
//              incrementa una cantidad de items de la lista
// Parametros:	Ninguno
//////////////////////////////////////////////////////////////////////
bool GuruUnZlib::ls_al_inicio(dword incremento)
{
	return contenido.al_inicio(incremento);
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Retornar el campo nombre del item actual de la lista
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
char * GuruUnZlib::ls_nombre_archivo()
{
	return contenido.item()->nombre_en_archivo_zip;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Retornar el registro con los datos del archivo actual de la lista
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
info_en_zip * GuruUnZlib::ls_registro()
{
	return contenido.item();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Borrar la lista que contiene la informacion del .zip
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
void GuruUnZlib::ls_borrar_lista()
{
	contenido.vaciar_lista();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Borrar el archivo actual de la lista.
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
void GuruUnZlib::ls_borrar_registro()
{
	contenido.borrar();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cambiar el separador de directorio de '/' a '\'
// Parametros:	Ruta a cambiar
//////////////////////////////////////////////////////////////////////
char * GuruUnZlib::cambio_separadores(char *nombre)
{
	char	* salida;
	char	* temp = nombre;

	while ((salida = strchr(nombre,'\\')) != NULL)
		*salida = '/';

	nombre = temp;

	return nombre;
}


char *GuruUnZlib::limpiar_nombre(char *nombre)
{
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   _splitpath( nombre, NULL, NULL, fname, ext );
   _makepath( nombre, NULL, NULL, fname, ext );

   return nombre;
}
