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

                        // GETTERS
        const std::string& getName() const;
        std::vector<std::vector<Room>>& getRooms();
        const Room& getRoom(int, int) const;
        Room& getRoom(int, int);
        std::vector<Room*> getAllRooms();
        std::vector<Room*> getRoomsByFloor(int);
        std::vector<Room*> getAvailableRooms();
        std::vector<Room*> getOccupiedRooms();
        int getTotalOccupancy() const;

                        // ASSIGNMENT / ROOM OPERATIONS
        bool addStudentToRoom(int floor, int number, const Student& student);
        bool removeStudentFromRoom(int studentId);
        bool removeStudentFromRoom(int floor, int number, int studentId);
        bool moveStudent(int studentId, int destFloor, int destNumber);

                        // SEARCH / FILTER
        std::vector<Student> getStudentsInRoom(int floor, int number) const;
        std::vector<Student> getAllStudents() const;
        std::vector<Student> findStudentsByName(const std::string& name) const;
        std::vector<Student> filterStudentsByYear(int year) const;
        std::vector<Student> filterStudentsByGender(const std::string& gender) const;
        bool isRoomAvailable(int floor, int number) const;
        bool canAssignTo(int floor, int number) const;

                        // METHODS
        Room* findRoomByStudentID(int);
        bool isStudentIn(int) const;
        bool isFull() const;
};

#endif