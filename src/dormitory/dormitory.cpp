#include "dormitory.h"
#include <vector>
#include <string>
#include <iostream>
#include "constants.h"
#include <algorithm>

Dormitory::Dormitory(int number) : number(number) {
    blocks.emplace_back(Block("A"));
    blocks.emplace_back(Block("B"));
    blocks.emplace_back(Block("C"));
    blocks.emplace_back(Block("D"));
    blocks.emplace_back(Block("E"));
}

Dormitory::~Dormitory() {}

int Dormitory::getNumber() const {
    return number;
}

const std::vector<Block>& Dormitory::getBlocks() const {
    return blocks;
}

Block* Dormitory::getBlock(const std::string& blockname) {
    for (auto& block : blocks) {
        if (block.getName() == blockname) {
            return &block;
        }
    }
    return nullptr;
}

const Block* Dormitory::getBlock(const std::string& blockname) const {
    for (const auto& block : blocks) {
        if (block.getName() == blockname) {
            return &block;
        }
    }
    return nullptr;
}

int Dormitory::getTotalOccupancy() const {
    int totalOccupancy = 0;
    for (const auto& block : blocks) {
        totalOccupancy += block.getTotalOccupancy();
    }
    return totalOccupancy;
}

std::vector<Block*> Dormitory::getAvailableBlocks() {
    std::vector<Block*> available;
    for (auto& block : blocks) {
        if (!block.isFull()) {
            available.push_back(&block);
        }
    }
    return available;
}

std::vector<Block*> Dormitory::getFullBlocks() {
    std::vector<Block*> fullBlocks;
    for (auto& block : blocks) {
        if (block.isFull()) {
            fullBlocks.push_back(&block);
        }
    }
    return fullBlocks;
}

const Restaurant& Dormitory::getRestaurant() const {
    return restaurant;
}

Restaurant& Dormitory::getRestaurant() {
    return restaurant;
}

std::vector<Room*> Dormitory::getAvailableRooms() {
    std::vector<Room*> available;
    for (auto& block : blocks) {
        std::vector<Room*> blockRooms = block.getAvailableRooms();
        available.insert(available.end(), blockRooms.begin(), blockRooms.end());
    }
    return available;
}

std::vector<Room*> Dormitory::getFullRooms() {
    std::vector<Room*> full;
    for (auto& block : blocks) {
        std::vector<Room*> blockRooms = block.getFullRooms();
        full.insert(full.end(), blockRooms.begin(), blockRooms.end());
    }
    return full;
}

std::vector<Room*> Dormitory::getRoomsByFloor(int floor) {
    std::vector<Room*> rooms;
    if (floor < 0 || floor > LAST_FLOOR) {
        return rooms;
    }
    for (auto& block : blocks) {
        std::vector<Room*> blockRooms = block.getRoomsByFloor(floor);
        rooms.insert(rooms.end(), blockRooms.begin(), blockRooms.end());
    }
    return rooms;
}

Room* Dormitory::findRoomByStudentID(int ID) {
    if (ID <= 0) return nullptr;
    for (auto& block : blocks) {
        Room* foundRoom = block.findRoomByStudentID(ID);
        if (foundRoom != nullptr) {
            return foundRoom;
        }
    }
    return nullptr;
}

bool Dormitory::isStudentIn(int ID) const {
    if (ID <= 0) return false;
    for (const auto& block : blocks) { 
        if (block.isStudentIn(ID)) {
            return true;
        }
    }
    return false;
}

bool Dormitory::isFull() const {
    for (const auto& block : blocks) {
        if (!block.isFull()) { 
            return false;
        }
    }
    return true;
}

bool Dormitory::assignStudentToRoom(const std::string& blockName, int floor, int number, const Student& student) {
    if (isStudentIn(student.getId())) return false; // Prevent clones
    
    Block* targetBlock = getBlock(blockName);
    if (targetBlock == nullptr) return false;
    
    Room* targetRoom = targetBlock->getRoom(floor, number);
    if (targetRoom == nullptr) return false;
    
    return targetRoom->addResident(student);
}

bool Dormitory::unassignStudent(int studentId) {
    if (studentId <= 0) return false;
    
    Room* room = findRoomByStudentID(studentId);
    if (room != nullptr) {
        return room->removeResident(studentId);
    }
    return false;
}

bool Dormitory::moveStudent(int studentId, const std::string& dstBlockName, int dstFloor, int dstNumber) {
    if (studentId <= 0) return false;
    
    Room* srcRoom = findRoomByStudentID(studentId);
    if (srcRoom == nullptr) return false;
    
    Block* targetBlock = getBlock(dstBlockName);
    if (targetBlock == nullptr) return false;
    
    Room* dstRoom = targetBlock->getRoom(dstFloor, dstNumber);
    if (dstRoom == nullptr) return false;

    if (srcRoom == dstRoom) return true; // Already there, count as success!
    if (dstRoom->isFull()) return false;

    const Student* student = srcRoom->findResidentById(studentId);
    if (student == nullptr) return false;
    
    Student studentCopy = *student;
    srcRoom->removeResident(studentId);
    return dstRoom->addResident(studentCopy);
}

bool Dormitory::updateStudent(int studentId, const Student& updatedStudent) {
    if (studentId <= 0 || !updatedStudent.isInitialized()) return false;
    
    Room* currentRoom = findRoomByStudentID(studentId);
    if (currentRoom != nullptr) {
        currentRoom->removeResident(studentId);
        return currentRoom->addResident(updatedStudent);
    }
    return false;
}

int Dormitory::getTotalCapacity() const {
    return Room::getCapacity() * (LAST_FLOOR + 1) * LAST_NUMBER * blocks.size();
}

