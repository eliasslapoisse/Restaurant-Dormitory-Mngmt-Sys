#ifndef STUDENT_H
#define STUDENT_H
#include <iostream>

class Student {

    private:
        int id;
        std::string fullName;
        std::string email;
        std::string phoneNumber;
        std::string gender;
        int academicYear;
        static int idCount;
                            // VERIFY FUNCTIONS
        bool verifyFullName(std::string) const;
        bool verifyEmail(std::string) const;
        bool verifyPhoneNumber(std::string) const;
        bool verifyAcademicYear(std::string) const;
        bool verifyGender(std::string) const;

    public:
                            // CONSTRUCTOR AND DESTRUCTOR
        Student(std::string, std::string, std::string, int, std::string);
        Student();

        ~Student();
                            // GETTERS AND SETTERS
        int getId() const;
        std::string getFullName() const;
        std::string getEmail() const;
        std::string getPhoneNumber() const;
        std::string getGender() const;
        int getAcademicYear() const;
        static int getIdCount();
        
        void setId(int);
        void setFullName(std::string);
        void setEmail(std::string);
        void setPhoneNumber(std::string);
        void setGender(std::string);
        void setAcademicYear(int);
                            // DISPLAY FUNCTION
        void display() const;
};

#endif