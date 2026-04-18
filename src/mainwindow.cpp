#include "mainwindow.h"
#include "datamanager.h"
#include "styles.h"
#include "dashboardwidget.h"
#include "studentswidget.h"
#include "dormitorywidget.h"
#include "restaurantwidget.h"
#include "maintenancewidget.h"
#include "inventorywidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QScrollArea>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("UDRMS — Sidi Abdollah NSAI");
    resize(1440, 900);
    setMinimumSize(1200, 750);
    setStyleSheet(Styles::APP_STYLE);

    m_clockTimer   = new QTimer(this);
    m_refreshTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    connect(m_refreshTimer, &QTimer::timeout, this, &MainWindow::refreshAll);
    m_clockTimer->start(1000);
    m_refreshTimer->start(30000); // auto-refresh every 30s

    buildUi();
    setPage(0);
}

void MainWindow::buildUi() {
    auto* central = new QWidget(this);
    central->setStyleSheet("background-color: #080C14;");
    setCentralWidget(central);

    auto* rootLay = new QVBoxLayout(central);
    rootLay->setContentsMargins(0, 0, 0, 0);
    rootLay->setSpacing(0);

    // ── Top Header ──────────────────────────────────────────────────────────────
    auto* headerWidget = new QWidget(central);
    headerWidget->setFixedHeight(62);
    headerWidget->setStyleSheet(
        "background-color: #0D1117; border-bottom: 1px solid #21293A;");
    buildHeader(headerWidget);
    rootLay->addWidget(headerWidget);

    // ── Body: Sidebar + Content ─────────────────────────────────────────────────
    auto* bodyLay = new QHBoxLayout();
    bodyLay->setContentsMargins(0, 0, 0, 0);
    bodyLay->setSpacing(0);

    // Sidebar
    m_sidebar = new QWidget(central);
    m_sidebar->setFixedWidth(230);
    m_sidebar->setStyleSheet("background-color: #0A0D16; border-right: 1px solid #21293A;");
    buildSidebar(m_sidebar);
    bodyLay->addWidget(m_sidebar);

    // Content stack
    m_stack = new QStackedWidget(central);
    m_stack->setStyleSheet("background-color: #0D1117;");

    m_dashboard   = new DashboardWidget(m_stack);
    m_students    = new StudentsWidget(m_stack);
    m_dorm        = new DormitoryWidget(m_stack);
    m_restaurant  = new RestaurantWidget(m_stack);
    m_maintenance = new MaintenanceWidget(m_stack);
    m_inventory   = new InventoryWidget(m_stack);

    m_stack->addWidget(m_dashboard);
    m_stack->addWidget(m_students);
    m_stack->addWidget(m_dorm);
    m_stack->addWidget(m_restaurant);
    m_stack->addWidget(m_maintenance);
    m_stack->addWidget(m_inventory);

    bodyLay->addWidget(m_stack, 1);
    rootLay->addLayout(bodyLay, 1);

    // Status bar
    statusBar()->setStyleSheet(
        "QStatusBar { background: #080C14; color: #8896A8; border-top: 1px solid #21293A; font-size: 12px; }");
    statusBar()->showMessage("  UDRMS Ready — Sidi Abdollah National School of Artificial Intelligence");
}

