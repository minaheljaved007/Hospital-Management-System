// MainFrame.h
// Full GUI declaration for Hospital Management System
// Modules: Dashboard, Patients, Doctors, Appointments, Billing, Pharmacy, Reports

#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/grid.h>
#include <wx/srchctrl.h>
#include <wx/statbox.h>
#include <wx/choice.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/spinctrl.h>
#include <wx/colordlg.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/timer.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// ─── Colour Palette ────────────────────────────────────────────────
#define CLR_BG          wxColour(15,  23,  42)    // Deep navy  
#define CLR_SIDEBAR     wxColour(10,  15,  30)    // Darker sidebar
#define CLR_CARD        wxColour(22,  35,  60)    // Card background
#define CLR_ACCENT      wxColour(56, 189, 248)    // Sky blue accent
#define CLR_ACCENT2     wxColour(34, 211, 238)    // Cyan secondary
#define CLR_GREEN       wxColour(34, 197, 94)     // Success green
#define CLR_RED         wxColour(239, 68,  68)    // Error / danger
#define CLR_ORANGE      wxColour(251, 146,  60)   // Warning orange
#define CLR_YELLOW      wxColour(250, 204,  21)   // Highlight yellow
#define CLR_TEXT        wxColour(226, 232, 240)   // Light text
#define CLR_MUTED       wxColour(100, 116, 139)   // Muted / placeholder
#define CLR_INPUT_BG    wxColour(30,  41,  59)    // Input field background
#define CLR_BORDER      wxColour(51,  65,  85)    // Border / separator

// ─── ID Constants ──────────────────────────────────────────────────
enum
{
    ID_NAV_DASHBOARD = wxID_HIGHEST + 1,
    ID_NAV_PATIENTS,
    ID_NAV_DOCTORS,
    ID_NAV_APPOINTMENTS,
    ID_NAV_BILLING,
    ID_NAV_PHARMACY,
    ID_NAV_REPORTS,
    ID_NAV_EXIT,

    ID_PATIENT_ADD,
    ID_PATIENT_SEARCH,
    ID_PATIENT_UPDATE,
    ID_PATIENT_DELETE,
    ID_PATIENT_CLEAR,
    ID_PATIENT_LIST,

    ID_DOCTOR_ADD,
    ID_DOCTOR_SEARCH,
    ID_DOCTOR_UPDATE,
    ID_DOCTOR_DELETE,
    ID_DOCTOR_CLEAR,
    ID_DOCTOR_LIST,

    ID_APPT_ADD,
    ID_APPT_CANCEL,
    ID_APPT_COMPLETE,
    ID_APPT_CLEAR,
    ID_APPT_LIST,

    ID_BILL_GENERATE,
    ID_BILL_VIEW,
    ID_BILL_CLEAR,
    ID_BILL_LIST,

    ID_PHARMA_ADD,
    ID_PHARMA_SEARCH,
    ID_PHARMA_CLEAR,
    ID_PHARMA_LIST,

    ID_REPORT_GENERATE,
    ID_TIMER_CLOCK
};

// ─── Data Structs ──────────────────────────────────────────────────
struct PatientRecord {
    int         id;
    std::string name, age, gender, phone, address;
    std::string disease, bloodGroup, emergency, doctor, room, admDate;
};

struct DoctorRecord {
    int         id;
    std::string name, age, gender, phone, address;
    std::string spec, qual, exp, avail, dept;
};

struct AppointmentRecord {
    int         id;
    std::string patientName, doctorName, date, time, disease, status;
};

struct BillRecord {
    int         billId, patientId;
    std::string patientName;
    double      consult, medicine, room, other, total;
    std::string status, date;
};

struct MedicineRecord {
    int         id;
    std::string name, category, manufacturer, expiry;
    double      price;
    int         qty;
};

// ─── MainFrame ─────────────────────────────────────────────────────
class MainFrame : public wxFrame
{
public:
    explicit MainFrame(const wxString& title);

private:
    // ── Layout panels
    wxPanel* m_sidebar;
    wxPanel* m_contentHost;
    wxBoxSizer* m_contentSizer;

