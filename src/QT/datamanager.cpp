#include "datamanager.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

DataManager* DataManager::m_instance = nullptr;

DataManager* DataManager::instance() {
    if (!m_instance) m_instance = new DataManager();
    return m_instance;
}

DataManager::DataManager(QObject* parent) : QObject(parent), m_uni("Sidi Abdollah National School of Artificial Intelligence") {
    initializeDormitories();
    seedDefaultData();
}

University& DataManager::uni() { return m_uni; }
const University& DataManager::uni() const { return m_uni; }

void DataManager::initializeDormitories() {
    for (int i = 1; i <= 6; i++) {
        m_uni.addDormitory(Dormitory(i));
    }
    // Billing defaults
    for (int i = 1; i <= 6; i++) {
        BillingSettings b;
        b.dormNumber      = i;
        b.monthlyRent     = 4500.0 + (i - 1) * 500.0;
        b.mealPlanFee     = 3200.0;
        b.maintenanceFee  = 250.0;
        b.mealPlanEnabled = true;
        m_billing.append(b);
    }
}

void DataManager::seedDefaultData() {
    // Default inventory
    QStringList items = {
        "Bed Mattress|Furniture|45|10|pieces|6500",
        "Desk Lamp|Electronics|30|5|pieces|1200",
        "Cleaning Spray|Cleaning|80|20|bottles|350",
        "Mop & Bucket Set|Cleaning|15|5|sets|800",
        "Fire Extinguisher|Maintenance|24|6|units|4500",
        "First Aid Kit|Maintenance|12|3|kits|1500",
        "Trash Bags (50L)|Cleaning|200|50|packs|180",
        "LED Bulb 12W|Electronics|150|30|pieces|95",
        "Bed Sheets Set|Furniture|60|15|sets|1800",
        "Office Chair|Furniture|20|5|pieces|5500"
    };
    for (const QString& s : items) {
        QStringList p = s.split("|");
        InventoryItem item;
        item.id       = m_nextInventoryId++;
        item.name     = p[0]; item.category = p[1];
        item.quantity = p[2].toInt(); item.minQuantity = p[3].toInt();
        item.unit     = p[4]; item.unitPrice = p[5].toDouble();
        m_inventory.append(item);
    }

    // Default announcements
    Announcement a1;
    a1.id = m_nextAnnouncementId++;
    a1.title = "Welcome to ENSIA Dormitories – Academic Year 2025/2026";
    a1.content = "Dear residents, welcome to the Sidi Abdollah National School of Artificial Intelligence dormitory complex. Please visit the administration office to complete your registration process.";
    a1.date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    a1.priority = "Info"; a1.targetGroup = "All";
    m_announcements.append(a1);

    Announcement a2;
    a2.id = m_nextAnnouncementId++;
    a2.title = "Maintenance Window – Water Outage Scheduled";
    a2.content = "Scheduled maintenance on the water supply system will occur this Saturday from 08:00 to 12:00. Please prepare accordingly.";
    a2.date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    a2.priority = "Warning"; a2.targetGroup = "All";
    m_announcements.append(a2);

    Announcement a3;
    a3.id = m_nextAnnouncementId++;
    a3.title = "🍽️ New Weekly Menu Available";
    a3.content = "The updated weekly menu for all dormitory restaurants has been published. Check the Restaurant section for details.";
    a3.date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    a3.priority = "Info"; a3.targetGroup = "Students";
    m_announcements.append(a3);

    logActivity("system", "System initialized — UDRMS loaded successfully");
}

// ── Maintenance ────────────────────────────────────────────────────────────────
int DataManager::addMaintenanceRequest(const MaintenanceRequest& r) {
    MaintenanceRequest req = r;
    req.id = m_nextMaintenanceId++;
    req.dateSubmitted = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    req.status = "Pending";
    m_maintenance.append(req);
    logActivity("maintenance", QString("New %1 request from Student #%2").arg(r.category).arg(r.studentId));
    emit dataChanged();
    return req.id;
}

bool DataManager::updateMaintenanceStatus(int id, const QString& status, const QString& notes) {
    for (auto& req : m_maintenance) {
        if (req.id == id) {
            req.status = status;
            if (!notes.isEmpty()) req.adminNotes = notes;
            logActivity("maintenance", QString("Request #%1 → %2").arg(id).arg(status));
            emit dataChanged();
            return true;
        }
    }
    return false;
}

// ── Inventory ──────────────────────────────────────────────────────────────────
int DataManager::addInventoryItem(const InventoryItem& item) {
    InventoryItem it = item;
    it.id = m_nextInventoryId++;
    m_inventory.append(it);
    logActivity("system", QString("Inventory: Added %1").arg(item.name));
    emit dataChanged();
    return it.id;
}

