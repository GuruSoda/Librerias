#include "GuruSession.h"

GuruSession::GuruSession()
{
}

GuruSession::GuruSession(HANDLE nuevo_manipulador)
{
	GuruToken = nuevo_manipulador;
}

GuruSession::~GuruSession()
{
}

HANDLE GuruSession::poner_token(HANDLE nuevo_manipulador)
{
	HANDLE viejo;

	viejo = GuruToken;
	GuruToken = nuevo_manipulador;

	return viejo;
}


bool GuruSession::login(char *usuario, char *dominio, char *clave, int tipo)
{
	bool salida = false;

	strcpy(logon_user.usuario,usuario);
	strcpy(logon_user.dominio,dominio);
	strcpy(logon_user.clave,clave);

	if (LogonUser(usuario,dominio,clave, tipo, LOGON32_PROVIDER_DEFAULT, &GuruToken))
		salida = true;
	else
		{
		GuruSession_error.descripcion();
		salida = false;
		}

	return salida;
}

HANDLE GuruSession::token()
{
	return GuruToken;
}

char * GuruSession::usuario_logon()
{
	return logon_user.usuario;
}
char * GuruSession::dominio_logon()
{
	return logon_user.dominio;
}

char * GuruSession::clave_logon()
{
	return logon_user.clave;
}


const char* GuruSession::mensaje_error()
{
	return GuruSession_error.mensaje();
}

dword GuruSession::ls_lista_privilegios(HANDLE hToken)
{
	TOKEN_INFORMATION_CLASS tipo_info = TokenPrivileges;
	TOKEN_PRIVILEGES					*buffer;
	dword								necesarios=0;
	dword								cant_privilegios=0;


	if (!GetTokenInformation(hToken, tipo_info, &buffer, 0, &necesarios))
		{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
			buffer = (TOKEN_PRIVILEGES*) new char [necesarios];
			if (!buffer) return 0;
			}
		else
			{
			GuruSession_error.descripcion();
			return 0;
			}
		}


	if (GetTokenInformation(hToken, tipo_info, buffer, necesarios, &necesarios))
		{
		DWORD	i=0;
		char	nombre_privilegio[256];
		char	descripcion_privilegio[256];
		DWORD	caracteres_nombre=256;
		DWORD	caracteres_descripcion=256;
		DWORD	lenguaje;
		reg_privilegio	rp;
		bool	con_error = false;

		GuruError	error;

		if (!lista_privilegios.EstaVacia()) lista_privilegios.vaciar_lista();


		for (i=0;i<buffer->PrivilegeCount;i++)
			{
			if (LookupPrivilegeName(NULL, &buffer->Privileges[i].Luid,nombre_privilegio,&caracteres_nombre))
				{
				strcpy(rp.nombre,nombre_privilegio);

				if (LookupPrivilegeDisplayName(NULL,nombre_privilegio,descripcion_privilegio,&caracteres_descripcion, &lenguaje))
					{
					strcpy(rp.nombre_display,descripcion_privilegio);

					rp.activo = (buffer->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED) ? true : false;

					lista_privilegios.agregar(&rp);
					cant_privilegios++;
					}
				else
					con_error = true;
				}
			else
				con_error = false;

			if (con_error)
				{
				strcpy(rp.nombre,error.descripcion());
				strcpy(rp.nombre_display,rp.nombre);
				rp.activo = false;

				lista_privilegios.agregar(&rp);
				con_error = false;
				cant_privilegios++;
				}

			caracteres_nombre = 256;
			caracteres_descripcion=256;
			}
		GuruToken = hToken;
		}

	return cant_privilegios;
}

bool GuruSession::ls_autoincrementar_privilegio(bool automatico)
{
	return lista_privilegios.autoincrementar(automatico);
}

void GuruSession::ls_incrementar()
{
	lista_privilegios++;
}

char *GuruSession::ls_nombre_privilegio()
{
	return lista_privilegios.item()->nombre;
}

char *GuruSession::ls_display_privilegio()
{
	return lista_privilegios.item()->nombre_display;
}

bool GuruSession::ls_estado_privilegio()
{
	return lista_privilegios.item()->activo;
}

bool GuruSession::ls_reset_privilegio()
{
	return lista_privilegios.ir_a(0);
}

