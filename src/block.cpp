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

const std::vector<std::vector<Room>>& Block::getRooms() const {
    return rooms;
}

const Room* Block::getRoom(int floor, int number) const {
    if (floor >= 0 && floor <= LAST_FLOOR && number > 0 && number <= LAST_NUMBER) {
        return &rooms[floor][number];
    }
    return nullptr;
}

Room* Block::getRoom(int floor, int number) {
    if (floor >= 0 && floor <= LAST_FLOOR && number > 0 && number <= LAST_NUMBER) {
        return &rooms[floor][number];
    }
    return nullptr;
}

std::vector<Room*> Block::getRoomsByFloor(int floor) {
    std::vector<Room*> result;
    if (floor < 0 || floor > LAST_FLOOR) {
        return result; 
    }
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

std::vector<Room*> Block::getFullRooms() {
    std::vector<Room*> full;
    for (int i = 0; i <= LAST_FLOOR; i++) {
        for (int j = 1; j <= LAST_NUMBER; j++) {
            if (rooms[i][j].isFull()) 
            {
                full.push_back(&rooms[i][j]);
            }
        }
    }
    return full;
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
    if (ID <= 0) return nullptr;
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
    if (ID <= 0) return false;
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