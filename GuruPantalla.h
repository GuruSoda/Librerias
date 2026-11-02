#if !defined __GURUPANTALLA_H__
#define __GURUPANTALLA_H__

#include "GuruCompat.h"
#include <windows.h>
#include "GuruTiposDeDatos.h"

class GuruPantalla
{
public:
	GuruPantalla();
	virtual ~GuruPantalla();

	bool	salvar_pantalla_jpg(const char* archivo);
	bool	salvar_pantalla_bmp(const char* archivo);

	bool	cargar_pantalla(unsigned char** pantalla);
	bool	poner_pantalla(unsigned char* pantalla);
	dword	cargar_pantalla_GetPixel(unsigned char** pantalla);
	dword	poner_pantalla_SetPixel(unsigned char* pantalla);

private:

	bool	armar_encabezados();
	bool	escribir_archivo(const char* nombre_archivo);

private:
	HDC			GuruDC;
	HDC			GuruCDC;
	HBITMAP		GuruCBitmap;
	BITMAPINFO*	GuruHeaderBmp;
};

#endif // !defined __GURUPANTALLA_H__

