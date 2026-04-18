#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QDateTime>
#include <QMap>
#include "university.h"

// ── Extra Data Structures ──────────────────────────────────────────────────────

struct MaintenanceRequest {
    int    id;
    int    studentId;
    QString studentName;
    int    dormNumber;
    QString blockName;
    int    floor;
    int    roomNumber;
    QString category;   // Plumbing / Electrical / Furniture / HVAC / Cleaning / Other
    QString description;
    QString priority;   // Low / Medium / High / Urgent
    QString status;     // Pending / In Progress / Resolved / Rejected
    QString dateSubmitted;
    QString adminNotes;
};

struct InventoryItem {
    int     id;
    QString name;
    QString category;   // Cleaning / Furniture / Electronics / Maintenance / Kitchen
    int     quantity;
    int     minQuantity;
    QString unit;
    double  unitPrice;
};

struct Announcement {
    int     id;
    QString title;
    QString content;
    QString date;
    QString priority;     // Info / Warning / Urgent
    QString targetGroup;  // All / Students / Dorm1 … Dorm6
};

struct BillingSettings {
    int    dormNumber;
    double monthlyRent;
    double mealPlanFee;
    double maintenanceFee;
    bool   mealPlanEnabled;
};

struct ActivityLog {
    QString timestamp;
    QString type;     // student / dorm / restaurant / maintenance / system
    QString message;
};

// ── Singleton DataManager ──────────────────────────────────────────────────────
class DataManager : public QObject {
    Q_OBJECT

public:
    static DataManager* instance();

    // ── University core ─────────────────────────────────────────────────────────
    University&              uni();
    const University&        uni() const;

    // ── Dormitory helpers ───────────────────────────────────────────────────────
    int  dormCount()                         const { return 6; }
    void initializeDormitories();

    // ── Maintenance ─────────────────────────────────────────────────────────────
    QList<MaintenanceRequest>&       maintenance()       { return m_maintenance; }
    const QList<MaintenanceRequest>& maintenance() const { return m_maintenance; }
    int  addMaintenanceRequest(const MaintenanceRequest& r);
    bool updateMaintenanceStatus(int id, const QString& status, const QString& notes = "");

    // ── Inventory ───────────────────────────────────────────────────────────────
    QList<InventoryItem>&       inventory()       { return m_inventory; }
    const QList<InventoryItem>& inventory() const { return m_inventory; }
    int  addInventoryItem(const InventoryItem& item);
    bool updateInventoryItem(int id, const InventoryItem& item);
    bool removeInventoryItem(int id);

    // ── Announcements ───────────────────────────────────────────────────────────
    QList<Announcement>&       announcements()       { return m_announcements; }
    const QList<Announcement>& announcements() const { return m_announcements; }
    int  addAnnouncement(const Announcement& a);
    bool removeAnnouncement(int id);

    // ── Billing ─────────────────────────────────────────────────────────────────
    QList<BillingSettings>&       billing()       { return m_billing; }
    const QList<BillingSettings>& billing() const { return m_billing; }
    BillingSettings* billingForDorm(int dormNum);
    void updateBilling(int dormNum, const BillingSettings& b);

    // ── Activity Log ────────────────────────────────────────────────────────────
    const QList<ActivityLog>& activityLog() const { return m_activityLog; }
    void  logActivity(const QString& type, const QString& msg);

    // ── Student credentials (ID → password) ────────────────────────────────────
    bool validateStudentLogin(int studentId, const QString& password) const;
    void setStudentPassword(int studentId, const QString& password);

    // ── Statistics helpers ──────────────────────────────────────────────────────
    int  totalStudents()           const;
    int  totalAssigned()           const;
    int  totalInPool()             const;
    int  totalCapacity()           const;
    double overallOccupancyRate()  const;
    int  pendingMaintenanceCount() const;
    int  lowStockCount()           const;
    int  dormOccupancy(int dormNum) const;
    int  dormCapacity(int dormNum)  const;

    // ── Persistence ─────────────────────────────────────────────────────────────
    bool saveAll(const QString& dir = ".");
    bool loadAll(const QString& dir = ".");

signals:
    void dataChanged();

private:
    explicit DataManager(QObject* parent = nullptr);
    static DataManager* m_instance;

    University                m_uni;
    QList<MaintenanceRequest> m_maintenance;
    QList<InventoryItem>      m_inventory;
    QList<Announcement>       m_announcements;
    QList<BillingSettings>    m_billing;
    QList<ActivityLog>        m_activityLog;
    QMap<int, QString>        m_studentPasswords;

    int  m_nextMaintenanceId  = 1;
    int  m_nextInventoryId    = 1;
    int  m_nextAnnouncementId = 1;

    void seedDefaultData();
};

#endif // DATAMANAGER_H