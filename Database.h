#pragma once
// ============================================================
//  Database.h  —  MySQL Connector/C++ wrapper
//  Hospital Management System
//  Connects to: localhost / XAMPP MySQL / database: hospital_db
// ============================================================

#include <string>
#include <vector>
#include <stdexcept>

// MySQL Connector/C++ headers
#ifdef USE_MYSQL_DB
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#endif

// ──────────────────────────────────────────────────────────
//  Data structs that mirror your existing plain-text records
// ──────────────────────────────────────────────────────────

struct DBPatient {
    int         id          = 0;
    std::string name;
    int         age         = 0;
    std::string gender;
    std::string disease;
    std::string phone;
    std::string address;
    std::string admitDate;
    std::string ward;
    std::string status;     // "Admitted" / "Discharged"
};

struct DBDoctor {
    int         id          = 0;
    std::string name;
    std::string specialization;
    std::string phone;
    std::string email;
    std::string schedule;
    std::string department;
    int         experience  = 0;
    double      fee         = 0.0;
    std::string status;     // "Available" / "On Leave"
};

struct DBAppointment {
    int         id            = 0;
    std::string patientName;
    std::string doctorName;
    std::string date;
    std::string time;
    std::string department;
    std::string reason;
    std::string status;       // "Scheduled" / "Completed" / "Cancelled"
    std::string notes;
    std::string contactNumber;
    std::string appointmentType;
};

struct DBBill {
    int         id            = 0;
    std::string patientName;
    std::string patientId;
    double      roomCharge    = 0.0;
    double      medicineCharge= 0.0;
    double      doctorFee     = 0.0;
    double      testCharges   = 0.0;
    double      otherCharges  = 0.0;
    double      totalAmount   = 0.0;
    std::string paymentStatus;  // "Paid" / "Unpaid" / "Partial"
    std::string billDate;
};

// ──────────────────────────────────────────────────────────
//  Database class  —  one singleton instance per app session
// ──────────────────────────────────────────────────────────

class Database {
public:
    // ── Connection settings (edit if your XAMPP uses different port/user) ──
    static constexpr const char* HOST     = "tcp://127.0.0.1:3306";
    static constexpr const char* USER     = "root";
    static constexpr const char* PASSWORD = "";          // XAMPP default: blank
    static constexpr const char* SCHEMA   = "hospital_db";

    // Singleton access
    static Database& instance();

    // Lifecycle
    bool  connect();
    void  disconnect();
    bool  isConnected() const;

    // Schema helper — call once on first run
    bool  createTablesIfNotExist();

    // ── Patients ──────────────────────────────────────────
    bool                    addPatient   (const DBPatient& p);
    bool                    updatePatient(const DBPatient& p);
    bool                    deletePatient(int id);
    std::vector<DBPatient>  getAllPatients();
    DBPatient               getPatientById(int id);

    // ── Doctors ───────────────────────────────────────────
    bool                    addDoctor   (const DBDoctor& d);
    bool                    updateDoctor(const DBDoctor& d);
    bool                    deleteDoctor(int id);
    std::vector<DBDoctor>   getAllDoctors();
    DBDoctor                getDoctorById(int id);

    // ── Appointments ──────────────────────────────────────
    bool                         addAppointment   (const DBAppointment& a);
    bool                         updateAppointment(const DBAppointment& a);
    bool                         deleteAppointment(int id);
    std::vector<DBAppointment>   getAllAppointments();

    // ── Bills ─────────────────────────────────────────────
    bool                    addBill   (const DBBill& b);
    bool                    updateBill(const DBBill& b);
    bool                    deleteBill(int id);
    std::vector<DBBill>     getAllBills();

    // ── Utility ───────────────────────────────────────────
    std::string getLastError() const { return m_lastError; }

private:
    Database() = default;
    ~Database() { disconnect(); }
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

#ifdef USE_MYSQL_DB
    sql::mysql::MySQL_Driver* m_driver     = nullptr;
    sql::Connection*          m_conn       = nullptr;
#else
    void*                     m_driver     = nullptr;
    void*                     m_conn       = nullptr;
#endif
    std::string               m_lastError;

    void setError(const std::string& msg) { m_lastError = msg; }
};
