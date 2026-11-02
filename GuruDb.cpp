// GuruDb.cpp: implementation of the GuruDb class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GuruDb.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


GuruDb *GuruDb::ptr_mio = NULL;

GuruDb::GuruDb()
{
	memset(&db_error, 0, sizeof(error_sql));
	memset(&db_mensaje, 0, sizeof(mensaje_sql));

	dberrhandle ((DBERRHANDLE_PROC) manejador_error);
	dbmsghandle ((DBMSGHANDLE_PROC) manejador_mensaje);

	ptr_mio = this;
	proceso = NULL;
	campos = NULL;
	auto_campos = FALSE;
}

GuruDb::~GuruDb()
{
	if (proceso)
		cerrar_conexion();

	if (campos)
		delete [] campos;
}

char * GuruDb::Inicializar(char *version)
{
	if (version)
		return strcpy(version, dbinit());
	else
		dbinit();

	return NULL;
}

bool GuruDb::login(char *usuario, char *clave, char *db)
{
	PLOGINREC ptr_login;

	if (!Inicializar(version))
		return false;

	ptr_login = dblogin(); // reservo memoria para el login

	if (!ptr_login)
		return false;


	// lleno la estructua del login
	DBSETLUSER (ptr_login,usuario);
	DBSETLPWD (ptr_login,clave);

	proceso = dbopen(ptr_login, db);

	// pude abrir la base ?
	if (!proceso)
		return false;

	dbfreelogin(ptr_login);

	return true;
}

bool GuruDb::agregar(char *sentencia)
{
	RETCODE	salida = FAIL;

	if (proceso)
		salida = dbcmd(proceso,sentencia);

	return (salida == FAIL) ? false : true;
}

void GuruDb::set_auto_campos(bool activar)
{
	auto_campos = activar;
}

bool GuruDb::get_auto_campos()
{
	return auto_campos;
}

bool GuruDb::enviar_auto(char *sentencia)
{
	bool	salida;
	INT		cant_campos;
	int		i;

	if ((salida = enviar(sentencia)))
		{
		if (campos) delete [] campos;

// reservo memoria para almacenar los punteros a las cadenas
		cant_campos = dbnumcols(proceso);
		campos = new char * [cant_campos];

// reservo memoria para cada campo en el array
		for (i=1;i<=cant_campos;i++)
			campos[i-1] = new char [dbcollen(proceso,i)];

// uno/ligo los campos a cada variable
		for (i=1;i<=cant_campos;i++)
			ligar(campos[i-1], i);
		}

	return salida;
}

char * GuruDb::campo(INT num_columna)
{
	if (campos)
		return campos[num_columna-1];
	else
		return NULL;
}

bool GuruDb::enviar(char *sentencia)
{
	RETCODE	salida;

	if (proceso)
		{
		salida = dbcmd(proceso,sentencia);
		if (salida == FAIL)
			salida = false;
		else
			{
			salida = dbsqlexec(proceso);
			if (salida)
				salida = dbresults (proceso);
			}
		}
	else
		salida = FAIL;

	return (salida == FAIL) ? false : true;
}

bool GuruDb::enviar()
{
	RETCODE	salida = FAIL;

	if (proceso)
		salida = dbsqlsend(proceso);
	else
		salida = FAIL;

	return (salida == FAIL) ? false : true;
}

bool GuruDb::enviarex(char *sentencia, ...)
{
	char aux_sentencia[GURUDB_BUFFER];
	va_list parametros;

	va_start (parametros,sentencia);
	vsprintf(aux_sentencia, sentencia, parametros);
	va_end(parametros);

	return enviar(aux_sentencia);
}


char * GuruDb::datos(char *columna, int num_columna)
{
	return strcpy(columna, (DBCHAR *)dbdata (proceso , num_columna));
}

RETCODE GuruDb::ligar(char *columna, int num_columna)
{
//	int len = strlen(columna);
	return dbbind (proceso,num_columna, NTBSTRINGBIND, 0, (unsigned char*)columna);
}

RETCODE GuruDb::ligar(long *columna, int num_columna)
{
	return dbbind (proceso,num_columna, INTBIND, 0, (unsigned char *)columna);
}

RETCODE GuruDb::ligar(int *columna, int num_columna)
{
	return dbbind (proceso,num_columna, SMALLBIND, 0, (unsigned char *)columna);
}

void GuruDb::liberar(char *data)
{
	delete data;
}

STATUS GuruDb::sacar_datos()
{
	if (proceso)
		return dbnextrow(proceso);
	else
		return FAIL;
}

