// GuruZlib.h: interface for the GuruZlib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUZLIB_H__F25FD141_9FD5_11D5_B65C_006097D8B488__INCLUDED_)
#define AFX_GURUZLIB_H__F25FD141_9FD5_11D5_B65C_006097D8B488__INCLUDED_

#include "GuruCompat.h"

#include "zlib\zip.h"
#include "GuruUnZlib.h"
#include "GuruListaGenerica.h"
#include "GuruError.h"
#include "GuruArchivos.h"

#include <Windows.h>


#define				GURUZLIB_DIRECTORIO			48
#define				GURUZLIB_ARCHIVO			32

#define				TODOS						1
#define				NO_SUBSDIR					2

typedef struct
		{
		char			nombre[MAX_PATH];
		char			desde_zip[MAX_PATH];
		unz_file_info	info_archivo;

		bool			respetar_directorio_origen;
		char *			comentario; // un comentario para el archivo a grabar
		}archivo_a_guardar;

typedef bool (grabar_archivo) (char *ruta_completa);

class GuruZlib  
{
public:
	GuruZlib();
	virtual ~GuruZlib();

	bool	cerrar();

	void	ls_agregar_archivo(const char *archivo, const char *archivo_zip = NULL, bool respetar_directorio = true, char *comentario = NULL);
	bool	ls_autoincrementar(bool estado);
	bool	ls_incrementar();
	bool	ls_grabar(const char *archivo_zip);
	void	ls_limpiar_lista();
	dword	ls_lista_desde_archivo_zip(const char *archivo_zip);
	dword	ls_lista_desde_directorio(const char *directorio, dword opciones = TODOS);
	char *	ls_nombre();
	char *	ls_desde_zip();

	void	notificacion_graba_archivo (grabar_archivo * notificacion_usuario);

	unz_file_info ls_info_archivo();

	void	crear_un_zip(const char *archivo_nuevo, const char *archivo_zip);

	void	cargar_comentario_gral(char *nuevo_comentario_gral);
	dword	cargar_archivo(const char *archivo_nuevo, char **buffer);
	void	arreglar_nombre(char *nombre_interno);

private: // metodos

	static	void cargando_archivos_subsdir (char *ruta_completa, WIN32_FIND_DATA *data, void *extra_parametro);

	bool	crear(const char *archivo_zip);	
	bool	grabar_a_zip_desde_archivo(const char *archivo, char *comentario = NULL, bool respetar_directorio_origen = true);
	bool	grabar_a_zip_desde_zip(const char * archivo, const char *archivo_zip, bool respetar_directorio = false, char *comentario = NULL);
	void	cargar_info_archivo(const char *archivo_nuevo,zip_fileinfo *info_archivo);

private: // atributos

	zipFile				handle_archivo;
	GuruError			GuruZlib_Error;
	grabar_archivo *	notifi_graba_archivo;

	char *				comentario_gral; // el mismo comentario se pondra en todos los archivos a zipear

	GuruListaGenerica <archivo_a_guardar> contenido;
};

#endif // !defined(AFX_GURUZLIB_H__F25FD141_9FD5_11D5_B65C_006097D8B488__INCLUDED_)
