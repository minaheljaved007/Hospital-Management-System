// MainFrame.cpp
// Full implementation of Hospital Management System GUI
// wxWidgets | C++17

#include "MainFrame.h"
#include <wx/dcbuffer.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include <wx/wfstream.h>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

// ═══════════════════════════════════════════════════════════════════
//  EVENT TABLE
// ═══════════════════════════════════════════════════════════════════
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    // Navigation
    EVT_BUTTON(ID_NAV_DASHBOARD,    MainFrame::OnNav)
    EVT_BUTTON(ID_NAV_PATIENTS,     MainFrame::OnNav)
    EVT_BUTTON(ID_NAV_DOCTORS,      MainFrame::OnNav)
    EVT_BUTTON(ID_NAV_APPOINTMENTS, MainFrame::OnNav)
    EVT_BUTTON(ID_NAV_BILLING,      MainFrame::OnNav)
    EVT_BUTTON(ID_NAV_PHARMACY,     MainFrame::OnNav)
    EVT_BUTTON(ID_NAV_REPORTS,      MainFrame::OnNav)
    EVT_BUTTON(ID_NAV_EXIT,         MainFrame::OnExit)
    // Patient
    EVT_BUTTON(ID_PATIENT_ADD,      MainFrame::OnPatientAdd)
    EVT_BUTTON(ID_PATIENT_UPDATE,   MainFrame::OnPatientUpdate)
    EVT_BUTTON(ID_PATIENT_DELETE,   MainFrame::OnPatientDelete)
    EVT_BUTTON(ID_PATIENT_CLEAR,    MainFrame::OnPatientClear)
    EVT_LIST_ITEM_SELECTED(ID_PATIENT_LIST, MainFrame::OnPatientSelect)
    // Doctor
    EVT_BUTTON(ID_DOCTOR_ADD,       MainFrame::OnDoctorAdd)
    EVT_BUTTON(ID_DOCTOR_UPDATE,    MainFrame::OnDoctorUpdate)
    EVT_BUTTON(ID_DOCTOR_DELETE,    MainFrame::OnDoctorDelete)
    EVT_BUTTON(ID_DOCTOR_CLEAR,     MainFrame::OnDoctorClear)
    EVT_LIST_ITEM_SELECTED(ID_DOCTOR_LIST, MainFrame::OnDoctorSelect)
    // Appointment
    EVT_BUTTON(ID_APPT_ADD,         MainFrame::OnApptAdd)
    EVT_BUTTON(ID_APPT_CANCEL,      MainFrame::OnApptCancel)
    EVT_BUTTON(ID_APPT_COMPLETE,    MainFrame::OnApptComplete)
    EVT_BUTTON(ID_APPT_CLEAR,       MainFrame::OnApptClear)
    EVT_LIST_ITEM_SELECTED(ID_APPT_LIST, MainFrame::OnApptSelect)
    // Billing
    EVT_BUTTON(ID_BILL_GENERATE,    MainFrame::OnBillGenerate)
    EVT_BUTTON(ID_BILL_CLEAR,       MainFrame::OnBillClear)
    EVT_LIST_ITEM_SELECTED(ID_BILL_LIST, MainFrame::OnBillSelect)
    // Pharmacy
    EVT_BUTTON(ID_PHARMA_ADD,       MainFrame::OnPharmaAdd)
    EVT_BUTTON(ID_PHARMA_CLEAR,     MainFrame::OnPharmaClear)
    EVT_LIST_ITEM_SELECTED(ID_PHARMA_LIST, MainFrame::OnPharmaSelect)
    // Reports
    EVT_BUTTON(ID_REPORT_GENERATE,  MainFrame::OnReportGenerate)
    // Timer
    EVT_TIMER(ID_TIMER_CLOCK,       MainFrame::OnTimer)
wxEND_EVENT_TABLE()

// ═══════════════════════════════════════════════════════════════════
//  CONSTRUCTOR
// ═══════════════════════════════════════════════════════════════════
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title,
              wxDefaultPosition, wxSize(1280, 760),
              wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL),
      m_clockTimer(this, ID_TIMER_CLOCK)
{
    SetBackgroundColour(CLR_BG);

    BuildLayout();

    // Start the clock
    m_clockTimer.Start(1000);

    // Show dashboard by default
    ShowPage(m_pageDashboard);
    RefreshDashboard();

    Centre();
}

// ═══════════════════════════════════════════════════════════════════
//  BUILD HELPERS
// ═══════════════════════════════════════════════════════════════════

void MainFrame::BuildLayout()
{
    auto* rootSizer = new wxBoxSizer(wxHORIZONTAL);

    // Sidebar (fixed 220px wide)
    m_sidebar = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(220, -1));
    m_sidebar->SetBackgroundColour(CLR_SIDEBAR);
    BuildSidebar();

    // Content area
    m_contentHost = new wxPanel(this, wxID_ANY);
    m_contentHost->SetBackgroundColour(CLR_BG);
    m_contentSizer = new wxBoxSizer(wxVERTICAL);
    m_contentHost->SetSizer(m_contentSizer);

    // Build ALL pages (hidden initially)
    m_pageDashboard    = BuildDashboardPage(m_contentHost);
    m_pagePatients     = BuildPatientsPage(m_contentHost);
    m_pageDoctors      = BuildDoctorsPage(m_contentHost);
    m_pageAppointments = BuildAppointmentsPage(m_contentHost);
    m_pageBilling      = BuildBillingPage(m_contentHost);
    m_pagePharmacy     = BuildPharmacyPage(m_contentHost);
    m_pageReports      = BuildReportsPage(m_contentHost);

    // Add all pages to sizer, then hide
    m_contentSizer->Add(m_pageDashboard,    1, wxEXPAND);
    m_contentSizer->Add(m_pagePatients,     1, wxEXPAND);
    m_contentSizer->Add(m_pageDoctors,      1, wxEXPAND);
    m_contentSizer->Add(m_pageAppointments, 1, wxEXPAND);
    m_contentSizer->Add(m_pageBilling,      1, wxEXPAND);
    m_contentSizer->Add(m_pagePharmacy,     1, wxEXPAND);
    m_contentSizer->Add(m_pageReports,      1, wxEXPAND);

    rootSizer->Add(m_sidebar,      0, wxEXPAND);
    rootSizer->Add(m_contentHost,  1, wxEXPAND);
    SetSizer(rootSizer);
}

