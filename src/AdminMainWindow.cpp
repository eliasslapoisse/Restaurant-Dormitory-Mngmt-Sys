#include "AdminMainWindow.h"
#include "appstate.h"
#include "stylehelper.h"
#include "LoginDialog.h"
#include "widgets/DashboardWidget.h"
#include "widgets/StudentPoolWidget.h"
#include "widgets/DormitoriesWidget.h"
#include "widgets/DormDetailWidget.h"
#include "widgets/RestaurantsWidget.h"
#include "widgets/RestaurantDetailWidget.h"
#include "widgets/MaintenanceWidget.h"
#include "widgets/SettingsWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QApplication>
#include <QScreen>
#include <QSignalMapper>

AdminMainWindow* AdminMainWindow::s_instance = nullptr;

AdminMainWindow::AdminMainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    s_instance = this;
    setWindowTitle("UDRMS — Admin Panel");
    resize(1300, 800);

    // Center on screen
    if (auto* screen = QGuiApplication::primaryScreen()) {
        auto geo = screen->availableGeometry();
        move((geo.width() - 1300) / 2, (geo.height() - 800) / 2);
    }
    buildUi();
}

AdminMainWindow::~AdminMainWindow() {
    if (s_instance == this) s_instance = nullptr;
}

void AdminMainWindow::buildUi() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(buildSidebar());

    // Content area
    contentStack = new QStackedWidget(this);
    contentStack->setStyleSheet("QStackedWidget { background:#F8F9FA; }");

    dashWidget       = new DashboardWidget(this);
    poolWidget       = new StudentPoolWidget(this);
    dormsWidget      = new DormitoriesWidget(this);
    dormDetailWidget = new DormDetailWidget(this);
    restsWidget      = new RestaurantsWidget(this);
    restDetailWidget = new RestaurantDetailWidget(this);
    maintWidget      = new MaintenanceWidget(this);
    settingsWidget   = new SettingsWidget(this);

    contentStack->addWidget(dashWidget);       // 0
    contentStack->addWidget(poolWidget);       // 1
    contentStack->addWidget(dormsWidget);      // 2
    contentStack->addWidget(dormDetailWidget); // 3
    contentStack->addWidget(restsWidget);      // 4
    contentStack->addWidget(restDetailWidget); // 5
    contentStack->addWidget(maintWidget);      // 6
    contentStack->addWidget(settingsWidget);   // 7

    mainLayout->addWidget(contentStack, 1);

    // Connect dorm/restaurant navigation signals
    connect(dormsWidget, &DormitoriesWidget::manageDorm,
            this, &AdminMainWindow::navigateToDorm);
    connect(restsWidget, &RestaurantsWidget::manageRestaurant,
            this, &AdminMainWindow::navigateToRestaurant);
    connect(dormDetailWidget, &DormDetailWidget::backClicked,
            this, [this]{ navigateTo(P_DORMS); });
    connect(restDetailWidget, &RestaurantDetailWidget::backClicked,
            this, [this]{ navigateTo(P_RESTS); });

    navigateTo(P_DASHBOARD);
}

