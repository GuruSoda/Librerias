// GuruUnZlib.h: interface for the GuruUnZlib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUUNZLIB_H__F25FD144_9FD5_11D5_B65C_006097D8B488__INCLUDED_)
#define AFX_GURUUNZLIB_H__F25FD144_9FD5_11D5_B65C_006097D8B488__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <unzip.h>
#include <GuruListaGenerica.h>
#include <GuruError.h>
#include <GuruTiposDeDatos.h>

// #include <Windows.h>

//////////////////////////////////////////////////////////////////////
// Mensajes de error
// gzerror da error al extraer el mensaje
//////////////////////////////////////////////////////////////////////

#define			GURUUNZLIB_DIRECTORIO					48
#define			GURUUNZLIB_ARCHIVO						32

#define			ARCHIVO_NO_ENCONTRADO_EN_ZIP			"Archivo no encontrado dentro del .zip"
#define			ERROR_ABRIENDO_ARCHIVO_EN_ZIP			"No se pudo abrir el archivo dentro del .zip"
#define			ERROR_OBTENIENDO_INFO_ARCHIVO_EN_ZIP	"No se ha podido obtener la informacion del archivo en el .zip"
#define			ERROR_LEYENDO_DATA_DE_ARCHIVO_EN_ZIP	"Error en la lectura del archivo en el .zip"

typedef	struct
		{
		unz_file_info	info_archivo_en_zip;
		char			nombre_en_archivo_zip[MAX_PATH];
		}info_en_zip;

//////////////////////////////////////////////////////////////////////
// Prototipo para los callbacks
//////////////////////////////////////////////////////////////////////
typedef bool (listando_contenido)(info_en_zip *, void *extra_parametro = NULL);

class GuruUnZlib  
{
public:
	GuruUnZlib();
	virtual ~GuruUnZlib();

	bool	extraer(const char *archivo_en_zip = NULL, const char *directorio = NULL, const char *archivo_zip = NULL);
	bool	extraer_a_memoria(const char *archivo_en_zip, const char *archivo_zip, char **buffer, unz_file_info *file_info = NULL);
	bool	escribir_archivo(const char *nombre, char *buffer, unz_file_info *info, const char *directorio);

//////////////////////////////////////////////////////////////////////
// Metodos para los callbacks
//////////////////////////////////////////////////////////////////////
	void	cargar_notificacion_cargando(listando_contenido *nuevo_notificador, void *extra_parametro = NULL);

//////////////////////////////////////////////////////////////////////
// Metodos para listar archivos de un archivo .zip
//////////////////////////////////////////////////////////////////////
public:
	void	crear_lista(bool crear = true);
	dword	ls_lista_archivo_zip(const char *nombre_zip, bool solo_listar = true);
	dword	ls_extraer_a(char *directorio);
	bool	ls_autoincrementar(bool estado);
	bool	ls_incrementar();
	bool	ls_al_inicio(dword incremento = 0);
	void	ls_borrar_lista();
	void    ls_borrar_registro();
	bool	ls_existe(char *nombre_archivo);
	bool	ls_buscar(char *nombre_archivo = NULL, char *crc = NULL, dword tamanio = 0);
	bool	ls_buscar_proximo(char *nombre_archivo);


//////////////////////////////////////////////////////////////////////
// Metodos para la extraccion de datos de la lista
//////////////////////////////////////////////////////////////////////
	info_en_zip *	ls_registro();
	char *			ls_nombre_archivo();

	char *			limpiar_nombre(char *nombre);

private:

	bool	abrir(const char *archivo_zip);
	bool	cerrar();

	char *	cambio_separadores(char *nombre);

private:

	unzFile			handle_archivo;
	char			nombre_zip[MAX_PATH];

	GuruError		GuruUnZlib_Error;

	GuruListaGenerica <info_en_zip>		contenido;
//////////////////////////////////////////////////////////////////////
// Funciones de notificacion de usuario
//////////////////////////////////////////////////////////////////////
	listando_contenido *		funcion_usuario_cargando;
	bool						agregar_a_lista;
	void *						extra_parametro;
};

#endif // !defined(AFX_GURUUNZLIB_H__F25FD144_9FD5_11D5_B65C_006097D8B488__INCLUDED_)