bool DataManager::updateInventoryItem(int id, const InventoryItem& item) {
    for (auto& it : m_inventory) {
        if (it.id == id) { it = item; it.id = id; emit dataChanged(); return true; }
    }
    return false;
}

bool DataManager::removeInventoryItem(int id) {
    for (int i = 0; i < m_inventory.size(); ++i) {
        if (m_inventory[i].id == id) { m_inventory.removeAt(i); emit dataChanged(); return true; }
    }
    return false;
}

// ── Announcements ──────────────────────────────────────────────────────────────
int DataManager::addAnnouncement(const Announcement& a) {
    Announcement ann = a;
    ann.id = m_nextAnnouncementId++;
    ann.date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    m_announcements.prepend(ann);
    logActivity("system", QString("Announcement posted: %1").arg(a.title));
    emit dataChanged();
    return ann.id;
}

bool DataManager::removeAnnouncement(int id) {
    for (int i = 0; i < m_announcements.size(); ++i) {
        if (m_announcements[i].id == id) { m_announcements.removeAt(i); emit dataChanged(); return true; }
    }
    return false;
}

// ── Billing ────────────────────────────────────────────────────────────────────
BillingSettings* DataManager::billingForDorm(int dormNum) {
    for (auto& b : m_billing) {
        if (b.dormNumber == dormNum) return &b;
    }
    return nullptr;
}

void DataManager::updateBilling(int dormNum, const BillingSettings& b) {
    for (auto& bs : m_billing) {
        if (bs.dormNumber == dormNum) { bs = b; emit dataChanged(); return; }
    }
}

// ── Activity Log ───────────────────────────────────────────────────────────────
void DataManager::logActivity(const QString& type, const QString& msg) {
    ActivityLog log;
    log.timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    log.type = type; log.message = msg;
    m_activityLog.prepend(log);
    if (m_activityLog.size() > 200) m_activityLog.removeLast();
}

// ── Student credentials ────────────────────────────────────────────────────────
bool DataManager::validateStudentLogin(int studentId, const QString& password) const {
    if (m_studentPasswords.contains(studentId)) {
        return m_studentPasswords[studentId] == password;
    }
    // Default: phone number as password
    auto checkPool = [&](const std::vector<Student>& pool) -> bool {
        for (const auto& s : pool) {
            if (s.getId() == studentId) {
                return QString::fromStdString(s.getPhoneNumber()) == password;
            }
        }
        return false;
    };
    if (checkPool(m_uni.getStudentPool())) return true;
    for (const auto& dorm : m_uni.getDormitories()) {
        for (const auto& s : dorm.getAllAssignedStudents()) {
            if (s.getId() == studentId) {
                return QString::fromStdString(s.getPhoneNumber()) == password;
            }
        }
    }
    return false;
}

void DataManager::setStudentPassword(int studentId, const QString& password) {
    m_studentPasswords[studentId] = password;
}

// ── Statistics ─────────────────────────────────────────────────────────────────
int DataManager::totalStudents() const {
    return totalInPool() + totalAssigned();
}

int DataManager::totalInPool() const {
    return static_cast<int>(m_uni.getStudentPool().size());
}

int DataManager::totalAssigned() const {
    int total = 0;
    for (const auto& d : m_uni.getDormitories()) total += d.getTotalOccupancy();
    return total;
}

int DataManager::totalCapacity() const {
    return m_uni.getTotalCapacity();
}

double DataManager::overallOccupancyRate() const {
    int cap = totalCapacity();
    if (cap == 0) return 0.0;
    return (double)totalAssigned() / cap * 100.0;
}

int DataManager::pendingMaintenanceCount() const {
    int c = 0;
    for (const auto& r : m_maintenance) if (r.status == "Pending") c++;
    return c;
}

int DataManager::lowStockCount() const {
    int c = 0;
    for (const auto& i : m_inventory) if (i.quantity <= i.minQuantity) c++;
    return c;
}

int DataManager::dormOccupancy(int dormNum) const {
    for (const auto& d : m_uni.getDormitories()) {
        if (d.getNumber() == dormNum) return d.getTotalOccupancy();
    }
    return 0;
}

int DataManager::dormCapacity(int dormNum) const {
    for (const auto& d : m_uni.getDormitories()) {
        if (d.getNumber() == dormNum) return d.getTotalCapacity();
    }
    return 0;
}