QWidget* AdminMainWindow::buildSidebar() {
    auto* sidebar = new QWidget(this);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(210);
    sidebar->setStyleSheet("QWidget#sidebar { background:white;"
                           "  border-right:1px solid #E9ECEF; }");

    auto* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);


    auto* header = new QFrame(sidebar);
    header->setFixedHeight(80);
    header->setStyleSheet(QString("QFrame { background:%1; }").arg(Style::gradientCSS("0","0","1","1")));
    auto* headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(20, 14, 20, 14);
    headerLayout->setSpacing(2);

    auto* titleLbl = new QLabel("UDRMS", header);
    titleLbl->setStyleSheet("QLabel { color:white; font-size:18px; font-weight:bold; background:transparent; }");
    auto* subLbl = new QLabel("Admin Panel", header);
    subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.8); font-size:11px; background:transparent; }");
    headerLayout->addWidget(titleLbl);
    headerLayout->addWidget(subLbl);

    layout->addWidget(header);


    struct NavItem { const char* icon; const char* label; int page; };
    static const NavItem items[] = {
        { "▣", "Dashboard",    P_DASHBOARD },
        { "♟", "Student Pool", P_POOL      },
        { "⌂", "Dormitories",  P_DORMS     },
        { "⚑", "Restaurants",  P_RESTS     },
        { "✦", "Maintenance",  P_MAINT     },
        { "⚙", "Settings",     P_SETTINGS  },
    };

    auto* navContainer = new QWidget(sidebar);
    navContainer->setStyleSheet("QWidget { background:transparent; }");
    auto* navLayout = new QVBoxLayout(navContainer);
    navLayout->setContentsMargins(12, 16, 12, 16);
    navLayout->setSpacing(4);

    for (int i = 0; i < 6; i++) {
        navBtns[i] = new QPushButton(
            QString("  %1  %2").arg(items[i].icon).arg(items[i].label), navContainer);
        navBtns[i]->setFixedHeight(42);
        navBtns[i]->setCursor(Qt::PointingHandCursor);
        navBtns[i]->setStyleSheet(
            "QPushButton { background:transparent; border:none; border-radius:8px;"
            "  padding:10px 14px; text-align:left; color:#495057; font-size:13px; }"
            "QPushButton:hover { background:#FFF3E0; color:#F7931E; }");
        connect(navBtns[i], &QPushButton::clicked, this, [this, i]{
            onNavClicked(i);
        });
        navLayout->addWidget(navBtns[i]);
    }
    navLayout->addStretch();
    layout->addWidget(navContainer, 1);


    auto* logoutFrame = new QFrame(sidebar);
    logoutFrame->setStyleSheet("QFrame { border-top:1px solid #E9ECEF; }");
    auto* logoutLayout = new QVBoxLayout(logoutFrame);
    logoutLayout->setContentsMargins(12, 12, 12, 12);
    auto* logoutBtn = new QPushButton("  ⇦  Logout", logoutFrame);
    logoutBtn->setFixedHeight(40);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(
        "QPushButton { background:transparent; border:none; border-radius:8px;"
        "  padding:10px 14px; text-align:left; color:#6C757D; font-size:13px; }"
        "QPushButton:hover { background:#F8D7DA; color:#DC3545; }");
    connect(logoutBtn, &QPushButton::clicked, this, &AdminMainWindow::onLogout);
    logoutLayout->addWidget(logoutBtn);
    layout->addWidget(logoutFrame);

    return sidebar;
}

void AdminMainWindow::setActiveNavBtn(int index) {
    QString activeStyle = Style::primaryBtnSS() +
        " QPushButton { text-align:left; padding:10px 14px; }";
    QString inactiveStyle =
        "QPushButton { background:transparent; border:none; border-radius:8px;"
        "  padding:10px 14px; text-align:left; color:#495057; font-size:13px; }"
        "QPushButton:hover { background:#FFF3E0; color:#F7931E; }";

    for (int i = 0; i < 6; i++)
        navBtns[i]->setStyleSheet(i == index ? activeStyle : inactiveStyle);
}

void AdminMainWindow::onNavClicked(int index) {
    // map nav index → page index
    static const int pageMap[] = {
        P_DASHBOARD, P_POOL, P_DORMS, P_RESTS, P_MAINT, P_SETTINGS
    };
    navigateTo(pageMap[index]);
    setActiveNavBtn(index);
}

void AdminMainWindow::navigateTo(int pageIndex) {
    // Determine which nav button to highlight
    static const int navMap[] = {
        0, 1, 2, -1, 3, -1, 4, 5  // page → nav btn index (-1 = sub-page)
    };
    if (pageIndex >= 0 && pageIndex < 8 && navMap[pageIndex] >= 0)
        setActiveNavBtn(navMap[pageIndex]);

    contentStack->setCurrentIndex(pageIndex);

    // Refresh the active page
    switch (pageIndex) {
    case P_DASHBOARD: dashWidget->refresh();       break;
    case P_POOL:      poolWidget->refresh();       break;
    case P_DORMS:     dormsWidget->refresh();      break;
    case P_RESTS:     restsWidget->refresh();      break;
    case P_MAINT:     maintWidget->refresh();      break;
    case P_SETTINGS:  settingsWidget->refresh();   break;
    default: break;
    }
}

void AdminMainWindow::navigateToDorm(int dormNum) {
    dormDetailWidget->loadDorm(dormNum);
    contentStack->setCurrentIndex(P_DORMDETAIL);
}

void AdminMainWindow::navigateToRestaurant(int dormNum) {
    restDetailWidget->loadRestaurant(dormNum);
    contentStack->setCurrentIndex(P_RESTDETAIL);
}

void AdminMainWindow::refreshAll() {
    dashWidget->refresh();
    poolWidget->refresh();
    dormsWidget->refresh();
    restsWidget->refresh();
    maintWidget->refresh();
    settingsWidget->refresh();
    if (contentStack->currentIndex() == P_RESTDETAIL)
        restDetailWidget->refresh();
}

void AdminMainWindow::onLogout() {
    close();
}
