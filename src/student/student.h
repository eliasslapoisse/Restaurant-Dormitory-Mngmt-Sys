#ifndef STUDENT_H
#define STUDENT_H
#include <string>

class Student {

    private:
        int id;
        std::string firstName;
        std::string familyName;
        std::string email;
        std::string phoneNumber;
        std::string gender;
        int academicYear;
        static int idCount;
        static const std::string defaultstr;
                            // VERIFY FUNCTIONS
        bool verifyName(std::string) const;
        bool verifyEmail(std::string) const;
        bool verifyPhoneNumber(std::string) const;
        bool verifyAcademicYear(int) const;
        bool verifyGender(std::string) const;
                            // ID SETTER
        void setId();

    public:
                            // CONSTRUCTOR AND DESTRUCTOR
        Student(const std::string&, const std::string&, const std::string&,
                const std::string&, const std::string&, int);
        Student();

        ~Student();
                            // GETTERS AND SETTERS
        int getId() const;
        const std::string& getFirstName() const;
        const std::string& getFamilyName() const;
        const std::string& getEmail() const;
        const std::string& getPhoneNumber() const;
        const std::string& getGender() const;
        int getAcademicYear() const;
        static int getIdCount();
        
        void setFirstName(const std::string&);
        void setFamilyName(const std::string&);
        void setEmail(const std::string&);
        void setPhoneNumber(const std::string&);
        void setGender(const std::string&);
        void setAcademicYear(int);
        void setStudent(const std::string&, const std::string&, const std::string&,
                        const std::string&, const std::string&, int);
                            // DISPLAY FUNCTION
        void display() const;
                            // PUBLIC VERIFIERS
        bool verifyInput(const std::string&, const std::string&, const std::string&,
                         const std::string&, const std::string&, int) const;
        bool isInitialized() const;
};

#endif