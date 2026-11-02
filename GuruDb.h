// GuruDb.h: interface for the GuruDb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GURUDB_H__24F783D5_F9B2_11D3_A69C_00104B0256E3__INCLUDED_)
#define AFX_GURUDB_H__24F783D5_F9B2_11D3_A69C_00104B0256E3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AUTOR_H
	#define		__AUTOR_H
	static char autor[] = "Autor: Fabricio D. Rossi (El Guru)";
#endif


//////////////////////////////////////////////////////////////////////
//
// Linkear con NTWDBLIB.LIB
//
//////////////////////////////////////////////////////////////////////

#define		DBNTWIN32

#include <String.h>
#include <Windows.h>
#include <Stdio.h>
#include "SQLFRONT.H"
#include "sqldb.h"

#include "GuruTiposDeDatos.h"

#ifdef GURU_EXPORTS
	#define GURU_API __declspec(dllexport)
#else
	#define GURU_API //__declspec(dllimport)
#endif


#define		NO_TUPLAS				NO_MORE_ROWS
#define		GURUDB_BUFFER			1024

typedef struct
		{
		int nro_mensaje;
		int estado_mensaje;
		int severo;
		char texto_mensaje[256];
		char nombre_server[25];
		char nombre_proceso[256];
		DBUSMALLINT linea;
		}mensaje_sql;

typedef struct
		{
		int severo;
		int error_db;
		int error_so;
		char desc_db[256];
		char desc_so[256];
		}error_sql;

class  GURU_API GuruDb
{
public:
	GuruDb();
	virtual ~GuruDb();

	char *	Inicializar(char *version);
	bool	login(char *usuario,char *clave,char *db);

	bool	agregar(char *sentencia);

	bool	enviarex(char *sentencia, ...);
	bool	enviar(char *sentencia);
	bool	enviar();

	RETCODE cancelar();
	void	cerrar_conexion();

	long	primera_tupla();
	long	ultima_tupla();		// retorna el ultimo numero de la tupla en el buffer
	long	actual_tupla();		// retorna el numero actual de la tupla en el buffer
	long	cantidad_tuplas();	// retorna la cantidad total de tuplas afectadas en la consulta

	char *	datos(char *columna, int num_columna);
	RETCODE ligar(char *columna, int num_columna);
	RETCODE	ligar(long *columna, int num_columna);
	RETCODE ligar(int *columna, int num_columna);

	void	liberar(char *data);

	bool	enviar_auto(char *sentencia);
	bool	get_auto_campos();
	void	set_auto_campos(bool activar);
	char *	campo(INT num_columna);

	char *	linea(char **ptr_linea);

	bool	activo();

	RETCODE fin_tuplas();
	RETCODE sacar_datos();
	RETCODE mostar_data();
	void	mostar_titulos();
	int		servers();
	int		servers(char *servers);

	static int manejador_error (PDBPROCESS ptr_proceso, int severo, int error_db, int error_so, const char * desc_db,const char * desc_so);
	static int manejador_mensaje (PDBPROCESS ptr_proceso,int nro_mensaje, int estado_mensaje, int severo, char *texto_mensaje, char * nombre_server, char *nombre_proceso, DBUSMALLINT linea);

	int		nro_error();
	char *	msg_error();


// datos
private:
	static GuruDb *ptr_mio;

	char			version[30];
	PDBPROCESS		proceso;
	error_sql		db_error;
	mensaje_sql		db_mensaje;
	char **			campos;
	bool			auto_campos;
};

#endif // !defined(AFX_GURUDB_H__24F783D5_F9B2_11D3_A69C_00104B0256E3__INCLUDED_)

