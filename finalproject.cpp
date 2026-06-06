#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <limits>
#include <algorithm>
#ifdef _WIN32
#include <conio.h>
#endif
using namespace std;
// GLOBAL CONSTANTS
const string LINE = "============================================================";
const string DASH = "------------------------------------------------------------";
// UTILITY FUNCTIONS
void showHeading(const string &title)
{
    cout << "\n"
         << LINE << endl;
    cout << "   " << title << endl;
    cout << LINE << endl;
}
void pause()
{
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
string getCurrentDate()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string day = (ltm->tm_mday < 10 ? "0" : "") + to_string(ltm->tm_mday);
    string month = ((ltm->tm_mon + 1) < 10 ? "0" : "") + to_string(ltm->tm_mon + 1);
    string year = to_string(1900 + ltm->tm_year);
    return day + "/" + month + "/" + year;
}
// Read a password from console
string readPassword()
{
    string password = "";
#ifdef _WIN32
    char ch;
    while (true)
    {
        ch = (char)_getch();
        if (ch == '\r' || ch == '\n')
        {
            cout << endl;
            break;
        }
        else if (ch == '\b' || ch == 127)
        {
            if (!password.empty())
            {
                password.pop_back();
                cout << "\b \b";
                cout.flush();
            }
        }
        else if ((unsigned char)ch >= 32 && (unsigned char)ch < 127)
        {
            password += ch;
            cout << '*';
            cout.flush();
        }
    }
#else
    system("stty -echo");
    getline(cin, password);
    system("stty echo");
    cout << endl;
#endif
    return password;
}

