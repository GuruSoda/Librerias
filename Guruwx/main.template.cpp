#include <wx/wx.h>

// Archivo generado por wxFormbuilder
#include "Catalogadorwsclui.h"

// reemplazar UI_MainFrame por el nombre de la clase generada en el .h por wxFormbuilder
class ui : public UI_MainFrame {
protected:
	void OnSalirClick(wxCommandEvent& event) { OutputDebugString(_T("Salir!"));}
public:
	ui(wxWindow* parent):UI_MainFrame(parent) {}
};

class wxWidgetsApp : public wxApp
{
public:
    wxWidgetsApp() {};
    virtual ~wxWidgetsApp() {};
    virtual bool OnInit();
};

DECLARE_APP(wxWidgetsApp)

IMPLEMENT_APP(wxWidgetsApp)

bool wxWidgetsApp::OnInit()
{
	ui* dialog = new ui( (wxWindow*)NULL );
	dialog->Show();
	SetTopWindow( dialog );
	return true;
}