bool GuruSession::ls_habilitar_privilegio(char *privilegio_habilitar)
{
	TOKEN_PRIVILEGES	tp;
	DWORD				req=0;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if ( !LookupPrivilegeValue( 0, privilegio_habilitar, &tp.Privileges[0].Luid))
		{
        GuruSession_error.descripcion();
		return false;
		}

	if (!AdjustTokenPrivileges(GuruToken, false, &tp, sizeof(tp), NULL, &req))
		{
		GuruSession_error.descripcion();
		return false;
		}

	return true;
}


DWORD GuruSession::ls_habilitar_todos_privilegios(HANDLE token)
{
	DWORD	cant,i;
	DWORD	cant_cambios=0;

	if (token == NULL)
		{
		if (!OpenProcessToken(::GetCurrentProcess(), TOKEN_READ | TOKEN_ADJUST_PRIVILEGES, &GuruToken))
			{
			GuruSession_error.descripcion();
			return 0;
			}
		}
	else
		GuruToken = token;


	cant = ls_lista_privilegios(GuruToken);

	ls_reset_privilegio();
	ls_autoincrementar_privilegio(true);

	for (i=0;i<cant;i++)
		{
		if (!ls_habilitar_privilegio(ls_nombre_privilegio()))
			cant_cambios++;
		}

	ls_reset_privilegio();

	return cant_cambios;
}

//////////////////////////////////////////////////////////////////////////////////
//	Pasos para darle al usuario derecho sobre el Windows Station y Desktop.
//
//	1.- Salvar el Handler del actual Windows Station (GetprocessWindowStation).
//	2.- Guardar el Handler del Windows Station (OpenWindowStation).
//	3.- Ponerle al actual Windows Station el handler del Windows Station (Paso 2) (SetProcessWindowStation)
//		para conseguir el actual default desktop.
//	4.- Conseguir el actual Handler al interartive DeskTop (OpenDesktop).
//	5.- Restaurar el Windows Station al llamador (SetProcessWindowStation) (Paso 1).
//	6.- Saco el SID de la session
//	7.- Permitir Full access al interactive Windows Station
//	8.- Permitir Full access al interactive Desktop
//	9.- Personificar el cliente para asegurar el acceso
//////////////////////////////////////////////////////////////////////////////////

bool GuruSession::privililegios_a_desktop_WStation()
{
	HWINSTA handler_viejo_act_ws;
	HWINSTA handler_act_ws;

	HDESK	handler_act_desktop;
	SID		*sid;
	DWORD	salida;

//	Paso 1.
	process_ws = handler_viejo_act_ws = GetProcessWindowStation();
	salida = GetLastError();

//	Paso 2.
	handler_act_ws = OpenWindowStation("winsta0", false, READ_CONTROL | WRITE_DAC);
	salida = GetLastError();

//	Paso 3.
	SetProcessWindowStation(handler_act_ws);
	salida = GetLastError();

//	Paso 4.
	handler_act_desktop = OpenDesktop("default", 0, false,  READ_CONTROL | WRITE_DAC | DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS);
	salida = GetLastError();

//	Paso 5.
	SetProcessWindowStation(handler_viejo_act_ws);
	salida = GetLastError();

	if (logon_sid(&sid))
		{
		AddTheAceWindowStation(handler_act_ws,sid);
		AddTheAceDesktop(handler_act_desktop,sid);

//		access_a_Desktop(handler_act_desktop,sid);
//		access_a_WindowStation(handler_act_ws,sid);
		delete sid;
		}

	return true;
}

HWINSTA GuruSession::original_ws()
{
    return process_ws;
}


////////////////////////////////////////////////////////////////////////////////////////////
//	pasos para sacar el Logon Id
//
//	1.-
////////////////////////////////////////////////////////////////////////////////////////////

bool GuruSession::logon_sid(SID **sid)
{
	TOKEN_INFORMATION_CLASS			clase = TokenGroups;
	TOKEN_GROUPS *					buffer = NULL;
	DWORD							tamanio=0;
	DWORD							i;
	DWORD							salida;
	bool							encontrado=false;


	if (!GetTokenInformation(GuruToken, clase, buffer, 0, &tamanio))
		{
		if ((salida = GetLastError()) == ERROR_INSUFFICIENT_BUFFER)
			{
			buffer = (TOKEN_GROUPS *)new char [tamanio];
			if (!buffer) return false;
			}
		}

	if (!GetTokenInformation(GuruToken,  clase, buffer, tamanio, &tamanio))  return false;

	for (i=0;i<buffer->GroupCount;i++)
		{
		if ((buffer->Groups[i].Attributes & SE_GROUP_LOGON_ID) == SE_GROUP_LOGON_ID)
			{
			if (IsValidSid(buffer->Groups[i].Sid))
				{
				tamanio = GetLengthSid(buffer->Groups[i].Sid);
				*sid = (SID*)new char [tamanio];

				if (!*sid) return false;

				if (!CopySid(tamanio,  *sid, buffer->Groups[i].Sid))
					return false;

				encontrado = true;
				break;
				}
			else
				return false;
			}
		}

	delete buffer;
	return encontrado;
}

