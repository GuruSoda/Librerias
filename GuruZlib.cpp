// GuruZlib.cpp: implementation of the GuruZlib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuruZlib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruZlib::GuruZlib()
{
	notifi_graba_archivo	= NULL;

	comentario_gral			= NULL;
}

GuruZlib::~GuruZlib()
{
	if (handle_archivo == NULL)
		cerrar();

	if (comentario_gral)
		delete comentario_gral;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		crear un nuevo archivo .zip
// Parametros:	Nombre del archivo a crear
// Nota:
//				1) Si el archivo existe lo sobreescribe.
//				2) Usado internamente
//////////////////////////////////////////////////////////////////////
bool GuruZlib::crear(const char *archivo_zip)
{
// 0 == crea el archivo
// 1 == agrega al final, creo que solo es para SFX

	handle_archivo = zipOpen(archivo_zip, 0);

	if (handle_archivo != NULL)
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Agregar un archivo a la lista de archivos
// Parametros:	Nombre del archivo que se va a agregar
//////////////////////////////////////////////////////////////////////
void GuruZlib::ls_agregar_archivo(const char *archivo, const char *archivo_zip, bool respetar_directorio, char *comentario)
{
	archivo_a_guardar nodo_nuevo;

	memset(&nodo_nuevo, 0, sizeof(archivo_a_guardar));

	if (archivo)
		{
		strcpy(nodo_nuevo.nombre,archivo);

		if (archivo_zip)
			strcpy(nodo_nuevo.desde_zip,archivo_zip);
		else
			nodo_nuevo.desde_zip[0] = '\0';

		nodo_nuevo.respetar_directorio_origen = respetar_directorio;

		memset(&nodo_nuevo.info_archivo, 0, sizeof(unz_file_info));

		nodo_nuevo.comentario = comentario;

		contenido.agregar(&nodo_nuevo);
		}
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Grabar la lista de archivo a un archivo .zip
// Parametros:	Nombre del archivo zip a crear.
//////////////////////////////////////////////////////////////////////
bool GuruZlib::ls_grabar(const char *archivo_zip)
{
	bool	salida=true;
	int		num_error;

	if (contenido.EstaVacia()) return false;

	if (crear(archivo_zip))
		{
		dword cant_total = contenido.total_elementos();

		contenido.autoincrementar(false);

		contenido.ir_a(0);

		for (dword i = 0; i < cant_total; i++)
			{
			if (stricmp(ls_desde_zip(),"") == 0) // grabo desde el disco
				{
				grabar_a_zip_desde_archivo(ls_nombre(), contenido.item()->comentario, contenido.item()->respetar_directorio_origen);
				}
			else // grabo desde otro archivo zip
				{
				grabar_a_zip_desde_zip(ls_nombre(),ls_desde_zip(), contenido.item()->respetar_directorio_origen);
				}

			if (notifi_graba_archivo)
				(*notifi_graba_archivo) (ls_nombre());

			// contenido.borrar();
			ls_incrementar();
			}

		cerrar();
		}
	else
		GuruZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));

	return salida;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Graba en un zip abierto, un archivo desde disco
// Parametros:	Nombre del archivo a tomar para grabar
// Nota:
//				1) Metodo usado internamente por la clase
//////////////////////////////////////////////////////////////////////
bool GuruZlib::grabar_a_zip_desde_archivo(const char *archivo, char *comentario, bool respetar_directorio)
{
	GuruUnZlib		unzip;
	zip_fileinfo	info_archivo;
	char			nombre_interno[MAX_PATH];
	char *			buffer;
	bool			salida = false;
	int				num_error;

	memset(&info_archivo,0,sizeof(zip_fileinfo));

	cargar_info_archivo(archivo,&info_archivo);

	strcpy(nombre_interno, archivo);

	arreglar_nombre(nombre_interno);

	dword cant_bytes = cargar_archivo(archivo, &buffer);

	if (cant_bytes)
		{
		if (!respetar_directorio)
			unzip.limpiar_nombre(nombre_interno);

		if (zipOpenNewFileInZip(handle_archivo, nombre_interno, &info_archivo, NULL, 0, NULL, 0, (comentario != NULL) ? comentario : comentario_gral, Z_DEFLATED, Z_DEFAULT_COMPRESSION) == ZIP_OK)
			{
			if (zipWriteInFileInZip(handle_archivo, buffer, cant_bytes) == ZIP_OK)
				{
				zipCloseFileInZip(handle_archivo);
				salida = true;
				}
			else
				GuruZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));
			}
		else
			GuruZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));

		delete buffer;
		}

	return salida;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Grabar en un zip un archivo que se encuentra en otro zip
