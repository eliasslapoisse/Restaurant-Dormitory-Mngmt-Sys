#include "studentportalwindow.h"
#include "datamanager.h"
#include "styles.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QDateTime>
#include <QFormLayout>

StudentPortalWindow::StudentPortalWindow(int studentId, QWidget* parent) 
    : QMainWindow(parent), m_studentId(studentId) {
    setWindowTitle("UDRMS — Student Portal");
    resize(900, 600);
    setStyleSheet(Styles::APP_STYLE);
    
    loadStudentData();
    buildUi();
}

void StudentPortalWindow::loadStudentData() {
    auto* DM = DataManager::instance();
    // Search dorms
    for (const auto& dorm : DM->uni().getDormitories()) {
        for(const auto& s : dorm.getAllAssignedStudents()) {
            if(s.getId() == m_studentId) {
                m_fullName = QString::fromStdString(s.getFirstName() + " " + s.getFamilyName());
                m_dormNum = dorm.getNumber();
                // We parse location string
                QString loc = QString::fromStdString(dorm.findStudentLocation(m_studentId));
                // Assuming format: "Block X, Floor Y, Room Z"
                if(loc != "Not Found") {
                    m_roomInfoLbl = new QLabel(loc);
                    // Just extracting variables for maintenance
                    QStringList parts = loc.split(",");
                    if(parts.size() == 3) {
                        m_blockName = parts[0].split(" ").last();
                        m_floor = parts[1].split(" ").last().toInt();
                        m_room = parts[2].split(" ").last().toInt();
                    }
                }
                return;
            }
        }
    }
    // Check pool if not found
    for (const auto& s : DM->uni().getStudentPool()) {
        if(s.getId() == m_studentId) {
            m_fullName = QString::fromStdString(s.getFirstName() + " " + s.getFamilyName());
            return;
        }
    }
}

