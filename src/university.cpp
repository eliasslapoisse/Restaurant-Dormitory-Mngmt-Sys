#include "university.h"
#include "constants.h"
#include <algorithm>
#include <fstream>
#include <sstream>

// Helper function to split strings by a delimiter
std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

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

    // Header & Global State
    outFile << "UNIVERSITY_DATA_V1\n";
    outFile << name << "\n";
    outFile << "GLOBAL_ID|" << Student::getIdCount() << "\n";

    // 1. Save the Student Pool
    outFile << "POOL_START\n";
    for (const auto& s : studentPool) {
        outFile << s.getId() << "|" << s.getFirstName() << "|" << s.getFamilyName() << "|"
                << s.getEmail() << "|" << s.getPhoneNumber() << "|" << s.getGender() << "|"
                << s.getAcademicYear() << "\n";
    }
    outFile << "POOL_END\n";

    // 2. Save Dormitories, Restaurant Data, and Assigned Students
    outFile << "DORMS_START\n";
    for (const auto& dorm : dormitories) {
        outFile << "DORM|" << dorm.getNumber() << "\n";
        
        // Save Restaurant Data
        const Restaurant& rest = dorm.getRestaurant();
        const auto* weeklyMenu = rest.getWeeklyMenu();
        
        outFile << "REST_MENU";
        for (int i = 0; i < 7; i++) {
            outFile << "|" << static_cast<int>(weeklyMenu[i].breakfast)
                    << "|" << static_cast<int>(weeklyMenu[i].lunch)
                    << "|" << static_cast<int>(weeklyMenu[i].dinner);
        }
        outFile << "\n";

        outFile << "REST_BR";
        for (int id : rest.getAteBreakfastList()) outFile << "|" << id;
        outFile << "\n";

        outFile << "REST_LU";
        for (int id : rest.getAteLunchList()) outFile << "|" << id;
        outFile << "\n";

        outFile << "REST_DI";
        for (int id : rest.getAteDinnerList()) outFile << "|" << id;
        outFile << "\n";
        
        // Save Room Assignments
        for (const auto& block : dorm.getBlocks()) {
            for (int f = 0; f <= LAST_FLOOR; f++) {
                for (int r = 1; r <= LAST_NUMBER; r++) {
                    const Room* room = block.getRoom(f, r);
                    if (room && room->isOccupied()) {
                        for (const auto& s : room->getResidents()) {
                            outFile << "STUDENT|" << s.getId() << "|" << s.getFirstName() << "|"
                                    << s.getFamilyName() << "|" << s.getEmail() << "|"
                                    << s.getPhoneNumber() << "|" << s.getGender() << "|"
                                    << s.getAcademicYear() << "|" << block.getName() << "|"
                                    << f << "|" << r << "\n";
                        }
                    }
                }
            }
        }
    }
    outFile << "DORMS_END\n";

    outFile.close();
    return true;
}

bool University::loadData(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) return false;

    std::string line;
    // Verify file format
    if (!std::getline(inFile, line) || line != "UNIVERSITY_DATA_V1") return false;

    if (std::getline(inFile, line)) {
        setName(line);
    }

    // Read the global ID count
    int savedGlobalIdCount = 0;
    if (std::getline(inFile, line) && line.find("GLOBAL_ID|") == 0) {
        savedGlobalIdCount = std::stoi(line.substr(10)); 
    }

    // Reset current state before loading
    studentPool.clear();
    dormitories.clear();

    std::string section = "";
    Dormitory* currentDorm = nullptr;

    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        // Section Headers
        if (line == "POOL_START") { section = "POOL"; continue; }
        if (line == "POOL_END") { section = ""; continue; }
        if (line == "DORMS_START") { section = "DORMS"; continue; }
        if (line == "DORMS_END") { section = ""; continue; }

        try {
            if (section == "POOL") {
                auto tokens = splitString(line, '|');
                if (tokens.size() == 7) {
                    int id = std::stoi(tokens[0]);
                    int year = std::stoi(tokens[6]);
                    
                    Student::setIdCount(id - 1);
                    Student s(tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], year);
                    studentPool.push_back(s);
                }
            } 
            else if (section == "DORMS") {
                auto tokens = splitString(line, '|');
                
                if (tokens[0] == "DORM" && tokens.size() == 2) {
                    int dormNum = std::stoi(tokens[1]);
                    dormitories.push_back(Dormitory(dormNum));
                    currentDorm = &dormitories.back();
                } 
                else if (tokens[0] == "REST_MENU" && tokens.size() == 22 && currentDorm) {
                    Restaurant::dailyMenu days[7];
                    int idx = 1;
                    for (int i = 0; i < 7; i++) {
                        days[i].breakfast = static_cast<Restaurant::morningDish>(std::stoi(tokens[idx++]));
                        days[i].lunch = static_cast<Restaurant::dish>(std::stoi(tokens[idx++]));
                        days[i].dinner = static_cast<Restaurant::dish>(std::stoi(tokens[idx++]));
                    }
                    currentDorm->getRestaurant().setWeeklyMenu(
                        days[0], days[1], days[2], days[3], days[4], days[5], days[6]
                    );
                }
                else if (tokens[0] == "REST_BR" && currentDorm) {
                    std::vector<int> list;
                    for (size_t i = 1; i < tokens.size(); i++) list.push_back(std::stoi(tokens[i]));
                    currentDorm->getRestaurant().setAteBreakfastList(list);
                }
                else if (tokens[0] == "REST_LU" && currentDorm) {
                    std::vector<int> list;
                    for (size_t i = 1; i < tokens.size(); i++) list.push_back(std::stoi(tokens[i]));
                    currentDorm->getRestaurant().setAteLunchList(list);
                }
                else if (tokens[0] == "REST_DI" && currentDorm) {
                    std::vector<int> list;
                    for (size_t i = 1; i < tokens.size(); i++) list.push_back(std::stoi(tokens[i]));
                    currentDorm->getRestaurant().setAteDinnerList(list);
                }
                else if (tokens[0] == "STUDENT" && tokens.size() == 11 && currentDorm) {
                    int id = std::stoi(tokens[1]);
                    int year = std::stoi(tokens[7]);
                    std::string blockName = tokens[8];
                    int floor = std::stoi(tokens[9]);
                    int roomNum = std::stoi(tokens[10]);

                    Student::setIdCount(id - 1);
                    Student s(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], year);

                    Block* block = currentDorm->getBlock(blockName);
                    if (block) {
                        Room* room = block->getRoom(floor, roomNum);
                        if (room) {
                            room->addResident(s);
                        }
                    }
                }
            }
        } 
        catch (const std::exception& e) {
            // Ignore corrupted lines
            continue;
        }
    }

    inFile.close();

    // Restore the absolute highest ID ever issued
    Student::setIdCount(savedGlobalIdCount);
    
    return true;
}