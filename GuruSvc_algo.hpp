#include <GuruSvc.h>

namespace GuruSvcAlgo {

	template <typename T>
	void iniciar_servicio(const string& nombre_interno,const string& nombre_display, int cant, char* parametro[], bool MsgOnDll=false) {
		T servicio(nombre_interno, nombre_display, MsgOnDll);

		if (cant >= 2) {
			if (stricmp(parametro[1], "-?") == 0 || stricmp(parametro[1], "/?") == 0 || stricmp(parametro[1], "-?") == 0 || stricmp(parametro[1], "/ayuda") == 0 ||
				stricmp(parametro[1], "/Instalar") == 0 || stricmp(parametro[1], "/DesInstalar") == 0 || stricmp(parametro[1], "/Manual") == 0 || stricmp(parametro[1], "/Automatico") == 0 || stricmp(parametro[1], "/Deshabilitado") == 0){

				servicio.ConfiguracionServicio(parametro[1]);

				return ;
			}

			if (stricmp(parametro[1], "/Apps") == 0) {
				servicio.ParametrosServicio(cant, parametro);
				servicio.StartApplication();

				return ;
			}
		}

		servicio.ParametrosServicio(cant, parametro);
		servicio.StartService();
	}

}