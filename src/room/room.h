#ifndef ROOM_H
#define ROOM_H
#include "student.h"
#include <vector>

class Room {

    private:
        int floor;
        int number;
        std::vector<Student> residents;
        static int capacity;

    public:
                    // CONSTRUCTOR AND DESTRUCTOR
        Room(int, int);
        ~Room();

                    // Setters and getters
        int getFloor() const;
        int getNumber() const;
        const std::vector<Student>& getResidents() const;
        std::vector<Student>& getResidents();
        static int getCapacity();
        int getCurrentOccupancy() const;

        static void setCapacity(int);
        void setResident(const std::vector<Student>&);

                    // METHODS
        bool isFull() const;
        bool isOccupied() const;
        bool isStudentIn(int id) const;
        bool addResident(const Student&);
        bool removeResident(int id);
        void clearResidents();
        const Student* findResidentById(int id) const;
        
};

#endif