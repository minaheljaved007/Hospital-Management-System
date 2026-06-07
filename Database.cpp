// ============================================================
//  Database.cpp  —  MySQL Connector/C++ implementation
//  Hospital Management System
// ============================================================

#include "Database.h"
#include <iostream>

// ──────────────────────────────────────────────────────────
//  Singleton
// ──────────────────────────────────────────────────────────

Database& Database::instance() {
    static Database db;
    return db;
}

#ifdef USE_MYSQL_DB

// ──────────────────────────────────────────────────────────
//  Connect / Disconnect
// ──────────────────────────────────────────────────────────

bool Database::connect() {
    try {
        m_driver = sql::mysql::get_mysql_driver_instance();
        m_conn   = m_driver->connect(HOST, USER, PASSWORD);
        m_conn->setSchema(SCHEMA);
        m_conn->setAutoCommit(true);
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("Connection failed: ") + e.what());
        return false;
    }
}

void Database::disconnect() {
    if (m_conn) {
        delete m_conn;
        m_conn = nullptr;
    }
}

bool Database::isConnected() const {
    return m_conn && !m_conn->isClosed();
}

// ──────────────────────────────────────────────────────────
//  Create Tables  (run once — safe to call every startup)
// ──────────────────────────────────────────────────────────

bool Database::createTablesIfNotExist() {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());

        // Patients
        stmt->execute(R"(
            CREATE TABLE IF NOT EXISTS patients (
                id            INT AUTO_INCREMENT PRIMARY KEY,
                name          VARCHAR(100) NOT NULL,
                age           INT,
                gender        VARCHAR(10),
                disease       VARCHAR(150),
                phone         VARCHAR(20),
                address       VARCHAR(200),
                admit_date    VARCHAR(30),
                ward          VARCHAR(50),
                status        VARCHAR(20) DEFAULT 'Admitted'
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
        )");

        // Doctors
        stmt->execute(R"(
            CREATE TABLE IF NOT EXISTS doctors (
                id               INT AUTO_INCREMENT PRIMARY KEY,
                name             VARCHAR(100) NOT NULL,
                specialization   VARCHAR(100),
                phone            VARCHAR(20),
                email            VARCHAR(100),
                schedule         VARCHAR(100),
                department       VARCHAR(100),
                experience       INT DEFAULT 0,
                fee              DECIMAL(10,2) DEFAULT 0.00,
                status           VARCHAR(20) DEFAULT 'Available'
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
        )");

        // Appointments
        stmt->execute(R"(
            CREATE TABLE IF NOT EXISTS appointments (
                id                INT AUTO_INCREMENT PRIMARY KEY,
                patient_name      VARCHAR(100),
                doctor_name       VARCHAR(100),
                date              VARCHAR(30),
                time              VARCHAR(20),
                department        VARCHAR(100),
                reason            VARCHAR(200),
                status            VARCHAR(20) DEFAULT 'Scheduled',
                notes             TEXT,
                contact_number    VARCHAR(20),
                appointment_type  VARCHAR(50)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
        )");

        // Bills
        stmt->execute(R"(
            CREATE TABLE IF NOT EXISTS bills (
                id                INT AUTO_INCREMENT PRIMARY KEY,
                patient_name      VARCHAR(100),
                patient_id        VARCHAR(50),
                room_charge       DECIMAL(10,2) DEFAULT 0.00,
                medicine_charge   DECIMAL(10,2) DEFAULT 0.00,
                doctor_fee        DECIMAL(10,2) DEFAULT 0.00,
                test_charges      DECIMAL(10,2) DEFAULT 0.00,
                other_charges     DECIMAL(10,2) DEFAULT 0.00,
                total_amount      DECIMAL(10,2) DEFAULT 0.00,
                payment_status    VARCHAR(20) DEFAULT 'Unpaid',
                bill_date         VARCHAR(30)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
        )");

        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("createTablesIfNotExist: ") + e.what());
        return false;
    }
}

// ══════════════════════════════════════════════════════════
//  PATIENTS
// ══════════════════════════════════════════════════════════

bool Database::addPatient(const DBPatient& p) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "INSERT INTO patients (name,age,gender,disease,phone,address,admit_date,ward,status) "
            "VALUES (?,?,?,?,?,?,?,?,?)"
        ));
        ps->setString(1, p.name);
        ps->setInt   (2, p.age);
        ps->setString(3, p.gender);
        ps->setString(4, p.disease);
        ps->setString(5, p.phone);
        ps->setString(6, p.address);
        ps->setString(7, p.admitDate);
        ps->setString(8, p.ward);
        ps->setString(9, p.status);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("addPatient: ") + e.what());
        return false;
    }
}

