#include "block.h"
#include "room.h"
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