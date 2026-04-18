#ifndef UNIVERSITY_H
#define UNIVERSITY_H

#include <string>
#include <vector>
#include "dormitory.h"
#include "student.h"

class University {

    private:
        std::string name;
        std::vector<Dormitory> dormitories;
        std::vector<Student> studentPool; // Students not yet assigned to a room

    public:
                            // CONSTRUCTORS AND DESTRUCTOR
        University(const std::string& name);
        University();
        ~University();

                            // GETTERS AND SETTERS
        const std::string& getName() const;
        bool setName(const std::string& name);
        const std::vector<Dormitory>& getDormitories() const;
        Dormitory* getDormitory(int dormNumber); 
        const std::vector<Student>& getStudentPool() const;

                            // DORMITORY MANAGEMENT
        bool addDormitory(const Dormitory& dorm);
        bool removeDormitory(int dormNumber);

                            // STUDENT POOL MANAGEMENT
        bool enrollStudent(const Student& student); // Fails if email/phone is duplicate
        bool removeFromPool(int studentID);

                            // ASSIGNMENT AND LIFECYCLE
        // Moves student: Pool -> Dormitory
        bool assignStudentToDormitory(int studentID, int dormNumber); 
        
        // Moves student: Dormitory -> Pool
        bool unassignStudent(int studentID); 
        
        // Completely deletes student from the entire system
        bool expelStudent(int studentID); 

                            // VALIDATION (UNIQUENESS CHECKS)
        // These check both the pool and all dormitories
        bool isEmailDuplicate(const std::string& email) const;
        bool isPhoneDuplicate(const std::string& phone) const;

                            // STATISTICS AND SEARCH
         // int getTotalStudentCount() const;
        int getTotalCapacity() const;
        std::string locateStudent(int studentID);

                            // FILE I/O (SAVE AND LOAD)
        bool saveData(const std::string& filename) const;
        bool loadData(const std::string& filename);
};

#endif