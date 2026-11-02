#if !defined __GURUDBLIBRAY_H__
#define __GURUDBLIBRAY_H__

//////////////////////////////////////////////////////////////////////
// Linkear con NTWDBLIB.LIB
//////////////////////////////////////////////////////////////////////

#include "GuruCompat.h"

#define		DBNTWIN32
//#define		_WINSCARD_H_ // esto es para compatibilidad con Visual Studio 2005 Express y SQL

#include <String.h>
#include <Windows.h>
#include <stdio.h>
#include "GuruTiposDeDatos.h"
#include "SQLFRONT.H"
#include "sqldb.h"

#ifdef GURU_EXPORTS
	#define GURU_API __declspec(dllexport)
#else
	#define GURU_API //__declspec(dllimport)
#endif

typedef struct
		{
		int				nro_mensaje;
		int				estado_mensaje;
		int				severo;
		char			texto_mensaje[256];
		char			nombre_server[25];
		char			nombre_proceso[256];
		DBUSMALLINT		linea;
		}mensaje_sql;

typedef struct
		{
		int		severo;
		int		error_db;
		int		error_so;
		char	desc_db[256];
		char	desc_so[256];
		}error_sql;


class GURU_API GuruDBLibrary
{
public:
	GuruDBLibrary();
	virtual ~GuruDBLibrary();

	bool	login(char *usuario, char *clave, char *servidor, char *base = NULL);
	bool	cerrar_conexion();

	bool	enviarex(char *sentencia, ...);
	bool	enviar(char *sentencia);

	bool	es_null(int numero_columna);
	bool	fin();

	byte *  operator [] (dword numero);

	int		numero_error();
	char *	mensaje_error();



protected:
	char *	inicializar(char *version);

	void	unir_columnas();

	static int manejador_error (PDBPROCESS ptr_proceso, int severo, int error_db, int error_so, const char * desc_db,const char * desc_so);
	static int manejador_mensaje (PDBPROCESS ptr_proceso,int nro_mensaje, int estado_mensaje, int severo, char *texto_mensaje, char * nombre_server, char *nombre_proceso, DBUSMALLINT linea);

protected:

	static GuruDBLibrary *Ptr_This;

	error_sql		db_error;
	mensaje_sql		db_mensaje;
	PDBPROCESS		proceso;

	char			version[30];
	unsigned char **			campos;

};

#endif // !defined(AFX_GURUDBLIBRAY_H__6C9B95F6_C67F_4E37_8AC0_7043933B4173__INCLUDED_)