// Validate that a string is not empty and has no pipe characters
bool isValidString(const string &s)
{
    if (s.empty())
        return false;
    if (s.find('|') != string::npos)
        return false;
    return true;
}
// Validate phone number (digits only, 7-15 chars)
bool isValidPhone(const string &phone)
{
    if (phone.length() < 7 || phone.length() > 15)
        return false;
    for (char c : phone)
        if (!isdigit(c) && c != '+' && c != '-')
            return false;
    return true;
}
// Validate blood group
bool isValidBloodGroup(const string &bg)
{
    string valid[] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    for (const string &v : valid)
        if (bg == v)
            return true;
    return false;
}
// Safe integer input with validation
int getIntInput(const string &prompt, int minVal, int maxVal)
{
    int val;
    while (true)
    {
        cout << prompt;
        if (cin >> val && val >= minVal && val <= maxVal)
        {
            return val;
        }
        cout << "  [ERROR] Invalid input! Please enter a number between "
             << minVal << " and " << maxVal << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}
// Safe double input
double getDoubleInput(const string &prompt)
{
    double val;
    while (true)
    {
        cout << prompt;
        if (cin >> val && val >= 0)
        {
            return val;
        }
        cout << "  [ERROR] Invalid amount! Please enter a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}
// Safe string input (non-empty, no pipe chars)
string getStringInput(const string &prompt)
{
    string val;
    while (true)
    {
        cout << prompt;
        getline(cin, val);
        // Trim leading/trailing spaces
        size_t start = val.find_first_not_of(" \t");
        size_t end = val.find_last_not_of(" \t");
        if (start != string::npos)
            val = val.substr(start, end - start + 1);
        if (isValidString(val))
            return val;
        cout << "  [ERROR] Input cannot be empty or contain '|'. Try again.\n";
    }
}
// Name input: only alphabets and spaces
string getNameInput(const string &prompt)
{
    string val;
    while (true)
    {
        cout << prompt;
        getline(cin, val);
        size_t start = val.find_first_not_of(" \t");
        size_t end = val.find_last_not_of(" \t");
        if (start != string::npos)
            val = val.substr(start, end - start + 1);

        bool valid = !val.empty();
        for (char c : val)
        {
            if (!isalpha(c) && c != ' ')
            {
                valid = false;
                break;
            }
        }
        if (valid)
            return val;
        cout << "  [ERROR] Name must contain letters and spaces only. Try again.\n";
    }
}
// Gender input: only M or F
string getGenderInput()
{
    string val;
    while (true)
    {
        cout << "  Enter Gender (M/F)   : ";
        getline(cin, val);
        if (val == "M" || val == "m")
            return "Male";
        if (val == "F" || val == "f")
            return "Female";
        cout << "  [ERROR] Please enter M or F only.\n";
    }
}
// ============================================================
// BASE CLASS: Person
// ============================================================
class Person
{
protected:
    int id;
    string name;
    int age;
    string gender;
    string phone;
    string address;

public:
    Person()
        : id(0), name(""), age(0), gender(""), phone(""), address("") {}

    Person(int i, const string &n, int a, const string &g,
           const string &p, const string &addr)
        : id(i), name(n), age(a), gender(g), phone(p), address(addr) {}

    // Getters
    int getId() const { return id; }
    string getName() const { return name; }
    int getAge() const { return age; }
    string getGender() const { return gender; }
    string getPhone() const { return phone; }
    string getAddress() const { return address; }
    // Setters
    void setPhone(const string &p) { phone = p; }
    void setAddress(const string &a) { address = a; }

    virtual void display()
    {
        cout << "  ID      : " << id << endl;
        cout << "  Name    : " << name << endl;
        cout << "  Age     : " << age << endl;
        cout << "  Gender  : " << gender << endl;
        cout << "  Phone   : " << phone << endl;
        cout << "  Address : " << address << endl;
    }
    virtual ~Person() {}
};
// DERIVED CLASS: Patient
class Patient : public Person
{
private:
    string disease;
    string bloodGroup;
    string emergencyContact;
    string assignedDoctor;
    string roomNumber;
    string admissionDate;
    static int totalPatients;

public:
    Patient() : Person()
    {
        disease = bloodGroup = emergencyContact = "";
        assignedDoctor = roomNumber = "";
        admissionDate = getCurrentDate();
        totalPatients++;
    }
    Patient(int i, const string &n, int a, const string &g,
            const string &p, const string &addr,
            const string &dis, const string &bg, const string &ec,
            const string &doc, const string &room)
        : Person(i, n, a, g, p, addr)
    {
        disease = dis;
        bloodGroup = bg;
        emergencyContact = ec;
        assignedDoctor = doc;
        roomNumber = room;
        admissionDate = getCurrentDate();
        totalPatients++;
    }
    string getDisease() const { return disease; }
    string getBloodGroup() const { return bloodGroup; }
    string getEmergencyContact() const { return emergencyContact; }
    string getAssignedDoctor() const { return assignedDoctor; }
    string getRoomNumber() const { return roomNumber; }
    string getAdmissionDate() const { return admissionDate; }
    void setDisease(const string &d) { disease = d; }
    void setAssignedDoctor(const string &d) { assignedDoctor = d; }
    void setRoomNumber(const string &r) { roomNumber = r; }
    static int getTotalPatients() { return totalPatients; }
    void display() override
    {
        Person::display();
        cout << "  Disease          : " << disease << endl;
        cout << "  Blood Group      : " << bloodGroup << endl;
        cout << "  Emergency Contact: " << emergencyContact << endl;
        cout << "  Assigned Doctor  : " << assignedDoctor << endl;
        cout << "  Room Number      : " << roomNumber << endl;
        cout << "  Admission Date   : " << admissionDate << endl;
    }
    void saveToFile() const
    {
        ofstream file("patients.txt", ios::app);
        if (file.is_open())
        {
            file << id << "|" << name << "|" << age << "|" << gender << "|"
                 << phone << "|" << address << "|" << disease << "|"
                 << bloodGroup << "|" << emergencyContact << "|"
                 << assignedDoctor << "|" << roomNumber << "|"
                 << admissionDate << "\n";
            file.close();
        }
    }
    ~Patient() {}
};
int Patient::totalPatients = 0;
// DERIVED CLASS: Doctor
class Doctor : public Person
{
private:
    string specialization;
    string qualification;
    int experience;
    string availability;
    string department;

    static int totalDoctors;

public:
    Doctor() : Person()
    {
        specialization = qualification = availability = department = "";
        experience = 0;
        totalDoctors++;
    }

    Doctor(int i, const string &n, int a, const string &g,
           const string &p, const string &addr,
           const string &spec, const string &qual,
           int exp, const string &avail, const string &dept)
        : Person(i, n, a, g, p, addr)
    {
        specialization = spec;
        qualification = qual;
        experience = exp;
        availability = avail;
        department = dept;
        totalDoctors++;
    }
    string getSpecialization() const { return specialization; }
    string getQualification() const { return qualification; }
    int getExperience() const { return experience; }
    string getAvailability() const { return availability; }
    string getDepartment() const { return department; }
    void setAvailability(const string &a) { availability = a; }
    void setDepartment(const string &d) { department = d; }
    static int getTotalDoctors() { return totalDoctors; }
    void display() override
    {
        Person::display();
        cout << "  Specialization : " << specialization << endl;
        cout << "  Qualification  : " << qualification << endl;
        cout << "  Experience     : " << experience << " years" << endl;
        cout << "  Availability   : " << availability << endl;
        cout << "  Department     : " << department << endl;
    }

    void saveToFile() const
    {
        ofstream file("doctors.txt", ios::app);
        if (file.is_open())
        {
            file << id << "|" << name << "|" << age << "|" << gender << "|"
                 << phone << "|" << address << "|" << specialization << "|"
                 << qualification << "|" << experience << "|"
                 << availability << "|" << department << "\n";
            file.close();
        }
    }

    ~Doctor() {}
};
int Doctor::totalDoctors = 0;
// CLASS: Appointment
class Appointment
{
private:
    int appointmentId;
    string patientName;
    string doctorName;
    string date;
    string time;
    string disease;
    string status;

public:
    Appointment()
        : appointmentId(0), patientName(""), doctorName(""),
          date(getCurrentDate()), time(""), disease(""), status("Pending") {}

    Appointment(int aid, const string &pName, const string &dName,
                const string &d, const string &t, const string &dis)
        : appointmentId(aid), patientName(pName), doctorName(dName),
          date(d), time(t), disease(dis), status("Pending") {}

    int getAppointmentId() const { return appointmentId; }
    string getPatientName() const { return patientName; }
    string getDoctorName() const { return doctorName; }
    string getDate() const { return date; }
    string getTime() const { return time; }
    string getDisease() const { return disease; }
    string getStatus() const { return status; }

    void setStatus(const string &s) { status = s; }

    void display()
    {
        cout << "  Appointment ID : " << appointmentId << endl;
        cout << "  Patient Name   : " << patientName << endl;
        cout << "  Doctor Name    : " << doctorName << endl;
        cout << "  Date           : " << date << endl;
        cout << "  Time           : " << time << endl;
        cout << "  Disease        : " << disease << endl;
        cout << "  Status         : " << status << endl;
    }

    void saveToFile() const
    {
        ofstream file("appointments.txt", ios::app);
        if (file.is_open())
        {
            file << appointmentId << "|" << patientName << "|" << doctorName << "|"
                 << date << "|" << time << "|" << disease << "|" << status << "\n";
            file.close();
        }
    }
};
// CLASS: Bill
class Bill
{
private:
    int billId;
    int patientId;
    string patientName;
    double consultationFee;
    double medicineCost;
    double roomCharges;
    double otherCharges;
    double totalAmount;
    string paymentStatus;
    string billDate;

    void calculateTotal()
    {
        totalAmount = consultationFee + medicineCost + roomCharges + otherCharges;
    }

public:
    Bill()
        : billId(0), patientId(0), patientName(""),
          consultationFee(0), medicineCost(0), roomCharges(0),
          otherCharges(0), totalAmount(0), paymentStatus("Unpaid"),
          billDate(getCurrentDate()) {}

    Bill(int bid, int pid, const string &pName,
         double cf, double mc, double rc, double oc)
        : billId(bid), patientId(pid), patientName(pName),
          consultationFee(cf), medicineCost(mc), roomCharges(rc),
          otherCharges(oc), paymentStatus("Unpaid"), billDate(getCurrentDate())
    {
        calculateTotal();
    }

    double getTotalAmount() const { return totalAmount; }
    string getPaymentStatus() const { return paymentStatus; }
    int getBillId() const { return billId; }
    string getPatientName() const { return patientName; }

    void setPaymentStatus(const string &s) { paymentStatus = s; }

    void display()
    {
        cout << "\n"
             << LINE << endl;
        cout << "              HOSPITAL BILL RECEIPT" << endl;
        cout << LINE << endl;
        cout << "  Bill ID         : " << billId << endl;
        cout << "  Patient ID      : " << patientId << endl;
        cout << "  Patient Name    : " << patientName << endl;
        cout << "  Bill Date       : " << billDate << endl;
        cout << DASH << endl;
        cout << fixed << setprecision(2);
        cout << "  Consultation Fee: Rs. " << setw(10) << consultationFee << endl;
        cout << "  Medicine Cost   : Rs. " << setw(10) << medicineCost << endl;
        cout << "  Room Charges    : Rs. " << setw(10) << roomCharges << endl;
        cout << "  Other Charges   : Rs. " << setw(10) << otherCharges << endl;
        cout << DASH << endl;
        cout << "  TOTAL AMOUNT    : Rs. " << setw(10) << totalAmount << endl;
        cout << "  Payment Status  : " << paymentStatus << endl;
        cout << LINE << endl;
    }

    void saveToFile() const
    {
        ofstream file("bills.txt", ios::app);
        if (file.is_open())
        {
            file << billId << "|" << patientId << "|" << patientName << "|"
                 << consultationFee << "|" << medicineCost << "|"
                 << roomCharges << "|" << otherCharges << "|"
                 << totalAmount << "|" << paymentStatus << "|" << billDate << "\n";
            file.close();
        }
    }
};
// CLASS: Medicine
class Medicine
{
private:
    int medicineId;
    string medicineName;
    string category;
    string manufacturer;
    double price;
    int quantity;
    string expiryDate;

public:
    Medicine()
        : medicineId(0), medicineName(""), category(""),
          manufacturer(""), price(0.0), quantity(0), expiryDate("") {}

    Medicine(int mid, const string &mName, const string &cat,
             const string &mfg, double p, int qty, const string &exp)
        : medicineId(mid), medicineName(mName), category(cat),
          manufacturer(mfg), price(p), quantity(qty), expiryDate(exp) {}

    int getMedicineId() const { return medicineId; }
    string getMedicineName() const { return medicineName; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    string getExpiryDate() const { return expiryDate; }

    void setQuantity(int q) { quantity = q; }

    void display()
    {
        cout << "  Medicine ID   : " << medicineId << endl;
        cout << "  Name          : " << medicineName << endl;
        cout << "  Category      : " << category << endl;
        cout << "  Manufacturer  : " << manufacturer << endl;
        cout << fixed << setprecision(2);
        cout << "  Price         : Rs. " << price << endl;
        cout << "  Stock         : " << quantity << " units" << endl;
        cout << "  Expiry Date   : " << expiryDate << endl;
    }

    void saveToFile() const
    {
        ofstream file("medicines.txt", ios::app);
        if (file.is_open())
        {
            file << medicineId << "|" << medicineName << "|" << category << "|"
                 << manufacturer << "|" << price << "|"
                 << quantity << "|" << expiryDate << "\n";
            file.close();
        }
    }
};
// CLASS: Login  (with password masking)
class Login
{
private:
    string adminUsername;
    string adminPassword;

    bool validateCredentials(const string &u, const string &p)
    {
        return (u == adminUsername && p == adminPassword);
    }

public:
    Login()
    {
        adminUsername = "admin";
        adminPassword = "hospital123"; // Default password
    }

    void changePassword()
    {
        cout << "\n  Enter Old Password : ";
        string oldPass = readPassword();
        if (oldPass != adminPassword)
        {
            cout << "  [ERROR] Incorrect old password!\n";
            pause();
            return;
        }
        cout << "  Enter New Password : ";
        string newPass = readPassword();
        if (newPass.length() < 6)
        {
            cout << "  [ERROR] Password must be at least 6 characters!\n";
            pause();
            return;
        }
        cout << "  Confirm New Password: ";
        string confirm = readPassword();
        if (newPass != confirm)
        {
            cout << "  [ERROR] Passwords do not match!\n";
            pause();
            return;
        }
        adminPassword = newPass;
        cout << "  [SUCCESS] Password changed successfully!\n";
        pause();
    }

    bool login()
    {
        clearScreen();
        showHeading("HOSPITAL MANAGEMENT SYSTEM - LOGIN");

        cout << "\n  Default Login Credentials:" << endl;
        cout << "  Username : admin" << endl;
        cout << "  Password : hospital123" << endl;
        cout << DASH << endl;

        int attempts = 3;

        while (attempts > 0)
        {
            string username, password;

            cout << "\n  Enter Username : ";
            cin >> username;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "  Enter Password : ";
            password = readPassword();

            if (validateCredentials(username, password))
            {
                cout << "\n  [SUCCESS] Login Successful! Welcome, " << username << "!\n";
                pause();
                return true;
            }
            else
            {
                attempts--;
                if (attempts > 0)
                    cout << "\n  [ERROR] Invalid credentials! " << attempts << " attempt(s) left.\n";
                else
                    cout << "\n  [LOCKED] Too many failed attempts. System locked.\n";
            }
        }
        return false;
    }

    // Expose changePassword via a menu-callable getter
    void showChangePasswordMenu()
    {
        showHeading("CHANGE PASSWORD");
        changePassword();
    }
};
// CLASS: PatientManager
class PatientManager
{
private:
    bool idExists(int id)
    {
        ifstream file("patients.txt");
        string line;
        while (getline(file, line))
        {
            if (line.empty())
                continue;
            size_t pos = line.find('|');
            if (pos == string::npos)
                continue;
            try
            {
                if (stoi(line.substr(0, pos)) == id)
                {
                    file.close();
                    return true;
                }
            }
            catch (...)
            {
            }
        }
        file.close();
        return false;
    }

public:
    void addPatient()
    {
        showHeading("ADD NEW PATIENT");

        int id = getIntInput("\n  Enter Patient ID   : ", 1, 999999);

        if (idExists(id))
        {
            cout << "\n  [ERROR] Patient ID " << id << " already exists!\n";
            pause();
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string name = getNameInput("  Enter Name         : ");
        int age = getIntInput("  Enter Age          : ", 1, 150);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string gender = getGenderInput();

        string phone;
        while (true)
        {
            phone = getStringInput("  Enter Phone        : ");
            if (isValidPhone(phone))
                break;
            cout << "  [ERROR] Invalid phone number! Use digits, +, or -.\n";
        }

        string address = getStringInput("  Enter Address      : ");
        string disease = getStringInput("  Enter Disease      : ");

        string bloodGroup;
        while (true)
        {
            bloodGroup = getStringInput("  Enter Blood Group (A+/A-/B+/B-/AB+/AB-/O+/O-): ");
            if (isValidBloodGroup(bloodGroup))
                break;
            cout << "  [ERROR] Invalid blood group! Use: A+, A-, B+, B-, AB+, AB-, O+, O-\n";
        }

        string emergencyContact;
        while (true)
        {
            emergencyContact = getStringInput("  Emergency Contact  : ");
            if (isValidPhone(emergencyContact))
                break;
            cout << "  [ERROR] Invalid phone number!\n";
        }

        string doctor = getStringInput("  Assigned Doctor    : ");
        string room = getStringInput("  Room Number        : ");

        Patient p(id, name, age, gender, phone, address,
                  disease, bloodGroup, emergencyContact, doctor, room);
        p.saveToFile();

        cout << "\n  [SUCCESS] Patient record saved successfully!\n";
        pause();
    }

    void viewAllPatients()
    {
        showHeading("ALL PATIENT RECORDS");

        ifstream file("patients.txt");
        if (!file.is_open() || file.peek() == EOF)
        {
            cout << "\n  [INFO] No patient records found.\n";
            file.close();
            pause();
            return;
        }

        string line;
        int count = 0;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[12];
            for (int i = 0; i < 12 && getline(ss, token[i], '|'); i++)
                ;

            cout << "\n"
                 << DASH << endl;
            cout << "  Patient ID        : " << token[0] << endl;
            cout << "  Name              : " << token[1] << endl;
            cout << "  Age               : " << token[2] << endl;
            cout << "  Gender            : " << token[3] << endl;
            cout << "  Phone             : " << token[4] << endl;
            cout << "  Address           : " << token[5] << endl;
            cout << "  Disease           : " << token[6] << endl;
            cout << "  Blood Group       : " << token[7] << endl;
            cout << "  Emergency Contact : " << token[8] << endl;
            cout << "  Assigned Doctor   : " << token[9] << endl;
            cout << "  Room Number       : " << token[10] << endl;
            cout << "  Admission Date    : " << token[11] << endl;
            count++;
        }
        file.close();

        cout << "\n"
             << DASH << endl;
        cout << "  Total Patients: " << count << endl;
        pause();
    }

    // FUNCTION OVERLOADING: Search by ID
    void searchPatient(int searchId)
    {
        showHeading("SEARCH PATIENT BY ID");

        ifstream file("patients.txt");
        if (!file.is_open())
        {
            cout << "\n  [ERROR] Cannot open records file.\n";
            pause();
            return;
        }

        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[12];
            for (int i = 0; i < 12 && getline(ss, token[i], '|'); i++)
                ;

            try
            {
                if (stoi(token[0]) == searchId)
                {
                    cout << "\n"
                         << DASH << endl;
                    cout << "  Patient ID        : " << token[0] << endl;
                    cout << "  Name              : " << token[1] << endl;
                    cout << "  Age               : " << token[2] << endl;
                    cout << "  Gender            : " << token[3] << endl;
                    cout << "  Phone             : " << token[4] << endl;
                    cout << "  Disease           : " << token[6] << endl;
                    cout << "  Blood Group       : " << token[7] << endl;
                    cout << "  Assigned Doctor   : " << token[9] << endl;
                    cout << "  Room              : " << token[10] << endl;
                    cout << "  Admission Date    : " << token[11] << endl;
                    found = true;
                    break;
                }
            }
            catch (...)
            {
            }
        }
        file.close();

        if (!found)
            cout << "\n  [NOT FOUND] No patient with ID " << searchId << " exists.\n";
        pause();
    }

    // FUNCTION OVERLOADING: Search by Name
    void searchPatient(const string &searchName)
    {
        showHeading("SEARCH PATIENT BY NAME");

        ifstream file("patients.txt");
        if (!file.is_open())
        {
            cout << "\n  [ERROR] Cannot open records file.\n";
            pause();
            return;
        }

        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[12];
            for (int i = 0; i < 12 && getline(ss, token[i], '|'); i++)
                ;

            string stored = token[1];
            string search = searchName;
            for (char &c : stored)
                c = tolower(c);
            for (char &c : search)
                c = tolower(c);

            if (stored.find(search) != string::npos)
            {
                cout << "\n"
                     << DASH << endl;
                cout << "  Patient ID  : " << token[0] << endl;
                cout << "  Name        : " << token[1] << endl;
                cout << "  Age         : " << token[2] << endl;
                cout << "  Disease     : " << token[6] << endl;
                cout << "  Doctor      : " << token[9] << endl;
                found = true;
            }
        }
        file.close();

        if (!found)
            cout << "\n  [NOT FOUND] No patient found with name '" << searchName << "'.\n";
        pause();
    }

    void searchMenu()
    {
        showHeading("SEARCH PATIENT");
        cout << "\n  1. Search by ID\n  2. Search by Name\n";
        int ch = getIntInput("\n  Enter choice: ", 1, 2);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (ch == 1)
        {
            int id = getIntInput("  Enter Patient ID: ", 1, 999999);
            searchPatient(id);
        }
        else
        {
            string name = getNameInput("  Enter Patient Name: ");
            searchPatient(name);
        }
    }

    void updatePatient()
    {
        showHeading("UPDATE PATIENT RECORD");
        int id = getIntInput("\n  Enter Patient ID to update: ", 1, 999999);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        ifstream file("patients.txt");
        ofstream temp("temp_patients.txt");

        if (!file.is_open())
        {
            cout << "\n  [ERROR] Cannot open records file.\n";
            pause();
            return;
        }

        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[12];
            for (int i = 0; i < 12 && getline(ss, token[i], '|'); i++)
                ;

            try
            {
                if (stoi(token[0]) == id)
                {
                    found = true;
                    cout << "\n  Current Disease : " << token[6] << endl;
                    cout << "  Current Phone   : " << token[4] << endl;
                    cout << "  Current Doctor  : " << token[9] << endl;
                    cout << "  Current Room    : " << token[10] << endl;

                    cout << "\n  (Press Enter to keep current value)\n";

                    cout << "  New Disease  : ";
                    string nd;
                    getline(cin, nd);
                    if (!nd.empty())
                        token[6] = nd;

                    string np;
                    while (true)
                    {
                        cout << "  New Phone    : ";
                        getline(cin, np);
                        if (np.empty())
                            break;
                        if (isValidPhone(np))
                        {
                            token[4] = np;
                            break;
                        }
                        cout << "  [ERROR] Invalid phone!\n";
                    }

                    cout << "  New Doctor   : ";
                    string ndr;
                    getline(cin, ndr);
                    if (!ndr.empty())
                        token[9] = ndr;

                    cout << "  New Room     : ";
                    string nr;
                    getline(cin, nr);
                    if (!nr.empty())
                        token[10] = nr;

                    temp << token[0] << "|" << token[1] << "|" << token[2] << "|"
                         << token[3] << "|" << token[4] << "|" << token[5] << "|"
                         << token[6] << "|" << token[7] << "|" << token[8] << "|"
                         << token[9] << "|" << token[10] << "|" << token[11] << "\n";

                    cout << "\n  [SUCCESS] Patient record updated!\n";
                }
                else
                {
                    temp << line << "\n";
                }
            }
            catch (...)
            {
                temp << line << "\n";
            }
        }

        file.close();
        temp.close();
        remove("patients.txt");
        rename("temp_patients.txt", "patients.txt");

        if (!found)
            cout << "\n  [NOT FOUND] Patient ID " << id << " not found.\n";
        pause();
    }

    void deletePatient()
    {
        showHeading("DELETE PATIENT RECORD");
        int id = getIntInput("\n  Enter Patient ID to delete: ", 1, 999999);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "\n  Are you sure you want to delete Patient ID " << id << "? (Y/N): ";
        char confirm;
        cin >> confirm;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (confirm != 'Y' && confirm != 'y')
        {
            cout << "\n  [CANCELLED] Deletion cancelled.\n";
            pause();
            return;
        }

        ifstream file("patients.txt");
        ofstream temp("temp_patients.txt");
        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[12];
            for (int i = 0; i < 12 && getline(ss, token[i], '|'); i++)
                ;

            try
            {
                if (stoi(token[0]) == id)
                {
                    found = true;
                    cout << "\n  [SUCCESS] Patient '" << token[1] << "' deleted.\n";
                }
                else
                {
                    temp << line << "\n";
                }
            }
            catch (...)
            {
                temp << line << "\n";
            }
        }

        file.close();
        temp.close();
        remove("patients.txt");
        rename("temp_patients.txt", "patients.txt");

        if (!found)
            cout << "\n  [NOT FOUND] Patient ID " << id << " does not exist.\n";
        pause();
    }

    void menu()
    {
        int choice;
        do
        {
            clearScreen();
            showHeading("PATIENT MANAGEMENT");
            cout << "\n  1. Add New Patient" << endl;
            cout << "  2. View All Patients" << endl;
            cout << "  3. Search Patient" << endl;
            cout << "  4. Update Patient" << endl;
            cout << "  5. Delete Patient" << endl;
            cout << "  0. Back to Main Menu" << endl;
            choice = getIntInput("\n  Enter your choice: ", 0, 5);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                addPatient();
                break;
            case 2:
                viewAllPatients();
                break;
            case 3:
                searchMenu();
                break;
            case 4:
                updatePatient();
                break;
            case 5:
                deletePatient();
                break;
            case 0:
                break;
            }
        } while (choice != 0);
    }
};

