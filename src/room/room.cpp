#include "room.h"
#include "student.h"
#include <vector>
#include "constants.h"
#include <algorithm>

int Room::capacity = 2;

                    // CONSTRUCTOR AND DESTRUCTOR
Room::Room(int floor, int number) : floor(0), number(0) {
    if ((floor >= 0 && floor <= LAST_FLOOR) &&
        (number > 0  && number <= LAST_NUMBER))
    {
        this->floor = floor;
        this->number = number;
    }
}

Room::~Room() {}

                    // GETTERS
int Room::getFloor() const {
    return floor;
}

int Room::getNumber() const {
    return number;
}

const std::vector<Student>& Room::getResidents() const {
    return residents;
}

int Room::getCapacity() {
    return capacity;
}

int Room::getCurrentOccupancy() const {
    return static_cast<int>(residents.size());
}

                    // SETTERS
void Room::setCapacity(int cap) {
    if (cap > 0) {
        capacity = cap;
    }
}

                    // METHODS
bool Room::isFull() const {
    return (capacity <= residents.size());
}

bool Room::isOccupied() const {
    return (residents.size() > 0);
}

const Student* Room::findResidentById(int id) const {
    for (const auto& student : residents) {
        if (student.getId() == id) {
            return &student;
        }
    }
    return nullptr;
}

bool Room::isStudentIn(int id) const {
    return findResidentById(id) != nullptr;
}

bool Room::addResident(const Student& student) {
    if (!student.isInitialized()) {
        return false;
    }
    if (!isFull() && !isStudentIn(student.getId())) {
        residents.push_back(student);
        return true;
    }
    return false;
}

bool Room::removeResident(int id) {
    if (isOccupied()) {
        if (isStudentIn(id)) {
            residents.erase(
                remove_if(residents.begin(), residents.end(), [id](const Student& s) {
                    return s.getId() == id;
                }),
                residents.end());
            return true;
        }
    }
    return false;
}

void Room::clearResidents() {
    residents.clear();
}