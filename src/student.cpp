#include "student.h"
#include <iostream>
#include <string>
#include <locale>
#include <algorithm>

int Student::idCount = 0;
const std::string Student::defaultstr = "N/A";

                    // VERIFY FUNCTIONS BODIES

bool Student::verifyName(std::string name) const {
    name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
    std::locale loc("");
    
    if (name.empty() || name.size() > 30) return false;

    int oneheiphen = 0;
    for (int i = 0; i < name.size(); i++) {
        if (i == 0 || i == name.size() - 1) {
            if (!std::isalpha(name[i], loc)) return false; 
        } else {
            if (!std::isalpha(name[i], loc) && name[i] != '-') return false; 
            if (name[i] == '-') oneheiphen++; 
        }
    }
    
    return (oneheiphen < 2);
}

bool Student::verifyEmail(std::string email) const {
    email.erase(std::remove(email.begin(), email.end(), ' '), email.end());
    std::locale loc("");
    
    if (email.empty() || email.size() > 254) return false;

    int oneat = 0;
    int indexat = -1;

    for (int i = 0; i < email.size(); i++) {
        if (email[i] == '@') {
            oneat++;
            indexat = i;
        }
    }

    if (oneat != 1 || indexat > 64 || indexat <= 0 || indexat == email.size() - 1) return false;

    if ((!std::isalpha(email[0], loc) && !std::isdigit(email[0], loc) && email[0] != '_') ||
        (!std::isalpha(email[indexat-1], loc) && !std::isdigit(email[indexat-1], loc) && email[indexat-1] != '_') ||
        (!std::isalpha(email[email.size()-1], loc) && !std::isdigit(email[email.size()-1], loc)) ||
        (!std::isalpha(email[indexat+1], loc) && !std::isdigit(email[indexat+1], loc))) {
        return false;
    }

    for (int i = 1; i < indexat - 1; i++) {
        if (!std::isalpha(email[i], loc) && !std::isdigit(email[i], loc) &&
            email[i] != '_' && email[i] != '-' && email[i] != '.' && email[i] != '+') {
            return false;
        }
    }

    for (int i = indexat + 2; i < email.size() - 1; i++) {
        if (!std::isalpha(email[i], loc) && !std::isdigit(email[i], loc) && email[i] != '-' && email[i] != '.') {
            return false;
        }
        if (email[i] == '.' && email[i+1] == '.') return false;
    }

    bool onedot = false;
    for (int i = indexat + 2; i < email.size() - 1; i++) {
        if (email[i] == '.') {
            onedot = true;
            break;
        }
    }

    return onedot;
}

bool Student::verifyPhoneNumber(std::string num) const {
    if (num.empty() || num.size() != 10) return false;
    
    std::locale loc("");
    for (int i = 0; i < num.size(); i++) {
        if (!std::isdigit(num[i], loc)) return false; 
    }
    
    return (num[0] == '0' && (num[1] == '5' || num[1] == '6' || num[1] == '7'));
}

bool Student::verifyAcademicYear(int year) const {
    return (year > 0 && year < 8);
}

bool Student::verifyGender(std::string gender) const {
    return (gender == "Male" || gender == "male" || gender == "M" || gender == "m" ||
            gender == "Female" || gender == "female" || gender == "F" || gender == "f");
}

bool Student::verifyInput(const std::string& firstName, const std::string& familyName, const std::string& email,
                          const std::string& phoneNumber, const std::string& gender, int academicYear) const {
    return (verifyName(firstName) && verifyName(familyName) && verifyEmail(email) &&
            verifyGender(gender) && verifyAcademicYear(academicYear) && verifyPhoneNumber(phoneNumber));
}

bool Student::isInitialized() const {
    return (id > 0);
}


                    // CONSTRUCTOR AND DESTRUCTOR

Student::Student(const std::string& firstName, const std::string& familyName, const std::string& email,
                 const std::string& phoneNumber, const std::string& gender, int academicYear)
                 : id(0), firstName(defaultstr), familyName(defaultstr), email(defaultstr),
                   phoneNumber(defaultstr), gender(defaultstr), academicYear(0)
{   
    if (verifyInput(firstName, familyName, email, phoneNumber, gender, academicYear)) 
    {
        this->firstName = firstName;
        this->familyName = familyName;
        this->email = email;
        this->phoneNumber = phoneNumber;
        this->gender = gender;
        this->academicYear = academicYear;
        setId();
    }
}

Student::Student() : id(0), firstName(defaultstr), familyName(defaultstr), email(defaultstr),
                     phoneNumber(defaultstr), gender(defaultstr), academicYear(0) {}

Student::~Student() {}


                    // GETTERS

int Student::getId() const { return id; }
const std::string& Student::getFirstName() const { return firstName; }
const std::string& Student::getFamilyName() const { return familyName; }
const std::string& Student::getEmail() const { return email; }
const std::string& Student::getPhoneNumber() const { return phoneNumber; }
const std::string& Student::getGender() const { return gender; }
int Student::getAcademicYear() const { return academicYear; }
int Student::getIdCount() { return idCount; }


                    // SETTERS

void Student::setId() {
    this->id = ++idCount;
}

bool Student::setFirstName(const std::string& firstName) {
    if (verifyName(firstName)) {
        this->firstName = firstName;
        return true;
    }
    return false;
}

bool Student::setFamilyName(const std::string& familyName) {
    if (verifyName(familyName)) {
        this->familyName = familyName;
        return true;
    }
    return false;
}

bool Student::setEmail(const std::string& email) {
    if (verifyEmail(email)) {
        this->email = email;
        return true;
    }
    return false;
}

bool Student::setPhoneNumber(const std::string& phoneNumber) {
    if (verifyPhoneNumber(phoneNumber)) {
        this->phoneNumber = phoneNumber;
        return true;
    }
    return false;
}

bool Student::setGender(const std::string& gender) {
    if (verifyGender(gender)) {
        this->gender = gender;
        return true;
    }
    return false;
}

bool Student::setAcademicYear(int academicYear) {
    if (verifyAcademicYear(academicYear)) {
        this->academicYear = academicYear;
        return true;
    }
    return false;
}

bool Student::setStudent(const std::string &firstName, const std::string &familyName, const std::string &email,
                         const std::string &phoneNumber, const std::string &gender, int academicYear) {
    if (verifyInput(firstName, familyName, email, phoneNumber, gender, academicYear) && !isInitialized()) 
    {
        this->firstName = firstName;
        this->familyName = familyName;
        this->email = email;
        this->phoneNumber = phoneNumber;
        this->gender = gender;
        this->academicYear = academicYear;
        setId();
        return true;
    }
    return false;
}

void Student::setIdCount(int highestID) {
    idCount = highestID;
}

                    // EQUAL OPERATOR
bool operator==(const Student& Student1, const Student& Student2) {
    return (Student1.id           == Student2.id          &&
            Student1.firstName    == Student2.firstName   &&
            Student1.familyName   == Student2.familyName  &&
            Student1.email        == Student2.email       &&
            Student1.phoneNumber  == Student2.phoneNumber &&
            Student1.gender       == Student2.gender      &&
            Student1.academicYear == Student2.academicYear);
}