#if !defined __GURUCORRER_H__
#define __GURUCORRER_H__

#include "GuruCompat.h"
#include "GuruError.h"

class GuruCorrer  
{
public:
	GuruCorrer();
	virtual ~GuruCorrer();

	bool	ejecutar(char *ejecutable, bool espera = false, word ventana = SW_SHOWNORMAL);
	const char*	mensaje_error();

protected:
	SECURITY_ATTRIBUTES * cargar_SD_generico(SECURITY_ATTRIBUTES *seguridad);

	GuruError	GuruCorrer_error;
};

#endif // !defined __GURUCORRER_H__