// cancela datos pendientes del ultimo Query
RETCODE GuruDb::cancelar()
{
	return dbcancel (proceso);
}

// cierra la conexion contra el SQL
void GuruDb::cerrar_conexion()
{
	dbclose(proceso);
}

RETCODE GuruDb::fin_tuplas()
{
	return dbresults (proceso);
}

// retorna el numero de la primera tupla disponible
long GuruDb::primera_tupla()
{
	return dbfirstrow(proceso);
}

// retorna el ultimo numero de la tupla en el buffer
long GuruDb::ultima_tupla()
{
	return dblastrow(proceso);
}

// retorna el numero actual de la tupla en el buffer
long GuruDb::actual_tupla()
{
	return dbcurrow(proceso);
}

// retorna la cantidad total de tuplas afectadas en la consulta
long GuruDb::cantidad_tuplas()
{
	return dbcount(proceso);
}

RETCODE GuruDb::mostar_data()
{
	return dbprrow(proceso);
}

void GuruDb::mostar_titulos()
{
	dbprhead(proceso);
}

bool GuruDb::activo()
{
	RETCODE	salida = dbdead(proceso);

	return (salida == FAIL) ? true : false;
}

int GuruDb::manejador_error (PDBPROCESS ptr_proceso, int severo, int error_db, int error_so, const char * desc_db,const char * desc_so)
{
	ptr_mio->db_error.severo = severo;
	ptr_mio->db_error.error_db = error_db;
	ptr_mio->db_error.error_so = error_so;

	if (desc_db)
		strcpy(ptr_mio->db_error.desc_db,desc_db);
	else
		strcpy(ptr_mio->db_error.desc_db,"");

	if (desc_so)
		strcpy(ptr_mio->db_error.desc_so,desc_so);
	else
		strcpy(ptr_mio->db_error.desc_so,"");

	return INT_CANCEL;
}

int GuruDb::manejador_mensaje (PDBPROCESS ptr_proceso,int nro_mensaje, int estado_mensaje, int severo, char *texto_mensaje, char * nombre_server, char *nombre_proceso, DBUSMALLINT linea)
{
	ptr_mio->db_mensaje.nro_mensaje = nro_mensaje;
	ptr_mio->db_mensaje.estado_mensaje = estado_mensaje;
	ptr_mio->db_mensaje.severo = severo;

	if (texto_mensaje)
		strcpy(ptr_mio->db_mensaje.texto_mensaje,texto_mensaje);
	else
		strcpy(ptr_mio->db_mensaje.texto_mensaje,"");

	if (nombre_server)
		strcpy(ptr_mio->db_mensaje.nombre_server,nombre_server);
	else
		strcpy(ptr_mio->db_mensaje.nombre_server,"");

	if (nombre_proceso)
		strcpy(ptr_mio->db_mensaje.nombre_proceso,nombre_proceso);
	else
		strcpy(ptr_mio->db_mensaje.nombre_proceso,"");

	ptr_mio->db_mensaje.linea = linea;


	return 0;
}

int GuruDb::nro_error()
{
	return db_error.error_db;
}

char *GuruDb::msg_error()
{
	return db_error.desc_db;
}

int GuruDb::servers()
{
	char *nombre_server;
	USHORT entradas;

	nombre_server = new char [15*100];

	if (!nombre_server) return 0;

	INT cantidad = dbserverenum (LOC_SEARCH, nombre_server, 15*100, &entradas);

	switch (cantidad)
		{
	case ENUM_SUCCESS :			// (0x0000)
						break;
	case MORE_DATA :			// (0x0001)
					break;
	case NET_NOT_AVAIL :		// (0x0002)
						break;
	case OUT_OF_MEMORY :		// (0x0004)
						break;
	case NOT_SUPPORTED :		// (0x0008)
						break;
	case ENUM_INVALID_PARAM :	// (0x0010)
							break;
		}

	delete nombre_server;
	return cantidad;
}

int GuruDb::servers(char *servers)
{
	return 0;
}


char *GuruDb::linea(char **ptr_linea)
{
	int cant_lugares=0;
	int i;

	for (i=1;i<=dbnumcols(proceso);i++)
		cant_lugares += dbcollen(proceso,i);

	*ptr_linea = new char [cant_lugares];


	if (*ptr_linea)
		{
		strcpy(*ptr_linea,"");
		for (i=1;i<=dbnumcols(proceso);i++)
			{
			;
			}
		}

	return *ptr_linea;
}