// CLASS: DoctorManager
class DoctorManager
{
private:
    bool idExists(int id)
    {
        ifstream file("doctors.txt");
        string line;
        while (getline(file, line))
        {
            if (line.empty())
                continue;
            size_t pos = line.find('|');
            if (pos == string::npos)
                continue;
            try
            {
                if (stoi(line.substr(0, pos)) == id)
                {
                    file.close();
                    return true;
                }
            }
            catch (...)
            {
            }
        }
        file.close();
        return false;
    }

public:
    void addDoctor()
    {
        showHeading("ADD NEW DOCTOR");

        int id = getIntInput("\n  Enter Doctor ID      : ", 1, 999999);

        if (idExists(id))
        {
            cout << "\n  [ERROR] Doctor ID already exists!\n";
            pause();
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string name = getNameInput("  Enter Name           : ");
        int age = getIntInput("  Enter Age            : ", 22, 90);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string gender = getGenderInput();

        string phone;
        while (true)
        {
            phone = getStringInput("  Enter Phone          : ");
            if (isValidPhone(phone))
                break;
            cout << "  [ERROR] Invalid phone number!\n";
        }

        string address = getStringInput("  Enter Address        : ");
        string spec = getStringInput("  Enter Specialization : ");
        string qual = getStringInput("  Enter Qualification  : ");
        int exp = getIntInput("  Experience (years)   : ", 0, 60);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string avail = getStringInput("  Availability (timing): ");
        string dept = getStringInput("  Department           : ");

        Doctor d(id, name, age, gender, phone, address,
                 spec, qual, exp, avail, dept);
        d.saveToFile();

        cout << "\n  [SUCCESS] Doctor record saved successfully!\n";
        pause();
    }

    void viewAllDoctors()
    {
        showHeading("ALL DOCTOR RECORDS");

        ifstream file("doctors.txt");
        if (!file.is_open() || file.peek() == EOF)
        {
            cout << "\n  [INFO] No doctor records found.\n";
            file.close();
            pause();
            return;
        }

        string line;
        int count = 0;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[11];
            for (int i = 0; i < 11 && getline(ss, token[i], '|'); i++)
                ;

            cout << "\n"
                 << DASH << endl;
            cout << "  Doctor ID      : " << token[0] << endl;
            cout << "  Name           : " << token[1] << endl;
            cout << "  Age            : " << token[2] << endl;
            cout << "  Gender         : " << token[3] << endl;
            cout << "  Phone          : " << token[4] << endl;
            cout << "  Specialization : " << token[6] << endl;
            cout << "  Qualification  : " << token[7] << endl;
            cout << "  Experience     : " << token[8] << " years" << endl;
            cout << "  Availability   : " << token[9] << endl;
            cout << "  Department     : " << token[10] << endl;
            count++;
        }
        file.close();

        cout << "\n"
             << DASH << endl;
        cout << "  Total Doctors: " << count << endl;
        pause();
    }

