    #ifndef DORMITORY_H
    #define DORMITORY_H
    #include <vector>
    #include <string>
    #include <iostream>
    #include "block.h"
    #include "restaurant.h"
    #include "student.h"

    class Dormitory {

        private:
            int number;
            std::vector<Block> blocks;
            Restaurant restaurant;

        public:
                            // CONSTRUCTOR AND DESTRUCTOR
            Dormitory(int);
            ~Dormitory();

                            // GETTERS
            int getNumber() const;
            const std::vector<Block>& getBlocks() const;
            Block* getBlock(const std::string& blockname);
            const Block* getBlock(const std::string& blockname) const;
            int getTotalOccupancy() const;
            std::vector<Block*> getAvailableBlocks();
            std::vector<Block*> getFullBlocks();
            const Restaurant& getRestaurant() const;
            Restaurant& getRestaurant();

                            // ROOM / BLOCK QUERIES
            std::vector<Room*> getAvailableRooms();
            std::vector<Room*> getFullRooms();
            std::vector<Room*> getRoomsByFloor(int floor);

                            // STUDENT SEARCH / FILTER
            Room* findRoomByStudentID(int);
            std::vector<Student> getAllAssignedStudents() const;
            std::vector<Student> findStudentsByName(const std::string& name) const;
            std::vector<Student> filterStudentsByYear(int year) const;
            std::vector<Student> getStudentsInBlock(const std::string& blockName) const;
            std::vector<Student> getStudentsOnFloor(int floor) const;


                            // ASSIGNMENT METHODS
            bool isStudentIn(int) const;
            bool isFull() const;
            bool assignStudentToRoom(const std::string& blockName, int floor, int number, const Student& student);
            bool unassignStudent(int studentId);
            bool moveStudent(int studentId, const std::string& dstBlockName, int dstFloor, int dstNumber);
            bool updateStudent(int studentId, const Student& updatedStudent);

                            // STATISTICS AND REPORTING
            int getTotalCapacity() const;
            double getOccupancyRate() const;

                            // VALIDATION METHODS
            bool canAssignStudent(const Block& block, const Room& room, int ID) const;

                            // ROOM MANAGEMENT
            bool swapStudents(int studentId1, int studentId2);
            void resetDormitory();

                            // Search Helpers for Validation
            bool containsStudentEmail(const std::string& email) const;
            bool containsStudentPhone(const std::string& phone) const;
                            // The "Hand-back" Unassign (Needed for University Pool)
            bool removeAndReturnStudent(int studentId, Student& outStudent);

                            // String Location (For the University's locateStudent function)
            std::string findStudentLocation(int studentId) const;

                            // Helper to match University expectations
            int getAvailableSpace() const { return getTotalCapacity() - getTotalOccupancy(); }
            bool addStudent(const Student& s); // Automatically finds the first empty room
    };

    #endif