double Dormitory::getOccupancyRate() const {
    if (getTotalCapacity() == 0) return 0.0;
    return double(getTotalOccupancy()) / getTotalCapacity();
}

bool Dormitory::canAssignStudent(const Block& block, const Room& room, int ID) const {
    if (ID <= 0) return false;
    return !isStudentIn(ID) && !block.isFull() && !room.isFull();
}

std::vector<Student> Dormitory::getAllAssignedStudents() const {
    std::vector<Student> allStudents;
    for (const auto& block : blocks) {
        for (int i = 0; i <= LAST_FLOOR; i++) {
            for (int j = 1; j <= LAST_NUMBER; j++) {
                const Room* room = block.getRoom(i, j);
                if (room != nullptr) {
                    for (const auto& student : room->getResidents()) {
                        allStudents.push_back(student);
                    }
                }
            }
        }
    }
    return allStudents;
}

std::vector<Student> Dormitory::findStudentsByName(const std::string& name) const {
    std::vector<Student> result;
    if (name.empty()) return result;

    std::string searchName = name;
    std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
    
    for (const auto& student : getAllAssignedStudents()) {
        std::string fullName = student.getFirstName() + " " + student.getFamilyName();
        std::transform(fullName.begin(), fullName.end(), fullName.begin(), ::tolower);
        
        if (fullName.find(searchName) != std::string::npos) {
            result.push_back(student);
        }
    }
    return result;
}

std::vector<Student> Dormitory::filterStudentsByYear(int year) const {
    std::vector<Student> result;
    if (year <= 0) return result;
    
    std::vector<Student> allStudents = getAllAssignedStudents();
    
    for (const auto& student : allStudents) {
        if (student.getAcademicYear() == year) {
            result.push_back(student);
        }
    }
    
    return result;
}

std::vector<Student> Dormitory::getStudentsInBlock(const std::string& blockName) const {
    std::vector<Student> result;
    const Block* block = getBlock(blockName);
    if (block == nullptr) return result;
    
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            const Room* room = block->getRoom(i, j);
            if (room != nullptr) {
                for (const auto& student : room->getResidents()) {
                    result.push_back(student);
                }
            }
        }
    }
    return result;
}

std::vector<Student> Dormitory::getStudentsOnFloor(int floor) const {
    std::vector<Student> result;
    if (floor < 0 || floor > LAST_FLOOR) return result;
    
    for (const auto& block : blocks) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            const Room* room = block.getRoom(floor, j);
            if (room != nullptr) {
                for (const auto& student : room->getResidents()) {
                    result.push_back(student);
                }
            }
        }
    }
    return result;
}

bool Dormitory::swapStudents(int studentId1, int studentId2) {
    if (studentId1 <= 0 || studentId2 <= 0 || studentId1 == studentId2) return false;

    Room* room1 = findRoomByStudentID(studentId1);
    Room* room2 = findRoomByStudentID(studentId2);

    if (room1 == nullptr || room2 == nullptr) return false;
    if (room1 == room2) return true; // Already in the same room

    const Student* s1 = room1->findResidentById(studentId1);
    const Student* s2 = room2->findResidentById(studentId2);

    if (s1 == nullptr || s2 == nullptr) return false;

    Student copy1 = *s1;
    Student copy2 = *s2;

    room1->removeResident(studentId1);
    room2->removeResident(studentId2);

    room1->addResident(copy2);
    room2->addResident(copy1);

    return true;
}

void Dormitory::resetDormitory() {
    for (auto& block : blocks) {
        for (int i = 0; i <= LAST_FLOOR; i++) {
            for (int j = 1; j <= LAST_NUMBER; j++) {
                Room* room = block.getRoom(i, j);
                if (room != nullptr) {
                    room->clearResidents();
                }
            }
        }
    }
}

bool Dormitory::containsStudentEmail(const std::string& email) const {
    for (const auto& s : getAllAssignedStudents()) {
        if (s.getEmail() == email) return true;
    }
    return false;
}

bool Dormitory::removeAndReturnStudent(int studentId, Student& outStudent) {
    Room* room = findRoomByStudentID(studentId);
    if (room == nullptr) return false;

    const Student* s = room->findResidentById(studentId);
    if (s != nullptr) {
        outStudent = *s; // Copy data
        return room->removeResident(studentId); // Delete from room
    }
    return false;
}

std::string Dormitory::findStudentLocation(int studentId) const {
    for (const auto& block : blocks) {
        for (int f = 0; f <= LAST_FLOOR; f++) {
            for (int r = 1; r <= LAST_NUMBER; r++) {
                const Room* room = block.getRoom(f, r);
                if (room && room->isStudentIn(studentId)) {
                    return "Block " + block.getName() + ", Floor " + std::to_string(f) + ", Room " + std::to_string(r);
                }
            }
        }
    }
    return "Not Found";
}

bool Dormitory::addStudent(const Student& s) {
    for (auto& block : blocks) {
        if (!block.isFull()) {
            for (auto* room : block.getAvailableRooms()) {
                return room->addResident(s);
            }
        }
    }
    return false;
}

bool Dormitory::containsStudentPhone(const std::string& phone) const {
    for (const auto& s : getAllAssignedStudents()) {
        if (s.getPhoneNumber() == phone) return true;
    }
    return false;
}

bool Dormitory::addStudent(const Student& s) {
    // Automatically finds the first empty/available room and assigns the student
    for (auto& block : blocks) {
        for (int f = 0; f <= LAST_FLOOR; f++) {
            for (int r = 1; r <= LAST_NUMBER; r++) {
                Room* room = block.getRoom(f, r);
                if (room && !room->isFull()) {
                    return room->addResident(s);
                }
            }
        }
    }
    return false;
}