    void searchDoctor(int searchId)
    {
        showHeading("SEARCH DOCTOR BY ID");
        ifstream file("doctors.txt");
        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[11];
            for (int i = 0; i < 11 && getline(ss, token[i], '|'); i++)
                ;

            try
            {
                if (stoi(token[0]) == searchId)
                {
                    cout << "\n"
                         << DASH << endl;
                    cout << "  Doctor ID      : " << token[0] << endl;
                    cout << "  Name           : " << token[1] << endl;
                    cout << "  Specialization : " << token[6] << endl;
                    cout << "  Department     : " << token[10] << endl;
                    cout << "  Availability   : " << token[9] << endl;
                    found = true;
                    break;
                }
            }
            catch (...)
            {
            }
        }
        file.close();

        if (!found)
            cout << "\n  [NOT FOUND] Doctor ID " << searchId << " not found.\n";
        pause();
    }

    void searchDoctor(const string &searchSpec)
    {
        showHeading("SEARCH DOCTOR BY SPECIALIZATION");
        ifstream file("doctors.txt");
        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[11];
            for (int i = 0; i < 11 && getline(ss, token[i], '|'); i++)
                ;

            string stored = token[6], search = searchSpec;
            for (char &c : stored)
                c = tolower(c);
            for (char &c : search)
                c = tolower(c);

            if (stored.find(search) != string::npos)
            {
                cout << "\n"
                     << DASH << endl;
                cout << "  Doctor ID      : " << token[0] << endl;
                cout << "  Name           : " << token[1] << endl;
                cout << "  Specialization : " << token[6] << endl;
                cout << "  Availability   : " << token[9] << endl;
                found = true;
            }
        }
        file.close();

        if (!found)
            cout << "\n  [NOT FOUND] No doctor with specialization '" << searchSpec << "'.\n";
        pause();
    }

    void updateDoctor()
    {
        showHeading("UPDATE DOCTOR RECORD");
        int id = getIntInput("\n  Enter Doctor ID to update: ", 1, 999999);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        ifstream file("doctors.txt");
        ofstream temp("temp_doctors.txt");
        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[11];
            for (int i = 0; i < 11 && getline(ss, token[i], '|'); i++)
                ;

            try
            {
                if (stoi(token[0]) == id)
                {
                    found = true;
                    cout << "\n  Current Availability : " << token[9] << endl;
                    cout << "  Current Department   : " << token[10] << endl;
                    cout << "  Current Phone        : " << token[4] << endl;
                    cout << "\n  (Press Enter to keep current value)\n";

                    cout << "  New Availability : ";
                    string na;
                    getline(cin, na);
                    if (!na.empty())
                        token[9] = na;

                    cout << "  New Department   : ";
                    string nd;
                    getline(cin, nd);
                    if (!nd.empty())
                        token[10] = nd;

                    string np;
                    while (true)
                    {
                        cout << "  New Phone        : ";
                        getline(cin, np);
                        if (np.empty())
                            break;
                        if (isValidPhone(np))
                        {
                            token[4] = np;
                            break;
                        }
                        cout << "  [ERROR] Invalid phone!\n";
                    }

                    temp << token[0] << "|" << token[1] << "|" << token[2] << "|"
                         << token[3] << "|" << token[4] << "|" << token[5] << "|"
                         << token[6] << "|" << token[7] << "|" << token[8] << "|"
                         << token[9] << "|" << token[10] << "\n";

                    cout << "\n  [SUCCESS] Doctor record updated!\n";
                }
                else
                {
                    temp << line << "\n";
                }
            }
            catch (...)
            {
                temp << line << "\n";
            }
        }

        file.close();
        temp.close();
        remove("doctors.txt");
        rename("temp_doctors.txt", "doctors.txt");

        if (!found)
            cout << "\n  [NOT FOUND] Doctor ID " << id << " not found.\n";
        pause();
    }

    void deleteDoctor()
    {
        showHeading("DELETE DOCTOR RECORD");
        int id = getIntInput("\n  Enter Doctor ID to delete: ", 1, 999999);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "\n  Are you sure? (Y/N): ";
        char confirm;
        cin >> confirm;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (confirm != 'Y' && confirm != 'y')
        {
            cout << "\n  [CANCELLED]\n";
            pause();
            return;
        }

        ifstream file("doctors.txt");
        ofstream temp("temp_doctors.txt");
        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[11];
            for (int i = 0; i < 11 && getline(ss, token[i], '|'); i++)
                ;

            try
            {
                if (stoi(token[0]) == id)
                {
                    found = true;
                    cout << "\n  [SUCCESS] Doctor '" << token[1] << "' deleted.\n";
                }
                else
                {
                    temp << line << "\n";
                }
            }
            catch (...)
            {
                temp << line << "\n";
            }
        }

        file.close();
        temp.close();
        remove("doctors.txt");
        rename("temp_doctors.txt", "doctors.txt");

        if (!found)
            cout << "\n  [NOT FOUND] Doctor ID " << id << " not found.\n";
        pause();
    }

    void menu()
    {
        int choice;
        do
        {
            clearScreen();
            showHeading("DOCTOR MANAGEMENT");
            cout << "\n  1. Add New Doctor" << endl;
            cout << "  2. View All Doctors" << endl;
            cout << "  3. Search by ID" << endl;
            cout << "  4. Search by Specialization" << endl;
            cout << "  5. Update Doctor" << endl;
            cout << "  6. Delete Doctor" << endl;
            cout << "  0. Back to Main Menu" << endl;
            choice = getIntInput("\n  Enter your choice: ", 0, 6);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                addDoctor();
                break;
            case 2:
                viewAllDoctors();
                break;
            case 3:
            {
                int id = getIntInput("  Enter Doctor ID: ", 1, 999999);
                searchDoctor(id);
                break;
            }
            case 4:
            {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                string spec = getStringInput("  Enter Specialization: ");
                searchDoctor(spec);
                break;
            }
            case 5:
                updateDoctor();
                break;
            case 6:
                deleteDoctor();
                break;
            case 0:
                break;
            }
        } while (choice != 0);
    }
};