bool Database::updatePatient(const DBPatient& p) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "UPDATE patients SET name=?,age=?,gender=?,disease=?,phone=?,address=?,"
            "admit_date=?,ward=?,status=? WHERE id=?"
        ));
        ps->setString(1, p.name);
        ps->setInt   (2, p.age);
        ps->setString(3, p.gender);
        ps->setString(4, p.disease);
        ps->setString(5, p.phone);
        ps->setString(6, p.address);
        ps->setString(7, p.admitDate);
        ps->setString(8, p.ward);
        ps->setString(9, p.status);
        ps->setInt   (10, p.id);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("updatePatient: ") + e.what());
        return false;
    }
}

bool Database::deletePatient(int id) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "DELETE FROM patients WHERE id=?"
        ));
        ps->setInt(1, id);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("deletePatient: ") + e.what());
        return false;
    }
}

std::vector<DBPatient> Database::getAllPatients() {
    std::vector<DBPatient> result;
    if (!isConnected()) return result;
    try {
        std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(
            "SELECT * FROM patients ORDER BY id"
        ));
        while (rs->next()) {
            DBPatient p;
            p.id        = rs->getInt("id");
            p.name      = rs->getString("name");
            p.age       = rs->getInt("age");
            p.gender    = rs->getString("gender");
            p.disease   = rs->getString("disease");
            p.phone     = rs->getString("phone");
            p.address   = rs->getString("address");
            p.admitDate = rs->getString("admit_date");
            p.ward      = rs->getString("ward");
            p.status    = rs->getString("status");
            result.push_back(p);
        }
    }
    catch (sql::SQLException& e) {
        setError(std::string("getAllPatients: ") + e.what());
    }
    return result;
}

DBPatient Database::getPatientById(int id) {
    DBPatient p;
    if (!isConnected()) return p;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "SELECT * FROM patients WHERE id=?"
        ));
        ps->setInt(1, id);
        std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
        if (rs->next()) {
            p.id        = rs->getInt("id");
            p.name      = rs->getString("name");
            p.age       = rs->getInt("age");
            p.gender    = rs->getString("gender");
            p.disease   = rs->getString("disease");
            p.phone     = rs->getString("phone");
            p.address   = rs->getString("address");
            p.admitDate = rs->getString("admit_date");
            p.ward      = rs->getString("ward");
            p.status    = rs->getString("status");
        }
    }
    catch (sql::SQLException& e) {
        setError(std::string("getPatientById: ") + e.what());
    }
    return p;
}

// ══════════════════════════════════════════════════════════
//  DOCTORS
// ══════════════════════════════════════════════════════════

bool Database::addDoctor(const DBDoctor& d) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "INSERT INTO doctors (name,specialization,phone,email,schedule,department,"
            "experience,fee,status) VALUES (?,?,?,?,?,?,?,?,?)"
        ));
        ps->setString(1, d.name);
        ps->setString(2, d.specialization);
        ps->setString(3, d.phone);
        ps->setString(4, d.email);
        ps->setString(5, d.schedule);
        ps->setString(6, d.department);
        ps->setInt   (7, d.experience);
        ps->setDouble(8, d.fee);
        ps->setString(9, d.status);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("addDoctor: ") + e.what());
        return false;
    }
}

bool Database::updateDoctor(const DBDoctor& d) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "UPDATE doctors SET name=?,specialization=?,phone=?,email=?,schedule=?,"
            "department=?,experience=?,fee=?,status=? WHERE id=?"
        ));
        ps->setString(1, d.name);
        ps->setString(2, d.specialization);
        ps->setString(3, d.phone);
        ps->setString(4, d.email);
        ps->setString(5, d.schedule);
        ps->setString(6, d.department);
        ps->setInt   (7, d.experience);
        ps->setDouble(8, d.fee);
        ps->setString(9, d.status);
        ps->setInt   (10, d.id);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("updateDoctor: ") + e.what());
        return false;
    }
}

bool Database::deleteDoctor(int id) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "DELETE FROM doctors WHERE id=?"
        ));
        ps->setInt(1, id);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("deleteDoctor: ") + e.what());
        return false;
    }
}