void MainWindow::buildHeader(QWidget* parent) {
    auto* lay = new QHBoxLayout(parent);
    lay->setContentsMargins(20, 0, 20, 0);
    lay->setSpacing(16);

    // Logo + Title
    auto* logoArea = new QHBoxLayout();
    auto* logoIcon = new QLabel("🏛", parent);
    logoIcon->setStyleSheet("font-size: 24px; background: transparent;");
    auto* titleLbl = new QLabel("UDRMS", parent);
    titleLbl->setStyleSheet("font-size: 20px; font-weight: 900; color: #FF6B35; background: transparent; letter-spacing: 2px;");
    auto* subTitleLbl = new QLabel("  Admin Portal", parent);
    subTitleLbl->setStyleSheet("font-size: 13px; color: #8896A8; background: transparent;");
    logoArea->addWidget(logoIcon);
    logoArea->addWidget(titleLbl);
    logoArea->addWidget(subTitleLbl);
    lay->addLayout(logoArea);

    lay->addStretch();

    // Live stats bar
    m_statsBar = new QLabel("", parent);
    m_statsBar->setStyleSheet(
        "background: #161B27; border: 1px solid #21293A; border-radius: 20px;"
        " padding: 4px 14px; color: #8896A8; font-size: 12px;");
    lay->addWidget(m_statsBar);

    lay->addStretch();

    // Clock
    m_clockLabel = new QLabel("", parent);
    m_clockLabel->setStyleSheet("color: #8896A8; font-size: 12px; background: transparent;");
    updateClock();
    lay->addWidget(m_clockLabel);

    // Separator
    auto* sep = new QFrame(parent);
    sep->setFrameShape(QFrame::VLine);
    sep->setStyleSheet("background: #21293A; max-width: 1px;");
    sep->setFixedHeight(30);
    lay->addWidget(sep);

    // Save / Load
    auto* saveBtn = new QPushButton("💾 Save", parent);
    saveBtn->setProperty("class", "secondary");
    saveBtn->setFixedHeight(34);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSave);
    lay->addWidget(saveBtn);

    auto* loadBtn = new QPushButton("📂 Load", parent);
    loadBtn->setProperty("class", "secondary");
    loadBtn->setFixedHeight(34);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::onLoad);
    lay->addWidget(loadBtn);

    // Logout
    auto* logoutBtn = new QPushButton("⏏  Logout", parent);
    logoutBtn->setProperty("class", "danger");
    logoutBtn->setFixedHeight(34);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::loggedOut);
    lay->addWidget(logoutBtn);
}

void MainWindow::buildSidebar(QWidget* parent) {
    auto* lay = new QVBoxLayout(parent);
    lay->setContentsMargins(12, 20, 12, 20);
    lay->setSpacing(4);

    // Admin badge
    auto* adminBadge = new QWidget(parent);
    adminBadge->setStyleSheet(
        "background: rgba(255,107,53,0.08); border: 1px solid rgba(255,107,53,0.2);"
        " border-radius: 10px;");
    auto* badgeLay = new QHBoxLayout(adminBadge);
    badgeLay->setContentsMargins(12, 10, 12, 10);
    auto* adminIcon = new QLabel("⚙️", adminBadge);
    adminIcon->setStyleSheet("font-size: 18px; background: transparent;");
    auto* adminInfo = new QVBoxLayout();
    auto* adminName = new QLabel("Administrator", adminBadge);
    adminName->setStyleSheet("font-size: 13px; font-weight: 700; color: #FF6B35; background: transparent;");
    auto* adminRole = new QLabel("Full Access", adminBadge);
    adminRole->setStyleSheet("font-size: 11px; color: #8896A8; background: transparent;");
    adminInfo->addWidget(adminName);
    adminInfo->addWidget(adminRole);
    adminInfo->setSpacing(2);
    badgeLay->addWidget(adminIcon);
    badgeLay->addLayout(adminInfo);
    lay->addWidget(adminBadge);
    lay->addSpacing(20);

    // Section label
    auto mkSectionLabel = [&](const QString& t) {
        auto* l = new QLabel(t, parent);
        l->setStyleSheet("font-size: 10px; font-weight: 800; color: #4A5568; letter-spacing: 2px; background: transparent; padding-left: 8px;");
        return l;
    };

    lay->addWidget(mkSectionLabel("MAIN NAVIGATION"));
    lay->addSpacing(6);

    struct NavItem { QString icon; QString label; };
    QList<NavItem> navItems = {
        {"📊", "Dashboard"},
        {"🎓", "Students"},
        {"🏠", "Dormitories"},
        {"🍽️", "Restaurant"},
        {"🔧", "Maintenance"},
        {"📦", "Inventory"}
    };

    for (int i = 0; i < navItems.size(); i++) {
        auto* btn = new QPushButton(
            QString("  %1   %2").arg(navItems[i].icon).arg(navItems[i].label), parent);
        btn->setCheckable(true);
        btn->setFixedHeight(44);
        btn->setStyleSheet(
            "QPushButton {"
            "  background: transparent; color: #8896A8; border: none;"
            "  border-radius: 10px; font-size: 13px; font-weight: 600;"
            "  text-align: left; padding-left: 10px;"
            "}"
            "QPushButton:hover {"
            "  background: rgba(255,107,53,0.08); color: #E8EDF5;"
            "}"
            "QPushButton:checked {"
            "  background: rgba(255,107,53,0.15); color: #FF6B35; font-weight: 700;"
            "  border-left: 3px solid #FF6B35;"
            "}");
        int idx = i;
        connect(btn, &QPushButton::clicked, this, [this, idx]{ setPage(idx); });
        m_navBtns.append(btn);
        lay->addWidget(btn);
    }

    lay->addStretch();

    // Separator
    auto* sep = new QFrame(parent);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("background: #21293A; border: none;");
    lay->addWidget(sep);
    lay->addSpacing(10);

    // Quick stats at bottom of sidebar
    auto* statsWidget = new QWidget(parent);
    statsWidget->setStyleSheet(
        "background: #161B27; border-radius: 10px; border: 1px solid #21293A;");
    auto* statsLay = new QVBoxLayout(statsWidget);
    statsLay->setContentsMargins(14, 12, 14, 12);
    statsLay->setSpacing(6);

    auto* statsTitle = new QLabel("Quick Overview", statsWidget);
    statsTitle->setStyleSheet("font-size: 11px; font-weight: 700; color: #FF6B35; background: transparent;");
    statsLay->addWidget(statsTitle);

    auto* DM = DataManager::instance();
    auto mkStat = [&](const QString& label, const QString& val, const QString& color) {
        auto* row = new QHBoxLayout();
        auto* l = new QLabel(label, statsWidget);
        l->setStyleSheet("font-size: 11px; color: #8896A8; background: transparent;");
        auto* v = new QLabel(val, statsWidget);
        v->setStyleSheet(QString("font-size: 11px; font-weight: 700; color: %1; background: transparent;").arg(color));
        row->addWidget(l);
        row->addStretch();
        row->addWidget(v);
        statsLay->addLayout(row);
    };
    mkStat("Students", QString::number(DM->totalStudents()), "#E8EDF5");
    mkStat("Assigned", QString::number(DM->totalAssigned()), "#2ECC71");
    mkStat("Pool", QString::number(DM->totalInPool()), "#F7931E");
    mkStat("Requests", QString::number(DM->pendingMaintenanceCount()), "#E74C3C");

    lay->addWidget(statsWidget);
}