// CLASS: AppointmentManager
class AppointmentManager
{
private:
    bool idExists(int id)
    {
        ifstream file("appointments.txt");
        string line;
        while (getline(file, line))
        {
            if (line.empty())
                continue;
            size_t pos = line.find('|');
            if (pos == string::npos)
                continue;
            try
            {
                if (stoi(line.substr(0, pos)) == id)
                {
                    file.close();
                    return true;
                }
            }
            catch (...)
            {
            }
        }
        file.close();
        return false;
    }

public:
    void createAppointment()
    {
        showHeading("CREATE NEW APPOINTMENT");

        int aid = getIntInput("\n  Enter Appointment ID : ", 1, 999999);

        if (idExists(aid))
        {
            cout << "\n  [ERROR] Appointment ID already exists!\n";
            pause();
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string pName = getNameInput("  Enter Patient Name   : ");
        string dName = getNameInput("  Enter Doctor Name    : ");
        string date = getStringInput("  Enter Date (DD/MM/YYYY): ");
        string time = getStringInput("  Enter Time (e.g. 10:30 AM): ");
        string disease = getStringInput("  Enter Disease/Reason : ");

        Appointment a(aid, pName, dName, date, time, disease);
        a.saveToFile();

        cout << "\n  [SUCCESS] Appointment created successfully!\n";
        pause();
    }

    void viewAllAppointments()
    {
        showHeading("ALL APPOINTMENTS");

        ifstream file("appointments.txt");
        if (!file.is_open() || file.peek() == EOF)
        {
            cout << "\n  [INFO] No appointments found.\n";
            file.close();
            pause();
            return;
        }

        string line;
        int count = 0;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[7];
            for (int i = 0; i < 7 && getline(ss, token[i], '|'); i++)
                ;

            cout << "\n"
                 << DASH << endl;
            cout << "  Appointment ID : " << token[0] << endl;
            cout << "  Patient        : " << token[1] << endl;
            cout << "  Doctor         : " << token[2] << endl;
            cout << "  Date           : " << token[3] << endl;
            cout << "  Time           : " << token[4] << endl;
            cout << "  Disease        : " << token[5] << endl;
            cout << "  Status         : " << token[6] << endl;
            count++;
        }
        file.close();

        cout << "\n"
             << DASH << endl;
        cout << "  Total Appointments: " << count << endl;
        pause();
    }