    // ── Active page panels (one shown at a time)
    wxPanel* m_pageDashboard;
    wxPanel* m_pagePatients;
    wxPanel* m_pageDoctors;
    wxPanel* m_pageAppointments;
    wxPanel* m_pageBilling;
    wxPanel* m_pagePharmacy;
    wxPanel* m_pageReports;
    wxPanel* m_activePage = nullptr;

    // ── Clock
    wxStaticText* m_clockLabel;
    wxTimer       m_clockTimer;

    // ── Patient page controls
    wxTextCtrl* m_txtPID;
    wxTextCtrl* m_txtPName;
    wxTextCtrl* m_txtPAge;
    wxChoice* m_choPGender;
    wxTextCtrl* m_txtPPhone;
    wxTextCtrl* m_txtPAddress;
    wxTextCtrl* m_txtPDisease;
    wxChoice* m_choPBlood;
    wxTextCtrl* m_txtPEmergency;
    wxTextCtrl* m_txtPDoctor;
    wxTextCtrl* m_txtPRoom;
    wxListCtrl* m_lstPatients;
    wxSearchCtrl* m_searchPatients;

    // ── Doctor page controls
    wxTextCtrl* m_txtDID;
    wxTextCtrl* m_txtDName;
    wxTextCtrl* m_txtDAge;
    wxChoice* m_choDGender;
    wxTextCtrl* m_txtDPhone;
    wxTextCtrl* m_txtDAddress;
    wxTextCtrl* m_txtDSpec;
    wxTextCtrl* m_txtDQual;
    wxTextCtrl* m_txtDExp;
    wxTextCtrl* m_txtDAvail;
    wxTextCtrl* m_txtDDept;
    wxListCtrl* m_lstDoctors;
    wxSearchCtrl* m_searchDoctors;

    // ── Appointment page controls
    wxTextCtrl* m_txtAID;
    wxTextCtrl* m_txtAPatient;
    wxTextCtrl* m_txtADoctor;
    wxTextCtrl* m_txtADate;
    wxTextCtrl* m_txtATime;
    wxTextCtrl* m_txtADisease;
    wxChoice* m_choAStatus;
    wxListCtrl* m_lstAppts;

    // ── Billing page controls
    wxTextCtrl* m_txtBillID;
    wxTextCtrl* m_txtBPatientID;
    wxTextCtrl* m_txtBPatientName;
    wxTextCtrl* m_txtBConsult;
    wxTextCtrl* m_txtBMed;
    wxTextCtrl* m_txtBRoom;
    wxTextCtrl* m_txtBOther;
    wxStaticText* m_lblBTotal;
    wxListCtrl* m_lstBills;

    // ── Pharmacy page controls
    wxTextCtrl* m_txtMID;
    wxTextCtrl* m_txtMName;
    wxTextCtrl* m_txtMCat;
    wxTextCtrl* m_txtMMfg;
    wxTextCtrl* m_txtMPrice;
    wxTextCtrl* m_txtMQty;
    wxTextCtrl* m_txtMExpiry;
    wxListCtrl* m_lstMeds;
    wxSearchCtrl* m_searchMeds;

    // ── Dashboard stat labels
    wxStaticText* m_statPatients;
    wxStaticText* m_statDoctors;
    wxStaticText* m_statAppts;
    wxStaticText* m_statRevenue;

    // ── Reports page
    wxTextCtrl* m_reportText = nullptr;

    // ──────────────── Build helpers ────────────────────────────────
    void BuildLayout();
    void BuildSidebar();

    wxPanel* BuildDashboardPage(wxWindow* parent);
    wxPanel* BuildPatientsPage(wxWindow* parent);
    wxPanel* BuildDoctorsPage(wxWindow* parent);
    wxPanel* BuildAppointmentsPage(wxWindow* parent);
    wxPanel* BuildBillingPage(wxWindow* parent);
    wxPanel* BuildPharmacyPage(wxWindow* parent);
    wxPanel* BuildReportsPage(wxWindow* parent);

