#ifndef BLOCK_H
#define BLOCK_H
#include <string>
#include <vector>
#include "room.h"

class Block {

    private:
        std::string name;
        std::vector<std::vector<Room>> rooms;
    
    public:
                        // CONSTRUCTOR AND DESTRUCTOR
        Block(const std::string&);
        ~Block();

                        // GETTERS AND SETTERS
        const std::string& getName() const;
        const std::vector<std::vector<Room>>& getRooms() const;
        const Room& getRoom(int, int) const;
        Room& getRoom(int, int);

        // METHODS
        const Room* findRoomByStudentID(int) const;
        bool isStudentIn(int) const;
        bool isFull() const;
        std::vector<const Room *> getAvailableRooms() const;
        int getTotalOccupancy() const;
};

#endif