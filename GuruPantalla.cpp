#include "GuruPantalla.h"
// #include <LibJpeg\jpegfile.h>

GuruPantalla::GuruPantalla()
{

}

GuruPantalla::~GuruPantalla()
{

}

bool GuruPantalla::salvar_pantalla_jpg(const char *archivo)
{
	bool salida = true;

	return salida;
}

bool GuruPantalla::salvar_pantalla_bmp(const char *archivo)
{
	bool salida = true;

	GuruDC = GetDC(NULL); // saco en DC de la pantalla completa

	GuruCDC = CreateCompatibleDC(GuruDC); // creo uno compatible para pasarlo a disco.

	int alto, ancho;

	alto = GetDeviceCaps(GuruDC, VERTRES);
	ancho = GetDeviceCaps(GuruDC, HORZRES);

	// width = ancho
	// height = alto
	GuruCBitmap = CreateCompatibleBitmap(GuruDC, ancho, alto);

	// selecciono al bitmap dentro del compatible bitmap
	SelectObject(GuruCDC, GuruCBitmap);

	// copio la zona de memoria (DC) a la nuevo zona de memoria creada (CDC)
	BitBlt(GuruCDC, 0,0, ancho, alto, GuruDC, 0,0, SRCCOPY);

	salida = escribir_archivo(archivo);

	DeleteObject(GuruCBitmap);

	DeleteDC(GuruCDC);

	ReleaseDC(NULL, GuruDC);

	return salida;
}


//////////////////////////////////////////////////////////////////////
// Tarea:	Crear los encabezado de un .BMP
// Nota:
//			Sacado de un ejemplo del MSDN
//////////////////////////////////////////////////////////////////////
bool GuruPantalla::armar_encabezados()
{
	bool	salida = true;
    BITMAP	bmp;
    WORD    cant_bits;

    // Saco informacion acerca del bitmap (color format, width, and height)
    if (!GetObject(GuruCBitmap, sizeof(BITMAP), (char *)&bmp))
        return false;

	// Convierto el formato del color a cantidad de Bits
	cant_bits = (word)(bmp.bmPlanes * bmp.bmBitsPixel);

	if (cant_bits == 1)
		cant_bits = 1;
	else if (cant_bits <= 4)
		cant_bits = 4;
	else if (cant_bits <= 8)
		cant_bits = 8;
	else if (cant_bits <= 16)
		cant_bits = 16;
	else if (cant_bits <= 24)
		cant_bits = 24;
	else cant_bits = 32;

//	cant_bits = 4;

	// Pido memoria para la estructura BITMAPINFO (GuruHeaderBmp)
	// Original:
	// Allocate memory for the BITMAPINFO structure. (This structure
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD
	// data structures.)

//	DWORD total = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cant_bits);

	unsigned int tam = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cant_bits);

	DWORD total = 1<< cant_bits;

	if (cant_bits != 24)
		GuruHeaderBmp = (BITMAPINFO *) new char [sizeof(BITMAPINFOHEADER) +
												sizeof(RGBQUAD) * (1<< cant_bits)];

	// There is no RGBQUAD array for the 24-bit-per-pixel format.

	else
		GuruHeaderBmp = (BITMAPINFO *) new BITMAPINFOHEADER;

	// Initialize the fields in the BITMAPINFO structure.
	GuruHeaderBmp->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GuruHeaderBmp->bmiHeader.biWidth = bmp.bmWidth;
	GuruHeaderBmp->bmiHeader.biHeight = bmp.bmHeight;
	GuruHeaderBmp->bmiHeader.biPlanes = bmp.bmPlanes;
	GuruHeaderBmp->bmiHeader.biBitCount = bmp.bmBitsPixel;

	if (cant_bits < 24)
		GuruHeaderBmp->bmiHeader.biClrUsed = (1<<cant_bits);

	// If the bitmap is not compressed, set the BI_RGB flag.
	GuruHeaderBmp->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color
	// indices and store the result in biSizeImage.
	// For Windows NT/2000, the width must be DWORD aligned unless
	// the bitmap is RLE compressed. This example shows this.
	// For Windows 95/98, the width must be WORD aligned unless the
	// bitmap is RLE compressed.
	unsigned long tamanio = ((GuruHeaderBmp->bmiHeader.biWidth * cant_bits) / 8) * GuruHeaderBmp->bmiHeader.biHeight;

	GuruHeaderBmp->bmiHeader.biSizeImage = ((GuruHeaderBmp->bmiHeader.biWidth * cant_bits +31) & ~31) /8
								  * GuruHeaderBmp->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the
	// device colors are important.
	GuruHeaderBmp->bmiHeader.biClrImportant = 0;

	return salida;
}