// ─── Sidebar ───────────────────────────────────────────────────────
void MainFrame::BuildSidebar()
{
    auto* sz = new wxBoxSizer(wxVERTICAL);

    // Logo / hospital name
    auto* logoPanel = new wxPanel(m_sidebar, wxID_ANY);
    logoPanel->SetBackgroundColour(wxColour(5, 10, 25));
    auto* logoSz = new wxBoxSizer(wxVERTICAL);

    auto* hosp = new wxStaticText(logoPanel, wxID_ANY, wxT("🏥 City Care"));
    hosp->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    hosp->SetForegroundColour(CLR_ACCENT);

    auto* sub = new wxStaticText(logoPanel, wxID_ANY, wxT("Hospital System"));
    sub->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    sub->SetForegroundColour(CLR_MUTED);

    // Clock label
    m_clockLabel = new wxStaticText(logoPanel, wxID_ANY, wxT("--:--:--"));
    m_clockLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    m_clockLabel->SetForegroundColour(CLR_ACCENT2);

    logoSz->Add(hosp,         0, wxALL, 8);
    logoSz->Add(sub,          0, wxLEFT | wxBOTTOM, 8);
    logoSz->Add(m_clockLabel, 0, wxLEFT | wxBOTTOM, 8);
    logoPanel->SetSizer(logoSz);
    sz->Add(logoPanel, 0, wxEXPAND);

    // Separator
    auto* line1 = new wxStaticLine(m_sidebar, wxID_ANY);
    line1->SetBackgroundColour(CLR_BORDER);
    sz->Add(line1, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    sz->AddSpacer(6);

    // Navigation buttons
    struct NavItem { wxString label; int id; };
    const NavItem items[] = {
        { wxT("  Dashboard"),     ID_NAV_DASHBOARD    },
        { wxT("  Patients"),      ID_NAV_PATIENTS     },
        { wxT("  Doctors"),       ID_NAV_DOCTORS      },
        { wxT("  Appointments"),  ID_NAV_APPOINTMENTS },
        { wxT("  Billing"),       ID_NAV_BILLING      },
        { wxT("  Pharmacy"),      ID_NAV_PHARMACY     },
        { wxT("  Reports"),       ID_NAV_REPORTS      },
    };
    for (auto& item : items)
        sz->Add(MakeNavButton(m_sidebar, item.label, item.id), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 6);

    sz->AddStretchSpacer(1);

    // Divider before exit
    auto* line2 = new wxStaticLine(m_sidebar, wxID_ANY);
    line2->SetBackgroundColour(CLR_BORDER);
    sz->Add(line2, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    sz->AddSpacer(6);
    sz->Add(MakeNavButton(m_sidebar, wxT("  Exit System"), ID_NAV_EXIT), 0, wxEXPAND | wxALL, 6);

    m_sidebar->SetSizer(sz);
}

// ═══════════════════════════════════════════════════════════════════
//  PAGE BUILDERS
// ═══════════════════════════════════════════════════════════════════

// ─── Dashboard ─────────────────────────────────────────────────────
wxPanel* MainFrame::BuildDashboardPage(wxWindow* parent)
{
    auto* page = new wxPanel(parent, wxID_ANY);
    page->SetBackgroundColour(CLR_BG);
    auto* outer = new wxBoxSizer(wxVERTICAL);

    // Header bar
    auto* hdr = new wxPanel(page, wxID_ANY);
    hdr->SetBackgroundColour(CLR_CARD);
    auto* hdrSz = new wxBoxSizer(wxHORIZONTAL);
    auto* hdrTitle = MakeLabel(hdr, wxT("Dashboard Overview"), 18, true, CLR_TEXT);
    auto* hdrSub   = MakeLabel(hdr, wxT("Real-time hospital statistics"), 10, false, CLR_MUTED);
    auto* hdrVSz   = new wxBoxSizer(wxVERTICAL);
    hdrVSz->Add(hdrTitle, 0);
    hdrVSz->Add(hdrSub,   0, wxTOP, 2);
    hdrSz->Add(hdrVSz, 1, wxALL | wxALIGN_CENTER_VERTICAL, 16);
    hdr->SetSizer(hdrSz);
    outer->Add(hdr, 0, wxEXPAND);
    outer->AddSpacer(16);

    // Stat cards row
    auto* cardRow = new wxBoxSizer(wxHORIZONTAL);
    cardRow->Add(MakeStatCard(page, wxT("Total Patients"),     wxT("0"), CLR_ACCENT,  &m_statPatients), 1, wxEXPAND | wxLEFT | wxRIGHT, 8);
    cardRow->Add(MakeStatCard(page, wxT("Total Doctors"),      wxT("0"), CLR_GREEN,   &m_statDoctors),  1, wxEXPAND | wxLEFT | wxRIGHT, 8);
    cardRow->Add(MakeStatCard(page, wxT("Appointments Today"), wxT("0"), CLR_ORANGE,  &m_statAppts),    1, wxEXPAND | wxLEFT | wxRIGHT, 8);
    cardRow->Add(MakeStatCard(page, wxT("Total Revenue"),      wxT("Rs. 0"), CLR_YELLOW, &m_statRevenue), 1, wxEXPAND | wxLEFT | wxRIGHT, 8);
    outer->Add(cardRow, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
    outer->AddSpacer(20);

    // Recent Appointments preview
    auto* recPanel = new wxPanel(page, wxID_ANY);
    recPanel->SetBackgroundColour(CLR_CARD);
    auto* recSz = new wxBoxSizer(wxVERTICAL);
    recSz->Add(MakeLabel(recPanel, wxT("Recent Appointments"), 13, true, CLR_TEXT), 0, wxALL, 14);
    auto* line = new wxStaticLine(recPanel, wxID_ANY);
    line->SetBackgroundColour(CLR_BORDER);
    recSz->Add(line, 0, wxEXPAND | wxLEFT | wxRIGHT, 14);

    auto* apptPrev = new wxListCtrl(recPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                    wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    apptPrev->SetBackgroundColour(CLR_CARD);
    apptPrev->SetForegroundColour(CLR_TEXT);
    apptPrev->AppendColumn(wxT("ID"),       wxLIST_FORMAT_LEFT, 60);
    apptPrev->AppendColumn(wxT("Patient"),  wxLIST_FORMAT_LEFT, 160);
    apptPrev->AppendColumn(wxT("Doctor"),   wxLIST_FORMAT_LEFT, 160);
    apptPrev->AppendColumn(wxT("Date"),     wxLIST_FORMAT_LEFT, 110);
    apptPrev->AppendColumn(wxT("Time"),     wxLIST_FORMAT_LEFT, 90);
    apptPrev->AppendColumn(wxT("Status"),   wxLIST_FORMAT_LEFT, 100);

    recSz->Add(apptPrev, 1, wxEXPAND | wxALL, 12);
    recPanel->SetSizer(recSz);
    outer->Add(recPanel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

    page->SetSizer(outer);
    return page;
}

// ─── Patients Page ─────────────────────────────────────────────────
wxPanel* MainFrame::BuildPatientsPage(wxWindow* parent)
{
    auto* page = new wxPanel(parent, wxID_ANY);
    page->SetBackgroundColour(CLR_BG);
    auto* outer = new wxBoxSizer(wxVERTICAL);

    // Header
    auto* hdr = new wxPanel(page, wxID_ANY);
    hdr->SetBackgroundColour(CLR_CARD);
    auto* hdrSz = new wxBoxSizer(wxHORIZONTAL);
    hdrSz->Add(MakeLabel(hdr, wxT("Patient Management"), 18, true, CLR_TEXT), 1, wxALL, 16);
    m_searchPatients = new wxSearchCtrl(hdr, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(240, 30));
    m_searchPatients->SetHint(wxT("Search patients..."));
    m_searchPatients->SetBackgroundColour(CLR_INPUT_BG);
    m_searchPatients->SetForegroundColour(CLR_TEXT);
    hdrSz->Add(m_searchPatients, 0, wxALL | wxALIGN_CENTER_VERTICAL, 12);
    hdr->SetSizer(hdrSz);
    outer->Add(hdr, 0, wxEXPAND);
    outer->AddSpacer(10);

    // Content: form (left) + list (right)
    auto* split = new wxBoxSizer(wxHORIZONTAL);

    // ── Form panel
    auto* formPanel = new wxPanel(page, wxID_ANY, wxDefaultPosition, wxSize(340, -1));
    formPanel->SetBackgroundColour(CLR_CARD);
    auto* fSz = new wxBoxSizer(wxVERTICAL);
    fSz->Add(MakeLabel(formPanel, wxT("Patient Details"), 13, true, CLR_ACCENT), 0, wxALL, 14);
    fSz->Add(new wxStaticLine(formPanel, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 14);
    fSz->AddSpacer(8);

    m_txtPID      = MakeField(formPanel, wxT("Patient ID (numeric)"));
    m_txtPName    = MakeField(formPanel, wxT("Full Name"));
    m_txtPAge     = MakeField(formPanel, wxT("Age"));
    m_choPGender  = new wxChoice(formPanel, wxID_ANY);
    m_choPGender->Append({wxT("Male"), wxT("Female"), wxT("Other")});
    m_choPGender->SetSelection(0);
    m_choPGender->SetBackgroundColour(CLR_INPUT_BG);
    m_choPGender->SetForegroundColour(CLR_TEXT);
    m_txtPPhone   = MakeField(formPanel, wxT("Phone (03XXXXXXXXX)"));
    m_txtPAddress = MakeField(formPanel, wxT("Address"));
    m_txtPDisease = MakeField(formPanel, wxT("Diagnosis / Disease"));
    m_choPBlood   = new wxChoice(formPanel, wxID_ANY);
    m_choPBlood->Append({wxT("A+"),wxT("A-"),wxT("B+"),wxT("B-"),wxT("AB+"),wxT("AB-"),wxT("O+"),wxT("O-")});
    m_choPBlood->SetSelection(0);
    m_choPBlood->SetBackgroundColour(CLR_INPUT_BG);
    m_choPBlood->SetForegroundColour(CLR_TEXT);
    m_txtPEmergency = MakeField(formPanel, wxT("Emergency Contact"));
    m_txtPDoctor    = MakeField(formPanel, wxT("Assigned Doctor"));
    m_txtPRoom      = MakeField(formPanel, wxT("Room Number"));

    AddFormRow(fSz, formPanel, wxT("Patient ID"),        m_txtPID);
    AddFormRow(fSz, formPanel, wxT("Full Name"),         m_txtPName);
    AddFormRow(fSz, formPanel, wxT("Age"),               m_txtPAge);
    AddFormRow(fSz, formPanel, wxT("Gender"),            m_choPGender);
    AddFormRow(fSz, formPanel, wxT("Phone"),             m_txtPPhone);
    AddFormRow(fSz, formPanel, wxT("Address"),           m_txtPAddress);
    AddFormRow(fSz, formPanel, wxT("Disease"),           m_txtPDisease);
    AddFormRow(fSz, formPanel, wxT("Blood Group"),       m_choPBlood);
    AddFormRow(fSz, formPanel, wxT("Emergency Contact"), m_txtPEmergency);
    AddFormRow(fSz, formPanel, wxT("Assigned Doctor"),   m_txtPDoctor);
    AddFormRow(fSz, formPanel, wxT("Room Number"),       m_txtPRoom);

    fSz->AddSpacer(10);
    auto* btnRow = new wxBoxSizer(wxHORIZONTAL);
    btnRow->Add(MakeActionButton(formPanel, wxT("Add"),    ID_PATIENT_ADD,    CLR_GREEN,  wxColour(5,10,20)),  1, wxALL, 4);
    btnRow->Add(MakeActionButton(formPanel, wxT("Update"), ID_PATIENT_UPDATE, CLR_ACCENT, wxColour(5,10,20)),  1, wxALL, 4);
    btnRow->Add(MakeActionButton(formPanel, wxT("Delete"), ID_PATIENT_DELETE, CLR_RED,    CLR_TEXT),           1, wxALL, 4);
    btnRow->Add(MakeActionButton(formPanel, wxT("Clear"),  ID_PATIENT_CLEAR,  CLR_MUTED,  CLR_TEXT),           1, wxALL, 4);
    fSz->Add(btnRow, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    fSz->AddSpacer(10);
    formPanel->SetSizer(fSz);
    split->Add(formPanel, 0, wxEXPAND | wxLEFT | wxBOTTOM, 12);
    split->AddSpacer(8);

    // ── List panel
    auto* listPanel = new wxPanel(page, wxID_ANY);
    listPanel->SetBackgroundColour(CLR_CARD);
    auto* lSz = new wxBoxSizer(wxVERTICAL);
    lSz->Add(MakeLabel(listPanel, wxT("Patient Records"), 12, true, CLR_TEXT), 0, wxALL, 12);

    m_lstPatients = new wxListCtrl(listPanel, ID_PATIENT_LIST, wxDefaultPosition, wxDefaultSize,
                                   wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    m_lstPatients->SetBackgroundColour(CLR_CARD);
    m_lstPatients->SetForegroundColour(CLR_TEXT);
    m_lstPatients->AppendColumn(wxT("ID"),       wxLIST_FORMAT_LEFT, 60);
    m_lstPatients->AppendColumn(wxT("Name"),     wxLIST_FORMAT_LEFT, 160);
    m_lstPatients->AppendColumn(wxT("Age"),      wxLIST_FORMAT_LEFT, 50);
    m_lstPatients->AppendColumn(wxT("Gender"),   wxLIST_FORMAT_LEFT, 70);
    m_lstPatients->AppendColumn(wxT("Phone"),    wxLIST_FORMAT_LEFT, 120);
    m_lstPatients->AppendColumn(wxT("Disease"),  wxLIST_FORMAT_LEFT, 130);
    m_lstPatients->AppendColumn(wxT("Doctor"),   wxLIST_FORMAT_LEFT, 130);
    m_lstPatients->AppendColumn(wxT("Room"),     wxLIST_FORMAT_LEFT, 70);
    m_lstPatients->AppendColumn(wxT("Blood"),    wxLIST_FORMAT_LEFT, 60);
    m_lstPatients->AppendColumn(wxT("Admitted"), wxLIST_FORMAT_LEFT, 100);

    lSz->Add(m_lstPatients, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    listPanel->SetSizer(lSz);
    split->Add(listPanel, 1, wxEXPAND | wxRIGHT | wxBOTTOM, 12);

    outer->Add(split, 1, wxEXPAND);
    page->SetSizer(outer);

    // Wire search
    m_searchPatients->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &MainFrame::OnPatientSearch, this);
    m_searchPatients->Bind(wxEVT_TEXT, [this](wxCommandEvent& e){ RefreshPatientList(e.GetString()); });

    return page;
}

// ─── Doctors Page ──────────────────────────────────────────────────
wxPanel* MainFrame::BuildDoctorsPage(wxWindow* parent)
{
    auto* page = new wxPanel(parent, wxID_ANY);
    page->SetBackgroundColour(CLR_BG);
    auto* outer = new wxBoxSizer(wxVERTICAL);

    // Header
    auto* hdr = new wxPanel(page, wxID_ANY);
    hdr->SetBackgroundColour(CLR_CARD);
    auto* hdrSz = new wxBoxSizer(wxHORIZONTAL);
    hdrSz->Add(MakeLabel(hdr, wxT("Doctor Management"), 18, true, CLR_TEXT), 1, wxALL, 16);
    m_searchDoctors = new wxSearchCtrl(hdr, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(240, 30));
    m_searchDoctors->SetHint(wxT("Search doctors..."));
    m_searchDoctors->SetBackgroundColour(CLR_INPUT_BG);
    m_searchDoctors->SetForegroundColour(CLR_TEXT);
    hdrSz->Add(m_searchDoctors, 0, wxALL | wxALIGN_CENTER_VERTICAL, 12);
    hdr->SetSizer(hdrSz);
    outer->Add(hdr, 0, wxEXPAND);
    outer->AddSpacer(10);

    auto* split = new wxBoxSizer(wxHORIZONTAL);

    // Form
    auto* formPanel = new wxPanel(page, wxID_ANY, wxDefaultPosition, wxSize(340, -1));
    formPanel->SetBackgroundColour(CLR_CARD);
    auto* fSz = new wxBoxSizer(wxVERTICAL);
    fSz->Add(MakeLabel(formPanel, wxT("Doctor Details"), 13, true, CLR_GREEN), 0, wxALL, 14);
    fSz->Add(new wxStaticLine(formPanel, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 14);
    fSz->AddSpacer(8);

    m_txtDID      = MakeField(formPanel, wxT("Doctor ID (numeric)"));
    m_txtDName    = MakeField(formPanel, wxT("Full Name (Dr. ...)"));
    m_txtDAge     = MakeField(formPanel, wxT("Age"));
    m_choDGender  = new wxChoice(formPanel, wxID_ANY);
    m_choDGender->Append({wxT("Male"), wxT("Female")});
    m_choDGender->SetSelection(0);
    m_choDGender->SetBackgroundColour(CLR_INPUT_BG);
    m_choDGender->SetForegroundColour(CLR_TEXT);
    m_txtDPhone   = MakeField(formPanel, wxT("Phone"));
    m_txtDAddress = MakeField(formPanel, wxT("Address"));
    m_txtDSpec    = MakeField(formPanel, wxT("Specialization"));
    m_txtDQual    = MakeField(formPanel, wxT("Qualifications"));
    m_txtDExp     = MakeField(formPanel, wxT("Experience (years)"));
    m_txtDAvail   = MakeField(formPanel, wxT("Availability (e.g. 9AM-5PM)"));
    m_txtDDept    = MakeField(formPanel, wxT("Department"));

    AddFormRow(fSz, formPanel, wxT("Doctor ID"),       m_txtDID);
    AddFormRow(fSz, formPanel, wxT("Full Name"),       m_txtDName);
    AddFormRow(fSz, formPanel, wxT("Age"),             m_txtDAge);
    AddFormRow(fSz, formPanel, wxT("Gender"),          m_choDGender);
    AddFormRow(fSz, formPanel, wxT("Phone"),           m_txtDPhone);
    AddFormRow(fSz, formPanel, wxT("Address"),         m_txtDAddress);
    AddFormRow(fSz, formPanel, wxT("Specialization"),  m_txtDSpec);
    AddFormRow(fSz, formPanel, wxT("Qualifications"),  m_txtDQual);
    AddFormRow(fSz, formPanel, wxT("Experience"),      m_txtDExp);
    AddFormRow(fSz, formPanel, wxT("Availability"),    m_txtDAvail);
    AddFormRow(fSz, formPanel, wxT("Department"),      m_txtDDept);

    fSz->AddSpacer(10);
    auto* btnRow = new wxBoxSizer(wxHORIZONTAL);
    btnRow->Add(MakeActionButton(formPanel, wxT("Add"),    ID_DOCTOR_ADD,    CLR_GREEN,  wxColour(5,10,20)), 1, wxALL, 4);
    btnRow->Add(MakeActionButton(formPanel, wxT("Update"), ID_DOCTOR_UPDATE, CLR_ACCENT, wxColour(5,10,20)), 1, wxALL, 4);
    btnRow->Add(MakeActionButton(formPanel, wxT("Delete"), ID_DOCTOR_DELETE, CLR_RED,    CLR_TEXT),          1, wxALL, 4);
    btnRow->Add(MakeActionButton(formPanel, wxT("Clear"),  ID_DOCTOR_CLEAR,  CLR_MUTED,  CLR_TEXT),          1, wxALL, 4);
    fSz->Add(btnRow, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    fSz->AddSpacer(10);
    formPanel->SetSizer(fSz);
    split->Add(formPanel, 0, wxEXPAND | wxLEFT | wxBOTTOM, 12);
    split->AddSpacer(8);

    // List
    auto* listPanel = new wxPanel(page, wxID_ANY);
    listPanel->SetBackgroundColour(CLR_CARD);
    auto* lSz = new wxBoxSizer(wxVERTICAL);
    lSz->Add(MakeLabel(listPanel, wxT("Doctor Records"), 12, true, CLR_TEXT), 0, wxALL, 12);
    m_lstDoctors = new wxListCtrl(listPanel, ID_DOCTOR_LIST, wxDefaultPosition, wxDefaultSize,
                                  wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    m_lstDoctors->SetBackgroundColour(CLR_CARD);
    m_lstDoctors->SetForegroundColour(CLR_TEXT);
    m_lstDoctors->AppendColumn(wxT("ID"),          wxLIST_FORMAT_LEFT, 60);
    m_lstDoctors->AppendColumn(wxT("Name"),        wxLIST_FORMAT_LEFT, 160);
    m_lstDoctors->AppendColumn(wxT("Age"),         wxLIST_FORMAT_LEFT, 50);
    m_lstDoctors->AppendColumn(wxT("Gender"),      wxLIST_FORMAT_LEFT, 70);
    m_lstDoctors->AppendColumn(wxT("Phone"),       wxLIST_FORMAT_LEFT, 120);
    m_lstDoctors->AppendColumn(wxT("Spec."),       wxLIST_FORMAT_LEFT, 130);
    m_lstDoctors->AppendColumn(wxT("Dept."),       wxLIST_FORMAT_LEFT, 130);
    m_lstDoctors->AppendColumn(wxT("Exp."),        wxLIST_FORMAT_LEFT, 60);
    m_lstDoctors->AppendColumn(wxT("Availability"),wxLIST_FORMAT_LEFT, 100);
    lSz->Add(m_lstDoctors, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    listPanel->SetSizer(lSz);
    split->Add(listPanel, 1, wxEXPAND | wxRIGHT | wxBOTTOM, 12);

    outer->Add(split, 1, wxEXPAND);
    page->SetSizer(outer);

    m_searchDoctors->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &MainFrame::OnDoctorSearch, this);
    m_searchDoctors->Bind(wxEVT_TEXT, [this](wxCommandEvent& e){ RefreshDoctorList(e.GetString()); });
    return page;
}

// ─── Appointments Page ─────────────────────────────────────────────
wxPanel* MainFrame::BuildAppointmentsPage(wxWindow* parent)
{
    auto* page = new wxPanel(parent, wxID_ANY);
    page->SetBackgroundColour(CLR_BG);
    auto* outer = new wxBoxSizer(wxVERTICAL);

    auto* hdr = new wxPanel(page, wxID_ANY);
    hdr->SetBackgroundColour(CLR_CARD);
    auto* hdrSz = new wxBoxSizer(wxHORIZONTAL);
    hdrSz->Add(MakeLabel(hdr, wxT("Appointment Scheduling"), 18, true, CLR_TEXT), 1, wxALL, 16);
    hdr->SetSizer(hdrSz);
    outer->Add(hdr, 0, wxEXPAND);
    outer->AddSpacer(10);

    auto* split = new wxBoxSizer(wxHORIZONTAL);

    auto* formPanel = new wxPanel(page, wxID_ANY, wxDefaultPosition, wxSize(320, -1));
    formPanel->SetBackgroundColour(CLR_CARD);
    auto* fSz = new wxBoxSizer(wxVERTICAL);
    fSz->Add(MakeLabel(formPanel, wxT("Appointment Details"), 13, true, CLR_ORANGE), 0, wxALL, 14);
    fSz->Add(new wxStaticLine(formPanel, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 14);
    fSz->AddSpacer(8);

    m_txtAID      = MakeField(formPanel, wxT("Appointment ID"));
    m_txtAPatient = MakeField(formPanel, wxT("Patient Name"));
    m_txtADoctor  = MakeField(formPanel, wxT("Doctor Name"));
    m_txtADate    = MakeField(formPanel, wxT("Date (DD/MM/YYYY)"));
    m_txtATime    = MakeField(formPanel, wxT("Time (e.g. 10:30 AM)"));
    m_txtADisease = MakeField(formPanel, wxT("Reason / Disease"));
    m_choAStatus  = new wxChoice(formPanel, wxID_ANY);
    m_choAStatus->Append({wxT("Pending"), wxT("Completed"), wxT("Cancelled")});
    m_choAStatus->SetSelection(0);
    m_choAStatus->SetBackgroundColour(CLR_INPUT_BG);
    m_choAStatus->SetForegroundColour(CLR_TEXT);

    AddFormRow(fSz, formPanel, wxT("Appt. ID"),    m_txtAID);
    AddFormRow(fSz, formPanel, wxT("Patient"),      m_txtAPatient);
    AddFormRow(fSz, formPanel, wxT("Doctor"),       m_txtADoctor);
    AddFormRow(fSz, formPanel, wxT("Date"),         m_txtADate);
    AddFormRow(fSz, formPanel, wxT("Time"),         m_txtATime);
    AddFormRow(fSz, formPanel, wxT("Reason"),       m_txtADisease);
    AddFormRow(fSz, formPanel, wxT("Status"),       m_choAStatus);

    fSz->AddSpacer(12);
    auto* btnCol = new wxBoxSizer(wxVERTICAL);
    btnCol->Add(MakeActionButton(formPanel, wxT("Book Appointment"),   ID_APPT_ADD,      CLR_GREEN,   wxColour(5,10,20)), 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    btnCol->Add(MakeActionButton(formPanel, wxT("Mark as Completed"), ID_APPT_COMPLETE, CLR_ACCENT,  wxColour(5,10,20)), 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    btnCol->Add(MakeActionButton(formPanel, wxT("Cancel Appointment"), ID_APPT_CANCEL,   CLR_RED,     CLR_TEXT),          0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    btnCol->Add(MakeActionButton(formPanel, wxT("Clear Form"),         ID_APPT_CLEAR,    CLR_MUTED,   CLR_TEXT),          0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    fSz->Add(btnCol, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    fSz->AddSpacer(10);
    formPanel->SetSizer(fSz);
    split->Add(formPanel, 0, wxEXPAND | wxLEFT | wxBOTTOM, 12);
    split->AddSpacer(8);

    auto* listPanel = new wxPanel(page, wxID_ANY);
    listPanel->SetBackgroundColour(CLR_CARD);
    auto* lSz = new wxBoxSizer(wxVERTICAL);
    lSz->Add(MakeLabel(listPanel, wxT("All Appointments"), 12, true, CLR_TEXT), 0, wxALL, 12);
    m_lstAppts = new wxListCtrl(listPanel, ID_APPT_LIST, wxDefaultPosition, wxDefaultSize,
                                 wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    m_lstAppts->SetBackgroundColour(CLR_CARD);
    m_lstAppts->SetForegroundColour(CLR_TEXT);
    m_lstAppts->AppendColumn(wxT("ID"),       wxLIST_FORMAT_LEFT, 60);
    m_lstAppts->AppendColumn(wxT("Patient"),  wxLIST_FORMAT_LEFT, 160);
    m_lstAppts->AppendColumn(wxT("Doctor"),   wxLIST_FORMAT_LEFT, 160);
    m_lstAppts->AppendColumn(wxT("Date"),     wxLIST_FORMAT_LEFT, 110);
    m_lstAppts->AppendColumn(wxT("Time"),     wxLIST_FORMAT_LEFT, 90);
    m_lstAppts->AppendColumn(wxT("Reason"),   wxLIST_FORMAT_LEFT, 150);
    m_lstAppts->AppendColumn(wxT("Status"),   wxLIST_FORMAT_LEFT, 100);
    lSz->Add(m_lstAppts, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    listPanel->SetSizer(lSz);
    split->Add(listPanel, 1, wxEXPAND | wxRIGHT | wxBOTTOM, 12);

    outer->Add(split, 1, wxEXPAND);
    page->SetSizer(outer);
    return page;
}

// ─── Billing Page ──────────────────────────────────────────────────
wxPanel* MainFrame::BuildBillingPage(wxWindow* parent)
{
    auto* page = new wxPanel(parent, wxID_ANY);
    page->SetBackgroundColour(CLR_BG);
    auto* outer = new wxBoxSizer(wxVERTICAL);

    auto* hdr = new wxPanel(page, wxID_ANY);
    hdr->SetBackgroundColour(CLR_CARD);
    hdr->SetSizer([&]{ auto* s = new wxBoxSizer(wxHORIZONTAL);
        s->Add(MakeLabel(hdr, wxT("Billing System"), 18, true, CLR_TEXT), 1, wxALL, 16);
        return s; }());
    outer->Add(hdr, 0, wxEXPAND);
    outer->AddSpacer(10);

    auto* split = new wxBoxSizer(wxHORIZONTAL);

    auto* formPanel = new wxPanel(page, wxID_ANY, wxDefaultPosition, wxSize(340, -1));
    formPanel->SetBackgroundColour(CLR_CARD);
    auto* fSz = new wxBoxSizer(wxVERTICAL);
    fSz->Add(MakeLabel(formPanel, wxT("Generate Bill"), 13, true, CLR_YELLOW), 0, wxALL, 14);
    fSz->Add(new wxStaticLine(formPanel, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 14);
    fSz->AddSpacer(8);

    m_txtBillID      = MakeField(formPanel, wxT("Bill ID"));
    m_txtBPatientID  = MakeField(formPanel, wxT("Patient ID"));
    m_txtBPatientName= MakeField(formPanel, wxT("Patient Name"));
    m_txtBConsult    = MakeField(formPanel, wxT("Consultation Fee (Rs.)"));
    m_txtBMed        = MakeField(formPanel, wxT("Medicine Cost (Rs.)"));
    m_txtBRoom       = MakeField(formPanel, wxT("Room Charges (Rs.)"));
    m_txtBOther      = MakeField(formPanel, wxT("Other Charges (Rs.)"));

    AddFormRow(fSz, formPanel, wxT("Bill ID"),        m_txtBillID);
    AddFormRow(fSz, formPanel, wxT("Patient ID"),     m_txtBPatientID);
    AddFormRow(fSz, formPanel, wxT("Patient Name"),   m_txtBPatientName);
    AddFormRow(fSz, formPanel, wxT("Consultation"),   m_txtBConsult);
    AddFormRow(fSz, formPanel, wxT("Medicine"),       m_txtBMed);
    AddFormRow(fSz, formPanel, wxT("Room"),           m_txtBRoom);
    AddFormRow(fSz, formPanel, wxT("Other"),          m_txtBOther);

    // Auto-calculate on text change
    auto calcFn = [this](wxCommandEvent&){ OnBillCalc(wxCommandEvent{}); };
    m_txtBConsult->Bind(wxEVT_TEXT, calcFn);
    m_txtBMed->Bind(wxEVT_TEXT, calcFn);
    m_txtBRoom->Bind(wxEVT_TEXT, calcFn);
    m_txtBOther->Bind(wxEVT_TEXT, calcFn);

    // Total display
    auto* totalCard = new wxPanel(formPanel, wxID_ANY);
    totalCard->SetBackgroundColour(wxColour(30, 50, 30));
    auto* tSz = new wxBoxSizer(wxHORIZONTAL);
    tSz->Add(MakeLabel(totalCard, wxT("TOTAL:"), 13, true, CLR_GREEN), 0, wxALL | wxALIGN_CENTER_VERTICAL, 12);
    m_lblBTotal = new wxStaticText(totalCard, wxID_ANY, wxT("Rs. 0.00"));
    m_lblBTotal->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    m_lblBTotal->SetForegroundColour(CLR_GREEN);
    tSz->Add(m_lblBTotal, 1, wxALL | wxALIGN_CENTER_VERTICAL, 12);
    totalCard->SetSizer(tSz);
    fSz->Add(totalCard, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    fSz->AddSpacer(10);
    auto* btnRow = new wxBoxSizer(wxHORIZONTAL);
    btnRow->Add(MakeActionButton(formPanel, wxT("Generate Bill"), ID_BILL_GENERATE, CLR_YELLOW, wxColour(10,10,5)), 2, wxALL, 4);
    btnRow->Add(MakeActionButton(formPanel, wxT("Clear"),         ID_BILL_CLEAR,    CLR_MUTED,  CLR_TEXT),          1, wxALL, 4);
    fSz->Add(btnRow, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    fSz->AddSpacer(10);
    formPanel->SetSizer(fSz);
    split->Add(formPanel, 0, wxEXPAND | wxLEFT | wxBOTTOM, 12);
    split->AddSpacer(8);

    auto* listPanel = new wxPanel(page, wxID_ANY);
    listPanel->SetBackgroundColour(CLR_CARD);
    auto* lSz = new wxBoxSizer(wxVERTICAL);
    lSz->Add(MakeLabel(listPanel, wxT("Billing Records"), 12, true, CLR_TEXT), 0, wxALL, 12);
    m_lstBills = new wxListCtrl(listPanel, ID_BILL_LIST, wxDefaultPosition, wxDefaultSize,
                                 wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    m_lstBills->SetBackgroundColour(CLR_CARD);
    m_lstBills->SetForegroundColour(CLR_TEXT);
    m_lstBills->AppendColumn(wxT("Bill ID"),    wxLIST_FORMAT_LEFT, 70);
    m_lstBills->AppendColumn(wxT("Patient ID"), wxLIST_FORMAT_LEFT, 80);
    m_lstBills->AppendColumn(wxT("Patient"),    wxLIST_FORMAT_LEFT, 160);
    m_lstBills->AppendColumn(wxT("Consult"),    wxLIST_FORMAT_RIGHT, 90);
    m_lstBills->AppendColumn(wxT("Medicine"),   wxLIST_FORMAT_RIGHT, 90);
    m_lstBills->AppendColumn(wxT("Room"),       wxLIST_FORMAT_RIGHT, 90);
    m_lstBills->AppendColumn(wxT("Other"),      wxLIST_FORMAT_RIGHT, 80);
    m_lstBills->AppendColumn(wxT("TOTAL"),      wxLIST_FORMAT_RIGHT, 100);
    m_lstBills->AppendColumn(wxT("Status"),     wxLIST_FORMAT_LEFT, 90);
    m_lstBills->AppendColumn(wxT("Date"),       wxLIST_FORMAT_LEFT, 100);
    lSz->Add(m_lstBills, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    listPanel->SetSizer(lSz);
    split->Add(listPanel, 1, wxEXPAND | wxRIGHT | wxBOTTOM, 12);

    outer->Add(split, 1, wxEXPAND);
    page->SetSizer(outer);
    return page;
}

// ─── Pharmacy Page ─────────────────────────────────────────────────
wxPanel* MainFrame::BuildPharmacyPage(wxWindow* parent)
{
    auto* page = new wxPanel(parent, wxID_ANY);
    page->SetBackgroundColour(CLR_BG);
    auto* outer = new wxBoxSizer(wxVERTICAL);

    auto* hdr = new wxPanel(page, wxID_ANY);
    hdr->SetBackgroundColour(CLR_CARD);
    auto* hdrSz = new wxBoxSizer(wxHORIZONTAL);
    hdrSz->Add(MakeLabel(hdr, wxT("Pharmacy Management"), 18, true, CLR_TEXT), 1, wxALL, 16);
    m_searchMeds = new wxSearchCtrl(hdr, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(240, 30));
    m_searchMeds->SetHint(wxT("Search medicines..."));
    m_searchMeds->SetBackgroundColour(CLR_INPUT_BG);
    m_searchMeds->SetForegroundColour(CLR_TEXT);
    hdrSz->Add(m_searchMeds, 0, wxALL | wxALIGN_CENTER_VERTICAL, 12);
    hdr->SetSizer(hdrSz);
    outer->Add(hdr, 0, wxEXPAND);
    outer->AddSpacer(10);

    auto* split = new wxBoxSizer(wxHORIZONTAL);

    auto* formPanel = new wxPanel(page, wxID_ANY, wxDefaultPosition, wxSize(320, -1));
    formPanel->SetBackgroundColour(CLR_CARD);
    auto* fSz = new wxBoxSizer(wxVERTICAL);
    fSz->Add(MakeLabel(formPanel, wxT("Medicine Details"), 13, true, CLR_ACCENT2), 0, wxALL, 14);
    fSz->Add(new wxStaticLine(formPanel, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 14);
    fSz->AddSpacer(8);

    m_txtMID     = MakeField(formPanel, wxT("Medicine ID"));
    m_txtMName   = MakeField(formPanel, wxT("Medicine Name"));
    m_txtMCat    = MakeField(formPanel, wxT("Category (e.g. Antibiotic)"));
    m_txtMMfg    = MakeField(formPanel, wxT("Manufacturer"));
    m_txtMPrice  = MakeField(formPanel, wxT("Price per unit (Rs.)"));
    m_txtMQty    = MakeField(formPanel, wxT("Stock Quantity"));
    m_txtMExpiry = MakeField(formPanel, wxT("Expiry Date (MM/YYYY)"));

    AddFormRow(fSz, formPanel, wxT("Medicine ID"),   m_txtMID);
    AddFormRow(fSz, formPanel, wxT("Name"),          m_txtMName);
    AddFormRow(fSz, formPanel, wxT("Category"),      m_txtMCat);
    AddFormRow(fSz, formPanel, wxT("Manufacturer"),  m_txtMMfg);
    AddFormRow(fSz, formPanel, wxT("Price (Rs.)"),   m_txtMPrice);
    AddFormRow(fSz, formPanel, wxT("Stock Qty"),     m_txtMQty);
    AddFormRow(fSz, formPanel, wxT("Expiry Date"),   m_txtMExpiry);

    fSz->AddSpacer(12);
    auto* btnRow = new wxBoxSizer(wxHORIZONTAL);
    btnRow->Add(MakeActionButton(formPanel, wxT("Add Stock"), ID_PHARMA_ADD,   CLR_ACCENT2, wxColour(5,10,20)), 2, wxALL, 4);
    btnRow->Add(MakeActionButton(formPanel, wxT("Clear"),     ID_PHARMA_CLEAR, CLR_MUTED,   CLR_TEXT),          1, wxALL, 4);
    fSz->Add(btnRow, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    fSz->AddSpacer(10);
    formPanel->SetSizer(fSz);
    split->Add(formPanel, 0, wxEXPAND | wxLEFT | wxBOTTOM, 12);
    split->AddSpacer(8);

    auto* listPanel = new wxPanel(page, wxID_ANY);
    listPanel->SetBackgroundColour(CLR_CARD);
    auto* lSz = new wxBoxSizer(wxVERTICAL);
    lSz->Add(MakeLabel(listPanel, wxT("Medicine Inventory"), 12, true, CLR_TEXT), 0, wxALL, 12);
    m_lstMeds = new wxListCtrl(listPanel, ID_PHARMA_LIST, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    m_lstMeds->SetBackgroundColour(CLR_CARD);
    m_lstMeds->SetForegroundColour(CLR_TEXT);
    m_lstMeds->AppendColumn(wxT("ID"),           wxLIST_FORMAT_LEFT, 60);
    m_lstMeds->AppendColumn(wxT("Name"),         wxLIST_FORMAT_LEFT, 160);
    m_lstMeds->AppendColumn(wxT("Category"),     wxLIST_FORMAT_LEFT, 130);
    m_lstMeds->AppendColumn(wxT("Manufacturer"), wxLIST_FORMAT_LEFT, 130);
    m_lstMeds->AppendColumn(wxT("Price"),        wxLIST_FORMAT_RIGHT, 90);
    m_lstMeds->AppendColumn(wxT("Stock"),        wxLIST_FORMAT_RIGHT, 80);
    m_lstMeds->AppendColumn(wxT("Expiry"),       wxLIST_FORMAT_LEFT, 100);
    lSz->Add(m_lstMeds, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    listPanel->SetSizer(lSz);
    split->Add(listPanel, 1, wxEXPAND | wxRIGHT | wxBOTTOM, 12);

    outer->Add(split, 1, wxEXPAND);
    page->SetSizer(outer);

    m_searchMeds->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &MainFrame::OnPharmaSearch, this);
    m_searchMeds->Bind(wxEVT_TEXT, [this](wxCommandEvent& e){ RefreshMedicineList(e.GetString()); });
    return page;
}

// ─── Reports Page ──────────────────────────────────────────────────
wxPanel* MainFrame::BuildReportsPage(wxWindow* parent)
{
    auto* page = new wxPanel(parent, wxID_ANY);
    page->SetBackgroundColour(CLR_BG);
    auto* outer = new wxBoxSizer(wxVERTICAL);

    auto* hdr = new wxPanel(page, wxID_ANY);
    hdr->SetBackgroundColour(CLR_CARD);
    hdr->SetSizer([&]{ auto* s = new wxBoxSizer(wxHORIZONTAL);
        s->Add(MakeLabel(hdr, wxT("Reports & Analytics"), 18, true, CLR_TEXT), 1, wxALL, 16);
        return s; }());
    outer->Add(hdr, 0, wxEXPAND);
    outer->AddSpacer(20);

    // Generate button
    auto* btnRow = new wxBoxSizer(wxHORIZONTAL);
    btnRow->AddStretchSpacer(1);
    btnRow->Add(MakeActionButton(page, wxT("Generate Full Hospital Report"), ID_REPORT_GENERATE, CLR_ACCENT, wxColour(5,10,20)), 0, wxALL, 8);
    btnRow->AddStretchSpacer(1);
    outer->Add(btnRow, 0, wxEXPAND);
    outer->AddSpacer(16);

    // Report text area
    auto* rPanel = new wxPanel(page, wxID_ANY);
    rPanel->SetBackgroundColour(CLR_CARD);
    auto* rSz = new wxBoxSizer(wxVERTICAL);
    rSz->Add(MakeLabel(rPanel, wxT("Report Output"), 12, true, CLR_TEXT), 0, wxALL, 12);
    auto* reportText = new wxTextCtrl(rPanel, wxID_ANY, wxT(""),
                                       wxDefaultPosition, wxDefaultSize,
                                       wxTE_MULTILINE | wxTE_READONLY | wxBORDER_NONE | wxTE_RICH2);
    reportText->SetBackgroundColour(CLR_INPUT_BG);
    reportText->SetForegroundColour(CLR_TEXT);
    reportText->SetFont(wxFont(11, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    rSz->Add(reportText, 1, wxEXPAND | wxALL, 12);
    rPanel->SetSizer(rSz);
    outer->Add(rPanel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

    page->SetSizer(outer);

    // Store pointer so OnReportGenerate can fill it
    m_reportText = reportText;
    return page;
}

// ═══════════════════════════════════════════════════════════════════
//  UI HELPERS
// ═══════════════════════════════════════════════════════════════════

wxButton* MainFrame::MakeNavButton(wxWindow* parent, const wxString& label, int id)
{
    auto* btn = new wxButton(parent, id, label,
                              wxDefaultPosition, wxSize(-1, 42),
                              wxBORDER_NONE);
    btn->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    btn->SetBackgroundColour(CLR_SIDEBAR);
    btn->SetForegroundColour(CLR_TEXT);

    // Hover highlight
    btn->Bind(wxEVT_ENTER_WINDOW, [btn](wxMouseEvent& e){
        btn->SetBackgroundColour(wxColour(30, 50, 80));
        btn->SetForegroundColour(CLR_ACCENT);
        btn->Refresh();
        e.Skip();
    });
    btn->Bind(wxEVT_LEAVE_WINDOW, [btn](wxMouseEvent& e){
        btn->SetBackgroundColour(CLR_SIDEBAR);
        btn->SetForegroundColour(CLR_TEXT);
        btn->Refresh();
        e.Skip();
    });
    return btn;
}

wxButton* MainFrame::MakeActionButton(wxWindow* parent, const wxString& label,
                                       int id, const wxColour& bg, const wxColour& fg)
{
    auto* btn = new wxButton(parent, id, label,
                              wxDefaultPosition, wxSize(-1, 38),
                              wxBORDER_NONE);
    btn->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    btn->SetBackgroundColour(bg);
    btn->SetForegroundColour(fg);
    return btn;
}

wxTextCtrl* MainFrame::MakeField(wxWindow* parent, const wxString& hint, bool readOnly)
{
    long style = wxBORDER_SIMPLE | (readOnly ? wxTE_READONLY : 0);
    auto* ctrl = new wxTextCtrl(parent, wxID_ANY, wxT(""),
                                 wxDefaultPosition, wxSize(-1, 30), style);
    ctrl->SetHint(hint);
    ApplyInputStyle(ctrl);
    return ctrl;
}

void MainFrame::ApplyInputStyle(wxTextCtrl* ctrl)
{
    ctrl->SetBackgroundColour(CLR_INPUT_BG);
    ctrl->SetForegroundColour(CLR_TEXT);
    ctrl->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
}

wxStaticText* MainFrame::MakeLabel(wxWindow* parent, const wxString& text,
                                    int ptSize, bool bold, const wxColour& clr)
{
    auto* lbl = new wxStaticText(parent, wxID_ANY, text);
    lbl->SetFont(wxFont(ptSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                         bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL));
    lbl->SetForegroundColour(clr);
    return lbl;
}

void MainFrame::AddFormRow(wxSizer* sizer, wxWindow* parent,
                            const wxString& label, wxWindow* ctrl)
{
    auto* row = new wxBoxSizer(wxHORIZONTAL);
    auto* lbl = new wxStaticText(parent, wxID_ANY, label);
    lbl->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    lbl->SetForegroundColour(CLR_MUTED);
    lbl->SetMinSize(wxSize(120, -1));
    row->Add(lbl,  0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 8);
    row->Add(ctrl, 1, wxEXPAND | wxRIGHT, 14);
    sizer->Add(row, 0, wxEXPAND | wxBOTTOM, 6);
}

wxPanel* MainFrame::MakeStatCard(wxWindow* parent, const wxString& title,
                                  const wxString& value, const wxColour& accent,
                                  wxStaticText** outLabel)
{
    auto* card = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 110));
    card->SetBackgroundColour(CLR_CARD);
    auto* sz = new wxBoxSizer(wxVERTICAL);

    // Coloured accent bar at top
    auto* bar = new wxPanel(card, wxID_ANY, wxDefaultPosition, wxSize(-1, 4));
    bar->SetBackgroundColour(accent);
    sz->Add(bar, 0, wxEXPAND);

    sz->AddSpacer(10);
    auto* titleLbl = new wxStaticText(card, wxID_ANY, title);
    titleLbl->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    titleLbl->SetForegroundColour(CLR_MUTED);

    auto* valLbl = new wxStaticText(card, wxID_ANY, value);
    valLbl->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    valLbl->SetForegroundColour(accent);

    sz->Add(titleLbl, 0, wxLEFT | wxRIGHT, 14);
    sz->Add(valLbl,   0, wxLEFT | wxRIGHT | wxTOP, 14);

    card->SetSizer(sz);
    if (outLabel) *outLabel = valLbl;
    return card;
}

void MainFrame::ShowPage(wxPanel* page)
{
    if (m_activePage) m_activePage->Hide();
    m_activePage = page;
    page->Show();
    m_contentSizer->Layout();
}

// ═══════════════════════════════════════════════════════════════════
//  FILE I/O
// ═══════════════════════════════════════════════════════════════════

std::vector<PatientRecord> MainFrame::LoadPatients()
{
    std::vector<PatientRecord> records;
    std::ifstream f("patients.txt");
    std::string line;
    while (std::getline(f, line))
    {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tok[12];
        int i = 0;
        while (i < 12 && std::getline(ss, tok[i], '|')) ++i;
        if (i < 11) continue;
        PatientRecord r;
        try { r.id = std::stoi(tok[0]); } catch(...){ continue; }
        r.name=tok[1]; r.age=tok[2]; r.gender=tok[3]; r.phone=tok[4];
        r.address=tok[5]; r.disease=tok[6]; r.bloodGroup=tok[7];
        r.emergency=tok[8]; r.doctor=tok[9]; r.room=tok[10];
        r.admDate = (i > 11) ? tok[11] : "";
        records.push_back(r);
    }
    return records;
}

std::vector<DoctorRecord> MainFrame::LoadDoctors()
{
    std::vector<DoctorRecord> records;
    std::ifstream f("doctors.txt");
    std::string line;
    while (std::getline(f, line))
    {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tok[11];
        int i = 0;
        while (i < 11 && std::getline(ss, tok[i], '|')) ++i;
        if (i < 10) continue;
        DoctorRecord r;
        try { r.id = std::stoi(tok[0]); } catch(...){ continue; }
        r.name=tok[1]; r.age=tok[2]; r.gender=tok[3]; r.phone=tok[4];
        r.address=tok[5]; r.spec=tok[6]; r.qual=tok[7]; r.exp=tok[8];
        r.avail=tok[9]; r.dept=tok[10];
        records.push_back(r);
    }
    return records;
}

std::vector<AppointmentRecord> MainFrame::LoadAppointments()
{
    std::vector<AppointmentRecord> records;
    std::ifstream f("appointments.txt");
    std::string line;
    while (std::getline(f, line))
    {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tok[7];
        int i = 0;
        while (i < 7 && std::getline(ss, tok[i], '|')) ++i;
        if (i < 7) continue;
        AppointmentRecord r;
        try { r.id = std::stoi(tok[0]); } catch(...){ continue; }
        r.patientName=tok[1]; r.doctorName=tok[2]; r.date=tok[3];
        r.time=tok[4]; r.disease=tok[5]; r.status=tok[6];
        records.push_back(r);
    }
    return records;
}

std::vector<BillRecord> MainFrame::LoadBills()
{
    std::vector<BillRecord> records;
    std::ifstream f("bills.txt");
    std::string line;
    while (std::getline(f, line))
    {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tok[10];
        int i = 0;
        while (i < 10 && std::getline(ss, tok[i], '|')) ++i;
        if (i < 9) continue;
        BillRecord r;
        try {
            r.billId    = std::stoi(tok[0]);
            r.patientId = std::stoi(tok[1]);
            r.patientName = tok[2];
            r.consult   = std::stod(tok[3]);
            r.medicine  = std::stod(tok[4]);
            r.room      = std::stod(tok[5]);
            r.other     = std::stod(tok[6]);
            r.total     = std::stod(tok[7]);
            r.status    = tok[8];
            r.date      = (i > 9) ? tok[9] : "";
        } catch(...) { continue; }
        records.push_back(r);
    }
    return records;
}

std::vector<MedicineRecord> MainFrame::LoadMedicines()
{
    std::vector<MedicineRecord> records;
    std::ifstream f("medicines.txt");
    std::string line;
    while (std::getline(f, line))
    {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string tok[7];
        int i = 0;
        while (i < 7 && std::getline(ss, tok[i], '|')) ++i;
        if (i < 7) continue;
        MedicineRecord r;
        try {
            r.id = std::stoi(tok[0]);
            r.name = tok[1]; r.category = tok[2]; r.manufacturer = tok[3];
            r.price = std::stod(tok[4]);
            r.qty   = std::stoi(tok[5]);
            r.expiry = tok[6];
        } catch(...) { continue; }
        records.push_back(r);
    }
    return records;
}

bool MainFrame::SavePatient(const PatientRecord& p, bool append)
{
    std::ofstream f("patients.txt", append ? std::ios::app : std::ios::trunc);
    if (!f) return false;
    f << p.id << "|" << p.name << "|" << p.age << "|" << p.gender << "|"
      << p.phone << "|" << p.address << "|" << p.disease << "|"
      << p.bloodGroup << "|" << p.emergency << "|" << p.doctor << "|"
      << p.room << "|" << p.admDate << "\n";
    return true;
}

bool MainFrame::SaveDoctor(const DoctorRecord& d, bool append)
{
    std::ofstream f("doctors.txt", append ? std::ios::app : std::ios::trunc);
    if (!f) return false;
    f << d.id << "|" << d.name << "|" << d.age << "|" << d.gender << "|"
      << d.phone << "|" << d.address << "|" << d.spec << "|" << d.qual << "|"
      << d.exp << "|" << d.avail << "|" << d.dept << "\n";
    return true;
}

bool MainFrame::SaveAppointment(const AppointmentRecord& a, bool append)
{
    std::ofstream f("appointments.txt", append ? std::ios::app : std::ios::trunc);
    if (!f) return false;
    f << a.id << "|" << a.patientName << "|" << a.doctorName << "|"
      << a.date << "|" << a.time << "|" << a.disease << "|" << a.status << "\n";
    return true;
}

bool MainFrame::SaveBill(const BillRecord& b)
{
    std::ofstream f("bills.txt", std::ios::app);
    if (!f) return false;
    f << std::fixed << std::setprecision(2);
    f << b.billId << "|" << b.patientId << "|" << b.patientName << "|"
      << b.consult << "|" << b.medicine << "|" << b.room << "|" << b.other << "|"
      << b.total << "|" << b.status << "|" << b.date << "\n";
    return true;
}

bool MainFrame::SaveMedicine(const MedicineRecord& m)
{
    std::ofstream f("medicines.txt", std::ios::app);
    if (!f) return false;
    f << m.id << "|" << m.name << "|" << m.category << "|" << m.manufacturer << "|"
      << std::fixed << std::setprecision(2) << m.price << "|" << m.qty << "|" << m.expiry << "\n";
    return true;
}

void MainFrame::RewritePatients(const std::vector<PatientRecord>& records)
{
    std::ofstream f("patients.txt", std::ios::trunc);
    for (auto& p : records)
    {
        f << p.id << "|" << p.name << "|" << p.age << "|" << p.gender << "|"
          << p.phone << "|" << p.address << "|" << p.disease << "|"
          << p.bloodGroup << "|" << p.emergency << "|" << p.doctor << "|"
          << p.room << "|" << p.admDate << "\n";
    }
}

void MainFrame::RewriteDoctors(const std::vector<DoctorRecord>& records)
{
    std::ofstream f("doctors.txt", std::ios::trunc);
    for (auto& d : records)
    {
        f << d.id << "|" << d.name << "|" << d.age << "|" << d.gender << "|"
          << d.phone << "|" << d.address << "|" << d.spec << "|" << d.qual << "|"
          << d.exp << "|" << d.avail << "|" << d.dept << "\n";
    }
}

void MainFrame::RewriteAppointments(const std::vector<AppointmentRecord>& records)
{
    std::ofstream f("appointments.txt", std::ios::trunc);
    for (auto& a : records)
    {
        f << a.id << "|" << a.patientName << "|" << a.doctorName << "|"
          << a.date << "|" << a.time << "|" << a.disease << "|" << a.status << "\n";
    }
}

// ═══════════════════════════════════════════════════════════════════
//  LIST REFRESHERS
// ═══════════════════════════════════════════════════════════════════

void MainFrame::RefreshPatientList(const wxString& filter)
{
    m_lstPatients->DeleteAllItems();
    auto records = LoadPatients();
    std::string flt = filter.Lower().ToStdString();
    long row = 0;
    for (auto& p : records)
    {
        std::string nameLow = p.name;
        for (char& c : nameLow) c = std::tolower(c);
        if (!flt.empty() && nameLow.find(flt) == std::string::npos &&
            std::to_string(p.id).find(flt) == std::string::npos)
            continue;

        m_lstPatients->InsertItem(row, wxString::Format(wxT("%d"), p.id));
        m_lstPatients->SetItem(row, 1,  wxString::FromUTF8(p.name));
        m_lstPatients->SetItem(row, 2,  wxString::FromUTF8(p.age));
        m_lstPatients->SetItem(row, 3,  wxString::FromUTF8(p.gender));
        m_lstPatients->SetItem(row, 4,  wxString::FromUTF8(p.phone));
        m_lstPatients->SetItem(row, 5,  wxString::FromUTF8(p.disease));
        m_lstPatients->SetItem(row, 6,  wxString::FromUTF8(p.doctor));
        m_lstPatients->SetItem(row, 7,  wxString::FromUTF8(p.room));
        m_lstPatients->SetItem(row, 8,  wxString::FromUTF8(p.bloodGroup));
        m_lstPatients->SetItem(row, 9,  wxString::FromUTF8(p.admDate));
        ++row;
    }
}

void MainFrame::RefreshDoctorList(const wxString& filter)
{
    m_lstDoctors->DeleteAllItems();
    auto records = LoadDoctors();
    std::string flt = filter.Lower().ToStdString();
    long row = 0;
    for (auto& d : records)
    {
        std::string nameLow = d.name;
        for (char& c : nameLow) c = std::tolower(c);
        if (!flt.empty() && nameLow.find(flt) == std::string::npos &&
            d.spec.find(flt) == std::string::npos)
            continue;

        m_lstDoctors->InsertItem(row, wxString::Format(wxT("%d"), d.id));
        m_lstDoctors->SetItem(row, 1, wxString::FromUTF8(d.name));
        m_lstDoctors->SetItem(row, 2, wxString::FromUTF8(d.age));
        m_lstDoctors->SetItem(row, 3, wxString::FromUTF8(d.gender));
        m_lstDoctors->SetItem(row, 4, wxString::FromUTF8(d.phone));
        m_lstDoctors->SetItem(row, 5, wxString::FromUTF8(d.spec));
        m_lstDoctors->SetItem(row, 6, wxString::FromUTF8(d.dept));
        m_lstDoctors->SetItem(row, 7, wxString::FromUTF8(d.exp));
        m_lstDoctors->SetItem(row, 8, wxString::FromUTF8(d.avail));
        ++row;
    }
}

void MainFrame::RefreshAppointmentList()
{
    m_lstAppts->DeleteAllItems();
    auto records = LoadAppointments();
    long row = 0;
    for (auto& a : records)
    {
        m_lstAppts->InsertItem(row, wxString::Format(wxT("%d"), a.id));
        m_lstAppts->SetItem(row, 1, wxString::FromUTF8(a.patientName));
        m_lstAppts->SetItem(row, 2, wxString::FromUTF8(a.doctorName));
        m_lstAppts->SetItem(row, 3, wxString::FromUTF8(a.date));
        m_lstAppts->SetItem(row, 4, wxString::FromUTF8(a.time));
        m_lstAppts->SetItem(row, 5, wxString::FromUTF8(a.disease));
        m_lstAppts->SetItem(row, 6, wxString::FromUTF8(a.status));

        // Colour code by status
        wxListItem item;
        item.SetId(row);
        if (a.status == "Completed")
            m_lstAppts->SetItemTextColour(row, CLR_GREEN);
        else if (a.status == "Cancelled")
            m_lstAppts->SetItemTextColour(row, CLR_RED);
        else
            m_lstAppts->SetItemTextColour(row, CLR_ORANGE);
        ++row;
    }
}

void MainFrame::RefreshBillList()
{
    m_lstBills->DeleteAllItems();
    auto records = LoadBills();
    long row = 0;
    for (auto& b : records)
    {
        m_lstBills->InsertItem(row, wxString::Format(wxT("%d"), b.billId));
        m_lstBills->SetItem(row, 1, wxString::Format(wxT("%d"), b.patientId));
        m_lstBills->SetItem(row, 2, wxString::FromUTF8(b.patientName));
        m_lstBills->SetItem(row, 3, wxString::Format(wxT("%.2f"), b.consult));
        m_lstBills->SetItem(row, 4, wxString::Format(wxT("%.2f"), b.medicine));
        m_lstBills->SetItem(row, 5, wxString::Format(wxT("%.2f"), b.room));
        m_lstBills->SetItem(row, 6, wxString::Format(wxT("%.2f"), b.other));
        m_lstBills->SetItem(row, 7, wxString::Format(wxT("Rs. %.2f"), b.total));
        m_lstBills->SetItem(row, 8, wxString::FromUTF8(b.status));
        m_lstBills->SetItem(row, 9, wxString::FromUTF8(b.date));

        if (b.status == "Paid")
            m_lstBills->SetItemTextColour(row, CLR_GREEN);
        else
            m_lstBills->SetItemTextColour(row, CLR_RED);
        ++row;
    }
}

void MainFrame::RefreshMedicineList(const wxString& filter)
{
    m_lstMeds->DeleteAllItems();
    auto records = LoadMedicines();
    std::string flt = filter.Lower().ToStdString();
    long row = 0;
    for (auto& m : records)
    {
        std::string nLow = m.name;
        for (char& c : nLow) c = std::tolower(c);
        if (!flt.empty() && nLow.find(flt) == std::string::npos &&
            m.category.find(flt) == std::string::npos)
            continue;

        m_lstMeds->InsertItem(row, wxString::Format(wxT("%d"), m.id));
        m_lstMeds->SetItem(row, 1, wxString::FromUTF8(m.name));
        m_lstMeds->SetItem(row, 2, wxString::FromUTF8(m.category));
        m_lstMeds->SetItem(row, 3, wxString::FromUTF8(m.manufacturer));
        m_lstMeds->SetItem(row, 4, wxString::Format(wxT("Rs. %.2f"), m.price));
        m_lstMeds->SetItem(row, 5, wxString::Format(wxT("%d"), m.qty));
        m_lstMeds->SetItem(row, 6, wxString::FromUTF8(m.expiry));

        if (m.qty < 10)
            m_lstMeds->SetItemTextColour(row, CLR_RED);  // Low stock warning
        ++row;
    }
}

void MainFrame::RefreshDashboard()
{
    auto pts  = LoadPatients();
    auto drs  = LoadDoctors();
    auto apts = LoadAppointments();
    auto bls  = LoadBills();

    if (m_statPatients) m_statPatients->SetLabel(wxString::Format(wxT("%zu"), pts.size()));
    if (m_statDoctors)  m_statDoctors->SetLabel(wxString::Format(wxT("%zu"), drs.size()));
    if (m_statAppts)    m_statAppts->SetLabel(wxString::Format(wxT("%zu"), apts.size()));

    double totalRev = 0.0;
    for (auto& b : bls) totalRev += b.total;
    if (m_statRevenue) m_statRevenue->SetLabel(wxString::Format(wxT("Rs. %.0f"), totalRev));
}

// ═══════════════════════════════════════════════════════════════════
//  VALIDATION
// ═══════════════════════════════════════════════════════════════════

bool MainFrame::ValidateInt(const wxString& s, int minV, int maxV, int& out)
{
    long v;
    if (!s.ToLong(&v) || v < minV || v > maxV) return false;
    out = (int)v;
    return true;
}

bool MainFrame::ValidateDouble(const wxString& s, double& out)
{
    return s.ToDouble(&out) && out >= 0.0;
}

bool MainFrame::ValidatePhone(const wxString& s)
{
    if (s.length() < 7 || s.length() > 15) return false;
    for (wxChar c : s)
        if (!wxIsdigit(c) && c != '+' && c != '-') return false;
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//  EVENT HANDLERS
// ═══════════════════════════════════════════════════════════════════

// ─── Clock ─────────────────────────────────────────────────────────
void MainFrame::OnTimer(wxTimerEvent&)
{
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%I:%M:%S %p", t);
    m_clockLabel->SetLabel(wxString::FromUTF8(buf));
}

// ─── Navigation ────────────────────────────────────────────────────
void MainFrame::OnNav(wxCommandEvent& evt)
{
    switch (evt.GetId())
    {
    case ID_NAV_DASHBOARD:
        ShowPage(m_pageDashboard);
        RefreshDashboard();
        break;
    case ID_NAV_PATIENTS:
        ShowPage(m_pagePatients);
        RefreshPatientList();
        break;
    case ID_NAV_DOCTORS:
        ShowPage(m_pageDoctors);
        RefreshDoctorList();
        break;
    case ID_NAV_APPOINTMENTS:
        ShowPage(m_pageAppointments);
        RefreshAppointmentList();
        break;
    case ID_NAV_BILLING:
        ShowPage(m_pageBilling);
        RefreshBillList();
        break;
    case ID_NAV_PHARMACY:
        ShowPage(m_pagePharmacy);
        RefreshMedicineList();
        break;
    case ID_NAV_REPORTS:
        ShowPage(m_pageReports);
        break;
    }
}

void MainFrame::OnExit(wxCommandEvent&)
{
    if (wxMessageBox(wxT("Are you sure you want to exit the Hospital System?"),
                     wxT("Confirm Exit"), wxYES_NO | wxICON_QUESTION) == wxYES)
        Close(true);
}

// ─── Patient Events ────────────────────────────────────────────────
void MainFrame::OnPatientAdd(wxCommandEvent&)
{
    int id;
    if (!ValidateInt(m_txtPID->GetValue(), 1, 999999, id))
    {
        wxMessageBox(wxT("Patient ID must be a positive integer (1–999999)."),
                     wxT("Validation Error"), wxOK | wxICON_WARNING);
        return;
    }
    wxString name = m_txtPName->GetValue().Trim();
    if (name.IsEmpty()) {
        wxMessageBox(wxT("Patient Name cannot be empty."), wxT("Validation Error"), wxOK | wxICON_WARNING);
        return;
    }

    // Check duplicate
    auto records = LoadPatients();
    for (auto& p : records)
        if (p.id == id) {
            wxMessageBox(wxString::Format(wxT("Patient ID %d already exists!"), id),
                         wxT("Duplicate ID"), wxOK | wxICON_ERROR);
            return;
        }

    // Get current date
    time_t now = time(nullptr); tm* t = localtime(&now);
    char buf[32]; strftime(buf, sizeof(buf), "%d/%m/%Y", t);

    PatientRecord p;
    p.id          = id;
    p.name        = name.ToStdString();
    p.age         = m_txtPAge->GetValue().Trim().ToStdString();
    p.gender      = m_choPGender->GetStringSelection().ToStdString();
    p.phone       = m_txtPPhone->GetValue().Trim().ToStdString();
    p.address     = m_txtPAddress->GetValue().Trim().ToStdString();
    p.disease     = m_txtPDisease->GetValue().Trim().ToStdString();
    p.bloodGroup  = m_choPBlood->GetStringSelection().ToStdString();
    p.emergency   = m_txtPEmergency->GetValue().Trim().ToStdString();
    p.doctor      = m_txtPDoctor->GetValue().Trim().ToStdString();
    p.room        = m_txtPRoom->GetValue().Trim().ToStdString();
    p.admDate     = buf;

    if (SavePatient(p))
    {
        wxMessageBox(wxString::Format(wxT("Patient '%s' added successfully!"), name),
                     wxT("Success"), wxOK | wxICON_INFORMATION);
        OnPatientClear(wxCommandEvent{});
        RefreshPatientList();
        RefreshDashboard();
    }
}

void MainFrame::OnPatientUpdate(wxCommandEvent&)
{
    if (m_selPatient < 0) {
        wxMessageBox(wxT("Please select a patient from the list first."),
                     wxT("No Selection"), wxOK | wxICON_WARNING);
        return;
    }
    int id;
    if (!ValidateInt(m_txtPID->GetValue(), 1, 999999, id)) return;

    auto records = LoadPatients();
    bool found = false;
    for (auto& p : records)
    {
        if (p.id == id)
        {
            p.name       = m_txtPName->GetValue().Trim().ToStdString();
            p.age        = m_txtPAge->GetValue().Trim().ToStdString();
            p.gender     = m_choPGender->GetStringSelection().ToStdString();
            p.phone      = m_txtPPhone->GetValue().Trim().ToStdString();
            p.address    = m_txtPAddress->GetValue().Trim().ToStdString();
            p.disease    = m_txtPDisease->GetValue().Trim().ToStdString();
            p.bloodGroup = m_choPBlood->GetStringSelection().ToStdString();
            p.emergency  = m_txtPEmergency->GetValue().Trim().ToStdString();
            p.doctor     = m_txtPDoctor->GetValue().Trim().ToStdString();
            p.room       = m_txtPRoom->GetValue().Trim().ToStdString();
            found = true;
        }
    }
    if (found)
    {
        RewritePatients(records);
        wxMessageBox(wxT("Patient record updated successfully!"), wxT("Updated"), wxOK | wxICON_INFORMATION);
        OnPatientClear(wxCommandEvent{});
        RefreshPatientList();
    }
}

void MainFrame::OnPatientDelete(wxCommandEvent&)
{
    if (m_selPatient < 0) {
        wxMessageBox(wxT("Please select a patient from the list first."),
                     wxT("No Selection"), wxOK | wxICON_WARNING);
        return;
    }
    int id;
    if (!ValidateInt(m_txtPID->GetValue(), 1, 999999, id)) return;
    wxString name = m_txtPName->GetValue();
    if (wxMessageBox(wxString::Format(wxT("Delete patient '%s' (ID: %d)?"), name, id),
                     wxT("Confirm Delete"), wxYES_NO | wxICON_WARNING) != wxYES) return;

    auto records = LoadPatients();
    records.erase(std::remove_if(records.begin(), records.end(),
                                  [id](const PatientRecord& p){ return p.id == id; }),
                  records.end());
    RewritePatients(records);
    wxMessageBox(wxT("Patient deleted."), wxT("Deleted"), wxOK | wxICON_INFORMATION);
    OnPatientClear(wxCommandEvent{});
    RefreshPatientList();
    RefreshDashboard();
    m_selPatient = -1;
}

void MainFrame::OnPatientClear(wxCommandEvent&)
{
    m_txtPID->Clear();
    m_txtPName->Clear();
    m_txtPAge->Clear();
    m_choPGender->SetSelection(0);
    m_txtPPhone->Clear();
    m_txtPAddress->Clear();
    m_txtPDisease->Clear();
    m_choPBlood->SetSelection(0);
    m_txtPEmergency->Clear();
    m_txtPDoctor->Clear();
    m_txtPRoom->Clear();
    m_selPatient = -1;
}

void MainFrame::OnPatientSelect(wxListEvent& evt)
{
    m_selPatient = evt.GetIndex();
    auto records = LoadPatients();
    if (m_selPatient >= (long)records.size()) return;

    // Need to match visible row to actual record (accounting for filter)
    wxString idStr = m_lstPatients->GetItemText(m_selPatient, 0);
    int selId; long tmp; idStr.ToLong(&tmp); selId = (int)tmp;

    for (auto& p : records)
    {
        if (p.id == selId)
        {
            m_txtPID->SetValue(wxString::Format(wxT("%d"), p.id));
            m_txtPName->SetValue(wxString::FromUTF8(p.name));
            m_txtPAge->SetValue(wxString::FromUTF8(p.age));
            int gIdx = m_choPGender->FindString(wxString::FromUTF8(p.gender));
            m_choPGender->SetSelection(gIdx >= 0 ? gIdx : 0);
            m_txtPPhone->SetValue(wxString::FromUTF8(p.phone));
            m_txtPAddress->SetValue(wxString::FromUTF8(p.address));
            m_txtPDisease->SetValue(wxString::FromUTF8(p.disease));
            int bIdx = m_choPBlood->FindString(wxString::FromUTF8(p.bloodGroup));
            m_choPBlood->SetSelection(bIdx >= 0 ? bIdx : 0);
            m_txtPEmergency->SetValue(wxString::FromUTF8(p.emergency));
            m_txtPDoctor->SetValue(wxString::FromUTF8(p.doctor));
            m_txtPRoom->SetValue(wxString::FromUTF8(p.room));
            break;
        }
    }
}

void MainFrame::OnPatientSearch(wxCommandEvent& evt)
{
    RefreshPatientList(evt.GetString());
}

// ─── Doctor Events ─────────────────────────────────────────────────
void MainFrame::OnDoctorAdd(wxCommandEvent&)
{
    int id;
    if (!ValidateInt(m_txtDID->GetValue(), 1, 999999, id)) {
        wxMessageBox(wxT("Doctor ID must be a positive integer."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    auto records = LoadDoctors();
    for (auto& d : records)
        if (d.id == id) {
            wxMessageBox(wxString::Format(wxT("Doctor ID %d already exists!"), id), wxT("Error"), wxOK | wxICON_ERROR);
            return;
        }

    DoctorRecord d;
    d.id      = id;
    d.name    = m_txtDName->GetValue().Trim().ToStdString();
    d.age     = m_txtDAge->GetValue().Trim().ToStdString();
    d.gender  = m_choDGender->GetStringSelection().ToStdString();
    d.phone   = m_txtDPhone->GetValue().Trim().ToStdString();
    d.address = m_txtDAddress->GetValue().Trim().ToStdString();
    d.spec    = m_txtDSpec->GetValue().Trim().ToStdString();
    d.qual    = m_txtDQual->GetValue().Trim().ToStdString();
    d.exp     = m_txtDExp->GetValue().Trim().ToStdString();
    d.avail   = m_txtDAvail->GetValue().Trim().ToStdString();
    d.dept    = m_txtDDept->GetValue().Trim().ToStdString();

    if (d.name.empty()) {
        wxMessageBox(wxT("Doctor name cannot be empty."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    if (SaveDoctor(d)) {
        wxMessageBox(wxString::Format(wxT("Doctor '%s' added successfully!"), d.name),
                     wxT("Success"), wxOK | wxICON_INFORMATION);
        OnDoctorClear(wxCommandEvent{});
        RefreshDoctorList();
        RefreshDashboard();
    }
}

void MainFrame::OnDoctorUpdate(wxCommandEvent&)
{
    if (m_selDoctor < 0) {
        wxMessageBox(wxT("Select a doctor from the list first."), wxT("No Selection"), wxOK | wxICON_WARNING);
        return;
    }
    int id; if (!ValidateInt(m_txtDID->GetValue(), 1, 999999, id)) return;
    auto records = LoadDoctors();
    for (auto& d : records)
    {
        if (d.id == id)
        {
            d.name   = m_txtDName->GetValue().Trim().ToStdString();
            d.age    = m_txtDAge->GetValue().Trim().ToStdString();
            d.gender = m_choDGender->GetStringSelection().ToStdString();
            d.phone  = m_txtDPhone->GetValue().Trim().ToStdString();
            d.address= m_txtDAddress->GetValue().Trim().ToStdString();
            d.spec   = m_txtDSpec->GetValue().Trim().ToStdString();
            d.qual   = m_txtDQual->GetValue().Trim().ToStdString();
            d.exp    = m_txtDExp->GetValue().Trim().ToStdString();
            d.avail  = m_txtDAvail->GetValue().Trim().ToStdString();
            d.dept   = m_txtDDept->GetValue().Trim().ToStdString();
        }
    }
    RewriteDoctors(records);
    wxMessageBox(wxT("Doctor record updated."), wxT("Updated"), wxOK | wxICON_INFORMATION);
    OnDoctorClear(wxCommandEvent{});
    RefreshDoctorList();
}

void MainFrame::OnDoctorDelete(wxCommandEvent&)
{
    if (m_selDoctor < 0) {
        wxMessageBox(wxT("Select a doctor from the list first."), wxT("No Selection"), wxOK | wxICON_WARNING);
        return;
    }
    int id; if (!ValidateInt(m_txtDID->GetValue(), 1, 999999, id)) return;
    if (wxMessageBox(wxString::Format(wxT("Delete Doctor ID %d?"), id),
                     wxT("Confirm"), wxYES_NO | wxICON_WARNING) != wxYES) return;
    auto records = LoadDoctors();
    records.erase(std::remove_if(records.begin(), records.end(),
                                  [id](const DoctorRecord& d){ return d.id == id; }), records.end());
    RewriteDoctors(records);
    wxMessageBox(wxT("Doctor deleted."), wxT("Deleted"), wxOK | wxICON_INFORMATION);
    OnDoctorClear(wxCommandEvent{});
    RefreshDoctorList();
    RefreshDashboard();
    m_selDoctor = -1;
}

void MainFrame::OnDoctorClear(wxCommandEvent&)
{
    m_txtDID->Clear(); m_txtDName->Clear(); m_txtDAge->Clear();
    m_choDGender->SetSelection(0); m_txtDPhone->Clear(); m_txtDAddress->Clear();
    m_txtDSpec->Clear(); m_txtDQual->Clear(); m_txtDExp->Clear();
    m_txtDAvail->Clear(); m_txtDDept->Clear();
    m_selDoctor = -1;
}

void MainFrame::OnDoctorSelect(wxListEvent& evt)
{
    m_selDoctor = evt.GetIndex();
    wxString idStr = m_lstDoctors->GetItemText(m_selDoctor, 0);
    int selId; long tmp; idStr.ToLong(&tmp); selId = (int)tmp;
    auto records = LoadDoctors();
    for (auto& d : records)
    {
        if (d.id == selId)
        {
            m_txtDID->SetValue(wxString::Format(wxT("%d"), d.id));
            m_txtDName->SetValue(wxString::FromUTF8(d.name));
            m_txtDAge->SetValue(wxString::FromUTF8(d.age));
            int gIdx = m_choDGender->FindString(wxString::FromUTF8(d.gender));
            m_choDGender->SetSelection(gIdx >= 0 ? gIdx : 0);
            m_txtDPhone->SetValue(wxString::FromUTF8(d.phone));
            m_txtDAddress->SetValue(wxString::FromUTF8(d.address));
            m_txtDSpec->SetValue(wxString::FromUTF8(d.spec));
            m_txtDQual->SetValue(wxString::FromUTF8(d.qual));
            m_txtDExp->SetValue(wxString::FromUTF8(d.exp));
            m_txtDAvail->SetValue(wxString::FromUTF8(d.avail));
            m_txtDDept->SetValue(wxString::FromUTF8(d.dept));
            break;
        }
    }
}

void MainFrame::OnDoctorSearch(wxCommandEvent& evt)
{
    RefreshDoctorList(evt.GetString());
}

// ─── Appointment Events ────────────────────────────────────────────
void MainFrame::OnApptAdd(wxCommandEvent&)
{
    int id; if (!ValidateInt(m_txtAID->GetValue(), 1, 999999, id)) {
        wxMessageBox(wxT("Appointment ID must be a positive integer."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    auto records = LoadAppointments();
    for (auto& a : records)
        if (a.id == id) {
            wxMessageBox(wxT("Appointment ID already exists!"), wxT("Error"), wxOK | wxICON_ERROR);
            return;
        }

    AppointmentRecord a;
    a.id          = id;
    a.patientName = m_txtAPatient->GetValue().Trim().ToStdString();
    a.doctorName  = m_txtADoctor->GetValue().Trim().ToStdString();
    a.date        = m_txtADate->GetValue().Trim().ToStdString();
    a.time        = m_txtATime->GetValue().Trim().ToStdString();
    a.disease     = m_txtADisease->GetValue().Trim().ToStdString();
    a.status      = m_choAStatus->GetStringSelection().ToStdString();

    if (a.patientName.empty() || a.doctorName.empty()) {
        wxMessageBox(wxT("Patient and Doctor names are required."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    if (SaveAppointment(a)) {
        wxMessageBox(wxT("Appointment booked successfully!"), wxT("Success"), wxOK | wxICON_INFORMATION);
        OnApptClear(wxCommandEvent{});
        RefreshAppointmentList();
    }
}

void MainFrame::OnApptCancel(wxCommandEvent&)
{
    if (m_selAppt < 0) {
        wxMessageBox(wxT("Select an appointment from the list first."), wxT("No Selection"), wxOK | wxICON_WARNING);
        return;
    }
    wxString idStr = m_lstAppts->GetItemText(m_selAppt, 0);
    int selId; long tmp; idStr.ToLong(&tmp); selId = (int)tmp;
    auto records = LoadAppointments();
    for (auto& a : records)
        if (a.id == selId) a.status = "Cancelled";
    RewriteAppointments(records);
    wxMessageBox(wxT("Appointment cancelled."), wxT("Updated"), wxOK | wxICON_INFORMATION);
    RefreshAppointmentList();
}

void MainFrame::OnApptComplete(wxCommandEvent&)
{
    if (m_selAppt < 0) {
        wxMessageBox(wxT("Select an appointment from the list first."), wxT("No Selection"), wxOK | wxICON_WARNING);
        return;
    }
    wxString idStr = m_lstAppts->GetItemText(m_selAppt, 0);
    int selId; long tmp; idStr.ToLong(&tmp); selId = (int)tmp;
    auto records = LoadAppointments();
    for (auto& a : records)
        if (a.id == selId) a.status = "Completed";
    RewriteAppointments(records);
    wxMessageBox(wxT("Appointment marked as Completed."), wxT("Updated"), wxOK | wxICON_INFORMATION);
    RefreshAppointmentList();
}

void MainFrame::OnApptClear(wxCommandEvent&)
{
    m_txtAID->Clear(); m_txtAPatient->Clear(); m_txtADoctor->Clear();
    m_txtADate->Clear(); m_txtATime->Clear(); m_txtADisease->Clear();
    m_choAStatus->SetSelection(0);
    m_selAppt = -1;
}

void MainFrame::OnApptSelect(wxListEvent& evt)
{
    m_selAppt = evt.GetIndex();
    m_txtAID->SetValue(m_lstAppts->GetItemText(m_selAppt, 0));
    m_txtAPatient->SetValue(m_lstAppts->GetItemText(m_selAppt, 1));
    m_txtADoctor->SetValue(m_lstAppts->GetItemText(m_selAppt, 2));
    m_txtADate->SetValue(m_lstAppts->GetItemText(m_selAppt, 3));
    m_txtATime->SetValue(m_lstAppts->GetItemText(m_selAppt, 4));
    m_txtADisease->SetValue(m_lstAppts->GetItemText(m_selAppt, 5));
    wxString stat = m_lstAppts->GetItemText(m_selAppt, 6);
    int sIdx = m_choAStatus->FindString(stat);
    m_choAStatus->SetSelection(sIdx >= 0 ? sIdx : 0);
}

// ─── Billing Events ────────────────────────────────────────────────
void MainFrame::OnBillCalc(wxCommandEvent&)
{
    double c = 0, m = 0, r = 0, o = 0;
    m_txtBConsult->GetValue().ToDouble(&c);
    m_txtBMed->GetValue().ToDouble(&m);
    m_txtBRoom->GetValue().ToDouble(&r);
    m_txtBOther->GetValue().ToDouble(&o);
    m_lblBTotal->SetLabel(wxString::Format(wxT("Rs. %.2f"), c + m + r + o));
}

void MainFrame::OnBillGenerate(wxCommandEvent&)
{
    int billId; if (!ValidateInt(m_txtBillID->GetValue(), 1, 999999, billId)) {
        wxMessageBox(wxT("Enter a valid Bill ID."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    int patId; if (!ValidateInt(m_txtBPatientID->GetValue(), 1, 999999, patId)) {
        wxMessageBox(wxT("Enter a valid Patient ID."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    double c = 0, m = 0, r = 0, o = 0;
    if (!ValidateDouble(m_txtBConsult->GetValue(), c) ||
        !ValidateDouble(m_txtBMed->GetValue(),    m) ||
        !ValidateDouble(m_txtBRoom->GetValue(),   r) ||
        !ValidateDouble(m_txtBOther->GetValue(),  o))
    {
        wxMessageBox(wxT("All charge fields must be valid positive numbers."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }

    time_t now = time(nullptr); tm* t = localtime(&now);
    char buf[32]; strftime(buf, sizeof(buf), "%d/%m/%Y", t);

    BillRecord b;
    b.billId      = billId;
    b.patientId   = patId;
    b.patientName = m_txtBPatientName->GetValue().Trim().ToStdString();
    b.consult     = c; b.medicine = m; b.room = r; b.other = o;
    b.total       = c + m + r + o;
    b.status      = "Unpaid";
    b.date        = buf;

    if (SaveBill(b)) {
        wxMessageBox(wxString::Format(
            wxT("Bill generated!\n\nPatient: %s\nTotal: Rs. %.2f\nStatus: Unpaid"),
            b.patientName, b.total),
            wxT("Bill Generated"), wxOK | wxICON_INFORMATION);
        OnBillClear(wxCommandEvent{});
        RefreshBillList();
        RefreshDashboard();
    }
}

void MainFrame::OnBillClear(wxCommandEvent&)
{
    m_txtBillID->Clear(); m_txtBPatientID->Clear(); m_txtBPatientName->Clear();
    m_txtBConsult->Clear(); m_txtBMed->Clear(); m_txtBRoom->Clear(); m_txtBOther->Clear();
    m_lblBTotal->SetLabel(wxT("Rs. 0.00"));
    m_selBill = -1;
}

void MainFrame::OnBillSelect(wxListEvent& evt)
{
    m_selBill = evt.GetIndex();
}

// ─── Pharmacy Events ───────────────────────────────────────────────
void MainFrame::OnPharmaAdd(wxCommandEvent&)
{
    int id; if (!ValidateInt(m_txtMID->GetValue(), 1, 999999, id)) {
        wxMessageBox(wxT("Medicine ID must be a positive integer."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    wxString name = m_txtMName->GetValue().Trim();
    if (name.IsEmpty()) {
        wxMessageBox(wxT("Medicine name cannot be empty."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    double price = 0;
    if (!ValidateDouble(m_txtMPrice->GetValue(), price)) {
        wxMessageBox(wxT("Price must be a valid positive number."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    int qty; if (!ValidateInt(m_txtMQty->GetValue(), 0, 999999, qty)) {
        wxMessageBox(wxT("Quantity must be a non-negative integer."), wxT("Error"), wxOK | wxICON_WARNING);
        return;
    }
    MedicineRecord m;
    m.id           = id;
    m.name         = name.ToStdString();
    m.category     = m_txtMCat->GetValue().Trim().ToStdString();
    m.manufacturer = m_txtMMfg->GetValue().Trim().ToStdString();
    m.price        = price;
    m.qty          = qty;
    m.expiry       = m_txtMExpiry->GetValue().Trim().ToStdString();

    if (SaveMedicine(m)) {
        wxMessageBox(wxString::Format(wxT("Medicine '%s' added to pharmacy!"), name),
                     wxT("Success"), wxOK | wxICON_INFORMATION);
        OnPharmaClear(wxCommandEvent{});
        RefreshMedicineList();
    }
}

void MainFrame::OnPharmaClear(wxCommandEvent&)
{
    m_txtMID->Clear(); m_txtMName->Clear(); m_txtMCat->Clear(); m_txtMMfg->Clear();
    m_txtMPrice->Clear(); m_txtMQty->Clear(); m_txtMExpiry->Clear();
    m_selMed = -1;
}

void MainFrame::OnPharmaSelect(wxListEvent& evt)
{
    m_selMed = evt.GetIndex();
    m_txtMID->SetValue(m_lstMeds->GetItemText(m_selMed, 0));
    m_txtMName->SetValue(m_lstMeds->GetItemText(m_selMed, 1));
    m_txtMCat->SetValue(m_lstMeds->GetItemText(m_selMed, 2));
    m_txtMMfg->SetValue(m_lstMeds->GetItemText(m_selMed, 3));
    wxString price = m_lstMeds->GetItemText(m_selMed, 4);
    price.Replace(wxT("Rs. "), wxT(""));
    m_txtMPrice->SetValue(price);
    m_txtMQty->SetValue(m_lstMeds->GetItemText(m_selMed, 5));
    m_txtMExpiry->SetValue(m_lstMeds->GetItemText(m_selMed, 6));
}

void MainFrame::OnPharmaSearch(wxCommandEvent& evt)
{
    RefreshMedicineList(evt.GetString());
}

// ─── Reports Event ─────────────────────────────────────────────────
void MainFrame::OnReportGenerate(wxCommandEvent&)
{
    auto* reportText = m_reportText;
    if (!reportText) return;

    auto pts  = LoadPatients();
    auto drs  = LoadDoctors();
    auto apts = LoadAppointments();
    auto bls  = LoadBills();
    auto meds = LoadMedicines();

    double totalRev = 0, paidRev = 0;
    int paid = 0, unpaid = 0;
    for (auto& b : bls) {
        totalRev += b.total;
        if (b.status == "Paid") { paidRev += b.total; ++paid; }
        else ++unpaid;
    }

    int pending = 0, completed = 0, cancelled = 0;
    for (auto& a : apts) {
        if (a.status == "Completed")  ++completed;
        else if (a.status == "Cancelled") ++cancelled;
        else ++pending;
    }

    time_t now = time(nullptr); tm* t = localtime(&now);
    char datebuf[32]; strftime(datebuf, sizeof(datebuf), "%d/%m/%Y %I:%M %p", t);

    wxString report;
    report += wxT("╔══════════════════════════════════════════════════════╗\n");
    report += wxT("║        CITY CARE HOSPITAL — MANAGEMENT REPORT       ║\n");
    report += wxT("╚══════════════════════════════════════════════════════╝\n\n");
    report += wxString::Format(wxT("  Report Generated : %s\n\n"), datebuf);
    report += wxT("─────────────────────────────────────────────────────\n");
    report += wxT("  STAFF & PATIENTS\n");
    report += wxT("─────────────────────────────────────────────────────\n");
    report += wxString::Format(wxT("  Total Patients Registered   : %zu\n"), pts.size());
    report += wxString::Format(wxT("  Total Doctors on Staff      : %zu\n"), drs.size());
    report += wxT("\n─────────────────────────────────────────────────────\n");
    report += wxT("  APPOINTMENTS\n");
    report += wxT("─────────────────────────────────────────────────────\n");
    report += wxString::Format(wxT("  Total Appointments          : %zu\n"), apts.size());
    report += wxString::Format(wxT("  Completed                   : %d\n"), completed);
    report += wxString::Format(wxT("  Pending                     : %d\n"), pending);
    report += wxString::Format(wxT("  Cancelled                   : %d\n"), cancelled);
    report += wxT("\n─────────────────────────────────────────────────────\n");
    report += wxT("  BILLING & REVENUE\n");
    report += wxT("─────────────────────────────────────────────────────\n");
    report += wxString::Format(wxT("  Total Bills Generated        : %zu\n"), bls.size());
    report += wxString::Format(wxT("  Paid Bills                   : %d\n"), paid);
    report += wxString::Format(wxT("  Unpaid Bills                 : %d\n"), unpaid);
    report += wxString::Format(wxT("  Total Revenue (All Bills)    : Rs. %.2f\n"), totalRev);
    report += wxString::Format(wxT("  Collected Revenue (Paid)     : Rs. %.2f\n"), paidRev);
    report += wxString::Format(wxT("  Outstanding Receivable       : Rs. %.2f\n"), totalRev - paidRev);
    report += wxT("\n─────────────────────────────────────────────────────\n");
    report += wxT("  PHARMACY\n");
    report += wxT("─────────────────────────────────────────────────────\n");
    report += wxString::Format(wxT("  Total Medicines in Stock     : %zu\n"), meds.size());

    int lowStock = 0;
    for (auto& m : meds) if (m.qty < 10) ++lowStock;
    report += wxString::Format(wxT("  Low Stock Alerts (< 10 units): %d\n"), lowStock);

    report += wxT("\n─────────────────────────────────────────────────────\n");
    report += wxT("  DOCTOR DEPARTMENTS\n");
    report += wxT("─────────────────────────────────────────────────────\n");
    for (auto& d : drs)
        report += wxString::Format(wxT("  %-28s  %s\n"),
                                    wxString::FromUTF8(d.name),
                                    wxString::FromUTF8(d.dept));

    report += wxT("\n╔══════════════════════════════════════════════════════╗\n");
    report += wxT("║              END OF REPORT                          ║\n");
    report += wxT("╚══════════════════════════════════════════════════════╝\n");

    reportText->SetValue(report);

    // Also save to file
    std::ofstream rf("report.txt", std::ios::app);
    rf << "\n=== REPORT: " << datebuf << " ===\n";
    rf << "Patients     : " << pts.size()  << "\n";
    rf << "Doctors      : " << drs.size()  << "\n";
    rf << "Appointments : " << apts.size() << "\n";
    rf << "Bills        : " << bls.size()  << "\n";
    rf << "Medicines    : " << meds.size() << "\n";
    rf << std::fixed << std::setprecision(2);
    rf << "Revenue      : Rs. " << totalRev << "\n";
    rf.close();

    wxMessageBox(wxT("Report generated and saved to report.txt"),
                 wxT("Report Ready"), wxOK | wxICON_INFORMATION);
}