<div align="center">

<!-- BANNER -->
<img src="https://capsule-render.vercel.app/api?type=waving&color=0:1565C0,100:00695C&height=200&section=header&text=Hospital%20Management%20System&fontSize=42&fontColor=ffffff&fontAlignY=38&desc=GUI-Based%20Desktop%20Application%20%7C%20C%2B%2B%20%7C%20wxWidgets%20%7C%20MySQL&descAlignY=58&descSize=16" width="100%"/>

<br/>

<!-- BADGES -->
![Language](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![GUI](https://img.shields.io/badge/GUI-wxWidgets%203.3.2-1565C0?style=for-the-badge&logo=windows&logoColor=white)
![Database](https://img.shields.io/badge/Database-MySQL%208.x-4479A1?style=for-the-badge&logo=mysql&logoColor=white)
![Compiler](https://img.shields.io/badge/Compiler-MinGW--w64-00695C?style=for-the-badge&logo=gnu&logoColor=white)
![IDE](https://img.shields.io/badge/IDE-VS%20Code-007ACC?style=for-the-badge&logo=visualstudiocode&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Status](https://img.shields.io/badge/Status-Active-2E7D32?style=for-the-badge)

<br/>

> **A fully featured, GUI-based Hospital Management System** built in C++17 with the wxWidgets framework and MySQL database backend — managing Patients, Doctors, Appointments, and Billing from a single professional desktop application.

<br/>

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Tech Stack](#-tech-stack)
- [Project Structure](#-project-structure)
- [OOP Concepts Applied](#-oop-concepts-applied)
- [Database Design](#-database-design)
- [System Architecture](#-system-architecture)
- [Prerequisites](#-prerequisites)
- [Installation & Setup](#-installation--setup)
- [Build Instructions](#-build-instructions)
- [Running the Application](#-running-the-application)
- [Screenshots](#-screenshots)
- [Known Issues Fixed](#-known-issues-fixed)
- [Future Enhancements](#-future-enhancements)
- [License](#-license)

---

## 🏥 Overview

The **Hospital Management System** is a desktop application designed to streamline core hospital operations. It replaces error-prone paper-based and spreadsheet systems with a clean, tabbed GUI where hospital staff can manage all records in real-time — with data persisted permanently in a MySQL database.

The project was built entirely in **C++17** using the **wxWidgets** cross-platform GUI framework, compiled with **MinGW-w64 (MSYS2)**, and connected to **MySQL via MySQL Connector/C++** running through **XAMPP**.

---

## ✨ Features

| Module | Functionality |
|--------|--------------|
| 🏠 **Dashboard** | Live statistics — total patients, doctors, appointments, billing summary |
| 👤 **Patients** | Register, edit, delete, search, and discharge patients |
| 👨‍⚕️ **Doctors** | Add doctor profiles, manage availability, filter by department |
| 📅 **Appointments** | Schedule, update status, cancel — linked to doctors |
| 💰 **Billing** | Generate itemised bills, mark paid/unpaid, view totals |
| 📊 **Reports** | Summary report across all modules with statistics |

### Core Capabilities
- ✅ Full **CRUD operations** on all four entities
- ✅ **Persistent MySQL storage** — data survives application restarts
- ✅ **Graceful degradation** — app works even if MySQL is unreachable (shows warning)
- ✅ **SQL injection prevention** via prepared statements
- ✅ **Auto table creation** on first run — no manual SQL setup needed
- ✅ **Tabbed AUI notebook** interface with all modules in one window

---

## 🛠 Tech Stack

```
┌─────────────────────────────────────────────────────────┐
│                    TECHNOLOGY STACK                      │
├─────────────────┬───────────────────────────────────────┤
│  Language        │  C++17                                │
│  GUI Framework   │  wxWidgets 3.3.2                      │
│  Compiler        │  MinGW-w64  (GCC 13.x via MSYS2)     │
│  IDE             │  Visual Studio Code                   │
│  Database        │  MySQL 8.x  (via XAMPP)               │
│  DB Connector    │  MySQL Connector/C++ 8.x              │
│  Local Server    │  XAMPP  (Apache + MySQL)              │
│  DB GUI          │  phpMyAdmin                           │
│  STL             │  C++17 Standard Library               │
│  Build System    │  VS Code tasks.json  (g++ command)    │
│  Platform        │  Windows (x64)                        │
└─────────────────┴───────────────────────────────────────┘
```

### Dependency Paths (your system)

```
C:\wxWidgets-3.3.2          ← wxWidgets installation
C:\msys64                   ← MSYS2 / MinGW-w64
C:\mysql-connector-c++      ← MySQL Connector/C++
C:\xampp                    ← XAMPP (MySQL server)
```

---

## 📁 Project Structure

```
HospitalManagementSystem/
│
├── 📄 App.cpp                  # Entry point — MyApp class, DB connect on startup
├── 📄 App.h                    # MyApp class declaration
│
├── 📄 MainFrame.cpp            # All GUI panels, event handlers, business logic
├── 📄 MainFrame.h              # MainFrame class declaration + enums + member vars
│
├── 📄 Database.cpp             # MySQL CRUD implementation — all 4 tables
├── 📄 Database.h               # Database singleton class + DBPatient/Doctor/... structs
│
├── 📄 gui.cpp                  # Reserved GUI helpers
│
├── 📄 appointments.txt         # Legacy flat-file storage (pre-database)
├── 📄 bills.txt                # Legacy flat-file storage (pre-database)
├── 📄 doctors.txt              # Legacy flat-file storage (pre-database)
│
└── 📁 .vscode/
    ├── c_cpp_properties.json   # IntelliSense — wx + MySQL include paths
    ├── tasks.json              # g++ build command with all flags
    └── launch.json             # F5 run/debug + DLL PATH injection
```

---

## 🧠 OOP Concepts Applied

### 1. Classes & Encapsulation
All data is private. Access is only through public methods:

```cpp
class Database {
private:
    sql::mysql::MySQL_Driver* m_driver = nullptr;
    sql::Connection*          m_conn   = nullptr;
    std::string               m_lastError;

public:
    bool connect();
    bool addPatient(const DBPatient& p);
    std::vector<DBPatient> getAllPatients();
};
```

### 2. Inheritance
Both application classes inherit from wxWidgets base classes:

```cpp
// MyApp — overrides wxApp entry point
class MyApp : public wxApp {
public:
    virtual bool OnInit() override;
};

// MainFrame — entire GUI lives in this wxFrame subclass
class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    // ...
};
```

### 3. Singleton Design Pattern
Only one database connection exists for the entire app session:

```cpp
class Database {
public:
    static Database& instance() {
        static Database db;   // created once, lives until app closes
        return db;
    }
private:
    Database() = default;
    Database(const Database&) = delete;            // no copy
    Database& operator=(const Database&) = delete; // no assign
};

// Usage anywhere:
Database::instance().addPatient(p);
```

### 4. STL Containers
All in-memory collections use `std::vector`:

```cpp
std::vector<Patient>     m_patients;
std::vector<Doctor>      m_doctors;
std::vector<Appointment> m_appointments;
std::vector<Bill>        m_bills;

// Range-based for loop — modern C++17
for (const DBPatient& p : Database::instance().getAllPatients()) {
    m_listPatients->InsertItem(...);
}
```

### 5. Structs as Data Transfer Objects
Clean separation between DB layer and GUI layer:

```cpp
struct DBPatient {
    int         id        = 0;
    std::string name, gender, disease;
    int         age       = 0;
    std::string phone, address, admitDate, ward, status;
};
// DBDoctor / DBAppointment / DBBill follow the same pattern
```

### 6. Exception Handling
All DB operations are wrapped in try-catch — app never crashes on DB error:

```cpp
try {
    m_conn = m_driver->connect(HOST, USER, PASSWORD);
    m_conn->setSchema(SCHEMA);
    return true;
}
catch (sql::SQLException& e) {
    setError(std::string("Connection failed: ") + e.what());
    return false;
}
```

### 7. Prepared Statements (Security)
Every write uses `?` placeholders — prevents SQL injection:

```cpp
auto ps = m_conn->prepareStatement(
    "INSERT INTO patients (name, age, gender, disease) VALUES (?, ?, ?, ?)"
);
ps->setString(1, p.name);
ps->setInt   (2, p.age);
ps->setString(3, p.gender);
ps->setString(4, p.disease);
ps->execute();
```

---

## 🗄 Database Design

### Database: `hospital_db`

```
patients                        doctors
┌────────────────────┐         ┌──────────────────────┐
│ id          INT PK │         │ id           INT PK  │
│ name        VARCHAR│         │ name         VARCHAR  │
│ age         INT    │         │ specialization VARCHAR│
│ gender      VARCHAR│         │ department   VARCHAR  │
│ disease     VARCHAR│         │ phone        VARCHAR  │
│ phone       VARCHAR│         │ email        VARCHAR  │
│ address     VARCHAR│         │ schedule     VARCHAR  │
│ admit_date  VARCHAR│         │ experience   INT      │
│ ward        VARCHAR│         │ fee          DECIMAL  │
│ status      VARCHAR│         │ status       VARCHAR  │
└────────────────────┘         └──────────────────────┘

appointments                    bills
┌────────────────────┐         ┌──────────────────────┐
│ id          INT PK │         │ id           INT PK  │
│ patient_name VARCHAR│        │ patient_name VARCHAR  │
│ doctor_name VARCHAR│         │ patient_id   VARCHAR  │
│ date        VARCHAR│         │ room_charge  DECIMAL  │
│ time        VARCHAR│         │ medicine_chg DECIMAL  │
│ department  VARCHAR│         │ doctor_fee   DECIMAL  │
│ reason      VARCHAR│         │ test_charges DECIMAL  │
│ status      VARCHAR│         │ other_charges DECIMAL │
│ notes       TEXT   │         │ total_amount DECIMAL  │
│ contact_num VARCHAR│         │ payment_status VARCHAR│
│ appt_type   VARCHAR│         │ bill_date    VARCHAR  │
└────────────────────┘         └──────────────────────┘
```

> **Tables are created automatically on first run** — no manual SQL setup needed.

---

## 🏗 System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                   PRESENTATION LAYER                     │
│  MainFrame.cpp  ──  wxAuiNotebook tabbed interface       │
│  Panels: Dashboard │ Patients │ Doctors │ Appointments   │
│           Bills │ Reports                                 │
└──────────────────────────┬──────────────────────────────┘
                           │ calls
┌──────────────────────────▼──────────────────────────────┐
│                   BUSINESS LOGIC LAYER                   │
│  Structs, Vectors, Validation, Event Handlers            │
│  OnAddPatient() │ OnDeleteDoctor() │ OnGenerateBill()    │
└──────────────────────────┬──────────────────────────────┘
                           │ calls
┌──────────────────────────▼──────────────────────────────┐
│                  DATA PERSISTENCE LAYER                  │
│  Database.h / Database.cpp  ──  Singleton class          │
│  MySQL Connector/C++  ──►  hospital_db  (XAMPP MySQL)   │
└─────────────────────────────────────────────────────────┘
```

---

## ⚙ Prerequisites

Before building, make sure you have all of these installed:

| Tool | Version | Where |
|------|---------|--------|
| **MSYS2 + MinGW-w64** | GCC 13.x | [msys2.org](https://www.msys2.org/) → installed at `C:\msys64` |
| **wxWidgets** | 3.3.2 | [wxwidgets.org](https://wxwidgets.org/) → installed at `C:\wxWidgets-3.3.2` |
| **XAMPP** | Latest | [apachefriends.org](https://www.apachefriends.org/) → MySQL on port 3306 |
| **MySQL Connector/C++** | 8.x | [dev.mysql.com/downloads/connector/cpp](https://dev.mysql.com/downloads/connector/cpp/) → at `C:\mysql-connector-c++` |
| **VS Code** | Latest | [code.visualstudio.com](https://code.visualstudio.com/) + C/C++ Extension |

---

## 🚀 Installation & Setup

### Step 1 — Verify MinGW is on PATH

Open Command Prompt and run:

```bash
g++ --version
```

Expected output: `g++ (Rev1, Built by MSYS2 project) 13.x.x`

> If not recognized → add `C:\msys64\mingw64\bin` to Windows PATH (System Properties → Environment Variables)

---

### Step 2 — Start XAMPP

1. Open **XAMPP Control Panel**
2. Click **Start** next to **Apache**
3. Click **Start** next to **MySQL**
4. Open `http://localhost/phpmyadmin`
5. Create a new database named exactly: **`hospital_db`**

> ℹ️ Tables are created automatically on first application launch — no SQL needed.

---

### Step 3 — Set Up MySQL Connector/C++

1. Download from: `https://dev.mysql.com/downloads/connector/cpp/`
2. Extract to: `C:\mysql-connector-c++`
3. Copy the DLL to MinGW bin:

```bash
copy C:\mysql-connector-c++\lib64\vs14\mysqlcppconn.dll C:\msys64\mingw64\bin\
copy C:\mysql-connector-c++\lib64\vs14\mysqlcppconn.dll <your-project-folder>\
```

---

### Step 4 — Open Project in VS Code

```bash
File → Open Folder → select your HospitalManagementSystem folder
```

All three `.vscode` config files are already included in the repo.

---

## 🔨 Build Instructions

Press **`Ctrl + Shift + B`** in VS Code.

The full build command executed is:

```bash
g++ -std=c++17 -mwindows \
    -DUNICODE -D_UNICODE -DWXUSINGDLL -D__WXMSW__ \
    -IC:/wxWidgets-3.3.2/include \
    -IC:/wxWidgets-3.3.2/lib/gcc_dll/mswu \
    -IC:/mysql-connector-c++/include \
    -IC:/mysql-connector-c++/include/jdbc \
    App.cpp MainFrame.cpp Database.cpp \
    -o HospitalSystem.exe \
    -LC:/wxWidgets-3.3.2/lib/gcc_dll \
    -lwxmsw33u_core -lwxbase33u -lwxmsw33u_adv -lwxmsw33u_aui \
    -LC:/mysql-connector-c++/lib64/vs14 \
    -lmysqlcppconn \
    -Wl,--subsystem,windows
```

**Successful build** produces `HospitalSystem.exe` in the project folder with no errors.

---

## ▶ Running the Application

**Method 1 — VS Code (recommended):**

```
Press F5
```

VS Code builds the project then launches the .exe automatically.

**Method 2 — Command Prompt:**

```bash
set PATH=C:\wxWidgets-3.3.2\lib\gcc_dll;C:\mysql-connector-c++\lib64\vs14;%PATH%
HospitalSystem.exe
```

> ⚠️ **XAMPP MySQL must be running** before launching the app. If MySQL is not running, the app will show a warning popup and continue without database saving.

---

## 📸 Screenshots

> _Add screenshots of your running application here_

```
[ Dashboard Screenshot ]      [ Patients Module Screenshot ]

[ Doctors Module Screenshot ] [ Billing Module Screenshot  ]
```

---

## 🐛 Known Issues Fixed

All bugs identified during the code audit have been resolved:

| # | File | Issue | Fix Applied |
|---|------|-------|-------------|
| 1 | `App.h` | File was completely empty — `MyApp` never declared | Added full class declaration |
| 2 | `MainFrame.h:221` | `BuildContentHost()` declared but never defined | Deleted dead declaration |
| 3 | `finalproject.cpp` | Dual `main()` clash with `App.cpp` | Separated into two build targets |
| 4 | `gui.cpp` | Empty file in build sources | Removed from build / flagged |
| 5 | `MainFrame.cpp:1244` | `%zu` crashes on MinGW/MSVC | Replaced with `%u` + `(unsigned)` cast |
| 6 | `MainFrame.cpp:760` | `SetClientData` heap corruption risk | Replaced with `m_reportTextCtrl` member variable |

---

## 🔮 Future Enhancements

- [ ] **User Authentication** — login screen with role-based access (admin, receptionist, doctor)
- [ ] **PDF Report Export** — generate downloadable bills and patient records as PDF
- [ ] **Real-time Search** — instant search bar across all list views
- [ ] **Appointment Reminders** — `wxTimer`-based notifications for upcoming appointments
- [ ] **Network MySQL** — migrate from localhost to remote MySQL for multi-workstation use
- [ ] **CSV / Excel Export** — export billing and patient data to spreadsheets
- [ ] **Prescription Module** — log medicines prescribed per patient
- [ ] **Audit Log** — track who added/edited/deleted records with timestamps

---

## 👨‍💻 Author

**[ Your Name Here ]**

> Built as part of OOP in C++ coursework using Robert Lafore's *Object-Oriented Programming in C++* (4th Edition) as reference.

---

## 📚 References

- [wxWidgets Documentation 3.3.2](https://docs.wxwidgets.org/3.3.2/)
- [MySQL Connector/C++ Developer Guide](https://dev.mysql.com/doc/connector-cpp/8.0/en/)
- [MSYS2 Documentation](https://www.msys2.org/)
- [cppreference.com — C++17](https://en.cppreference.com/)
- Lafore, Robert. *Object-Oriented Programming in C++*, 4th Edition. Sams Publishing.

---

<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:00695C,100:1565C0&height=100&section=footer" width="100%"/>

**Hospital Management System** — Built with ❤️ in C++

![C++](https://img.shields.io/badge/C++-17-00599C?style=flat-square&logo=cplusplus)
![wxWidgets](https://img.shields.io/badge/wxWidgets-3.3.2-1565C0?style=flat-square)
![MySQL](https://img.shields.io/badge/MySQL-8.x-4479A1?style=flat-square&logo=mysql)
![MinGW](https://img.shields.io/badge/MinGW--w64-GCC%2013-00695C?style=flat-square&logo=gnu)

</div>
