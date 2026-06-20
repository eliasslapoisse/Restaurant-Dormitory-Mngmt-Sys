#include "appstate.h"

AppState::AppState() {
    for (int i = 1; i <= 6; i++)
        university.addDormitory(Dormitory(i));
}

AppState& AppState::instance() {
    static AppState s;
    return s;
}

void AppState::logActivity(const QString& action, const QString& description) {
    ActivityLog log;
    log.timestamp   = QDateTime::currentDateTime();
    log.action      = action;
    log.description = description;
    activityLog.prepend(log);
    if (activityLog.size() > 50)
        activityLog.removeLast();
}

const Student* AppState::findStudentById(int id) const {
    for (const auto& s : university.getStudentPool())
        if (s.getId() == id) return &s;
    for (const auto& dorm : university.getDormitories())
        for (const auto& block : dorm.getBlocks())
            for (const auto& floorVec : block.getRooms())
                for (const auto& room : floorVec)
                    for (const auto& s : room.getResidents())
                        if (s.getId() == id) return &s;
    return nullptr;
}

Dormitory* AppState::findStudentDorm(int id) {
    for (const auto& dorm : university.getDormitories())
        if (dorm.isStudentIn(id))
            return university.getDormitory(dorm.getNumber());
    return nullptr;
}

QVector<Student> AppState::allStudents() const {
    QVector<Student> result;
    for (const auto& s : university.getStudentPool())
        result.append(s);
    for (const auto& dorm : university.getDormitories())
        for (const auto& s : dorm.getAllAssignedStudents())
            result.append(s);
    return result;
}

bool AppState::updateStudentData(int id,
                                  const std::string& fn, const std::string& ln,
                                  const std::string& email, const std::string& phone,
                                  const std::string& gender, int year)
{
    // Try assigned student first
    for (int i = 1; i <= 6; i++) {
        Dormitory* d = university.getDormitory(i);
        if (!d || !d->isStudentIn(id)) continue;
        // Build an updated student preserving ID
        int savedCount = Student::getIdCount();
        Student::setIdCount(id - 1);
        Student updated(fn, ln, email, phone, gender, year);
        Student::setIdCount(savedCount);
        if (updated.getId() != id) return false;
        return d->updateStudent(id, updated);
    }
    // Pool student
    for (const auto& s : university.getStudentPool()) {
        if (s.getId() != id) continue;
        int savedCount = Student::getIdCount();
        university.removeFromPool(id);
        Student::setIdCount(id - 1);
        Student updated(fn, ln, email, phone, gender, year);
        Student::setIdCount(savedCount);
        if (updated.getId() != id) { return false; }
        return university.enrollStudent(updated);
    }
    return false;
}

bool AppState::assignStudentToRoom(int studentId, int dormNum,
                                    const std::string& block, int floor, int roomNum)
{
    Dormitory* targetDorm = university.getDormitory(dormNum);
    if (!targetDorm) return false;

    Block* b = targetDorm->getBlock(block);
    if (!b) return false;
    Room* r = b->getRoom(floor, roomNum);
    if (!r || r->isFull()) return false;

    // Pool student?
    for (const auto& s : university.getStudentPool()) {
        if (s.getId() != studentId) continue;
        Student copy = s;
        university.removeFromPool(studentId);
        bool ok = targetDorm->assignStudentToRoom(block, floor, roomNum, copy);
        if (!ok) university.enrollStudent(copy);
        return ok;
    }

    // In some dorm?
    for (int i = 1; i <= 6; i++) {
        Dormitory* d = university.getDormitory(i);
        if (!d || !d->isStudentIn(studentId)) continue;
        if (d->getNumber() == dormNum) {
            return d->moveStudent(studentId, block, floor, roomNum);
        } else {
            Student extracted;
            if (!d->removeAndReturnStudent(studentId, extracted)) return false;
            bool ok = targetDorm->assignStudentToRoom(block, floor, roomNum, extracted);
            if (!ok) d->addStudent(extracted);
            return ok;
        }
    }
    return false;
}
