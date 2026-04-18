#include "university.h"
#include <algorithm>
#include <fstream>

// CONSTRUCTORS AND DESTRUCTOR

University::University(const std::string& name) : name("Unnamed University") {
    setName(name);
}

University::University() : name("Unnamed University") {}

University::~University() {}

// GETTERS AND SETTERS

const std::string& University::getName() const {
    return name;
}

bool University::setName(const std::string& name) {
    if (name.empty() || name.size() > 50) return false;
    this->name = name;
    return true;
}

const std::vector<Dormitory>& University::getDormitories() const {
    return dormitories;
}

const std::vector<Student>& University::getStudentPool() const {
    return studentPool;
}

Dormitory* University::getDormitory(int dormNumber) {
    for (auto& dorm : dormitories) {
        if (dorm.getNumber() == dormNumber) return &dorm;
    }
    return nullptr;
}

// VALIDATION (UNIQUENESS CHECKS)

bool University::isEmailDuplicate(const std::string& email) const {
    // Check Pool
    for (const auto& s : studentPool) {
        if (s.getEmail() == email) return true;
    }
    // Check all Dorms
    for (const auto& dorm : dormitories) {
        if (dorm.containsStudentEmail(email)) return true;
    }
    return false;
}

bool University::isPhoneDuplicate(const std::string& phone) const {
    // Check Pool
    for (const auto& s : studentPool) {
        if (s.getPhoneNumber() == phone) return true;
    }
    // Check all Dorms
    for (const auto& dorm : dormitories) {
        if (dorm.containsStudentPhone(phone)) return true;
    }
    return false;
}

// DORMITORY MANAGEMENT

bool University::addDormitory(const Dormitory& dorm) {
    if (getDormitory(dorm.getNumber()) != nullptr) return false; // Name must be unique
    dormitories.push_back(dorm);
    return true;
}

bool University::removeDormitory(int dormNumber) {
    for (auto it = dormitories.begin(); it != dormitories.end(); ++it) {
        if (it->getNumber() == dormNumber) {
            // Logic: You might want to move students back to pool before deleting dorm
            dormitories.erase(it);
            return true;
        }
    }
    return false;
}

// STUDENT POOL MANAGEMENT

bool University::enrollStudent(const Student& student) {
    // Guard Clauses: The "Triple Check"
    if (!student.isInitialized()) return false;
    if (isEmailDuplicate(student.getEmail())) return false;
    if (isPhoneDuplicate(student.getPhoneNumber())) return false;

    studentPool.push_back(student);
    return true;
}

bool University::removeFromPool(int studentID) {
    for (auto it = studentPool.begin(); it != studentPool.end(); ++it) {
        if (it->getId() == studentID) {
            studentPool.erase(it);
            return true;
        }
    }
    return false;
}

// ASSIGNMENT AND LIFECYCLE

bool University::assignStudentToDormitory(int studentID, int dormNumber) {
    Dormitory* targetDorm = getDormitory(dormNumber);
    
    // Guard: Dorm exists and has space
    if (targetDorm == nullptr || targetDorm->getAvailableSpace() <= 0) return false;

    for (auto it = studentPool.begin(); it != studentPool.end(); ++it) {
        if (it->getId() == studentID) {
            if (targetDorm->addStudent(*it)) {
                studentPool.erase(it);
                return true;
            }
            return false;
        }
    }
    return false;
}

bool University::unassignStudent(int studentID) {
    for (auto& dorm : dormitories) {
        Student movingStudent;
        // Logic: Extract student from dorm and put them back in the pool
        if (dorm.removeAndReturnStudent(studentID, movingStudent)) {
            studentPool.push_back(movingStudent);
            return true;
        }
    }
    return false;
}

bool University::expelStudent(int studentID) {
    // 1. Check Pool
    if (removeFromPool(studentID)) return true;

    // 2. Check Dorms
    for (auto& dorm : dormitories) {
        if (dorm.unassignStudent(studentID)) return true;
    }
    return false;
}

// STATISTICS AND SEARCH

int University::getTotalCapacity() const {
    int total = 0;
    for (const auto& dorm : dormitories) {
        total += dorm.getTotalCapacity();
    }
    return total;
}

std::string University::locateStudent(int studentID) {
    // 1. Search Pool
    for (const auto& s : studentPool) {
        if (s.getId() == studentID) return "Status: Waiting in Student Pool";
    }

    // 2. Search Dormitories
    for (const auto& dorm : dormitories) {
        std::string path = dorm.findStudentLocation(studentID); 
        if (path != "Not Found") {
            return "Dorm: " + std::to_string(dorm.getNumber()) + " | " + path;
        }
    }

    return "Error: Student ID not found in university system.";
}

// FILE I/O (SAVE AND LOAD)

bool University::saveData(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return false;

    // Header
    outFile << "UNIVERSITY_DATA_V1\n";
    outFile << name << "\n";

    // Save Pool
    outFile << "POOL_START " << studentPool.size() << "\n";
    for (const auto& s : studentPool) {
        // We assume Student has a formatted output or we write fields
        outFile << s.getId() << "|" << s.getEmail() << "|" << s.getPhoneNumber() << "\n";
    }

    // Save Dormitories
    outFile << "DORMS_START " << dormitories.size() << "\n";
    for (const auto& d : dormitories) {
        // d.saveToStream(outFile); // Delegate to Dormitory
    }

    outFile.close();
    return true;
}

bool University::loadData(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) return false;

    // Implementation depends on the final save format chosen
    // But logically: Clear current vectors and rebuild them from file lines.
    
    inFile.close();
    return true;
}