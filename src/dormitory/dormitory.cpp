#include "dormitory.h"
#include "student.h"
#include <string>

                    // CONSTRUCTOR AND DESTRUCTOR

Dormitory::Dormitory(int num, const std::vector<Block*>& blockPointers) : num(num) {
    blocks.reserve(blockPointers.size());
    for (Block* block : blockPointers) {
        if (block) {
            blocks.push_back(*block);
        }
    }
}

Dormitory::~Dormitory() {}

                    // GETTERS

int Dormitory::getNum() const {
    return num;
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
    int total = 0;
    for (const auto& block : blocks) {
        total += block.getTotalOccupancy();
    }
    return total;
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

std::vector<Block*> Dormitory::getOccupiedBlocks() {
    std::vector<Block*> occupied;
    for (auto& block : blocks) {
        if (block.isFull()) {
            occupied.push_back(&block);
        }
    }
    return occupied;
}

                    // ROOM / BLOCK QUERIES

std::vector<Room*> Dormitory::getAllRooms() {
    std::vector<Room*> rooms;
    for (auto& block : blocks) {
        auto blockRooms = block.getAllRooms();
        rooms.insert(rooms.end(), blockRooms.begin(), blockRooms.end());
    }
    return rooms;
}

std::vector<Room*> Dormitory::getAvailableRooms() {
    std::vector<Room*> rooms;
    for (auto& block : blocks) {
        auto available = block.getAvailableRooms();
        rooms.insert(rooms.end(), available.begin(), available.end());
    }
    return rooms;
}

std::vector<Room*> Dormitory::getOccupiedRooms() {
    std::vector<Room*> rooms;
    for (auto& block : blocks) {
        auto occupied = block.getOccupiedRooms();
        rooms.insert(rooms.end(), occupied.begin(), occupied.end());
    }
    return rooms;
}

std::vector<Room*> Dormitory::getRoomsByBlock(const std::string& blockName) {
    Block* block = getBlock(blockName);
    return block ? block->getAllRooms() : std::vector<Room*>();
}

std::vector<Room*> Dormitory::getRoomsByFloor(const std::string& blockName, int floor) {
    Block* block = getBlock(blockName);
    return block ? block->getRoomsByFloor(floor) : std::vector<Room*>();
}

                    // STUDENT SEARCH / FILTER

std::vector<Student> Dormitory::getAllAssignedStudents() const {
    std::vector<Student> students;
    for (const auto& block : blocks) {
        auto blockStudents = block.getAllStudents();
        students.insert(students.end(), blockStudents.begin(), blockStudents.end());
    }
    return students;
}

std::vector<Student> Dormitory::findStudentsByName(const std::string& name) const {
    std::vector<Student> students;
    for (const auto& block : blocks) {
        auto blockStudents = block.findStudentsByName(name);
        students.insert(students.end(), blockStudents.begin(), blockStudents.end());
    }
    return students;
}

std::vector<Student> Dormitory::filterStudentsByYear(int year) const {
    std::vector<Student> students;
    for (const auto& block : blocks) {
        auto blockStudents = block.filterStudentsByYear(year);
        students.insert(students.end(), blockStudents.begin(), blockStudents.end());
    }
    return students;
}

std::vector<Student> Dormitory::filterStudentsByGender(const std::string& gender) const {
    std::vector<Student> students;
    for (const auto& block : blocks) {
        auto blockStudents = block.filterStudentsByGender(gender);
        students.insert(students.end(), blockStudents.begin(), blockStudents.end());
    }
    return students;
}

                    // ASSIGNMENT METHODS

Room* Dormitory::findRoomByStudentID(int ID) {
    for (auto& block : blocks) {
        Room* room = block.findRoomByStudentID(ID);
        if (room) {
            return room;
        }
    }
    return nullptr;
}

bool Dormitory::isStudentIn(int ID) const {
    for (const auto& block : blocks) {
        if (block.isStudentIn(ID)) {
            return true;
        }
    }
    return false;
}

bool Dormitory::isStudentAssigned(int studentId) const {
    return isStudentIn(studentId);
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
    if (isStudentAssigned(student.getId())) {
        return false;
    }
    Block* block = getBlock(blockName);
    return block ? block->addStudentToRoom(floor, number, student) : false;
}

bool Dormitory::unassignStudent(int studentId) {
    Room* room = findRoomByStudentID(studentId);
    return room ? room->removeResident(studentId) : false;
}

bool Dormitory::moveStudent(int studentId, const std::string& dstBlockName, int dstFloor, int dstNumber) {
    Room* sourceRoom = findRoomByStudentID(studentId);
    if (!sourceRoom) {
        return false;
    }

    const Student* student = sourceRoom->findResidentById(studentId);
    if (!student) {
        return false;
    }

    Block* dstBlock = getBlock(dstBlockName);
    if (!dstBlock) {
        return false;
    }

    if (!dstBlock->addStudentToRoom(dstFloor, dstNumber, *student)) {
        return false;
    }

    return sourceRoom->removeResident(studentId);
}
