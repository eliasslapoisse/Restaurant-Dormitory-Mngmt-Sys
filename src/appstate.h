#pragma once
#include <QDateTime>
#include <QString>
#include <QVector>
#include "university.h"

struct ActivityLog {
    QDateTime timestamp;
    QString   action;
    QString   description;
};

struct Complaint {
    int       id          = 0;
    int       studentId   = 0;
    QString   studentName;
    QDateTime dateTime;
    QString   category;
    QString   description;
    QString   status      = "Pending";
    QString   adminReply;
};

class AppState {
public:
    static AppState& instance();

    University           university;
    QVector<ActivityLog> activityLog;
    QVector<Complaint>   complaints;
    int                  complaintIdCounter = 0;

    enum Role { ADMIN, STUDENT } role = ADMIN;
    int currentStudentId = -1;

    void logActivity(const QString& action, const QString& description);

    const Student* findStudentById(int id) const;
    Dormitory*     findStudentDorm(int id);
    QVector<Student> allStudents() const;

    bool updateStudentData(int id,
                           const std::string& fn, const std::string& ln,
                           const std::string& email, const std::string& phone,
                           const std::string& gender, int year);

    bool assignStudentToRoom(int studentId, int dormNum,
                             const std::string& block, int floor, int roomNum);

private:
    AppState();
    AppState(const AppState&)            = delete;
    AppState& operator=(const AppState&) = delete;
};