// Parametro:	Nombre del archivo que se encuentra en el zip.
//				Nombre del archivo zip que lo contiene.
// Nota:
//				1) Metodo utilizado internamente
//////////////////////////////////////////////////////////////////////
bool GuruZlib::grabar_a_zip_desde_zip(const char * archivo, const char *archivo_zip, bool respetar_directorio, char *comentario)
{
	bool			salida = false;
	GuruUnZlib		unzip;
	char	*		buffer;
	unz_file_info	info_archivo;
	zip_fileinfo	zip_info;
	int				num_error;
	char			nombre_archivo[_MAX_PATH];

	if (unzip.extraer_a_memoria(archivo, archivo_zip, &buffer, &info_archivo))
		{
		zip_info.dosDate = info_archivo.dosDate;
		zip_info.external_fa = info_archivo.external_fa;
		zip_info.internal_fa = info_archivo.internal_fa;

		strcpy(nombre_archivo, archivo);

		if (!respetar_directorio)
			unzip.limpiar_nombre(nombre_archivo);

		if (zipOpenNewFileInZip(handle_archivo, nombre_archivo, &zip_info, NULL, 0, NULL, 0, (comentario == NULL) ? comentario_gral : comentario, Z_DEFLATED, Z_DEFAULT_COMPRESSION) == ZIP_OK)
			{
			if (zipWriteInFileInZip(handle_archivo, buffer, info_archivo.uncompressed_size) == ZIP_OK)
				{
				zipCloseFileInZip(handle_archivo);
				salida = true;
				}
			else
				GuruZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));
			}
		else
			GuruZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));

		delete buffer;
		}

	return salida;
}