    // Shared UI helpers
    wxButton* MakeNavButton(wxWindow* parent, const wxString& label, int id);
    wxButton* MakeActionButton(wxWindow* parent, const wxString& label, int id,
                                   const wxColour& bg, const wxColour& fg);
    wxTextCtrl* MakeField(wxWindow* parent, const wxString& hint, bool readOnly = false);
    wxStaticText* MakeLabel(wxWindow* parent, const wxString& text, int ptSize = 10,
                             bool bold = false, const wxColour& clr = CLR_TEXT);
    void          AddFormRow(wxSizer* sizer, wxWindow* parent,
                              const wxString& label, wxWindow* ctrl);
    wxPanel* MakeStatCard(wxWindow* parent, const wxString& title,
                                const wxString& value, const wxColour& accent,
                                wxStaticText** outLabel = nullptr);
    void          ApplyInputStyle(wxTextCtrl* ctrl);
    void          ShowPage(wxPanel* page);

    // ──────────────── File I/O ──────────────────────────────────────
    std::vector<PatientRecord>     LoadPatients();
    std::vector<DoctorRecord>      LoadDoctors();
    std::vector<AppointmentRecord> LoadAppointments();
    std::vector<BillRecord>        LoadBills();
    std::vector<MedicineRecord>    LoadMedicines();

    bool SavePatient(const PatientRecord& p, bool append = true);
    bool SaveDoctor(const DoctorRecord& d, bool append = true);
    bool SaveAppointment(const AppointmentRecord& a, bool append = true);
    bool SaveBill(const BillRecord& b);
    bool SaveMedicine(const MedicineRecord& m);

    void RewritePatients(const std::vector<PatientRecord>& records);
    void RewriteDoctors(const std::vector<DoctorRecord>& records);
    void RewriteAppointments(const std::vector<AppointmentRecord>& records);

    // ──────────────── List refreshers ──────────────────────────────
    void RefreshPatientList(const wxString& filter = "");
    void RefreshDoctorList(const wxString& filter = "");
    void RefreshAppointmentList();
    void RefreshBillList();
    void RefreshMedicineList(const wxString& filter = "");
    void RefreshDashboard();

    // ──────────────── Validation ───────────────────────────────────
    bool ValidateInt(const wxString& s, int minV, int maxV, int& out);
    bool ValidateDouble(const wxString& s, double& out);
    bool ValidatePhone(const wxString& s);

    // ──────────────── Event handlers ───────────────────────────────
    // Navigation
    void OnNav(wxCommandEvent& evt);
    void OnExit(wxCommandEvent& evt);
    void OnTimer(wxTimerEvent& evt);

    // Patient events
    void OnPatientAdd(wxCommandEvent& evt);
    void OnPatientUpdate(wxCommandEvent& evt);
    void OnPatientDelete(wxCommandEvent& evt);
    void OnPatientClear(wxCommandEvent& evt);
    void OnPatientSelect(wxListEvent& evt);
    void OnPatientSearch(wxCommandEvent& evt);

    // Doctor events
    void OnDoctorAdd(wxCommandEvent& evt);
    void OnDoctorUpdate(wxCommandEvent& evt);
    void OnDoctorDelete(wxCommandEvent& evt);
    void OnDoctorClear(wxCommandEvent& evt);
    void OnDoctorSelect(wxListEvent& evt);
    void OnDoctorSearch(wxCommandEvent& evt);

    // Appointment events
    void OnApptAdd(wxCommandEvent& evt);
    void OnApptCancel(wxCommandEvent& evt);
    void OnApptComplete(wxCommandEvent& evt);
    void OnApptClear(wxCommandEvent& evt);
    void OnApptSelect(wxListEvent& evt);

    // Billing events
    void OnBillGenerate(wxCommandEvent& evt);
    void OnBillClear(wxCommandEvent& evt);
    void OnBillCalc(wxCommandEvent& evt);
    void OnBillSelect(wxListEvent& evt);

    // Pharmacy events
    void OnPharmaAdd(wxCommandEvent& evt);
    void OnPharmaClear(wxCommandEvent& evt);
    void OnPharmaSelect(wxListEvent& evt);
    void OnPharmaSearch(wxCommandEvent& evt);

    // Reports
    void OnReportGenerate(wxCommandEvent& evt);

    // ──────────────── Currently-selected list row indices
    long m_selPatient    = -1;
    long m_selDoctor     = -1;
    long m_selAppt       = -1;
    long m_selBill       = -1;
    long m_selMed        = -1;

    wxDECLARE_EVENT_TABLE();
};