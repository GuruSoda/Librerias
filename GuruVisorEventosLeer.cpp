#include "GuruVisorEventosLeer.h"

GuruVisorEventosLeer::GuruVisorEventosLeer(void)
{
}

GuruVisorEventosLeer::~GuruVisorEventosLeer(void)
{
}

// nombre_registro = Application | Security | System 
RegistroVisorEventos GuruVisorEventosLeer::primer_registro(const char* computadora,const char* nombre_registro)
{
	EVENTLOGRECORD* registrowin = NULL;
	RegistroVisorEventos registro;

	try {
		if ((handle = OpenEventLog(computadora, nombre_registro)) == NULL) throw error.descripcion();

		registrowin = cargar_registro();

		armar_registro(registrowin, registro);

	}catch (const char* e) {cout << e << endl;}

	if (registrowin) delete registrowin;

	return RegistroVisorEventos(registro);
}

RegistroVisorEventos GuruVisorEventosLeer::siguiente_registro()
{
	EVENTLOGRECORD* registrowin = NULL;
	RegistroVisorEventos registro;

	try {
		registrowin = cargar_registro();

		armar_registro(registrowin, registro);

	}catch (const char* e) {cout << e << endl;}

	if (registrowin) delete registrowin;

	return RegistroVisorEventos(registro);
}

EVENTLOGRECORD* GuruVisorEventosLeer::cargar_registro()
{
	EVENTLOGRECORD* registrowin = NULL;
	EVENTLOGRECORD	temp_buffer	= {0};
	dword	tamanio_buffer_registro = 0;
	dword	leidos = 0;
	dword	aleer  = 0;

	// siempre tiene que dar insuficiente buffer
	ReadEventLog(handle, EVENTLOG_SEQUENTIAL_READ | EVENTLOG_FORWARDS_READ, 0, (EVENTLOGRECORD *) &temp_buffer, 0, &leidos, &aleer);

	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) throw error.descripcion();

	tamanio_buffer_registro = aleer + 1;

	registrowin = (EVENTLOGRECORD *) new char [tamanio_buffer_registro + 4];

	memset(registrowin, 0, tamanio_buffer_registro);

	if (ReadEventLog(handle, EVENTLOG_SEQUENTIAL_READ | EVENTLOG_FORWARDS_READ, 0, registrowin, tamanio_buffer_registro, &leidos, &aleer) == 0) throw error.descripcion();

	return registrowin;
}

void GuruVisorEventosLeer::armar_registro(EVENTLOGRECORD* registrowin, RegistroVisorEventos& registro)
{
//////////////////////////////////////////////////////////////////////
// Fecha y hora
//////////////////////////////////////////////////////////////////////
	registro.fecha_hora_escrito = (dword)*(const time_t *)&registrowin->TimeWritten;

//////////////////////////////////////////////////////////////////////
// Origen
//////////////////////////////////////////////////////////////////////
	registro.origen = (const char *)((LPBYTE)registrowin + sizeof(EVENTLOGRECORD));

//////////////////////////////////////////////////////////////////////
// Computadora
//////////////////////////////////////////////////////////////////////
	dword largo_source = (dword)strlen((char *)((LPBYTE)registrowin + sizeof(EVENTLOGRECORD)));
	registro.computadora = (const char *)((LPBYTE)registrowin + sizeof(EVENTLOGRECORD) + largo_source+1);

//////////////////////////////////////////////////////////////////////
// Categoria
//////////////////////////////////////////////////////////////////////
	registro.categoria = registrowin->EventCategory;

//////////////////////////////////////////////////////////////////////
// ID
//////////////////////////////////////////////////////////////////////
	registro.id_suceso = registrowin->EventID;

//////////////////////////////////////////////////////////////////////
// Usuario
//////////////////////////////////////////////////////////////////////
	char usuario[256] = {0};char dominio[256] = {0};
	dword largo_usuario = sizeof(usuario);dword largo_dominio = sizeof(dominio);
	SID_NAME_USE tipo_sid;

	if (registrowin->UserSidLength)
		if (LookupAccountSid(NULL, (PSID)((LPBYTE) registrowin + registrowin->UserSidOffset), usuario, &largo_usuario, dominio, &largo_dominio, &tipo_sid))
			{
			registro.usuario = dominio;
			registro.usuario = "\\";
			registro.usuario = usuario;
			}

//////////////////////////////////////////////////////////////////////
// Descripcion (Solo el total de cadenas)
//////////////////////////////////////////////////////////////////////
	registro.cadenas_descripcion = registrowin->NumStrings;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

	registro.cargado = true;
}

bool GuruVisorEventosLeer::armar_descripcion(RegistroVisorEventos& registro)
{
	char*		comienzo = NULL;
	char**		parametros = NULL;
	int			i=0;
/*
	parametros = new char * [registro.cadenas_descripcion];

	comienzo = (char *)((LPBYTE)registro + (registro->StringOffset));

	for (i=0;i<registro->NumStrings;i++)
		{
		if (comienzo[0] == '%' && comienzo[1] == '%')
			{
			GuruVisorEventos_error.descripcion(atoi(&comienzo[2]));

			if (GuruVisorEventos_error.mensaje())
				{
				parametros[i] = new char [strlen(GuruVisorEventos_error.mensaje()) + 1];
				strcpy(parametros[i], GuruVisorEventos_error.mensaje());
				}
			else
				{
				parametros[i] = new char [strlen(comienzo) + 1];
				strcpy(parametros[i],comienzo);
				}
			}
		else
			{
			parametros[i] = new char [strlen(comienzo) + 1];
			strcpy(parametros[i],comienzo);
			}

		comienzo += strlen(comienzo)+1;
		}

	cargar_buffer_info((char*)sacar_descripcion_reg(parametros, (DWORD)registro->EventID));

	delete [] parametros;
*/
	return true;
}
