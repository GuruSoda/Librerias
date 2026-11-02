#ifndef __GURUBALLOON_H__
#define __GURUBALLOON_H__

#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/dcclient.h>

enum {
	TIMER_BALOON=99
};

class TaskBarBaloon : public wxFrame
{
public:
    TaskBarBaloon(wxString sTitle, wxString sMessage);
    virtual ~TaskBarBaloon() { delete timer; }

    /** painting bg */
    void OnPaint(wxPaintEvent& event);
    /** timer to close window */
    void OnTimerTick(wxTimerEvent & event);
    /** click on the baloon */
	void OnClick(wxMouseEvent & event);

    /** display the baloon and run the timer */
    void showBaloon(unsigned int iTimeoutSec);
private:
    wxTimer* timer;

    DECLARE_EVENT_TABLE();
};


#endif