std::vector<DBDoctor> Database::getAllDoctors() {
    std::vector<DBDoctor> result;
    if (!isConnected()) return result;
    try {
        std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(
            "SELECT * FROM doctors ORDER BY id"
        ));
        while (rs->next()) {
            DBDoctor d;
            d.id             = rs->getInt("id");
            d.name           = rs->getString("name");
            d.specialization = rs->getString("specialization");
            d.phone          = rs->getString("phone");
            d.email          = rs->getString("email");
            d.schedule       = rs->getString("schedule");
            d.department     = rs->getString("department");
            d.experience     = rs->getInt("experience");
            d.fee            = rs->getDouble("fee");
            d.status         = rs->getString("status");
            result.push_back(d);
        }
    }
    catch (sql::SQLException& e) {
        setError(std::string("getAllDoctors: ") + e.what());
    }
    return result;
}

DBDoctor Database::getDoctorById(int id) {
    DBDoctor d;
    if (!isConnected()) return d;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "SELECT * FROM doctors WHERE id=?"
        ));
        ps->setInt(1, id);
        std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
        if (rs->next()) {
            d.id             = rs->getInt("id");
            d.name           = rs->getString("name");
            d.specialization = rs->getString("specialization");
            d.phone          = rs->getString("phone");
            d.email          = rs->getString("email");
            d.schedule       = rs->getString("schedule");
            d.department     = rs->getString("department");
            d.experience     = rs->getInt("experience");
            d.fee            = rs->getDouble("fee");
            d.status         = rs->getString("status");
        }
    }
    catch (sql::SQLException& e) {
        setError(std::string("getDoctorById: ") + e.what());
    }
    return d;
}

// ══════════════════════════════════════════════════════════
//  APPOINTMENTS
// ══════════════════════════════════════════════════════════

bool Database::addAppointment(const DBAppointment& a) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "INSERT INTO appointments (patient_name,doctor_name,date,time,department,"
            "reason,status,notes,contact_number,appointment_type) VALUES (?,?,?,?,?,?,?,?,?,?)"
        ));
        ps->setString(1,  a.patientName);
        ps->setString(2,  a.doctorName);
        ps->setString(3,  a.date);
        ps->setString(4,  a.time);
        ps->setString(5,  a.department);
        ps->setString(6,  a.reason);
        ps->setString(7,  a.status);
        ps->setString(8,  a.notes);
        ps->setString(9,  a.contactNumber);
        ps->setString(10, a.appointmentType);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("addAppointment: ") + e.what());
        return false;
    }
}

bool Database::updateAppointment(const DBAppointment& a) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "UPDATE appointments SET patient_name=?,doctor_name=?,date=?,time=?,"
            "department=?,reason=?,status=?,notes=?,contact_number=?,appointment_type=? WHERE id=?"
        ));
        ps->setString(1,  a.patientName);
        ps->setString(2,  a.doctorName);
        ps->setString(3,  a.date);
        ps->setString(4,  a.time);
        ps->setString(5,  a.department);
        ps->setString(6,  a.reason);
        ps->setString(7,  a.status);
        ps->setString(8,  a.notes);
        ps->setString(9,  a.contactNumber);
        ps->setString(10, a.appointmentType);
        ps->setInt   (11, a.id);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("updateAppointment: ") + e.what());
        return false;
    }
}

bool Database::deleteAppointment(int id) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "DELETE FROM appointments WHERE id=?"
        ));
        ps->setInt(1, id);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("deleteAppointment: ") + e.what());
        return false;
    }
}

std::vector<DBAppointment> Database::getAllAppointments() {
    std::vector<DBAppointment> result;
    if (!isConnected()) return result;
    try {
        std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(
            "SELECT * FROM appointments ORDER BY id"
        ));
        while (rs->next()) {
            DBAppointment a;
            a.id              = rs->getInt("id");
            a.patientName     = rs->getString("patient_name");
            a.doctorName      = rs->getString("doctor_name");
            a.date            = rs->getString("date");
            a.time            = rs->getString("time");
            a.department      = rs->getString("department");
            a.reason          = rs->getString("reason");
            a.status          = rs->getString("status");
            a.notes           = rs->getString("notes");
            a.contactNumber   = rs->getString("contact_number");
            a.appointmentType = rs->getString("appointment_type");
            result.push_back(a);
        }
    }
    catch (sql::SQLException& e) {
        setError(std::string("getAllAppointments: ") + e.what());
    }
    return result;
}

// ══════════════════════════════════════════════════════════
//  BILLS
// ══════════════════════════════════════════════════════════