void StudentPortalWindow::buildUi() {
    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* rootLay = new QVBoxLayout(central);
    rootLay->setContentsMargins(30, 30, 30, 30);
    rootLay->setSpacing(20);

    // Header
    auto* headerRow = new QHBoxLayout();
    m_welcomeLbl = new QLabel(QString("🎓 Welcome, %1!").arg(m_fullName), this);
    m_welcomeLbl->setStyleSheet("font-size: 26px; font-weight: 900; color: #FF6B35;");
    headerRow->addWidget(m_welcomeLbl);
    headerRow->addStretch();
    
    auto* logoutBtn = new QPushButton("⏏ Logout", this);
    logoutBtn->setProperty("class", "danger");
    connect(logoutBtn, &QPushButton::clicked, this, &StudentPortalWindow::loggedOut);
    headerRow->addWidget(logoutBtn);
    rootLay->addLayout(headerRow);

    auto* tabs = new QTabWidget(this);
    
    // TAB 1: My Room
    auto* roomPage = new QWidget();
    auto* roomLay = new QVBoxLayout(roomPage);
    if(m_dormNum != -1) {
        roomLay->addWidget(new QLabel(QString("You are assigned to Dormitory %1").arg(m_dormNum)));
        roomLay->addWidget(m_roomInfoLbl);
        // Find roommates
        auto* DM = DataManager::instance();
        Dormitory* d = DM->uni().getDormitory(m_dormNum);
        if(d) {
            Block* b = d->getBlock(m_blockName.toStdString());
            if(b) {
                const Room* r = b->getRoom(m_floor, m_room);
                if(r) {
                    QString rmStr = "<b>Your Roommates:</b><br>";
                    for(const auto& res : r->getResidents()) {
                        if(res.getId() != m_studentId) {
                            rmStr += QString("• %1 %2<br>").arg(QString::fromStdString(res.getFirstName())).arg(QString::fromStdString(res.getFamilyName()));
                        }
                    }
                    if(r->getCurrentOccupancy() <= 1) rmStr += "<i>No roommates currently.</i>";
                    auto* rmlbl = new QLabel(rmStr);
                    rmlbl->setStyleSheet("font-size: 14px;");
                    roomLay->addWidget(rmlbl);
                }
            }
        }
    } else {
        roomLay->addWidget(new QLabel("You are currently in the Student Pool and have not been assigned a room yet."));
    }
    roomLay->addStretch();
    tabs->addTab(roomPage, "🏠 My Room");

    // TAB 2: Maintenance
    auto* maintPage = new QWidget();
    auto* maintLay = new QVBoxLayout(maintPage);
    auto* form = new QFormLayout();
    m_maintCategory = new QComboBox();
    m_maintCategory->addItems({"Plumbing", "Electrical", "Furniture", "Heating/AC", "Cleaning", "Other"});
    m_maintPriority = new QComboBox();
    m_maintPriority->addItems({"Low", "Medium", "High", "Urgent"});
    m_maintDesc = new QLineEdit();
    
    form->addRow("Issue Category:", m_maintCategory);
    form->addRow("Priority Level:", m_maintPriority);
    form->addRow("Description:", m_maintDesc);
    
    auto* subBtn = new QPushButton("Submit Request");
    connect(subBtn, &QPushButton::clicked, this, &StudentPortalWindow::onSubmitMaintenance);
    if(m_dormNum == -1) subBtn->setEnabled(false); // Can't submit if not in a dorm

    maintLay->addWidget(new QLabel("<b>Submit a Maintenance Request for your room:</b>"));
    maintLay->addLayout(form);
    maintLay->addWidget(subBtn);
    maintLay->addStretch();
    tabs->addTab(maintPage, "🔧 Maintenance");

    // TAB 3: Restaurant Menu
    auto* menuPage = new QWidget();
    auto* menuLay = new QVBoxLayout(menuPage);
    if(m_dormNum != -1) {
        auto* DM = DataManager::instance();
        Dormitory* d = DM->uni().getDormitory(m_dormNum);
        int dayNum = QDateTime::currentDateTime().date().dayOfWeek() % 7;
        auto menu = d->getRestaurant().getDailyMenu(static_cast<Restaurant::day>(dayNum));
        
        QString mStr = QString("<h3>Today's Menu (Dorm %1)</h3>"
                               "<b>☀️ Breakfast:</b> %2<br><br>"
                               "<b>🌤️ Lunch:</b> %3<br><br>"
                               "<b>🌙 Dinner:</b> %4")
                               .arg(m_dormNum)
                               .arg(QString::fromStdString(Restaurant::getMorningDishName(menu.breakfast)))
                               .arg(QString::fromStdString(Restaurant::getDishName(menu.lunch)))
                               .arg(QString::fromStdString(Restaurant::getDishName(menu.dinner)));
        auto* ml = new QLabel(mStr);
        ml->setStyleSheet("font-size: 16px;");
        menuLay->addWidget(ml);
    } else {
        menuLay->addWidget(new QLabel("Menus are specific to dormitories. Please wait for room assignment."));
    }
    menuLay->addStretch();
    tabs->addTab(menuPage, "🍽️ Restaurant");

    rootLay->addWidget(tabs);
}

void StudentPortalWindow::onSubmitMaintenance() {
    if(m_maintDesc->text().isEmpty()) {
        QMessageBox::warning(this, "Empty Field", "Please describe the issue.");
        return;
    }
    MaintenanceRequest r;
    r.studentId = m_studentId;
    r.studentName = m_fullName;
    r.dormNumber = m_dormNum;
    r.blockName = m_blockName;
    r.floor = m_floor;
    r.roomNumber = m_room;
    r.category = m_maintCategory->currentText();
    r.priority = m_maintPriority->currentText();
    r.description = m_maintDesc->text();
    
    DataManager::instance()->addMaintenanceRequest(r);
    QMessageBox::information(this, "Success", "Maintenance request submitted to administration.");
    m_maintDesc->clear();
}