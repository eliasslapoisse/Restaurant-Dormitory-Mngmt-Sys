#include "block.h"
#include "room.h"
#include "student.h"
#include <string>
#include "constants.h"

                    // CONSTRUCTOR AND DESTRUCTOR

Block::Block(const std::string& name) : name(name), 
       rooms(LAST_FLOOR + 1, std::vector<Room>(LAST_NUMBER + 1, Room(0, 0))) {
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            rooms[i][j] = Room(i, j);
        }
    }
}

Block::~Block() {}

                    // GETTERS

const std::string& Block::getName() const {
    return name;
}

std::vector<std::vector<Room>>& Block::getRooms() {
    return rooms;
}

const Room& Block::getRoom(int floor, int number) const {
    return rooms[floor][number];
}

Room& Block::getRoom(int floor, int number) {
    return rooms[floor][number];
}

std::vector<Room*> Block::getAllRooms() {
    std::vector<Room*> result;
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            result.push_back(&rooms[i][j]);
        }
    }
    return result;
}

std::vector<Room*> Block::getRoomsByFloor(int floor) {
    std::vector<Room*> result;
    for (int j = 1; j <= LAST_NUMBER; j++) {
        result.push_back(&rooms[floor][j]);
    }
    return result;
}

std::vector<Room*> Block::getAvailableRooms() {
    std::vector<Room*> available;
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            if (!rooms[i][j].isFull()) 
            {
                available.push_back(&rooms[i][j]);
            }
        }
    }
    return available;
}

std::vector<Room*> Block::getOccupiedRooms() {
    std::vector<Room*> occupied;
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            if (rooms[i][j].isFull()) 
            {
                occupied.push_back(&rooms[i][j]);
            }
        }
    }
    return occupied;
}

int Block::getTotalOccupancy() const {
    int total = 0;
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            total += rooms[i][j].getCurrentOccupancy();
        }
    }
    return total;
}

                    // ASSIGNMENT / ROOM OPERATIONS

bool Block::addStudentToRoom(int floor, int number, const Student& student) {
    if (floor < 0 || floor > LAST_FLOOR || number < 1 || number > LAST_NUMBER) {
        return false;
    }
    return rooms[floor][number].addResident(student);
}

bool Block::removeStudentFromRoom(int studentId) {
    Room* room = findRoomByStudentID(studentId);
    if (!room) {
        return false;
    }
    return room->removeResident(studentId);
}

bool Block::removeStudentFromRoom(int floor, int number, int studentId) {
    if (floor < 0 || floor > LAST_FLOOR || number < 1 || number > LAST_NUMBER) {
        return false;
    }
    return rooms[floor][number].removeResident(studentId);
}

bool Block::moveStudent(int studentId, int destFloor, int destNumber) {
    Room* sourceRoom = findRoomByStudentID(studentId);
    if (!sourceRoom) {
        return false;
    }
    if (destFloor < 0 || destFloor > LAST_FLOOR || destNumber < 1 || destNumber > LAST_NUMBER) {
        return false;
    }
    if (sourceRoom->getFloor() == destFloor && sourceRoom->getNumber() == destNumber) {
        return false;
    }

    Room& destinationRoom = rooms[destFloor][destNumber];
    if (destinationRoom.isFull() || destinationRoom.isStudentIn(studentId)) {
        return false;
    }

    const Student* student = sourceRoom->findResidentById(studentId);
    if (!student) {
        return false;
    }

    if (!destinationRoom.addResident(*student)) {
        return false;
    }
    return sourceRoom->removeResident(studentId);
}

                    // SEARCH / FILTER

std::vector<Student> Block::getStudentsInRoom(int floor, int number) const {
    std::vector<Student> students;
    if (floor < 0 || floor > LAST_FLOOR || number < 1 || number > LAST_NUMBER) {
        return students;
    }
    const auto& residents = rooms[floor][number].getResidents();
    students.insert(students.end(), residents.begin(), residents.end());
    return students;
}

std::vector<Student> Block::getAllStudents() const {
    std::vector<Student> students;
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            const auto& residents = rooms[i][j].getResidents();
            students.insert(students.end(), residents.begin(), residents.end());
        }
    }
    return students;
}

std::vector<Student> Block::findStudentsByName(const std::string& name) const {
    std::vector<Student> results;
    for (const auto& student : getAllStudents()) {
        if (student.getFirstName().find(name) != std::string::npos ||
            student.getFamilyName().find(name) != std::string::npos) {
            results.push_back(student);
        }
    }
    return results;
}

std::vector<Student> Block::filterStudentsByYear(int year) const {
    std::vector<Student> results;
    for (const auto& student : getAllStudents()) {
        if (student.getAcademicYear() == year) {
            results.push_back(student);
        }
    }
    return results;
}

std::vector<Student> Block::filterStudentsByGender(const std::string& gender) const {
    std::vector<Student> results;
    for (const auto& student : getAllStudents()) {
        if (student.getGender() == gender) {
            results.push_back(student);
        }
    }
    return results;
}

bool Block::isRoomAvailable(int floor, int number) const {
    if (floor < 0 || floor > LAST_FLOOR || number < 1 || number > LAST_NUMBER) {
        return false;
    }
    return !rooms[floor][number].isFull();
}

bool Block::canAssignTo(int floor, int number) const {
    return isRoomAvailable(floor, number);
}

                    // METHODS

Room* Block::findRoomByStudentID(int ID) {
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            if (rooms[i][j].isStudentIn(ID))
            {
                return &rooms[i][j];
            }
        }
    }
    return nullptr;
}

bool Block::isStudentIn(int ID) const {
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            if (rooms[i][j].isStudentIn(ID))
            {
                return true;
            }
        }
    }
    return false;
}

bool Block::isFull() const {
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            if (!rooms[i][j].isFull())
            {
                return false;
            }
        }
    }
    return true;
}