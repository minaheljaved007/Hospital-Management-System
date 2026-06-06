// wxWidgets application entry point
// Hospital Management System

#include "App.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    MainFrame* frame = new MainFrame("City Care Hospital — Management System");
    frame->SetMinSize(wxSize(1280, 750));
    frame->Show(true);
    frame->Centre();
    return true;
}