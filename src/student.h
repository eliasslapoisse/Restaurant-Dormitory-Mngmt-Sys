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
        
                            // GETTERS 
        int getId() const;
        const std::string& getFirstName() const;
        const std::string& getFamilyName() const;
        const std::string& getEmail() const;
        const std::string& getPhoneNumber() const;
        const std::string& getGender() const;
        int getAcademicYear() const;
        static int getIdCount();
        
                            // BOOLEAN SETTERS
        bool setFirstName(const std::string&);
        bool setFamilyName(const std::string&);
        bool setEmail(const std::string&);
        bool setPhoneNumber(const std::string&);
        bool setGender(const std::string&);
        bool setAcademicYear(int);
        bool setStudent(const std::string&, const std::string&, const std::string&,
                        const std::string&, const std::string&, int);
        static void setIdCount(int highestID);
        
                            // PUBLIC VERIFIERS
        bool verifyInput(const std::string&, const std::string&, const std::string&,
                         const std::string&, const std::string&, int) const;
        bool isInitialized() const;

                            // EQUAL OPERATOR
        friend bool operator==(const Student &, const Student &);
};

#endif