    void cancelAppointment()
    {
        showHeading("CANCEL APPOINTMENT");
        int aid = getIntInput("\n  Enter Appointment ID to cancel: ", 1, 999999);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        ifstream file("appointments.txt");
        ofstream temp("temp_appointments.txt");
        string line;
        bool found = false;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[7];
            for (int i = 0; i < 7 && getline(ss, token[i], '|'); i++)
                ;

            try
            {
                if (stoi(token[0]) == aid)
                {
                    found = true;
                    temp << token[0] << "|" << token[1] << "|" << token[2] << "|"
                         << token[3] << "|" << token[4] << "|" << token[5] << "|Cancelled\n";
                    cout << "\n  [SUCCESS] Appointment " << aid << " cancelled.\n";
                }
                else
                {
                    temp << line << "\n";
                }
            }
            catch (...)
            {
                temp << line << "\n";
            }
        }

        file.close();
        temp.close();
        remove("appointments.txt");
        rename("temp_appointments.txt", "appointments.txt");

        if (!found)
            cout << "\n  [NOT FOUND] Appointment ID not found.\n";
        pause();
    }

    void menu()
    {
        int choice;
        do
        {
            clearScreen();
            showHeading("APPOINTMENT MANAGEMENT");
            cout << "\n  1. Create Appointment" << endl;
            cout << "  2. View All Appointments" << endl;
            cout << "  3. Cancel Appointment" << endl;
            cout << "  0. Back to Main Menu" << endl;
            choice = getIntInput("\n  Enter your choice: ", 0, 3);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                createAppointment();
                break;
            case 2:
                viewAllAppointments();
                break;
            case 3:
                cancelAppointment();
                break;
            case 0:
                break;
            }
        } while (choice != 0);
    }
};
// CLASS: BillingManager
class BillingManager
{
public:
    void generateBill()
    {
        showHeading("GENERATE PATIENT BILL");

        int bid = getIntInput("\n  Enter Bill ID         : ", 1, 999999);
        int pid = getIntInput("  Enter Patient ID      : ", 1, 999999);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string pName = getNameInput("  Enter Patient Name    : ");
        double cf = getDoubleInput("  Consultation Fee (Rs.): ");
        double mc = getDoubleInput("  Medicine Cost (Rs.)   : ");
        double rc = getDoubleInput("  Room Charges (Rs.)    : ");
        double oc = getDoubleInput("  Other Charges (Rs.)   : ");

        Bill b(bid, pid, pName, cf, mc, rc, oc);
        b.display();
        b.saveToFile();

        cout << "\n  [SUCCESS] Bill generated and saved!\n";
        pause();
    }

