#ifndef __GURUIMAGEPANEL_HPP__
#define __GURUIMAGEPANEL_HPP__

#include <wx/wx.h>
#include <wx/image.h>

/*
Basado en el siguiente ejemplo:
https://wiki.wxwidgets.org/An_image_panel
otro:
https://github.com/m13253/wx-image-viewer/blob/master/imageview.cpp
*/
class wxImagePanel : public wxPanel {
	wxImage		image;
	wxBitmap	bitImage;
public:
	wxImagePanel(wxFrame* parent) :wxPanel(parent) {
		Connect(wxEVT_PAINT, wxPaintEventHandler(wxImagePanel::paintEvent), NULL, this);
	}

	wxImagePanel(wxFrame* parent, wxString file):wxPanel(parent) {
		cargarImagen(file);
		Connect(wxEVT_PAINT, wxPaintEventHandler(wxImagePanel::paintEvent), NULL, this);
	}

	bool cargarImagen(const wxString& file) {
		// load the file... ideally add a check to see if loading was successful
		if (image.Ok()) 
			image.Destroy();

		if (!image.LoadFile(file))
			return false;

		paintNow();

		return true;
	}

	void paintEvent(wxPaintEvent & evt) {
		// depending on your system you may need to look at double-buffered dcs
		wxPaintDC dc(this);
		render(dc);
	}

	void paintNow() {
		// depending on your system you may need to look at double-buffered dcs
		wxClientDC dc(this);
		render(dc);
	}

	void render(wxDC& dc) {
		bitImage = wxBitmap(image.Scale(dc.GetSize().GetWidth(), dc.GetSize().GetHeight()));
		dc.DrawBitmap(bitImage, 0, 0, false);
	}
};

#endif
