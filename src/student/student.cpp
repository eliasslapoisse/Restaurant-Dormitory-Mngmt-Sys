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
    int oneheiphen = 0;
    if (name.empty()) {
        return false;
    }
    if (name.size() > 30) {
        return false;
    }
    for (int i = 0; i < name.size(); i++) {
        if (i == 0 || i == name.size()-1)
        {
            if (!std::isalpha(name[i], loc)) { return false; }
        }
        else
        {
            if (!std::isalpha(name[i], loc) && name[i] != '-') { return false; }
            if (name[i] == '-') { oneheiphen++; }
        }
    }
    return (oneheiphen < 2);
}

bool Student::verifyEmail(std::string email) const {
    email.erase(std::remove(email.begin(), email.end(), ' '), email.end());
    std::locale loc("");
    int oneat = 0;
    bool onedot = false;
    int indexat = -1;
    if (email.empty()) {
        return false;
    }
    if (email.size() > 254) {
        return false;
    }
    for (int i = 0; i < email.size(); i++) {
        if (email[i] == '@') {
            oneat++;
            indexat = i;
        }
    }
    if (oneat != 1) { return false; }
    if (indexat > 64 || indexat <= 0 || indexat == email.size()-1) { return false; }

    if     ((!std::isalpha(email[0], loc)              && !std::isdigit(email[0])         && email[0] != '_')         ||
            (!std::isalpha(email[indexat-1], loc)      && !std::isdigit(email[indexat-1]) && email[indexat-1] != '_') ||
            (!std::isalpha(email[email.size()-1], loc) && !std::isdigit(email[email.size()-1]))                       ||
            (!std::isalpha(email[indexat+1], loc)      && !std::isdigit(email[indexat+1]))                            )
            { return false; }

    for (int i = 1; i < indexat - 1; i++) {
        if (!std::isalpha(email[i], loc) && !std::isdigit(email[i]) && email[i] != '_'
            && email[i] != '-' && email[i] != '.' && email[i] != '+')
            { return false; }
    }
    for (int i = indexat + 2; i < email.size()-1; i++) {
        if (!std::isalpha(email[i], loc) && !std::isdigit(email[i]) 
            &&    email[i] != '-'   &&   email[i] != '.')
            {
                return false;
            }
        if (email[i] == '.' && email[i+1] == '.')
            {
                return false;
            }
    }
    for (int i = indexat + 2; i < email.size()-1; i++) {
        if (email[i] == '.') {
            onedot = true;
            break;
        }
    }
    return onedot;
}

bool Student::verifyPhoneNumber(std::string num) const {
    num.erase(std::remove(num.begin(), num.end(), ' '), num.end());
    num.erase(std::remove(num.begin(), num.end(), '-'), num.end());
    if (num.empty()) { return false; }
    if (num.size() != 10) { return false; }
    for (int i = 0; i < num.size(); i++) {
        if (!std::isdigit(num[i])) {
            return false;
        }
    }
    return (num[0] == '0' && (num[1] == '5' || num[1] == '6' || num[1] == '7'));
}

bool Student::verifyAcademicYear(int year) const {
    return (year > 0 && year < 8);
}

bool Student::verifyGender(std::string gender) const {
    if (gender.empty()) { return false; }
    gender.erase(std::remove(gender.begin(), gender.end(), ' '), gender.end());
    return    (gender == "Male" || gender == "male" || gender == "M" || gender == "m" ||
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
        setFirstName(firstName);
        setFamilyName(familyName);
        setEmail(email);
        setPhoneNumber(phoneNumber);
        setGender(gender);
        setAcademicYear(academicYear);
        setId();
    }
}

Student::Student() : id(0), firstName(defaultstr), familyName(defaultstr), email(defaultstr),
                     phoneNumber(defaultstr), gender(defaultstr), academicYear(0) {}

Student::~Student() {}

                    // GETTERS

int Student::getId() const {
    return id;
}

const std::string& Student::getFirstName() const {
    return firstName;
}

const std::string& Student::getFamilyName() const {
    return familyName;
}

const std::string& Student::getEmail() const {
    return email;
}

const std::string& Student::getPhoneNumber() const {
    return phoneNumber;
}

const std::string& Student::getGender() const {
    return gender;
}

int Student::getAcademicYear() const {
    return academicYear;
}

int Student::getIdCount() {
    return idCount;
}

                    // SETTERS

void Student::setId() {
    this->id = ++idCount;
}

void Student::setFirstName(const std::string& firstName) {
    if (verifyName(firstName)) {
        this->firstName = firstName;
    }
}

void Student::setFamilyName(const std::string& familyName) {
    if (verifyName(familyName)) {
        this->familyName = familyName;
    }
}

void Student::setEmail(const std::string& email) {
    if (verifyEmail(email)) {
        this->email = email;
    }
}

void Student::setPhoneNumber(const std::string& phoneNumber) {
    if (verifyPhoneNumber(phoneNumber)) {
        this->phoneNumber = phoneNumber;
    }
}

void Student::setGender(const std::string& gender) {
    if (verifyGender(gender)) {
        this->gender = gender;
    }
}

void Student::setAcademicYear(int academicYear) {
    if (verifyAcademicYear(academicYear)) {
        this->academicYear = academicYear;
    }
}

void Student::setStudent(const std::string &firstName, const std::string &familyName, const std::string &email,
                         const std::string &phoneNumber, const std::string &gender, int academicYear) {
    if (verifyInput(firstName, familyName, email, phoneNumber, gender, academicYear) && !isInitialized()) 
    {
        setFirstName(firstName);
        setFamilyName(familyName);
        setEmail(email);
        setPhoneNumber(phoneNumber);
        setGender(gender);
        setAcademicYear(academicYear);
        setId();
    }
}