/////////////////////////////////////////////////////////////////////
// Tarea:	Limpiar la lista de archivos que se usa para
//			despues generar un archivo .zip
// Parametros:
//////////////////////////////////////////////////////////////////////
void GuruZlib::ls_limpiar_lista()
{
	// libero la memoria del campo comentario
	dword cant = contenido.total_elementos();

	bool anterior = contenido.autoincrementar(true);
	char *ptr_comentario;

	for (dword i = 0; i < cant; i++)
		{
		if ((ptr_comentario = contenido.item()->comentario) != NULL)
			delete ptr_comentario;
		}

	contenido.autoincrementar(anterior);

	contenido.vaciar_lista();
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Llenar la lista de archivos a grabar desde otro .zip
// Parametros:	El nombre del archivo .zip a sacar los archivos
//////////////////////////////////////////////////////////////////////
dword GuruZlib::ls_lista_desde_archivo_zip(const char *archivo_zip)
{
	GuruUnZlib unzip;

	dword cantidad = unzip.ls_lista_archivo_zip(archivo_zip);

	if (!cantidad) return 0;

	for (dword i=0; i < cantidad; i++)
		{
		ls_agregar_archivo(unzip.ls_nombre_archivo(), archivo_zip);
		}

	return cantidad;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar la lista de archivos de uns
// Parametros:	El nombre del archivo .zip a sacar los archivos
//////////////////////////////////////////////////////////////////////
dword GuruZlib::ls_lista_desde_directorio(const char *directorio, dword opciones)
{
	char			dir_busqueda[MAX_PATH];
	GuruArchivos	ga;

	strcpy(dir_busqueda,directorio);

	ga.crear_lista(false);

	ga.notificacion_carga((carga_archivos*)cargando_archivos_subsdir, this);

	ga.ls_subsdirs(dir_busqueda);

	return contenido.total_elementos();
}

void GuruZlib::cargando_archivos_subsdir (char *ruta_completa, WIN32_FIND_DATA *data, void *extra_parametro)
{
	GuruZlib  *esto = (GuruZlib*) extra_parametro;

	esto->ls_agregar_archivo(ruta_completa);
}

void GuruZlib::notificacion_graba_archivo (grabar_archivo *	notificacion_usuario)
{
	notifi_graba_archivo = notificacion_usuario;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Permitir que se autoincremente por cada acceso a un item de la lista
// Parametros:
//				1) TRUE  = Se imcremente en forma automatica
//				2) FALSE = No se imcrementa automaticamente
//////////////////////////////////////////////////////////////////////
bool GuruZlib::ls_autoincrementar(bool estado)
{
	return contenido.autoincrementar(estado);
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Incrementar en 1, la posicion actual de la lista
// Parametros:	Ninguno
//////////////////////////////////////////////////////////////////////
bool GuruZlib::ls_incrementar()
{
	return contenido++;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Retornar el campo nombre del item actual de la lista
// Parametros:	Ninguno.
//////////////////////////////////////////////////////////////////////
char * GuruZlib::ls_nombre()
{
	return contenido.item()->nombre;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Retornar el campo desde_zip del item actual de la lista
// Parametros:	Ninguno
//////////////////////////////////////////////////////////////////////
char * GuruZlib::ls_desde_zip()
{
	return contenido.item()->desde_zip;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Retornar la estructura unz_file_info del item actual de la lista
// Parametros:	Ninguno
//////////////////////////////////////////////////////////////////////
unz_file_info GuruZlib::ls_info_archivo()
{
	return contenido.item()->info_archivo;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Crea un archivo zip con un solo archivo en su interior
// Parametros:	Nombre del archivo a poner en el zip, y el nombre del zip
//////////////////////////////////////////////////////////////////////
void GuruZlib::crear_un_zip(const char *archivo_nuevo, const char *archivo_zip)
{
	char *	buffer;
	int		num_error = 0;

	if (archivo_nuevo)
		{
		if (crear(archivo_zip))
			{
			zip_fileinfo info_archivo;

			memset(&info_archivo, 0, sizeof(zip_fileinfo));

			cargar_info_archivo(archivo_nuevo, &info_archivo);

			char nombre_interno[MAX_PATH];

			strcpy(nombre_interno, archivo_nuevo);

			arreglar_nombre(nombre_interno);

			DWORD cant_bytes = cargar_archivo(archivo_nuevo, &buffer);

			if (zipOpenNewFileInZip(handle_archivo, nombre_interno, &info_archivo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) == ZIP_OK)
				{
				if (zipWriteInFileInZip(handle_archivo, buffer, cant_bytes) == ZIP_OK)
					{
					zipCloseFileInZip(handle_archivo);
					}
				else
					GuruZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));
				}
			else
				GuruZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));

			cerrar();

			}
		else
			GuruZlib_Error.cargar_descripcion(gzerror(handle_archivo, &num_error));
		}

	delete buffer;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Modificar el nombre del archivo para ingresarlo a un .zip
// Parametros:	El nombre del archivo a arreglar
//////////////////////////////////////////////////////////////////////
void GuruZlib::arreglar_nombre(char *nombre_interno)
{
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( nombre_interno, drive, dir, fname, ext);

	while (dir[0] == '\\')
		strcpy(dir,&dir[1]);

	strcpy(path_buffer, dir);
	strcat(path_buffer, fname);
	strcat(path_buffer,ext);

	strcpy(nombre_interno,path_buffer);
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar la informacion del archivo a grabar en el .zip
// Parametros:	El archivo a cargar en el zip y la estructura
// Nota:
//				1) Metodo usado intenamente por la clase
//////////////////////////////////////////////////////////////////////
void GuruZlib::cargar_info_archivo(const char *archivo_nuevo,zip_fileinfo *info_archivo)
{
	HANDLE			hFind;
	WIN32_FIND_DATA	data_archivo;
	FILETIME		ftLocal;

	hFind = FindFirstFile(archivo_nuevo,&data_archivo);

	if (hFind != INVALID_HANDLE_VALUE)
		{
		FileTimeToLocalFileTime(&(data_archivo.ftLastWriteTime),&ftLocal);
		FileTimeToDosDateTime(&ftLocal,((WORD *)&info_archivo->dosDate)+1,((WORD *)&info_archivo->dosDate)+0);
		FindClose(hFind);

//////////////////////////////////////////////////////////////////////
//	si existe dosDate cargada no interesa el tmz_date
/*
		SYSTEMTIME st;

		if (FileTimeToSystemTime(&data_archivo.ftLastWriteTime, &st))
			{
			info_archivo->tmz_date.tm_hour	= st.wHour;
			info_archivo->tmz_date.tm_mday	= st.wDay;
			info_archivo->tmz_date.tm_min	= st.wMinute;
			info_archivo->tmz_date.tm_mon	= st.wMonth;
			info_archivo->tmz_date.tm_sec	= st.wSecond;
			info_archivo->tmz_date.tm_year	= st.wYear;
			}
*/
//////////////////////////////////////////////////////////////////////
		}
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cargar el contenido del archivo a memoria
// Parametros:	Nombre del archivo y un puntero en donde dejarlo
// Nota:
//				1) El archivo es cargado completamente a memoria
//				2) El metodo se encarga de pedir memoria, pero no la libera
//////////////////////////////////////////////////////////////////////
dword GuruZlib::cargar_archivo(const char *archivo_nuevo, char **buffer)
{
	HANDLE		archivo;
	dword		leidos=0;

	archivo = CreateFile(archivo_nuevo, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (archivo != INVALID_HANDLE_VALUE)
		{
		dword	tamanio = GetFileSize(archivo, NULL);

		*buffer = new char [tamanio+1];

		if (ReadFile(archivo, *buffer, tamanio, &leidos, NULL))
			{
			CloseHandle(archivo);
			return leidos;
			}
		else
			{
			GuruZlib_Error.descripcion();
			return 0;
			}
		}
	else
		{
		GuruZlib_Error.descripcion();
		return 0;
		}

	return 0;
}

void GuruZlib::cargar_comentario_gral(char *nuevo_comentario_gral)
{
	if (nuevo_comentario_gral)
		{
		if (comentario_gral) delete comentario_gral;

		comentario_gral = new char [strlen (nuevo_comentario_gral) + 1];

		strcpy(comentario_gral, nuevo_comentario_gral);
		}
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Cerrar el handle del archivo zip
// Parametros:	Ninguno
//////////////////////////////////////////////////////////////////////
bool GuruZlib::cerrar()
{
	if (zipClose(handle_archivo, (comentario_gral) ? comentario_gral : NULL) == ZIP_OK)
		{
		handle_archivo = NULL;
		return true;
		}
	else
		return false;
}