/*
The following list describes the SQL Server datatypes.
The dbconvert and dbwillconvert functions use these types.
In addition, dbcoltype, dbalttype, dbrettype each returns one of these types.

Datatype			Description

SQLARRAY			1 byte with variable length
SQLBINARY			binary
SQLBIT				bit
SQLCHAR				char
SQLDATETIM4			4-byte smalldatetime
SQLDATETIME			8-byte datetime
SQLDATETIMN			datetime or smalldatetime with null values allowed
SQLDECIMAL			decimal
SQLFLT4				4-byte real
SQLFLT8				8-byte float
SQLFLTN				float or real with null values allowed
SQLIMAGE			image
SQLINT1				1-byte tinyint
SQLINT2				2-byte smallint
SQLINT4				4-byte int
SQLINTN				tinyint, smallint, or int with null values allowed
SQLMONEY4			4-byte smallmoney
SQLMONEY			8-byte money
SQLMONEYN			money or smallmoney with null values allowed
SQLNUMERIC			numeric
SQLTEXT				text
SQLVARBINARY		varbinary
SQLVARCHAR			varchar



==============================================================================================
				Severity

Error severity		number		Description

EXINFO				1			Informational, nonerror.
EXUSER				2			User error.
EXNONFATAL			3			Nonfatal error.
EXCONVERSION		4			Error in DB-Library data conversion.
EXSERVER			5			The server has returned an error flag.
EXTIME				6			Timeout period exceeded while waiting for a response from the server ¾ the DBPROCESS is still alive.
EXPROGRAM			7			Coding error in user program.
EXRESOURCE			8			Running out of resources ¾ the DBPROCESS may be dead.
EXCOMM				9			Failure in communication with server ¾ the DBPROCESS is dead.
EXFATAL				10			Fatal error ¾ the DBPROCESS is dead.
EXCONSISTENCY		11			Internal software error ¾ notify your primary support provider.

=============================================================================================

				Error

Error no.	Error/Error severity	Description

10000		SQLEMEMEXRESOURCE		Unable to allocate sufficient memory.
10001		SQLENULLEXPROGRAM		NULL DBPROCESS pointer encountered.
10002		SQLENLOGEXCONSISTENCY	NULL LOGINREC pointer encountered.
10003		SQLEPWDEXUSER			Login incorrect.
10004		SQLECONNEXCOMM			Unable to connect: SQL Server is unavailable or does not exist.
10005		SQLEDDNEEXINFO			DBPROCESS is dead or not enabled.
10006		SQLENULLOEXCONSISTENCY	Attempt to login with NULL LOGINREC.
10007		SQLESMSGEXSERVER		General SQL Server error: Check messages from SQL Server.
10008		SQLEBTOKEXCOMM			Bad token from SQL Server: Datastream processing out of synchronization.
10009		SQLENSPEEXPROGRAM		General nonspecific DB-Library error.
10010		SQLEREADEXCOMM			Read from SQL Server failed.
10011		SQLECNOREXPROGRAM		Column number out of range.
10012		SQLETSITEXINFO			Attempt to call dbtsput with an invalid timestamp.
10013		SQLEPARMEXCONSISTENCY	Invalid parameter in DB-Library function reference.
10014		SQLEAUTNEXPROGRAM		Attempt to update the timestamp of a table with no timestamp column.
10015		SQLECOFLEXCONVERSION	Data conversion resulted in overflow.
10016		SQLERDCNEXCONVERSION	Requested data conversion does not exist.
10017		SQLEICNEXPROGRAM		Invalid value for computeid or invalid compute column number.
10018		SQLECLOSEXCOMM			Error in closing network connection.
10019		SQLENTXTEXPROGRAM		Attempt to get text point/timestamp from a nontext column.
10020		SQLEDNTIEXPROGRAM		Attempt to use dbtxtsput to put a new text timestamp into a column whose datatype is neither SQLTEXT nor SQLIMAGE.
10021		SQLETMTDEXPROGRAM		Attempt to send too much TEXT data through dbmoretext.
10022		SQLEASECEXPROGRAM		Attempt to send an empty command buffer to the SQL Server.
10023		SQLENTLLEXUSER			Name too long for LOGINREC field.
10024		SQLETIMEEXTIME			SQL Server connection timed out.
10025		SQLEWRITEXCOMM			Write to SQL Server failed.
10026		SQLEMODEEXCOMM			Network connection not in correct mode ¾ invalid SQL Server connection.
10027		SQLEOOBEXCOMM			Error in sending out-of-band data to SQL Server.
10028		SQLEITIMEXPROGRAM		Illegal timeout value specified.
10029		SQLEDBPSEXRESOURCE		Maximum number of DBPROCESSes already allocated.
10030		SQLEIOPTEXPROGRAM		Attempt to use invalid dboption.
10031		SQLEASNLEXPROGRAM		Attempt to set fields in a null LOGINREC.
10032		SQLEASULEXPROGRAM		Attempt to set unknown LOGINREC field.
10033		SQLENPRMEXPROGRAM		NULL parameter not allowed for this dboption.
10034		SQLEDBOPEXPROGRAM		Invalid or out of range dbn parameter.
10035		SQLENSIPEXPROGRAM		Negative starting index passed to dbstrcpy.
10036		SQLECNULLEXPROGRAM		NULL destination variable not allowed.
10037		SQLESEOFEXCOMM			Unexpected EOF from SQL Server.
10038		SQLERPNDEXPROGRAM		Attempt to initiate a new SQL Server operation with results pending.
10039		SQLECSYNEXCONVERSION	Attempt to convert data stopped by syntax error in source field.
10040		SQLENONETEXCOMM			DB-Library network communications layer not loaded.
10041		SQLEBTYPEXPROGRAM		Unknown bind type passed to DB-Library function.
10042		SQLEABNCEXPROGRAM		Attempt to bind to a nonexistent column.
10043		SQLEABMTEXPROGRAM		User attempted a dbbind with mismatched column and variable types.
10044		SQLEABNPEXPROGRAM		Attempt to bind using NULL pointers.
10045		SQLEBNCREXPROGRAM		Attempt to bind user variable to a nonexistent compute row.
10046		SQLEAAMTEXPROGRAM		User attempted a dbaltbind with mismatched column and variable types.
10047		SQLENXIDEXNONFATAL		The server did not grant us a distributed-transaction ID.
10048		SQLEIFNBEXPROGRAM		Illegal field number passed to bcp_control.
10049		SQLEKBCOEXINFO			1000 rows successfully bulk copied to host file.
10050		SQLEBBCIEXINFO			Batch successfully bulk copied to SQL Server.
10051		SQLEKBCIEXINFO			1000 rows sent to SQL Server.
10052		SQLEBCWEEXNONFATAL		I/O error while writing bcp datafile.
10053		SQLEBCNNEXUSER			Attempt to bulk copy a null value into a server column that does not accept null values.
10054		SQLEBCOREXCONSISTENCY	Attempt to bulk copy an oversized row to SQL Server.
10055		SQLEBCPIEXPROGRAM		Call bcp_init before any other bcp routines.
10056		SQLEBCPNEXPROGRAM		Use bcp_bind, bcp_collen, and bcp_colptr only after calling bcp_init with the copy direction set to DB_IN.
10057		SQLEBCPBEXPROGRAM		Do not use bcp_bind after bcp_init has been passed a non-null input filename.
10058		SQLEVDPTEXUSER			For bulk copy, all variable-length data must have either a length-prefix or a terminator specified.
10059		SQLEBIVIEXPROGRAM		Use bcp_columns and bcp_colfmt only after bcp_init has been passed a valid input file.
10060		SQLEBCBCEXPROGRAM		Call bcp_columns before bcp_colfmt.
10061		SQLEBCFOEXUSER			Host files must contain at least one column: bcp.
10062		SQLEBCVHEXPROGRAM		Call bcp_exec only after bcp_init has been passed a valid host file.
10063		SQLEBCUOEXRESOURCE		Unable to open host datafile: bcp.
10064		SQLEBUOEEXRESOURCE		Unable to open error file: bcp.
10065		SQLEBWEFEXNONFATAL		I/O error while writing bcp error file.
10066		SQLEBTMTEXPROGRAM		Attempt to send too much text data with bcp_moretext.
10067		SQLEBEOFEXNONFATAL		Unexpected EOF encountered in bcp datafile.
10068		SQLEBCSIEXCONSISTENCY	Host-file columns may be skipped only when copying into the server.
10069		SQLEPNULEXCONSISTENCY	NULL program pointer encountered.
10070		SQLEBSKERREXCONSISTENCY	Cannot seek in data file.
10071		SQLEBDIOEXPROGRAM		Bad bulk-copy direction.
10072		SQLEBCNTEXUSER			Attempt to use bulk copy with a nonexistent server table.
10073		SQLEMDBPEXPROGRAM		Attempt to set maximum number of DPPROCESS lower than 1.
10075		SQLCRSINVEXPROGRAM		Invalid cursor statement.
10076		SQLCRSCMDEXPROGRAM		Attempt to call cursor functions when there are commands waiting to be executed.
10077		SQLCRSNOINDEXINFO		One of the tables involved in the cursor statement does not have a unique index.
10078		SQLCRSDISEXPROGRAM		Cursor statement contains one of the disallowed phrases COMPUTE, UNION, FOR BROWSE, or SELECT INTO.
10079		SQLCRSAGREXPROGRAM		Aggregate functions are not allowed in a cursor statement.
10080		SQLCRSORDEXPROGRAM		Only fully keyset driven cursors can have ORDER BY, GROUP BY, or HAVING PHRASES.
10081		SQLCRSMEMEXPROGRAM		Keyset or window scroll size exceeds the memory limitations of this machine.
10082		SQLCRSBSKEYEXPROGRAM	Keyset cannot be scrolled backward in mixed cursors with a previous fetch type.
10083		SQLCRSNORESEXINFO		Cursor statement generated no results.
10084		SQLCRSVIEWEXPROGRAM		A view cannot be joined with another table or a view in a cursor statement.
10085		SQLCRSBUFREXPROGRAM		Row buffering should not be turned on when using cursor functions.
10086		SQLCRSFROWNEXINFO		Row number to be fetched is outside valid range.
10087		SQLCRSBROLEXPROGRAM		Backward scrolling cannot be used in a forward scrolling cursor.
10088		SQLCRSFRANDEXPROGRAM	Fetch types RANDOM and RELATIVE can only be used within the keyset of keyset driven cursors.
10089		SQLCRSFLASTEXPROGRAM	Fetch type LAST requires fully keyset driven cursors.
10090		SQLCRSROEXPROGRAM		Data locking or modifications cannot be made in a READONLY cursor.
10091		SQLCRSTABEXPROGRAM		Table name must be determined in operations involving data locking or modifications.
10092		SQLCRSUPDTABEXPROGRAM	Update or insert operations using bind variables require single table cursors.
10093		SQLCRSUPDNBEXPROGRAM	Update or insert operations cannot use bind variables when binding type is NOBIND.
10094		SQLCRSVIINDEXPROGRAM	The view used in the cursor statement does not include all the unique index columns of the underlying tables.
10095		SQLCRSNOUPDEXINFO		Update or delete operation did not affect any rows.
10096		SQLCRSOS2EXPROGRAM		Cursors are not supported for this server.
10097		SQLEBCSAEXPROGRAM		The BCP hostfile %s contains only %ld rows. Skipping all of these rows is not allowed.
10098		SQLCRSROEXPROGRAM		Data locking or modifications cannot be made in a READONLY cursor.
10099		SQLEBCNEEXPROGRAM		The table %s contains only %ld rows. Copying up to row %ld is not possible.
10100		SQLEBCSKEXPROGRAM		The table %s contains only %ld rows. Skipping all of these rows is not allowed.
10101		SQLEUVBFEXPROGRAM		Attempt to read unknown version of bcp format file.
10102		SQLEBIHCEXPROGRAM		Incorrect host-column number found in bcp format file.
10103		SQLEBWFFEXRESOURCE		I/O error while reading bcp format file.
10104		SQLNUMVALEXPROGRAM		The data stored in the DBNUMERIC/DBDECIMAL structure is invalid.
10105		SQLEOLDVREXPROGRAM		The SQL Server's TDS is obsolete with this version of DB-Library.

==============================================================================================

*/
