#ifndef __GURUDOMAINNT_HPP__
#define __GURUDOMAINNT_HPP__

//////////////////////////////////////////////////////////////////////
//	Link: Netapi32.lib
//////////////////////////////////////////////////////////////////////

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

/* // otra opcion para lo anterior
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <winsock2.h>
*/

#include <string>
#include <vector>

#include <windows.h>
#include <winsock2.h>
#include <lm.h>
#include <Lmaccess.h>
#include <Lmapibuf.h>
#include <dsgetdc.h>

#include <GuruError.hpp>
#include <GuruNT.hpp>
#include <GuruString_algo.hpp>

namespace GuruDominioNT {

	static std::string nombreDominio() {
		WKSTA_INFO_100* bufptr = NULL;
		std::string nombreDominio;
		NET_API_STATUS codigo;
		
		if ((codigo = NetWkstaGetInfo(NULL, 100, (LPBYTE *)&bufptr)) != NERR_Success) throw GuruNT::GuruNTException(GuruError().descripcion(codigo));

		nombreDominio = GuruStringAlgo::unicode_ansi(bufptr->wki100_langroup);

		NetApiBufferFree(bufptr);

		return nombreDominio;
	}

	static std::vector<std::string>& nombresControladoresDominio(std::vector<std::string>& controladores) {
		DWORD dwRet;
		PDOMAIN_CONTROLLER_INFO pdcInfo;

		// Get a domain controller for the domain this computer is on.
		dwRet = DsGetDcName(NULL, NULL, NULL, NULL, 0, &pdcInfo);
		if (ERROR_SUCCESS == dwRet) {
			HANDLE hGetDc;

			// Open the enumeration.
			dwRet = DsGetDcOpen(pdcInfo->DomainName,
				DS_NOTIFY_AFTER_SITE_RECORDS,
				NULL,
				NULL,
				NULL,
				0,
				&hGetDc);
			if (ERROR_SUCCESS == dwRet) {
				LPTSTR pszDnsHostName;

				/*
				Enumerate each domain controller and print its name to the
				debug window.
				*/
				while (TRUE) {
					ULONG ulSocketCount;
					LPSOCKET_ADDRESS rgSocketAddresses;

					dwRet = DsGetDcNext(
						hGetDc,
						&ulSocketCount,
						&rgSocketAddresses,
						&pszDnsHostName);

					if (ERROR_SUCCESS == dwRet) {
						OutputDebugString(pszDnsHostName);
						OutputDebugString(TEXT("\n"));

						// Free the allocated string.
						NetApiBufferFree(pszDnsHostName);

						// Free the socket address array.
						LocalFree(rgSocketAddresses);
					} else if (ERROR_NO_MORE_ITEMS == dwRet) {
						// The end of the list has been reached.
						break;
					} else if (ERROR_FILEMARK_DETECTED == dwRet) {
						/*
						DS_NOTIFY_AFTER_SITE_RECORDS was specified in
						DsGetDcOpen and the end of the site-specific
						records was reached.
						*/
						OutputDebugString(
							TEXT("End of site-specific domain controllers\n"));
						continue;
					} else {
						// Some other error occurred.
						break;
					}
				}

				// Close the enumeration.
				DsGetDcClose(hGetDc);
			}

			// Free the DOMAIN_CONTROLLER_INFO structure. 
			NetApiBufferFree(pdcInfo);
		}
		return controladores;
	}

	static std::string nombreConstroladorDominio(const std::string& dominio) {
/*
		LPCWSTR lpServer = NULL;
		LPCWSTR lpDomain = GuruStringAlgo::ansi_unicode(dominio);
		LPCWSTR lpDcName = NULL;
		std::string controladorDominio;

		NET_API_STATUS codigo = NetGetDCName(lpServer, lpDomain, (LPBYTE *)&lpDcName);

		controladorDominio = GuruStringAlgo::unicode_ansi(lpDcName);

		NetApiBufferFree(bufptr);


		DSGETDCAPI DWORD DsGetDcNameA(
		LPCSTR                   ComputerName,
		LPCSTR                   DomainName,
		GUID                     *DomainGuid,
		LPCSTR                   SiteName,
		ULONG                    Flags,
		PDOMAIN_CONTROLLER_INFOA *DomainControllerInfo
		);

*/
		return std::string();
	}

	// Retorno cada cuantos dias hay que cambiar la clave.
	static int maxPasswordAge(const std::string& computadora="") {
		USER_MODALS_INFO_0* pBuf = NULL;
		NET_API_STATUS codigo;
		int totalDias = 0;

		if ((codigo = NetUserModalsGet(GuruStringAlgo::ansi_unicode(computadora).c_str(), 0, (LPBYTE *)&pBuf)) != NERR_Success) throw GuruNT::GuruNTException(GuruError().descripcion(codigo));
		
		totalDias = ((pBuf->usrmod0_max_passwd_age / 60) / 60) / 24;
		
		NetApiBufferFree(pBuf);

		return  totalDias;	
	}
}

#endif
