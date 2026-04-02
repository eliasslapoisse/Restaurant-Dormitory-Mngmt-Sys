#include "room.h"
#include "student.h"
#include <vector>

int Room::capacity = 2;
const int lastFloor = 5;
const int lastNumber = 48;

                    // CONSTRUCTOR AND DESTRUCTOR
Room::Room(int floor, int number) : floor(0), number(0) {
    if ((floor >= 0 && floor <= lastFloor) &&
        (number >= 0  && number <= lastNumber))
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
    return residents.size();
}

                    // SETTERS
void Room::setCapacity(int cap) {
    if (cap > 0) {
        capacity = cap;
    }
}

void Room::setResident(const std::vector<Student>& RESIDENTS) {
    if (RESIDENTS.size() <= capacity) {
        for (const auto& s : RESIDENTS) {
            if (!s.isInitialized()) return;
        }
        residents = RESIDENTS;
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

bool Room::isStudentIn(const Student& student) const {
    return findResidentById(student.getId()) != nullptr;
}

bool Room::addResident(const Student& student) {
    if (!isFull() && !isStudentIn(student)) {
        residents.emplace_back(student);
        return true;
    }
    return false;
}

bool Room::removeResident(const Student& student) {
    if (isOccupied()) {
        if (isStudentIn(student)) {
            residents.erase(
                remove(residents.begin(), residents.end(), student),
                residents.end());
            return true;
        }
    }
    return false;
}

void Room::clearResidents() {
    residents.clear();
}

void Room::swapResidents(Room& r1, Room& r2) {
    std::swap(r1.residents, r2.residents);
}