    void viewAllBills()
    {
        showHeading("ALL BILLING RECORDS");

        ifstream file("bills.txt");
        if (!file.is_open() || file.peek() == EOF)
        {
            cout << "\n  [INFO] No billing records found.\n";
            file.close();
            pause();
            return;
        }

        string line;
        double totalRevenue = 0;
        int count = 0;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[10];
            for (int i = 0; i < 10 && getline(ss, token[i], '|'); i++)
                ;

            cout << "\n"
                 << DASH << endl;
            cout << "  Bill ID      : " << token[0] << endl;
            cout << "  Patient ID   : " << token[1] << endl;
            cout << "  Patient Name : " << token[2] << endl;
            cout << fixed << setprecision(2);
            cout << "  Total Amount : Rs. " << token[7] << endl;
            cout << "  Status       : " << token[8] << endl;
            cout << "  Date         : " << token[9] << endl;

            try
            {
                totalRevenue += stod(token[7]);
            }
            catch (...)
            {
            }
            count++;
        }
        file.close();

        cout << "\n"
             << LINE << endl;
        cout << "  Total Bills   : " << count << endl;
        cout << fixed << setprecision(2);
        cout << "  Total Revenue : Rs. " << totalRevenue << endl;
        cout << LINE << endl;
        pause();
    }

    void menu()
    {
        int choice;
        do
        {
            clearScreen();
            showHeading("BILLING SYSTEM");
            cout << "\n  1. Generate New Bill" << endl;
            cout << "  2. View All Bills" << endl;
            cout << "  0. Back to Main Menu" << endl;
            choice = getIntInput("\n  Enter your choice: ", 0, 2);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                generateBill();
                break;
            case 2:
                viewAllBills();
                break;
            case 0:
                break;
            }
        } while (choice != 0);
    }
};

// CLASS: PharmacyManager
class PharmacyManager
{
public:
    void addMedicine()
    {
        showHeading("ADD NEW MEDICINE");

        int mid = getIntInput("\n  Enter Medicine ID   : ", 1, 999999);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string mName = getStringInput("  Enter Medicine Name : ");
        string cat = getStringInput("  Enter Category      : ");
        string mfg = getStringInput("  Enter Manufacturer  : ");
        double price = getDoubleInput("  Enter Price (Rs.)   : ");
        int qty = getIntInput("  Enter Stock Qty     : ", 0, 999999);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string expiry = getStringInput("  Enter Expiry Date   : ");

        Medicine m(mid, mName, cat, mfg, price, qty, expiry);
        m.saveToFile();

        cout << "\n  [SUCCESS] Medicine added to pharmacy!\n";
        pause();
    }

    void viewAllMedicines()
    {
        showHeading("PHARMACY - ALL MEDICINES");

        ifstream file("medicines.txt");
        if (!file.is_open() || file.peek() == EOF)
        {
            cout << "\n  [INFO] No medicines in pharmacy.\n";
            file.close();
            pause();
            return;
        }

        string line;
        int count = 0;

        while (getline(file, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[7];
            for (int i = 0; i < 7 && getline(ss, token[i], '|'); i++)
                ;

            cout << "\n"
                 << DASH << endl;
            cout << "  Medicine ID  : " << token[0] << endl;
            cout << "  Name         : " << token[1] << endl;
            cout << "  Category     : " << token[2] << endl;
            cout << "  Manufacturer : " << token[3] << endl;
            cout << fixed << setprecision(2);
            cout << "  Price        : Rs. " << token[4] << endl;
            cout << "  Stock        : " << token[5] << " units" << endl;
            cout << "  Expiry       : " << token[6] << endl;

            try
            {
                if (stoi(token[5]) < 10)
                    cout << "  *** [WARNING] LOW STOCK! ***\n";
            }
            catch (...)
            {
            }
            count++;
        }
        file.close();

        cout << "\n"
             << DASH << endl;
        cout << "  Total Medicines: " << count << endl;
        pause();
    }

    void getMedicineInfo()
    {
        showHeading("MEDICINE INFORMATION DATABASE");

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string mName = getStringInput("\n  Enter Medicine Name to search: ");

        cout << "\n"
             << LINE << endl;
        cout << "  Searching for: " << mName << endl;
        cout << LINE << endl;

        // Built-in medicine reference database
        string medicines[][4] = {
            {"Paracetamol", "Analgesic/Antipyretic", "Fever, headache, body pain", "500mg-1g every 4-6 hours"},
            {"Amoxicillin", "Antibiotic", "Bacterial infections", "250mg-500mg every 8 hours"},
            {"Ibuprofen", "NSAID/Painkiller", "Pain, inflammation, fever", "200mg-400mg every 4-6 hours"},
            {"Metformin", "Antidiabetic", "Type 2 Diabetes", "500mg-1000mg twice daily"},
            {"Omeprazole", "Proton Pump Inhibitor", "Acidity, GERD, stomach ulcers", "20-40mg once daily"},
            {"Azithromycin", "Antibiotic", "Respiratory infections, STIs", "500mg once daily for 3 days"},
            {"Cetirizine", "Antihistamine", "Allergies, hay fever, urticaria", "5-10mg once daily"},
            {"Lisinopril", "ACE Inhibitor", "Hypertension, heart failure", "5-40mg once daily"},
            {"Atorvastatin", "Statin", "High cholesterol", "10-80mg once daily"},
            {"Aspirin", "Antiplatelet/Analgesic", "Pain, fever, blood clot prevention", "75-325mg once daily"},
        };

        bool found = false;
        string search = mName;
        for (char &c : search)
            c = tolower(c);

        for (int i = 0; i < 10; i++)
        {
            string stored = medicines[i][0];
            for (char &c : stored)
                c = tolower(c);

            if (stored.find(search) != string::npos)
            {
                cout << "\n  Name        : " << medicines[i][0] << endl;
                cout << "  Category    : " << medicines[i][1] << endl;
                cout << "  Used For    : " << medicines[i][2] << endl;
                cout << "  Dosage Info : " << medicines[i][3] << endl;
                cout << "\n  [NOTE] Always consult a doctor before use.\n";
                found = true;
            }
        }

        if (!found)
            cout << "\n  [INFO] Medicine '" << mName << "' not found in reference database.\n";

        pause();
    }

    void menu()
    {
        int choice;
        do
        {
            clearScreen();
            showHeading("PHARMACY MANAGEMENT");
            cout << "\n  1. Add Medicine to Stock" << endl;
            cout << "  2. View All Medicines" << endl;
            cout << "  3. Search Medicine Information" << endl;
            cout << "  0. Back to Main Menu" << endl;
            choice = getIntInput("\n  Enter your choice: ", 0, 3);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                addMedicine();
                break;
            case 2:
                viewAllMedicines();
                break;
            case 3:
                getMedicineInfo();
                break;
            case 0:
                break;
            }
        } while (choice != 0);
    }
};

