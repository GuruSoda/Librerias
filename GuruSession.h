#if !defined __GURUSESSION_H__
#define __GURUSESSION_H__

//////////////////////////////////////////////////////////////////////
// Linkear con Advapi32.lib y User32.lib
//////////////////////////////////////////////////////////////////////

#define				TODO_WINSTA		(WINSTA_ACCESSCLIPBOARD | WINSTA_ACCESSGLOBALATOMS | WINSTA_CREATEDESKTOP | WINSTA_ENUMDESKTOPS | WINSTA_ENUMERATE | WINSTA_EXITWINDOWS | WINSTA_READATTRIBUTES | WINSTA_READSCREEN | WINSTA_WRITEATTRIBUTES | DELETE | READ_CONTROL | WRITE_DAC | WRITE_OWNER)

/*
WINSTA_ACCESSCLIPBOARD			Required to use the clipboard.
WINSTA_ACCESSGLOBALATOMS		Required to manipulate global atoms.
WINSTA_CREATEDESKTOP			Required to create new desktop objects on the window station.
WINSTA_ENUMDESKTOPS				Required to enumerate existing desktop objects.
WINSTA_ENUMERATE				Required for the window station to be enumerated.
WINSTA_EXITWINDOWS				Required to successfully call the ExitWindows and ExitWindowsEx functions.
WINSTA_READATTRIBUTES			Required to read the attributes of a window station object.
WINSTA_READSCREEN				Required to access screen contents.
WINSTA_WRITEATTRIBUTES			Required to modify the attributes of a window station object.
*/

#define				TODO_DESKTOP	(DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW | DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALPLAYBACK | DESKTOP_JOURNALRECORD | DESKTOP_READOBJECTS | DESKTOP_SWITCHDESKTOP | DESKTOP_WRITEOBJECTS | DELETE | READ_CONTROL | WRITE_DAC | WRITE_OWNER)

/*
DESKTOP_CREATEMENU				Required to create a menu on the desktop.
DESKTOP_CREATEWINDOW			Required to create a window on the desktop.
DESKTOP_ENUMERATE				Required for the desktop to be enumerated.
DESKTOP_HOOKCONTROL				Required to establish any of the window hooks.
DESKTOP_JOURNALPLAYBACK			Required to perform journal playback on the desktop.
DESKTOP_JOURNALRECORD			Required to perform journal recording on the desktop.
DESKTOP_READOBJECTS				Required to read objects on the desktop.
DESKTOP_SWITCHDESKTOP			Required to activate the desktop using SwitchDesktop.
DESKTOP_WRITEOBJECTS			Required to write objects on the desktop.
*/

#ifndef WINSTA_ALL
#define WINSTA_ALL (WINSTA_ACCESSCLIPBOARD  | WINSTA_ACCESSGLOBALATOMS |\
   WINSTA_CREATEDESKTOP    | WINSTA_ENUMDESKTOPS      |\
   WINSTA_ENUMERATE        | WINSTA_EXITWINDOWS       |\
   WINSTA_READATTRIBUTES   | WINSTA_READSCREEN        |\
   WINSTA_WRITEATTRIBUTES  | DELETE                   |\
   READ_CONTROL            | WRITE_DAC                |\
   WRITE_OWNER)
#endif

#ifndef DESKTOP_ALL
#define DESKTOP_ALL (DESKTOP_CREATEMENU      | DESKTOP_CREATEWINDOW  |\
   DESKTOP_ENUMERATE       | DESKTOP_HOOKCONTROL   |\
   DESKTOP_JOURNALPLAYBACK | DESKTOP_JOURNALRECORD |\
   DESKTOP_READOBJECTS     | DESKTOP_SWITCHDESKTOP |\
   DESKTOP_WRITEOBJECTS    | DELETE                |\
   READ_CONTROL            | WRITE_DAC             |\
   WRITE_OWNER)
#endif

#ifndef GENERIC_ACCESS
#define GENERIC_ACCESS (GENERIC_READ    | GENERIC_WRITE |\
   GENERIC_EXECUTE | GENERIC_ALL)
#endif
//  se utilizan las siguientes constantes para compatibilidad con VC++
////////////////////////////////////////////////////////////////////////
//                                                                    //
//                          User and Group related SID attributes     //
//                                                                    //
////////////////////////////////////////////////////////////////////////

//
// Group attributes
//
/*
#define SE_GROUP_MANDATORY              (0x00000001L)
#define SE_GROUP_ENABLED_BY_DEFAULT     (0x00000002L)
#define SE_GROUP_ENABLED                (0x00000004L)
#define SE_GROUP_OWNER                  (0x00000008L)
#define SE_GROUP_USE_FOR_DENY_ONLY      (0x00000010L)
#define SE_GROUP_LOGON_ID               (0xC0000000L)
*/
// #include <Windows.h>

#include "GuruListaGenerica.h"
#include "GuruError.h"
#include "GuruTiposDeDatos.h"

typedef	struct
		{
		char	nombre_display[256];
		char	nombre[64];
		bool	activo;
		}reg_privilegio;

typedef	struct
		{
		char	usuario[64];
		char	dominio[64];
		char	clave[25];
		}reg_logon;

class GuruSession
{
public:
	GuruSession();
	GuruSession(HANDLE);
	virtual ~GuruSession();

	HANDLE	poner_token(HANDLE nuevo_manipulador);

	bool	login(char *usuario, char *dominio, char *clave, int tipo = LOGON32_LOGON_INTERACTIVE);
	char *	usuario_logon();
	char *	dominio_logon();
	char *	clave_logon();

	const char*	mensaje_error();

	bool	privililegios_a_desktop_WStation();
	bool	logon_sid(SID**);
	bool	access_a_WindowStation(HWINSTA token_win,SID *sid);
	bool	access_a_Desktop(HDESK token_desk,SID *sid);

	bool	AddTheAceDesktop(HDESK hdesk, PSID psid);
	bool	AddTheAceWindowStation(HWINSTA hwinsta, PSID psid);

	HANDLE	token();
	HWINSTA original_ws();

//////////////////////////////////////////////////////////////////////
	DWORD	ls_lista_privilegios(HANDLE hToken);

	char *	ls_nombre_privilegio();
	char *	ls_display_privilegio();
	bool	ls_estado_privilegio();

	bool	ls_habilitar_privilegio(char *privilegio_habilitar);
	DWORD	ls_habilitar_todos_privilegios(HANDLE token = NULL);

	bool	ls_autoincrementar_privilegio(bool automatico);
	void	ls_incrementar();
	bool	ls_reset_privilegio();


private:
	GuruError	GuruSession_error;
	HANDLE		GuruToken;
	HWINSTA		process_ws;

	reg_logon	logon_user;

	GuruListaGenerica	<reg_privilegio> lista_privilegios;
};

#endif // !defined __GURUSESSION_H__