bool GuruSession::access_a_WindowStation(HWINSTA token_win, SID *sid)
{
	ACCESS_ALLOWED_ACE		*ace		=	NULL;
	SECURITY_INFORMATION	dsi			=	DACL_SECURITY_INFORMATION;
	SECURITY_DESCRIPTOR		*sd			=	NULL;
	SECURITY_DESCRIPTOR		*sd_nuevo	=	NULL;
	DWORD					tamanio		=	0,i;
	DWORD					nuevo_tamanio	=	0;
	BOOL					acl_presente	= false;
	BOOL					existe_acl		= false;
	ACL						*acl		= NULL;
	ACL						*nuevo_acl	= NULL;
	ACL_SIZE_INFORMATION	tamanio_acl;
	void					*buffer		= NULL;


	if (!GetUserObjectSecurity(token_win, &dsi, sd, 0, &tamanio))
		{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
			sd = (SECURITY_DESCRIPTOR*) new char [tamanio];
			sd_nuevo = (SECURITY_DESCRIPTOR*) new char [tamanio];
			}
		}

	if (!GetUserObjectSecurity(token_win, &dsi, sd, tamanio, &tamanio)) return false;

	// creo un nuevo Dacl
	if (!InitializeSecurityDescriptor(sd_nuevo, SECURITY_DESCRIPTOR_REVISION)) return false;

	// saco el el Dacl desde el Security Descriptor
	if (!GetSecurityDescriptorDacl(sd, &acl_presente, &acl, &existe_acl)) return false;

	memset(&tamanio_acl,0,sizeof(ACL_SIZE_INFORMATION));
	tamanio_acl.AclBytesInUse = sizeof(ACL);

	if (acl)
		if (!GetAclInformation(acl, &tamanio_acl, sizeof(ACL_SIZE_INFORMATION),  AclSizeInformation))
			return false;

	nuevo_tamanio = tamanio_acl.AclBytesInUse + (2 * sizeof(ACCESS_ALLOWED_ACE)) + (2 * GetLengthSid(&sid)) - (2 * sizeof(DWORD));

//	genero un nuevo ACL
	nuevo_acl = (ACL *)new char [nuevo_tamanio];

	if (!nuevo_acl) return false;

/* y para NT 3.51? */ /* inicializo el nuevo ACL*/
	if (!InitializeAcl(nuevo_acl,nuevo_tamanio, ACL_REVISION)) return false;

	if (acl_presente)
		{
		if (tamanio_acl.AceCount)
			{
			for (i=0;i<tamanio_acl.AceCount;i++)
				{
				if (!GetAce(acl, i, &buffer))	return false;

				if (!AddAce(nuevo_acl, ACL_REVISION, MAXDWORD, buffer, ((ACE_HEADER*)buffer)->AceSize))
					return false;
				}
			}
		}

	ace = (ACCESS_ALLOWED_ACE*)new char [sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(&sid) -  sizeof(DWORD)];

	if (!ace) return false;

	ace->Header.AceType = ACCESS_ALLOWED_ACE_TYPE;
	ace->Header.AceFlags = CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE;
	ace->Header.AceSize = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(&sid) - sizeof(DWORD);

/* estara bien ?*/
	ace->Mask = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL;


	DWORD largo = GetLengthSid(sid);
	if (!CopySid(largo,&ace->SidStart,sid)) return false;

/**/if (!AddAce(nuevo_acl, ACL_REVISION, MAXDWORD, ace, ace->Header.AceSize)) return false;

	ace->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
	ace->Mask			 = TODO_WINSTA;

	if (!AddAce(nuevo_acl, ACL_REVISION, MAXDWORD, ace, ace->Header.AceSize)) return false;


	if (!SetSecurityDescriptorDacl(sd_nuevo, true, nuevo_acl, false)) return false;

/* ??? (2 parametro) */
	if (!SetUserObjectSecurity(token_win, &dsi, sd_nuevo))
		{
		GuruSession_error.descripcion();
		return false;
		}

	delete nuevo_acl;
	delete sd_nuevo;
	delete ace;
	delete sd;

	return true;
}