// ============================================================
// CLASS: ReportManager
// ============================================================
class ReportManager
{
public:
    int countRecords(const string &filename)
    {
        ifstream file(filename);
        int count = 0;
        string line;
        while (getline(file, line))
            if (!line.empty())
                count++;
        file.close();
        return count;
    }

    void generateSummaryReport()
    {
        showHeading("HOSPITAL SUMMARY REPORT");

        int totalPatients = countRecords("patients.txt");
        int totalDoctors = countRecords("doctors.txt");
        int totalAppointments = countRecords("appointments.txt");
        int totalBills = countRecords("bills.txt");
        int totalMedicines = countRecords("medicines.txt");

        double totalRevenue = 0;
        ifstream billFile("bills.txt");
        string line;
        while (getline(billFile, line))
        {
            if (line.empty())
                continue;
            stringstream ss(line);
            string token[10];
            for (int i = 0; i < 10 && getline(ss, token[i], '|'); i++)
                ;
            try
            {
                if (!token[7].empty())
                    totalRevenue += stod(token[7]);
            }
            catch (...)
            {
            }
        }
        billFile.close();

        cout << "\n  Report Date        : " << getCurrentDate() << endl;
        cout << "\n"
             << DASH << endl;
        cout << "  Total Patients     : " << setw(5) << totalPatients << endl;
        cout << "  Total Doctors      : " << setw(5) << totalDoctors << endl;
        cout << "  Total Appointments : " << setw(5) << totalAppointments << endl;
        cout << "  Total Bills        : " << setw(5) << totalBills << endl;
        cout << "  Total Medicines    : " << setw(5) << totalMedicines << endl;
        cout << DASH << endl;
        cout << fixed << setprecision(2);
        cout << "  TOTAL REVENUE      : Rs. " << totalRevenue << endl;
        cout << DASH << endl;

        ofstream rFile("report.txt", ios::app);
        rFile << "\n=== REPORT: " << getCurrentDate() << " ===\n";
        rFile << "Patients     : " << totalPatients << "\n";
        rFile << "Doctors      : " << totalDoctors << "\n";
        rFile << "Appointments : " << totalAppointments << "\n";
        rFile << "Bills        : " << totalBills << "\n";
        rFile << "Medicines    : " << totalMedicines << "\n";
        rFile << fixed << setprecision(2);
        rFile << "Revenue      : Rs. " << totalRevenue << "\n";
        rFile.close();

        cout << "\n  [SUCCESS] Report also saved to report.txt\n";
        pause();
    }

    void menu()
    {
        int choice;
        do
        {
            clearScreen();
            showHeading("REPORTS");
            cout << "\n  1. Generate Summary Report" << endl;
            cout << "  0. Back to Main Menu" << endl;
            choice = getIntInput("\n  Enter your choice: ", 0, 1);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                generateSummaryReport();
                break;
            case 0:
                break;
            }
        } while (choice != 0);
    }
};
// CLASS: Hospital (Main Controller)
class Hospital
{
private:
    Login loginSystem;
    PatientManager patientMgr;
    DoctorManager doctorMgr;
    AppointmentManager appointmentMgr;
    BillingManager billingMgr;
    PharmacyManager pharmacyMgr;
    ReportManager reportMgr;

    string hospitalName;
    string hospitalAddress;

    void showWelcomeScreen()
    {
        clearScreen();
        cout << "\n\n";
        cout << LINE << endl;
        cout << LINE << endl;
        cout << "\n";
        cout << "         ***  WELCOME TO  ***" << endl;
        cout << "\n";
        cout << "     " << hospitalName << endl;
        cout << "     " << hospitalAddress << endl;
        cout << "\n";
        cout << "     Hospital Management System v2.0" << endl;
        cout << "     Language : C++ | OOP | File Handling" << endl;
        cout << "\n";
        cout << LINE << endl;
        cout << LINE << endl;
        cout << "\n";
        pause();
    }

public:
    Hospital(const string &name, const string &addr)
        : hospitalName(name), hospitalAddress(addr) {}

    void run()
    {
        showWelcomeScreen();

        if (!loginSystem.login())
        {
            cout << "\n  System locked. Goodbye!\n";
            return;
        }

        int choice;
        do
        {
            clearScreen();
            showHeading("MAIN MENU - " + hospitalName);
            cout << "\n  1.  Patient Management" << endl;
            cout << "  2.  Doctor Management" << endl;
            cout << "  3.  Appointment Scheduling" << endl;
            cout << "  4.  Billing System" << endl;
            cout << "  5.  Pharmacy" << endl;
            cout << "  6.  Medicine Information" << endl;
            cout << "  7.  Reports" << endl;
            cout << "  8.  Change Password" << endl;
            cout << "  0.  Exit System" << endl;
            choice = getIntInput("\n  Enter your choice: ", 0, 8);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                patientMgr.menu();
                break;
            case 2:
                doctorMgr.menu();
                break;
            case 3:
                appointmentMgr.menu();
                break;
            case 4:
                billingMgr.menu();
                break;
            case 5:
                pharmacyMgr.menu();
                break;
            case 6:
                pharmacyMgr.getMedicineInfo();
                break;
            case 7:
                reportMgr.menu();
                break;
            case 8:
                loginSystem.showChangePasswordMenu();
                break;
            case 0:
                clearScreen();
                cout << "\n\n"
                     << LINE << endl;
                cout << "   Thank you for using " << hospitalName << " System!\n";
                cout << "   Stay Safe and Healthy!\n";
                cout << LINE << "\n\n"
                     << endl;
                break;
            default:
                cout << "\n  [ERROR] Invalid choice!\n";
                pause();
            }
        } while (choice != 0);
    }

    ~Hospital() {}
};
int main()
{
    Hospital myHospital("CITY CARE HOSPITAL", "123 Main Road, Lahore, Pakistan");
    myHospital.run();
    return 0;
}