void MainWindow::setPage(int index) {
    m_stack->setCurrentIndex(index);
    m_activePage = index;
    setSidebarActive(index);
    refreshAll();
}

void MainWindow::setSidebarActive(int index) {
    for (int i = 0; i < m_navBtns.size(); i++) {
        m_navBtns[i]->setChecked(i == index);
    }
}

void MainWindow::refreshAll() {
    auto* DM = DataManager::instance();
    m_statsBar->setText(
        QString("📊 %1 Students  |  🏠 %2 Assigned  |  ⏳ %3 Pool  |  🔧 %4 Pending")
            .arg(DM->totalStudents())
            .arg(DM->totalAssigned())
            .arg(DM->totalInPool())
            .arg(DM->pendingMaintenanceCount()));

    // Refresh current page
    switch (m_activePage) {
        case 0: m_dashboard->refresh();   break;
        case 1: m_students->refresh();    break;
        case 2: m_dorm->refresh();        break;
        case 3: m_restaurant->refresh();  break;
        case 4: m_maintenance->refresh(); break;
        case 5: m_inventory->refresh();   break;
    }
}

void MainWindow::updateClock() {
    if (m_clockLabel)
        m_clockLabel->setText(QDateTime::currentDateTime().toString("ddd, MMM d  hh:mm:ss"));
}

void MainWindow::onSave() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Save Directory", ".");
    if (dir.isEmpty()) return;
    if (DataManager::instance()->saveAll(dir)) {
        statusBar()->showMessage("  ✅ Data saved successfully to " + dir, 5000);
        DataManager::instance()->logActivity("system", "Data saved to " + dir);
    } else {
        QMessageBox::critical(this, "Save Error", "Failed to save data. Check directory permissions.");
    }
}

void MainWindow::onLoad() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Data Directory", ".");
    if (dir.isEmpty()) return;
    if (DataManager::instance()->loadAll(dir)) {
        refreshAll();
        statusBar()->showMessage("  ✅ Data loaded successfully from " + dir, 5000);
    } else {
        QMessageBox::warning(this, "Load Warning", "Could not fully load data. Some files may be missing.");
    }
}