bool GuruSession::access_a_Desktop(HDESK token_desk,SID *sid)
{
	ACCESS_ALLOWED_ACE		*ace		= NULL;
	SECURITY_INFORMATION	dsi			=	DACL_SECURITY_INFORMATION;
	SECURITY_DESCRIPTOR		*sd			=	NULL;
	SECURITY_DESCRIPTOR		*sd_nuevo	=	NULL;
	dword					tamanio		=	0,i;
	dword					nuevo_tamanio	=	0;
	BOOL					acl_presente	= false;
	BOOL					existe_acl		= false;
	ACL						*acl		= NULL;
	ACL						*nuevo_acl	= NULL;
	ACL_SIZE_INFORMATION	tamanio_acl;
	void					*buffer		= NULL;


	if (!GetUserObjectSecurity(token_desk, &dsi, sd, 0, &tamanio))
		{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
			sd = (SECURITY_DESCRIPTOR*) new char [tamanio];
			sd_nuevo = (SECURITY_DESCRIPTOR*) new char [tamanio];
			}
		}


	if (!GetUserObjectSecurity(token_desk, &dsi, sd, tamanio, &tamanio)) return false;

	if (!InitializeSecurityDescriptor(sd_nuevo, SECURITY_DESCRIPTOR_REVISION))  return false;

	if (!GetSecurityDescriptorDacl(sd, &acl_presente, &acl, &existe_acl))  return false;

	memset(&tamanio_acl,0,sizeof(ACL_SIZE_INFORMATION));
	tamanio_acl.AclBytesInUse = sizeof(ACL);

	if (acl)
		if (!GetAclInformation(acl, &tamanio_acl, sizeof(ACL_SIZE_INFORMATION),  AclSizeInformation))
			return false;

	nuevo_tamanio = tamanio_acl.AclBytesInUse + sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(&sid) - sizeof(DWORD);

	nuevo_acl = (ACL *) new char [nuevo_tamanio];

/* y para NT 3.51? */
	if (!InitializeAcl(nuevo_acl,nuevo_tamanio, ACL_REVISION))  return false;

	if (acl_presente)
		{
		if (tamanio_acl.AceCount)
			{
			for (i=0;i<tamanio_acl.AceCount;i++)
				{
				if (!GetAce(acl, i, &buffer)) return false;

				if (!AddAce(nuevo_acl, ACL_REVISION, MAXDWORD, buffer, ((ACE_HEADER*)buffer)->AceSize))
					return false;
				}
			}
		}
/*=====================================================================================*/

	if (!AddAccessAllowedAce(nuevo_acl, ACL_REVISION, TODO_DESKTOP, sid))
		return false;


	if (!SetSecurityDescriptorDacl(sd_nuevo, true, nuevo_acl, false))
		return false;

/* ??? (2 parametro) */
	if (!SetUserObjectSecurity(token_desk, &dsi, sd_nuevo))
		{
//		int salida = GetLastError();
		return false;
		}


	delete nuevo_acl;
	delete sd_nuevo;
	delete ace;
	delete sd;

	return true;
}