bool Database::addBill(const DBBill& b) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "INSERT INTO bills (patient_name,patient_id,room_charge,medicine_charge,"
            "doctor_fee,test_charges,other_charges,total_amount,payment_status,bill_date) "
            "VALUES (?,?,?,?,?,?,?,?,?,?)"
        ));
        ps->setString(1,  b.patientName);
        ps->setString(2,  b.patientId);
        ps->setDouble(3,  b.roomCharge);
        ps->setDouble(4,  b.medicineCharge);
        ps->setDouble(5,  b.doctorFee);
        ps->setDouble(6,  b.testCharges);
        ps->setDouble(7,  b.otherCharges);
        ps->setDouble(8,  b.totalAmount);
        ps->setString(9,  b.paymentStatus);
        ps->setString(10, b.billDate);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("addBill: ") + e.what());
        return false;
    }
}

bool Database::updateBill(const DBBill& b) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "UPDATE bills SET patient_name=?,patient_id=?,room_charge=?,medicine_charge=?,"
            "doctor_fee=?,test_charges=?,other_charges=?,total_amount=?,payment_status=?,"
            "bill_date=? WHERE id=?"
        ));
        ps->setString(1,  b.patientName);
        ps->setString(2,  b.patientId);
        ps->setDouble(3,  b.roomCharge);
        ps->setDouble(4,  b.medicineCharge);
        ps->setDouble(5,  b.doctorFee);
        ps->setDouble(6,  b.testCharges);
        ps->setDouble(7,  b.otherCharges);
        ps->setDouble(8,  b.totalAmount);
        ps->setString(9,  b.paymentStatus);
        ps->setString(10, b.billDate);
        ps->setInt   (11, b.id);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("updateBill: ") + e.what());
        return false;
    }
}

bool Database::deleteBill(int id) {
    if (!isConnected()) return false;
    try {
        std::unique_ptr<sql::PreparedStatement> ps(m_conn->prepareStatement(
            "DELETE FROM bills WHERE id=?"
        ));
        ps->setInt(1, id);
        ps->execute();
        return true;
    }
    catch (sql::SQLException& e) {
        setError(std::string("deleteBill: ") + e.what());
        return false;
    }
}

std::vector<DBBill> Database::getAllBills() {
    std::vector<DBBill> result;
    if (!isConnected()) return result;
    try {
        std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(
            "SELECT * FROM bills ORDER BY id"
        ));
        while (rs->next()) {
            DBBill b;
            b.id             = rs->getInt("id");
            b.patientName    = rs->getString("patient_name");
            b.patientId      = rs->getString("patient_id");
            b.roomCharge     = rs->getDouble("room_charge");
            b.medicineCharge = rs->getDouble("medicine_charge");
            b.doctorFee      = rs->getDouble("doctor_fee");
            b.testCharges    = rs->getDouble("test_charges");
            b.otherCharges   = rs->getDouble("other_charges");
            b.totalAmount    = rs->getDouble("total_amount");
            b.paymentStatus  = rs->getString("payment_status");
            b.billDate       = rs->getString("bill_date");
            result.push_back(b);
        }
    }
    catch (sql::SQLException& e) {
        setError(std::string("getAllBills: ") + e.what());
    }
    return result;
}

#else // USE_MYSQL_DB

bool Database::connect() {
    setError("Database compiled without MySQL support.");
    return false;
}

void Database::disconnect() {}

bool Database::isConnected() const {
    return false;
}

bool Database::createTablesIfNotExist() {
    return false;
}

bool Database::addPatient(const DBPatient& p) {
    return false;
}

bool Database::updatePatient(const DBPatient& p) {
    return false;
}

bool Database::deletePatient(int id) {
    return false;
}

std::vector<DBPatient> Database::getAllPatients() {
    return {};
}

DBPatient Database::getPatientById(int id) {
    return {};
}

bool Database::addDoctor(const DBDoctor& d) {
    return false;
}

bool Database::updateDoctor(const DBDoctor& d) {
    return false;
}

bool Database::deleteDoctor(int id) {
    return false;
}

std::vector<DBDoctor> Database::getAllDoctors() {
    return {};
}

DBDoctor Database::getDoctorById(int id) {
    return {};
}

bool Database::addAppointment(const DBAppointment& a) {
    return false;
}

bool Database::updateAppointment(const DBAppointment& a) {
    return false;
}

bool Database::deleteAppointment(int id) {
    return false;
}

std::vector<DBAppointment> Database::getAllAppointments() {
    return {};
}

bool Database::addBill(const DBBill& b) {
    return false;
}

bool Database::updateBill(const DBBill& b) {
    return false;
}

bool Database::deleteBill(int id) {
    return false;
}

std::vector<DBBill> Database::getAllBills() {
    return {};
}

#endif // USE_MYSQL_DB

