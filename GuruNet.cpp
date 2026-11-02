// net.cpp: implementation of the net class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuruNet.h"
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuruNet::GuruNet()
{

}

GuruNet::~GuruNet()
{

}

//////////////////////////////////////////////////////////////////////
// Nota:
//      * Modificado el 04/07/2003 para mejor performance
//      * El metodo pide memoria para el tamanio de nombre y debe
//        ser liberado por el programa
//////////////////////////////////////////////////////////////////////
char *GuruNet::nombre_pdc(char **nombre)
{
	unsigned char * buffer;
//	WCHAR LPCWSTR 

	NetGetDCName(NULL,NULL, &buffer);

	int largo_nombre_pdc = wcslen((LPCWSTR)buffer);

	*nombre = new char [largo_nombre_pdc + 2];

	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)buffer, (largo_nombre_pdc + 1)* 2, *nombre, largo_nombre_pdc + 2, NULL, NULL);

	NetApiBufferFree(buffer);

	return *nombre;
}

char *GuruNet::pc_a_dominio(char *nombre_pc)
{
// sacado de = "Microsoft Knowledge Base Article - 111544"
//**********************************************************************
// 
//  FUNCTION:     GetCurrentUserAndDomain - This function looks up
//                the user name and domain name for the user account
//                associated with the calling thread.
// 
//  PARAMETERS:   szUser - a buffer that receives the user name
//                pcchUser - the size, in characters, of szUser
//                szDomain - a buffer that receives the domain name
//                pcchDomain - the size, in characters, of szDomain
// 
//  RETURN VALUE: TRUE if the function succeeds. Otherwise, FALSE and
//                GetLastError() will return the failure reason.
// 
//                If either of the supplied buffers are too small, 
//                GetLastError() will return ERROR_INSUFFICIENT_BUFFER
//                and pcchUser and pcchDomain will be adjusted to 
//                reflect the required buffer sizes.
// 
//**********************************************************************
/*
BOOL GetCurrentUserAndDomain(PTSTR szUser, PDWORD pcchUser, 
      PTSTR szDomain, PDWORD pcchDomain) {

   BOOL         fSuccess = FALSE;
   HANDLE       hToken   = NULL;
   PTOKEN_USER  ptiUser  = NULL;
   DWORD        cbti     = 0;
   SID_NAME_USE snu;

   __try {

      // Get the calling thread's access token.
      if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE,
            &hToken)) {

         if (GetLastError() != ERROR_NO_TOKEN)
            __leave;

         // Retry against process token if no thread token exists.
         if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, 
               &hToken))
            __leave;
      }

      // Obtain the size of the user information in the token.
      if (GetTokenInformation(hToken, TokenUser, NULL, 0, &cbti)) {

         // Call should have failed due to zero-length buffer.
         __leave;
   
      } else {

         // Call should have failed due to zero-length buffer.
         if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            __leave;
      }

      // Allocate buffer for user information in the token.
      ptiUser = (PTOKEN_USER) HeapAlloc(GetProcessHeap(), 0, cbti);
      if (!ptiUser)
         __leave;

      // Retrieve the user information from the token.
      if (!GetTokenInformation(hToken, TokenUser, ptiUser, cbti, &cbti))
         __leave;

      // Retrieve user name and domain name based on user's SID.
      if (!LookupAccountSid(NULL, ptiUser->User.Sid, szUser, pcchUser, 
            szDomain, pcchDomain, &snu))
         __leave;
      
      fSuccess = TRUE;

   } __finally {

      // Free resources.
      if (hToken)
         CloseHandle(hToken);

      if (ptiUser)
         HeapFree(GetProcessHeap(), 0, ptiUser);
   }

   return fSuccess;
}
*/
	return nombre_pc;
}
	
char *GuruNet::dominio_a_pc(char *nombre_dominio)
{
	unsigned char *	buffer = NULL;
	WCHAR			wdominio[64];
	char			dominio[32];
	char			*pdc;
	WCHAR			wpdc[50];

	strcpy(dominio, nombre_dominio);

	if (MultiByteToWideChar(CP_ACP, 0, (const char *) dominio, strlen(dominio)+1, wdominio, sizeof(wdominio)/sizeof(wdominio[0])) == 0)
		{
		GuruNet_Error.descripcion();
		return NULL;
		}

//	wprintf(L"wdominio: %s\n", wdominio);
	// intento encontrar el dominio desde la maquina local, sino puedo uso el pdc
	NET_API_STATUS salida_api = NetGetAnyDCName(NULL, wdominio, &buffer);

//	wprintf(L"buffer: %s\n", buffer);

	if (salida_api != NERR_Success)
		{
//		GuruNet_Error.descripcion(salida_api);
//		printf("%s\n",GuruNet_Error.mensaje());

		nombre_pdc(&pdc);
		MultiByteToWideChar(CP_ACP, 0, (const char *) pdc, strlen(pdc)+1, wpdc, sizeof(wpdc)/sizeof(wpdc[0]));
		delete pdc;

//		wprintf(L"wdominio: %s - wpdc: %s\n", wdominio, wpdc);
		salida_api = NetGetAnyDCName(wpdc, wdominio, &buffer);
		if (salida_api != NERR_Success)
			{
//			printf("salio por error\n");
			char   nombre_pc[MAX_COMPUTERNAME_LENGTH + 2];
			DWORD  largo=MAX_COMPUTERNAME_LENGTH+1;//para win95
			char   *nombre_pc_pdc;

			GetComputerName(nombre_pc, &largo);

			nombre_pdc(&nombre_pc_pdc);

			// pregunto si estoy parado en un PDC, esta funcion falla si se ejecuta en un PDC (microsoft !!!!!!)
			if (stricmp(nombre_pc, &nombre_pc_pdc[2]) == 0)
				{
				strcpy(nombre_dominio, nombre_pc_pdc);
				delete nombre_pc_pdc;
				return nombre_dominio;
				}
			else
				delete nombre_pc_pdc;


			GuruNet_Error.descripcion(salida_api);
			return NULL;
			}
		}

	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)buffer, 50, nombre_dominio, 50, NULL, NULL);

	NetApiBufferFree(buffer);

	return nombre_dominio;
}

const char *GuruNet::mensaje_error()
{
	return GuruNet_Error.mensaje().c_str();
}

WCHAR * GuruNet::charawide(char *cadena,WCHAR *widecadena,DWORD tamanio)
{
	int salida;

	if ((salida = MultiByteToWideChar(CP_ACP, 0, (const char *)cadena, strlen(cadena), widecadena, tamanio)) == 0)
		return NULL;
	else
		return widecadena;
}

char * GuruNet::wideachar(WCHAR *widecadena,char *cadena,DWORD tamanio)
{
	if (WideCharToMultiByte(CP_ACP, 0,(LPCWSTR) widecadena, -1,cadena,tamanio,NULL,NULL) == 0)
		return NULL;
	else
		return cadena;
}

char *GuruNet::fecha(char *buffer,const long tiempo)
{
	struct tm *desde;
	
	desde = localtime((const time_t*)&tiempo);
	
	sprintf(buffer,"%02d/%02d/%04d",desde->tm_mday,desde->tm_mon+1,desde->tm_year+1900);

	return buffer;
}

char *GuruNet::hora(char *buffer,const long tiempo)
{
	struct tm *desde;

	desde = localtime((const time_t*)&tiempo);

	sprintf(buffer,"%02d:%02d:%02d",desde->tm_hour,desde->tm_min,desde->tm_sec);		

	return buffer;
}
