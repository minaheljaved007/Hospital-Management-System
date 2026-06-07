# Hospital Management System

<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=0:1565C0,100:00695C&height=200&section=header&text=Hospital%20Management%20System&fontSize=42&fontColor=ffffff&desc=GUI%20Desktop%20App%20%7C%20C%2B%2B%20%7C%20wxWidgets%20%7C%20MySQL&descSize=16" width="100%"/>
</div>

---

## 📋 Table of Contents
- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Installation & Setup](#installation--setup)
- [Build Instructions](#build-instructions)
- [Running the Application](#running-the-application)
- [Optional MySQL Support](#optional-mysql-support)
- [Known Issues Fixed](#known-issues-fixed)
- [Future Enhancements](#future-enhancements)
- [License](#license)

---

## Overview
The **Hospital Management System** is a desktop application that manages patients, doctors, appointments, and billing. It is written in **C++17**, uses **wxWidgets 3.3.2** for the GUI, and can store data either in plain‑text files (default) or in a **MySQL 8.x** database (optional).

---

## Prerequisites
To build and run this application on Windows, you need the following components installed:

1. **MSYS2 (64-bit)**
   - Download and install from: [msys2.org](https://www.msys2.org/)
   - Default installation directory: `C:\msys64`

2. **MSYS2 Development Packages (Clang, LLD, wxWidgets 3.2)**
   - Open the **MSYS2 MINGW64** terminal (ensure you use the blue **MINGW64** environment shortcut, not the MSYS or UCRT64 ones).
   - Install the required toolchain and wxWidgets libraries by running:
     ```bash
     pacman -S mingw-w64-x86_64-clang mingw-w64-x86_64-lld mingw-w64-x86_64-wxwidgets3.2
     ```

3. **MySQL Connector/C++ (8.x or 9.x)**
   - Download the **Windows (x86, 64-bit), ZIP Archive** from the official site: [MySQL Downloads](https://dev.mysql.com/downloads/connector/cpp/)
   - Extract the ZIP archive and rename the folder to `mysql-connector-c++`, then place it at `C:\mysql-connector-c++` so that paths like `C:\mysql-connector-c++\include` and `C:\mysql-connector-c++\lib64` exist.
   - For runtime execution, copy the following DLLs from `C:\mysql-connector-c++\lib64\` to your project root (beside `HospitalSystem.exe`):
     - `mysqlcppconn-10-vs14.dll`
     - `libcrypto-3-x64.dll`
     - `libssl-3-x64.dll`
     *(Note: These DLLs have already been copied to this repository's root for convenience!)*

4. **XAMPP (MySQL Server)**
   - Download and install from: [apachefriends.org](https://www.apachefriends.org/)
   - Start the **Apache** and **MySQL** services from the XAMPP Control Panel.

5. **Visual Studio Code**
   - Install [VS Code](https://code.visualstudio.com/) along with the Microsoft **C/C++** extension.

---

## Installation & Setup
1. **Add MSYS2 MinGW64 to the System PATH**:
   - Add `C:\msys64\mingw64\bin` to your system environment `PATH` variable.
   - Alternatively, you can temporarily append it in your active PowerShell session before compiling/running:
     ```powershell
     $env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH
     ```

2. **Setup XAMPP MySQL Database**:
   - With MySQL running in XAMPP, open your web browser at: `http://localhost/phpmyadmin/`
   - Create a new database named exactly **`hospital_db`**.
   - *Note: Do not create or import any tables.* The application automatically creates all necessary tables (`patients`, `doctors`, `appointments`, `bills`) on its first launch.

3. **Open the Project**:
   - Open VS Code and open the repository folder: `C:\Users\Tayyab\Documents\Hospital-Management-System`.

---

## Build Instructions
### Option 1: Build via VS Code (Recommended)
1. Open the project folder in VS Code.
2. Press `Ctrl+Shift+B` (or go to *Terminal → Run Build Task...* and select `Build Hospital Managment System (wxWidgets)`).
3. The build command compiles `App.cpp`, `MainFrame.cpp`, and `Database.cpp` using Clang++ and links wxWidgets and MySQL Connector.

### Option 2: Build via Terminal
Open a PowerShell terminal in the project root folder and execute:
```powershell
C:/msys64/mingw64/bin/clang++.exe App.cpp MainFrame.cpp Database.cpp -o HospitalSystem.exe -IC:/msys64/mingw64/lib/wx/include/msw-unicode-3.2 -IC:/msys64/mingw64/include/wx-3.2 -IC:/mysql-connector-c++/include -IC:/mysql-connector-c++/include/jdbc -LC:/msys64/mingw64/lib "-lwx_mswu_aui-3.2" "-lwx_mswu_propgrid-3.2" "-lwx_mswu_xrc-3.2" "-lwx_mswu_html-3.2" "-lwx_mswu_qa-3.2" "-lwx_mswu_core-3.2" "-lwx_baseu_xml-3.2" "-lwx_baseu_net-3.2" "-lwx_baseu-3.2" -LC:/mysql-connector-c++/lib64/vs14 -lmysqlcppconn -std=c++17 -Wall -mwindows -DUNICODE -D_UNICODE -DWXUSINGDLL -D__WXMSW__ -fuse-ld=lld
```

---

## Running the Application
Make sure the MySQL Connector DLLs (`mysqlcppconn-10-vs14.dll`, `libcrypto-3-x64.dll`, `libssl-3-x64.dll`) are in the project root folder.

### Option 1: Run via VS Code
- Press **F5** (or select *Run → Start Debugging*).

### Option 2: Run via Terminal (PowerShell)
In the project root, run:
```powershell
.\HospitalSystem.exe
```

### Option 3: Double-click Executable
- Locate `HospitalSystem.exe` in Windows File Explorer and double-click it.

### Default Login Credentials
- **Username:** `admin`
- **Password:** `hospital123`

If MySQL (via XAMPP) is not running, the application will display a warning connection popup and fallback to local plain-text file storage (`patients.txt`, `doctors.txt`, etc.).


---

## Optional MySQL Support
To compile with real MySQL storage:
1. Verify the MySQL Connector DLLs are reachable (see step 3 of *Installation & Setup*).
2. Open `.vscode/tasks.json` and add the macro `-DUSE_MYSQL_DB` to the `args` array, e.g.:
   ```json
   "args": [ "-DUSE_MYSQL_DB", "App.cpp", ... ]
   ```
3. Re‑run the build task (`Ctrl+Shift+B`).
4. Launch the program; it will now create the required tables in `hospital_db` and persist data there.

---

## Known Issues Fixed
| # | File(s) | Issue | Fix Applied |
|---|----------|-------|-------------|
| 1 | `App.h` | Empty file – no `MyApp` declaration | Added full class declaration |
| 2 | `MainFrame.h:221` | Undeclared `BuildContentHost()` | Removed dead declaration |
| 3 | `finalproject.cpp` | Two `main()` functions causing duplicate symbols | Split into separate build targets |
| 4 | `gui.cpp` | Empty file still compiled | Removed from build list |
| 5 | `MainFrame.cpp:1244` | `%zu` format crashes on MinGW/MSVC | Replaced with `%u` and cast |
| 6 | `MainFrame.cpp:760` | `SetClientData` heap‑corruption risk | Replaced with member `m_reportTextCtrl` |
| 7 | `Database.h` / `Database.cpp` | MySQL headers caused compile/ABI errors & duplicate comment block | Wrapped includes & members in `#ifdef USE_MYSQL_DB`; added stub implementations; cleaned duplicate comment |
| 8 | `tasks.json` (build args) | PowerShell split `-lwx_mswu_aui-3.2` → `.2` errors | Quoted problematic flags |
| 9 | Runtime execution | Missing DLLs caused silent exit | Added clear PATH instruction before launch |

---

## Future Enhancements
- User authentication with role‑based access (admin, receptionist, doctor) 
- PDF report export for bills and patient summaries
- Real‑time search across all list views
- Appointment reminders via `wxTimer`
- Remote MySQL support for multi‑workstation environments
- CSV/Excel export of records
- Prescription module and audit log

---

## License
This project is provided under the **MIT License**. See `LICENSE` for details.

---

<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=0:00695C,100:1565C0&height=100&section=footer" width="100%"/>
  <p><strong>Hospital Management System — Built with ❤️ in C++</strong></p>
</div>