bool GuruPantalla::escribir_archivo(const char *nombre_archivo)
{
	bool salida = true;

	HANDLE archivo;						// file handle
	BITMAPFILEHEADER header_archivo;	// bitmap file-header
	PBITMAPINFOHEADER header_data;		// bitmap info-header
	unsigned char *imagen;						// memory pointer
	dword dwTotal;						// total count of bytes
	dword cb;							// incremental count of bytes
	dword dwTmp;

// armo el encabezado del archivo .bmp
	armar_encabezados();

	header_data = (BITMAPINFOHEADER *) GuruHeaderBmp;
	imagen = new unsigned char [header_data->biSizeImage];

    if (!imagen) return false;

    // Retrieve the color table (RGBQUAD array) and the bits
    // (array of palette indices) from the DIB.
    if (!GetDIBits(GuruCDC, GuruCBitmap, 0, (word) header_data->biHeight, imagen, GuruHeaderBmp, DIB_RGB_COLORS))
		{
        return false;
		}

    // Create the .BMP file.
    archivo = CreateFile(nombre_archivo,
                   GENERIC_READ | GENERIC_WRITE,
                   (dword) 0,
                    NULL,
                   CREATE_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL,
                   (HANDLE) NULL);

    if (archivo == INVALID_HANDLE_VALUE)
        return false;

    header_archivo.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"

    // Compute the size of the entire file.
    header_archivo.bfSize = (dword) (sizeof(BITMAPFILEHEADER) +
                 header_data->biSize + header_data->biClrUsed
                 * sizeof(RGBQUAD) + header_data->biSizeImage);
    header_archivo.bfReserved1 = 0;
    header_archivo.bfReserved2 = 0;

    // Compute the offset to the array of color indices.
    header_archivo.bfOffBits = (dword) sizeof(BITMAPFILEHEADER) +
                    header_data->biSize + header_data->biClrUsed
                    * sizeof (RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.
    if (!WriteFile(archivo, (LPVOID) &header_archivo, sizeof(BITMAPFILEHEADER),
        (LPDWORD) &dwTmp,  NULL))
    {
	return false;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
    if (!WriteFile(archivo, (LPVOID) header_data, sizeof(BITMAPINFOHEADER)
                  + header_data->biClrUsed * sizeof (RGBQUAD),
                  (LPDWORD) &dwTmp, ( NULL)) )
       return false;

    // Copy the array of color indices into the .BMP file.
    dwTotal = cb = header_data->biSizeImage;

    if (!WriteFile(archivo, (char *) imagen, (int) cb, (LPDWORD) &dwTmp,NULL))
           return false;

    // Close the .BMP file.
     if (!CloseHandle(archivo))
           return false;

    // Free memory.
	delete imagen;

	return salida;
}

/*
bool GuruPantalla::cargar_pantalla(unsigned char** pantalla)
{
	HDC GuruDC = GetDC(NULL); // saco en DC de la pantalla completa

	int alto, ancho;

	alto = GetDeviceCaps(GuruDC, VERTRES);
	ancho = GetDeviceCaps(GuruDC, HORZRES);

	HDC GuruCDC = CreateCompatibleDC(GuruDC);

	// width = ancho
	// height = alto
	HBITMAP	 GuruCBitmap = CreateCompatibleBitmap(GuruDC, ancho, alto);

	// selecciono al bitmap dentro del compatible bitmap
	SelectObject(GuruCDC, GuruCBitmap);

	// copio la zona de memoria (DC) a la nuevo zona de memoria creada (CDC)
	BitBlt(GuruCDC, 0, 0, ancho, alto, GuruDC, 0, 0, SRCCOPY);

    BITMAP	data_bmp;

    // Saco informacion acerca del bitmap (color format, width, and height)
	if (!GetObject(GuruCBitmap, sizeof(BITMAP), (char *)&data_bmp))
		return false;


	DWORD tamanio_imagen = (data_bmp.bmHeight * data_bmp.bmWidth) * ((data_bmp.bmBitsPixel * data_bmp.bmPlanes) / 8);

	*pantalla = new unsigned char [tamanio_imagen + 2];

	BITMAPINFO * header_data = (BITMAPINFO *) new char [sizeof(BITMAPINFOHEADER)];

	memset(header_data, 0, sizeof(BITMAPINFOHEADER));

	header_data->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	header_data->bmiHeader.biWidth = data_bmp.bmWidth;
	header_data->bmiHeader.biHeight = data_bmp.bmHeight;
	header_data->bmiHeader.biPlanes = data_bmp.bmPlanes;
	header_data->bmiHeader.biBitCount = data_bmp.bmBitsPixel;
	header_data->bmiHeader.biCompression = BI_RGB;
	header_data->bmiHeader.biSizeImage = ((alto*ancho)*data_bmp.bmBitsPixel)/8; //tamanio_imagen;
	header_data->bmiHeader.biClrUsed = 0;
	header_data->bmiHeader.biClrImportant = 0;

	int canti = GetDIBits(GuruCDC, GuruCBitmap, 0, data_bmp.bmHeight, *pantalla, header_data , DIB_RGB_COLORS);

	bool salida = JpegFile::RGBToJpegFile("aver.jpg", (unsigned char*)*pantalla, data_bmp.bmWidth, data_bmp.bmHeight, true, 80);

	if (canti == 0) MessageBox(NULL, "", "Basta !!!!", 0);

	DeleteDC(GuruCDC);

	DeleteObject(GuruCBitmap);

	ReleaseDC(NULL, GuruDC);

	return true;
}
*/
bool GuruPantalla::poner_pantalla(unsigned char* pantalla)
{
	int		alto, ancho;
	HDC		GuruCDC;
	HBITMAP	GuruCBitmap;

	HDC GuruDC = GetDC(NULL); // saco en DC de la pantalla completa

	alto = GetDeviceCaps(GuruDC, VERTRES);
	ancho = GetDeviceCaps(GuruDC, HORZRES);

	GuruCDC = CreateCompatibleDC(GuruDC);

	GuruCBitmap = CreateCompatibleBitmap(GuruDC, ancho, alto);

// selecciono al bitmap dentro del compatible bitmap
	SelectObject(GuruCDC, GuruCBitmap);

    BITMAP	data_bmp;

    // Saco informacion acerca del bitmap (color format, width, and height)
	if (!GetObject(GuruCBitmap, sizeof(BITMAP), (char *)&data_bmp))
		return false;

	DWORD tamanio_imagen = (data_bmp.bmHeight * data_bmp.bmWidth) * 4;

	// copio la zona de memoria (DC) a la nuevo zona de memoria creada (CDC)
	BitBlt(GuruDC, 0, 0, ancho, alto, GuruCDC, 0, 0, SRCCOPY);

	DeleteDC(GuruCDC);

	DeleteObject(GuruCBitmap);

	ReleaseDC(NULL, GuruDC);

/*
	unsigned char* aux_pantalla = pantalla;
	DWORD pixel;

	for (int y = alto; y >= 1 ; y--)
		{
		for (int x = 0; x < ancho; x++)
			{
			memcpy(&pixel, pantalla, sizeof(DWORD));

			pixel = SetPixel(GuruDC, x, y-1, pixel);

			pantalla += sizeof(COLORREF);
			}
		}

	pantalla = aux_pantalla;
*/
	return true;
}

dword GuruPantalla::cargar_pantalla_GetPixel(unsigned char** pantalla)
{
	int		alto, ancho;
	HDC		GuruCDC;
	HBITMAP	GuruCBitmap;

	HDC GuruDC = GetDC(NULL); // saco en DC de la pantalla completa

	alto = GetDeviceCaps(GuruDC, VERTRES);
	ancho = GetDeviceCaps(GuruDC, HORZRES);

	GuruCDC = CreateCompatibleDC(GuruDC);

	GuruCBitmap = CreateCompatibleBitmap(GuruDC, ancho, alto);

// selecciono al bitmap dentro del compatible bitmap
	SelectObject(GuruCDC, GuruCBitmap);

	// copio la zona de memoria (DC) a la nuevo zona de memoria creada (CDC)
	BitBlt(GuruCDC, 0, 0, ancho, alto, GuruDC, 0, 0, SRCCOPY);

    BITMAP	data_bmp;

    // Saco informacion acerca del bitmap (color format, width, and height)
	if (!GetObject(GuruCBitmap, sizeof(BITMAP), (char *)&data_bmp))
		return 0;

	DWORD tamanio_imagen = (data_bmp.bmHeight * data_bmp.bmWidth) * 4;

	*pantalla = new unsigned char [tamanio_imagen + 2];

	unsigned char *aux_pantalla = *pantalla;
//	DWORD pixel;

	for (int y = alto; y >= 1 ; y--)
		{
		for (int x = 0; x < ancho; x++)
			{
			*((COLORREF*)(*pantalla)) = GetPixel(GuruCDC, x, y-1);
			//pixel = GetPixel(GuruCDC, x, y-1);
			//memcpy(*pantalla, &pixel, sizeof(DWORD));

			*pantalla += sizeof(COLORREF);
			}
		}

	*pantalla = aux_pantalla;

	DeleteDC(GuruCDC);

	DeleteObject(GuruCBitmap);

	ReleaseDC(NULL, GuruDC);


	return tamanio_imagen;
}

dword GuruPantalla::poner_pantalla_SetPixel(unsigned char *pantalla)
{
	int		alto, ancho;
	HDC		GuruCDC;
	HBITMAP	GuruCBitmap;

	HDC GuruDC = GetDC(NULL); // saco en DC de la pantalla completa

	alto = GetDeviceCaps(GuruDC, VERTRES);
	ancho = GetDeviceCaps(GuruDC, HORZRES);

	GuruCDC = CreateCompatibleDC(GuruDC);

	GuruCBitmap = CreateCompatibleBitmap(GuruDC, ancho, alto);

// selecciono al bitmap dentro del compatible bitmap
	SelectObject(GuruCDC, GuruCBitmap);

    BITMAP	data_bmp;

    // Saco informacion acerca del bitmap (color format, width, and height)
	if (!GetObject(GuruCBitmap, sizeof(BITMAP), (char *)&data_bmp))
		return 0;

	DWORD tamanio_imagen = (data_bmp.bmHeight * data_bmp.bmWidth) * 4;

	unsigned char *aux_pantalla = pantalla;
	DWORD pixel;

	for (int y = alto; y >= 1 ; y--)
		{
		for (int x = 0; x < ancho; x++)
			{
			memcpy(&pixel, pantalla, sizeof(DWORD));

			pixel = SetPixel(GuruDC, x, y-1, pixel);

			pantalla += sizeof(COLORREF);
			}
		}

	pantalla = aux_pantalla;

	return tamanio_imagen;
}
