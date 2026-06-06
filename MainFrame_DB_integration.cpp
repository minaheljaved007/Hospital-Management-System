// ============================================================
//  MainFrame_DB_integration.cpp
//  EXACT LINES TO ADD/CHANGE in your existing MainFrame.cpp
//  This is NOT a complete replacement — it shows only the
//  functions that need a database call added to them.
// ============================================================

// ── STEP 1: Add this include at the TOP of MainFrame.cpp ─────
#include "Database.h"

// ══════════════════════════════════════════════════════════════
//  EXAMPLE: How to SAVE a patient to DB when user clicks Save
//  Find your existing OnAddPatient (or similar) handler and
//  add the DB call right after you validate the form fields.
// ══════════════════════════════════════════════════════════════

// INSIDE your existing add-patient event handler, after you
// have collected all the field values into local variables,
// ADD these lines at the bottom before closing the dialog:

void MainFrame::OnSavePatient_DB_Example()
{
    // --- Your existing code that reads form fields ---
    // wxString name    = m_txtPatientName->GetValue();
    // wxString age     = m_txtAge->GetValue();
    // ... etc ...

    // --- NEW: Build DBPatient and save to MySQL ---
    DBPatient p;
    p.name      = std::string(m_txtPatientName->GetValue().mb_str());
    p.age       = wxAtoi(m_txtAge->GetValue());
    p.gender    = std::string(m_cmbGender->GetValue().mb_str());
    p.disease   = std::string(m_txtDisease->GetValue().mb_str());
    p.phone     = std::string(m_txtPhone->GetValue().mb_str());
    p.address   = std::string(m_txtAddress->GetValue().mb_str());
    p.admitDate = std::string(m_txtAdmitDate->GetValue().mb_str());
    p.ward      = std::string(m_cmbWard->GetValue().mb_str());
    p.status    = "Admitted";

    if (Database::instance().isConnected()) {
        if (!Database::instance().addPatient(p)) {
            wxMessageBox("Failed to save patient to database:\n" +
                         wxString(Database::instance().getLastError()),
                         "DB Error", wxOK | wxICON_ERROR);
        }
    }
    // Your existing .txt file save code can remain OR be removed
    // once you are happy the DB is working correctly.
}

// ══════════════════════════════════════════════════════════════
//  EXAMPLE: How to LOAD patients from DB into your list/grid
//  Find your existing LoadPatients() function and add this.
// ══════════════════════════════════════════════════════════════

void MainFrame::OnLoadPatients_DB_Example()
{
    if (!Database::instance().isConnected()) {
        // Fall back to existing .txt file loading
        // LoadPatients();  // your original function
        return;
    }

    std::vector<DBPatient> patients = Database::instance().getAllPatients();

    // Clear existing list
    m_listPatients->DeleteAllItems();  // adjust to your actual list control name

    for (const DBPatient& p : patients) {
        long idx = m_listPatients->InsertItem(m_listPatients->GetItemCount(),
                                               wxString(p.name));
        m_listPatients->SetItem(idx, 1, wxString::Format("%d", p.age));
        m_listPatients->SetItem(idx, 2, wxString(p.gender));
        m_listPatients->SetItem(idx, 3, wxString(p.disease));
        m_listPatients->SetItem(idx, 4, wxString(p.status));
    }
}

// ══════════════════════════════════════════════════════════════
//  EXAMPLE: How to DELETE a patient from DB
// ══════════════════════════════════════════════════════════════

void MainFrame::OnDeletePatient_DB_Example()
{
    long selectedItem = m_listPatients->GetNextItem(-1, wxLIST_NEXT_ALL,
                                                     wxLIST_STATE_SELECTED);
    if (selectedItem == -1) {
        wxMessageBox("Please select a patient to delete.", "No Selection",
                     wxOK | wxICON_INFORMATION);
        return;
    }

    // Get the DB id you stored (you can store it via SetItemData)
    int patientId = (int)m_listPatients->GetItemData(selectedItem);

    int confirm = wxMessageBox("Are you sure you want to delete this patient?",
                               "Confirm Delete", wxYES_NO | wxICON_WARNING);
    if (confirm == wxYES) {
        if (Database::instance().isConnected()) {
            Database::instance().deletePatient(patientId);
        }
        m_listPatients->DeleteItem(selectedItem);
    }
}

// ══════════════════════════════════════════════════════════════
//  The same pattern applies for Doctors, Appointments, Bills:
//
//  Save:   Database::instance().addDoctor(d);
//  Update: Database::instance().updateDoctor(d);
//  Delete: Database::instance().deleteDoctor(id);
//  Load:   auto list = Database::instance().getAllDoctors();
//
//  Just replace "Doctor" with "Appointment" or "Bill" as needed.
// ══════════════════════════════════════════════════════════════
