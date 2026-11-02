#include "GuruDBLibrary.h"

GuruDBLibrary *GuruDBLibrary::Ptr_This = NULL;

GuruDBLibrary::GuruDBLibrary()
{
	dberrhandle ((DBERRHANDLE_PROC) manejador_error);
	dbmsghandle ((DBMSGHANDLE_PROC) manejador_mensaje);

	Ptr_This	= this;
	proceso		= NULL;
	campos		= NULL;
}

GuruDBLibrary::~GuruDBLibrary()
{
	if (proceso)
		cerrar_conexion();

	if (campos)
		delete campos;
}

char * GuruDBLibrary::inicializar(char *version)
{
	if (version)
		return strcpy(version, dbinit());
	else
		dbinit();

	return NULL;
}

bool GuruDBLibrary::login(char *usuario, char *clave, char *servidor, char *base)
{
	PLOGINREC ptr_login;

	if (!inicializar(version))
		return false;

	ptr_login = dblogin(); // reservo memoria para el login

	if (!ptr_login)
		return false;

	// lleno la estructua del login
	DBSETLUSER (ptr_login,usuario);
	DBSETLPWD (ptr_login,clave);

	proceso = dbopen(ptr_login, servidor);

	// pude abrir la base ?
	if (!proceso)
		return false;

	dbfreelogin(ptr_login);

	if (base)
		return (dbuse(proceso, base) == SUCCEED) ? true : false;

	return true;
}

bool GuruDBLibrary::enviar(char *sentencia)
{
	if (!proceso) return false;

	if (dbcmd(proceso,sentencia) == SUCCEED)
		{
		if (dbsqlexec(proceso) == SUCCEED)
			{
			if (dbresults(proceso) == SUCCEED)
				{
				unir_columnas();
				return true;
				}
			else
				return false;
			}
		else
			return false;
		}
	else
		return false;
}

bool GuruDBLibrary::enviarex(char *sentencia, ...)
{
	char aux_sentencia[1024];
	va_list parametros;
	
	va_start (parametros,sentencia);
	vsprintf(aux_sentencia, sentencia, parametros);
	va_end(parametros);

	return enviar(aux_sentencia);
}

void GuruDBLibrary::unir_columnas()
{
		if (campos) delete [] campos;

// reservo memoria para almacenar los punteros a las cadenas
		int	cant_campos = dbnumcols(proceso);
		campos = new unsigned char * [cant_campos];

		for (int i=1;i<=cant_campos;i++)
			{
			// reservo memoria para cada campo en el array
			campos[i-1] = new unsigned char [dbcollen(proceso,i) + 1];

			// me fijo que tipo de dato es y uno/ligo los campos a cada variable
			int tipo_dato = dbcoltype (proceso, i);
			switch (tipo_dato)
				{
			case SQLCHAR: 
						dbbind (proceso, i, NTBSTRINGBIND, 0, (unsigned char*)campos[i-1]);
						break;

			case SQLINT1: // byte
						dbbind (proceso, i, TINYBIND, 0, (unsigned char *)campos[i-1]);						
						break;

			case SQLINT2: // int
						dbbind (proceso, i, SMALLBIND, 0, (unsigned char *)campos[i-1]);
						break;

			case SQLINT4: // long 
						dbbind (proceso, i, INTBIND, 0, (unsigned char *)campos[i-1]);
						break;

			default:
					dbbind (proceso, i, NTBSTRINGBIND, 0, (unsigned char*)campos[i-1]);
				}
			}
}

byte * GuruDBLibrary::operator [] (dword numero)
{
	return campos[numero-1];
}

bool GuruDBLibrary::es_null(int numero_columna)
{
	return (dbdatlen(proceso, numero_columna) == 0) ? true : false;
}

bool GuruDBLibrary::fin()
{
	return (dbnextrow(proceso) == NO_MORE_ROWS) ? true : false;
}

bool GuruDBLibrary::cerrar_conexion()
{
	bool salida = (dbclose(proceso) == SUCCEED) ? true : false;

	proceso = NULL;

	return salida;
} 

int GuruDBLibrary::numero_error()
{
	return db_error.error_db;
}

char *GuruDBLibrary::mensaje_error()
{
	return db_mensaje.texto_mensaje;
}

int GuruDBLibrary::manejador_error (PDBPROCESS ptr_proceso, int severo, int error_db, int error_so, const char * desc_db,const char * desc_so)
{
	Ptr_This->db_error.severo = severo;
	Ptr_This->db_error.error_db = error_db;
	Ptr_This->db_error.error_so = error_so;

	if (desc_db) 
		strcpy(Ptr_This->db_error.desc_db,desc_db); 
	else 
		strcpy(Ptr_This->db_error.desc_db,"");

	if (desc_so) 
		strcpy(Ptr_This->db_error.desc_so,desc_so); 
	else 
		strcpy(Ptr_This->db_error.desc_so,"");

	return INT_CANCEL;
}

int GuruDBLibrary::manejador_mensaje (PDBPROCESS ptr_proceso,int nro_mensaje, int estado_mensaje, int severo, char *texto_mensaje, char * nombre_server, char *nombre_proceso, DBUSMALLINT linea)
{
	Ptr_This->db_mensaje.nro_mensaje = nro_mensaje;
	Ptr_This->db_mensaje.estado_mensaje = estado_mensaje;
	Ptr_This->db_mensaje.severo = severo;

	if (texto_mensaje)  
		strcpy(Ptr_This->db_mensaje.texto_mensaje,texto_mensaje); 
	else 
		strcpy(Ptr_This->db_mensaje.texto_mensaje,"");

	if (nombre_server)  
		strcpy(Ptr_This->db_mensaje.nombre_server,nombre_server); 
	else 
		strcpy(Ptr_This->db_mensaje.nombre_server,"");

	if (nombre_proceso) 
		strcpy(Ptr_This->db_mensaje.nombre_proceso,nombre_proceso); 
	else 
		strcpy(Ptr_This->db_mensaje.nombre_proceso,"");

	Ptr_This->db_mensaje.linea = linea;

	return 0;
}