bool GuruSession::AddTheAceWindowStation(HWINSTA hwinsta, PSID psid)
{
        ACCESS_ALLOWED_ACE   *pace = NULL;
        ACL_SIZE_INFORMATION aclSizeInfo;
        BOOL                 bDaclExist;
        BOOL                 bDaclPresent;
        bool                 bSuccess  = false; // assume function will
                                                //fail
        dword                dwNewAclSize;
        dword                dwSidSize = 0;
        dword                dwSdSizeNeeded;
        PACL                 pacl;
        PACL                 pNewAcl   = NULL;
        PSECURITY_DESCRIPTOR psd       = NULL;
        PSECURITY_DESCRIPTOR psdNew    = NULL;
        PVOID                pTempAce;
        SECURITY_INFORMATION si        = DACL_SECURITY_INFORMATION;
        unsigned int         i;

        try
             {
             //
             // obtain the dacl for the windowstation
             //
             if (!GetUserObjectSecurity(hwinsta, &si, psd, dwSidSize, &dwSdSizeNeeded))
             if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                  {
                  psd = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSdSizeNeeded );
                  if (psd == NULL)
                       throw false; // __leave; // __leave == solo microsoft

                  psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSdSizeNeeded);
                  if (psdNew == NULL)
                       throw false; // __leave; // __leave == solo microsoft

                  dwSidSize = dwSdSizeNeeded;

                  if (!GetUserObjectSecurity(hwinsta, &si, psd, dwSidSize, &dwSdSizeNeeded))
                       throw false; // __leave; // __leave == solo microsoft
				}
             else
                   throw false; // __leave; // __leave == solo microsoft

             //
             // create a new dacl
        //
             if (!InitializeSecurityDescriptor(psdNew, SECURITY_DESCRIPTOR_REVISION))
                  throw false; // __leave; // __leave == solo microsoft

             //
        // get dacl from the security descriptor
             //
             if (!GetSecurityDescriptorDacl(psd, &bDaclPresent, &pacl, &bDaclExist ))
                  throw false; // __leave; // __leave == solo microsoft

             //
             // initialize
             //
             ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
             aclSizeInfo.AclBytesInUse = sizeof(ACL);

             //
             // call only if the dacl is not NULL
             //
             if (pacl != NULL)
                  {
                  // get the file ACL size info
                  if (!GetAclInformation(pacl, (LPVOID)&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION), AclSizeInformation))
                       throw false; // __leave; // __leave == solo microsoft
                   }

             //
             // compute the size of the new acl
             //
             dwNewAclSize = aclSizeInfo.AclBytesInUse + (2 *
             sizeof(ACCESS_ALLOWED_ACE)) + (2 * GetLengthSid(psid)) - (2 *
             sizeof(DWORD));

             //
             // allocate memory for the new acl
             //
             pNewAcl = (PACL)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,dwNewAclSize);
             if (pNewAcl == NULL)
                  throw false; // __leave; // __leave == solo microsoft

             //
             // initialize the new dacl
             //
             if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
                  throw false; // __leave; // __leave == solo microsoft

             //
             // if DACL is present, copy it to a new DACL
             //
             if (bDaclPresent) // only copy if DACL was present
                  {
                  // copy the ACEs to our new ACL
                  if (aclSizeInfo.AceCount)
                       {
                       for (i=0; i < aclSizeInfo.AceCount; i++)
                            {
                            // get an ACE
                            if (!GetAce(pacl, i, &pTempAce))
                                 throw false; // __leave; // __leave == solo microsoft

                            // add the ACE to the new ACL
                            if (!AddAce(pNewAcl, ACL_REVISION, MAXDWORD, pTempAce,((ACE_HEADER*)pTempAce)->AceSize))
                                 throw false; // __leave; // __leave == solo microsoft
                             }
                        }
                  }

             //
             // add the first ACE to the windowstation
             //
             pace = (ACCESS_ALLOWED_ACE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,  sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) - sizeof(DWORD));
             if (pace == NULL)
                  throw false; // __leave; // __leave == solo microsoft

             pace->Header.AceType  = ACCESS_ALLOWED_ACE_TYPE;
             pace->Header.AceFlags = CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE;

             pace->Header.AceSize  = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) - sizeof(DWORD);
             pace->Mask            = GENERIC_ACCESS;

             if (!CopySid(GetLengthSid(psid), &pace->SidStart, psid))
                  throw false; // __leave; // __leave == solo microsoft

             if (!AddAce(pNewAcl, ACL_REVISION, MAXDWORD,(LPVOID)pace, pace->Header.AceSize))
                  throw false; // __leave; // __leave == solo microsoft

             //
             // add the second ACE to the windowstation
             //
             pace->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
             pace->Mask            = WINSTA_ALL;

             if (!AddAce(pNewAcl, ACL_REVISION, MAXDWORD,(LPVOID)pace, pace->Header.AceSize))
                  throw false; // __leave; // __leave == solo microsoft

                  //
                  // set new dacl for the security descriptor
                  //
              if (!SetSecurityDescriptorDacl(psdNew, true, pNewAcl, false))
                    throw false; // __leave; // __leave == solo microsoft

                   //
         // set the new security descriptor for the windowstation
         //
         if (!SetUserObjectSecurity(hwinsta, &si, psdNew))
            throw false; // __leave; // __leave == solo microsoft
         //
         // indicate success
         //
         bSuccess = true;
             }
        catch (bool error) //__finally
             {
             bSuccess = error;
             }
             //
             // free the allocated buffers
             //
             if (pace != NULL)
                  HeapFree(GetProcessHeap(), 0, (LPVOID)pace);

             if (pNewAcl != NULL)
                  HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

             if (psd != NULL)
                  HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

             if (psdNew != NULL)
                  HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);


        return bSuccess;

   }

   bool GuruSession::AddTheAceDesktop(HDESK hdesk, PSID psid)
   {

        ACL_SIZE_INFORMATION aclSizeInfo;
        BOOL                 bDaclExist;
        BOOL                 bDaclPresent;
        bool                 bSuccess  = false; // assume function will
                                                // fail
        dword                dwNewAclSize;
        dword                dwSidSize = 0;
        dword                dwSdSizeNeeded;
        PACL                 pacl;
        PACL                 pNewAcl = NULL;
        PSECURITY_DESCRIPTOR psd       = NULL;
        PSECURITY_DESCRIPTOR psdNew    = NULL;
        PVOID                pTempAce;
        SECURITY_INFORMATION si        = DACL_SECURITY_INFORMATION;
        unsigned int         i;

        try
             {
             //
             // obtain the security descriptor for the desktop object
             //
             if (!GetUserObjectSecurity(hdesk, &si, psd, dwSidSize, &dwSdSizeNeeded ))
                  {
                  if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                       {
                       psd = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSdSizeNeeded);
                       if (psd == NULL)
                            throw false; // __leave; // __leave == solo microsoft

                       psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,dwSdSizeNeeded);
                       if (psdNew == NULL)
                            throw false;// __leave; // __leave == solo microsoft

                       dwSidSize = dwSdSizeNeeded;

                       if (!GetUserObjectSecurity(hdesk, &si, psd, dwSidSize, &dwSdSizeNeeded))
                            throw false; // __leave; // __leave == solo microsoft
                       }
                  else
                       throw false; // __leave; // __leave == solo microsoft
                  }

             //
             // create a new security descriptor
             //
             if (!InitializeSecurityDescriptor(psdNew, SECURITY_DESCRIPTOR_REVISION))
               throw false; // __leave; // __leave == solo microsoft

             //
             // obtain the dacl from the security descriptor
             //
             if (!GetSecurityDescriptorDacl(psd,&bDaclPresent,&pacl,&bDaclExist))
                  throw false; // __leave; // __leave == solo microsoft

             //
             // initialize
             //
             ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
             aclSizeInfo.AclBytesInUse = sizeof(ACL);

             //
             // call only if NULL dacl
             //
             if (pacl != NULL)
                  {
                  //
                  // determine the size of the ACL info
                  //
                  if (!GetAclInformation(pacl,(LPVOID)&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION), AclSizeInformation))
                       throw false; // __leave; // __leave == solo microsoft
                   }

             //
             // compute the size of the new acl
             //
        dwNewAclSize = aclSizeInfo.AclBytesInUse + sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) - sizeof(DWORD);

             //
             // allocate buffer for the new acl
             //
             pNewAcl = (PACL)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwNewAclSize);
             if (pNewAcl == NULL)
                  throw false; // __leave; // __leave == solo microsoft

             //
             // initialize the new acl
             //
             if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
                  throw false; // __leave; // __leave == solo microsoft

             //
             // if DACL is present, copy it to a new DACL
             //
             if (bDaclPresent) // only copy if DACL was present
                  {
                  // copy the ACEs to our new ACL
                  if (aclSizeInfo.AceCount)
                       {
                       for (i=0; i < aclSizeInfo.AceCount; i++)
                            {
                            // get an ACE
                            if (!GetAce(pacl, i, &pTempAce))
                                 throw false; // __leave; // __leave == solo microsoft

                            // add the ACE to the new ACL
                            if (!AddAce(pNewAcl,ACL_REVISION, MAXDWORD, pTempAce, ((ACE_HEADER*)pTempAce)->AceSize))
                                 throw false; // __leave; // __leave == solo microsoft
                             }
                        }
                  }

             //
             // add ace to the dacl
             //
             if (!AddAccessAllowedAce(pNewAcl,ACL_REVISION,DESKTOP_ALL,psid))
                  throw false; // __leave; // __leave == solo microsoft

             //
             // set new dacl to the new security descriptor
             //
             if (!SetSecurityDescriptorDacl(psdNew, true, pNewAcl,false))
                  throw false; // __leave; // __leave == solo microsoft

             //
             // set the new security descriptor for the desktop object
             //
             if (!SetUserObjectSecurity(hdesk, &si, psdNew))
                  throw false; // __leave; // __leave == solo microsoft

             //
             // indicate success
             //
             bSuccess = true;
             }

        catch (bool error)
            {
            bSuccess = error;
            }

            //
            // free buffers
            //
            if (pNewAcl != NULL)
                 HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

             if (psd != NULL)
                  HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

             if (psdNew != NULL)
                  HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);

        return bSuccess;
   }