// ── Persistence ────────────────────────────────────────────────────────────────
bool DataManager::saveAll(const QString& dir) {
    // Save university core data
    m_uni.saveData((dir + "/udrms_data.dat").toStdString());

    // Save extras as JSON
    QJsonObject root;

    // Maintenance
    QJsonArray maintArr;
    for (const auto& r : m_maintenance) {
        QJsonObject o;
        o["id"] = r.id; o["studentId"] = r.studentId; o["studentName"] = r.studentName;
        o["dormNumber"] = r.dormNumber; o["blockName"] = r.blockName;
        o["floor"] = r.floor; o["roomNumber"] = r.roomNumber;
        o["category"] = r.category; o["description"] = r.description;
        o["priority"] = r.priority; o["status"] = r.status;
        o["dateSubmitted"] = r.dateSubmitted; o["adminNotes"] = r.adminNotes;
        maintArr.append(o);
    }
    root["maintenance"] = maintArr;
    root["nextMaintenanceId"] = m_nextMaintenanceId;

    // Inventory
    QJsonArray invArr;
    for (const auto& it : m_inventory) {
        QJsonObject o;
        o["id"] = it.id; o["name"] = it.name; o["category"] = it.category;
        o["quantity"] = it.quantity; o["minQuantity"] = it.minQuantity;
        o["unit"] = it.unit; o["unitPrice"] = it.unitPrice;
        invArr.append(o);
    }
    root["inventory"] = invArr;
    root["nextInventoryId"] = m_nextInventoryId;

    // Announcements
    QJsonArray annArr;
    for (const auto& a : m_announcements) {
        QJsonObject o;
        o["id"] = a.id; o["title"] = a.title; o["content"] = a.content;
        o["date"] = a.date; o["priority"] = a.priority; o["targetGroup"] = a.targetGroup;
        annArr.append(o);
    }
    root["announcements"] = annArr;
    root["nextAnnouncementId"] = m_nextAnnouncementId;

    // Billing
    QJsonArray billArr;
    for (const auto& b : m_billing) {
        QJsonObject o;
        o["dormNumber"] = b.dormNumber; o["monthlyRent"] = b.monthlyRent;
        o["mealPlanFee"] = b.mealPlanFee; o["maintenanceFee"] = b.maintenanceFee;
        o["mealPlanEnabled"] = b.mealPlanEnabled;
        billArr.append(o);
    }
    root["billing"] = billArr;

    QFile f(dir + "/udrms_extras.json");
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(QJsonDocument(root).toJson());
    f.close();
    return true;
}

bool DataManager::loadAll(const QString& dir) {
    m_uni.loadData((dir + "/udrms_data.dat").toStdString());

    QFile f(dir + "/udrms_extras.json");
    if (!f.open(QIODevice::ReadOnly)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (doc.isNull()) return false;

    QJsonObject root = doc.object();

    // Maintenance
    m_maintenance.clear();
    m_nextMaintenanceId = root["nextMaintenanceId"].toInt(1);
    for (const auto& v : root["maintenance"].toArray()) {
        QJsonObject o = v.toObject();
        MaintenanceRequest r;
        r.id = o["id"].toInt(); r.studentId = o["studentId"].toInt();
        r.studentName = o["studentName"].toString();
        r.dormNumber = o["dormNumber"].toInt(); r.blockName = o["blockName"].toString();
        r.floor = o["floor"].toInt(); r.roomNumber = o["roomNumber"].toInt();
        r.category = o["category"].toString(); r.description = o["description"].toString();
        r.priority = o["priority"].toString(); r.status = o["status"].toString();
        r.dateSubmitted = o["dateSubmitted"].toString(); r.adminNotes = o["adminNotes"].toString();
        m_maintenance.append(r);
    }

    // Inventory
    m_inventory.clear();
    m_nextInventoryId = root["nextInventoryId"].toInt(1);
    for (const auto& v : root["inventory"].toArray()) {
        QJsonObject o = v.toObject();
        InventoryItem it;
        it.id = o["id"].toInt(); it.name = o["name"].toString();
        it.category = o["category"].toString(); it.quantity = o["quantity"].toInt();
        it.minQuantity = o["minQuantity"].toInt(); it.unit = o["unit"].toString();
        it.unitPrice = o["unitPrice"].toDouble();
        m_inventory.append(it);
    }

    // Announcements
    m_announcements.clear();
    m_nextAnnouncementId = root["nextAnnouncementId"].toInt(1);
    for (const auto& v : root["announcements"].toArray()) {
        QJsonObject o = v.toObject();
        Announcement a;
        a.id = o["id"].toInt(); a.title = o["title"].toString();
        a.content = o["content"].toString(); a.date = o["date"].toString();
        a.priority = o["priority"].toString(); a.targetGroup = o["targetGroup"].toString();
        m_announcements.append(a);
    }

    // Billing
    for (const auto& v : root["billing"].toArray()) {
        QJsonObject o = v.toObject();
        int dn = o["dormNumber"].toInt();
        BillingSettings b;
        b.dormNumber = dn; b.monthlyRent = o["monthlyRent"].toDouble();
        b.mealPlanFee = o["mealPlanFee"].toDouble(); b.maintenanceFee = o["maintenanceFee"].toDouble();
        b.mealPlanEnabled = o["mealPlanEnabled"].toBool();
        updateBilling(dn, b);
    }

    emit dataChanged();
    return true;
}