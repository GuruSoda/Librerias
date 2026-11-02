#ifndef __GURUCAPTURA_PANTALLA_H__
#define __GURUCAPTURA_PANTALLA_H__

#include <string>
#include <algorithm>
#include <wx/bitmap.h>
#include <wx/dcscreen.h>
#include <wx/dcmemory.h>

using namespace std;

namespace GuruCapturaPantalla {

	static wxBitmapType tipo_archivo(const string& archivo) {
		string::size_type lugar = archivo.rfind('.');

		if (lugar == string::npos) return wxBITMAP_TYPE_INVALID;

		string extension;

		extension = archivo.substr(lugar + 1);

		transform(extension.begin(), extension.end(), extension.begin(), tolower);

		if (extension.compare("png") == 0)
			return wxBITMAP_TYPE_PNG;
		else if (extension.compare("bmp") == 0)
			return wxBITMAP_TYPE_BMP;
		else if (extension.compare("jpg") == 0)
			return wxBITMAP_TYPE_JPEG;
		else if (extension.compare("jpeg") == 0)
			return wxBITMAP_TYPE_JPEG;
		else if (extension.compare("xpm") == 0)
			return wxBITMAP_TYPE_XPM;
		else if (extension.compare("xbm") == 0)
			return wxBITMAP_TYPE_XBM;
		else if (extension.compare("pcx") == 0)
			return wxBITMAP_TYPE_PCX;
		else if (extension.compare("gif") == 0)
			return wxBITMAP_TYPE_GIF;
		else return wxBITMAP_TYPE_INVALID;
	}

	static void capturar(const string& archivo) {
		wxSize screenSize = wxGetDisplaySize();
		wxBitmap bitmap(screenSize.x, screenSize.y);
		wxScreenDC dc;
		wxMemoryDC memDC;
		memDC.SelectObject(bitmap);
		memDC.Blit(0, 0, screenSize.x, screenSize.y, &dc, 0, 0);
		memDC.SelectObject(wxNullBitmap);

		wxBitmapType tipo = GuruCapturaPantalla::tipo_archivo(archivo);

		if (tipo == wxBITMAP_TYPE_INVALID) return;

		bitmap.SaveFile(archivo, tipo);
	}

};

#endif