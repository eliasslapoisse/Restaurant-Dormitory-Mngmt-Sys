#ifndef DORMITORY_H
#define DORMITORY_H
#include <vector>
#include <string>
#include <iostream>
#include "block.h"

class Dormitory {

    private:
        int num;
        std::vector<Block> blocks;

    public:
                        // CONSTRUCTOR AND DESTRUCTOR
        Dormitory(int, const std::vector<Block*>&);
        ~Dormitory();

                        // GETTERS
        int getNum() const;
        const std::vector<Block>& getBlocks() const;
        Block* getBlock(const std::string& blockname);
        const Block* getBlock(const std::string& blockname) const;
        int getTotalOccupancy() const;
        std::vector<Block*> getAvailableBlocks();
        std::vector<Block*> getOccupiedBlocks();

                        // ROOM / BLOCK QUERIES
        std::vector<Room*> getAllRooms();
        std::vector<Room*> getAvailableRooms();
        std::vector<Room*> getOccupiedRooms();
        std::vector<Room*> getRoomsByBlock(const std::string& blockName);
        std::vector<Room*> getRoomsByFloor(const std::string& blockName, int floor);

                        // STUDENT SEARCH / FILTER
        std::vector<Student> getAllAssignedStudents() const;
        std::vector<Student> findStudentsByName(const std::string& name) const;
        std::vector<Student> filterStudentsByYear(int year) const;
        std::vector<Student> filterStudentsByGender(const std::string& gender) const;

                        // ASSIGNMENT METHODS
        Room* findRoomByStudentID(int);
        bool isStudentIn(int) const;
        bool isStudentAssigned(int studentId) const;
        bool isFull() const;
        bool assignStudentToRoom(const std::string& blockName, int floor, int number, const Student& student);
        bool unassignStudent(int studentId);
        bool moveStudent(int studentId, const std::string& dstBlockName, int dstFloor, int dstNumber);
};

#endif