#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>

class DashboardWidget;
class StudentPoolWidget;
class DormitoriesWidget;
class DormDetailWidget;
class RestaurantsWidget;
class RestaurantDetailWidget;
class MaintenanceWidget;
class SettingsWidget;

class AdminMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit AdminMainWindow(QWidget* parent = nullptr);
    ~AdminMainWindow() override;

    static AdminMainWindow* instance() { return s_instance; }

    void navigateTo(int pageIndex);
    void navigateToDorm(int dormNum);
    void navigateToRestaurant(int dormNum);
    void refreshAll();

private slots:
    void onLogout();
    void onNavClicked(int index);

private:
    static AdminMainWindow* s_instance;

    QStackedWidget*        contentStack;
    QPushButton*           navBtns[6];

    DashboardWidget*         dashWidget;
    StudentPoolWidget*       poolWidget;
    DormitoriesWidget*       dormsWidget;
    DormDetailWidget*        dormDetailWidget;
    RestaurantsWidget*       restsWidget;
    RestaurantDetailWidget*  restDetailWidget;
    MaintenanceWidget*       maintWidget;
    SettingsWidget*          settingsWidget;

    // Indices in contentStack
    enum Pages {
        P_DASHBOARD = 0,
        P_POOL      = 1,
        P_DORMS     = 2,
        P_DORMDETAIL= 3,
        P_RESTS     = 4,
        P_RESTDETAIL= 5,
        P_MAINT     = 6,
        P_SETTINGS  = 7
    };

    void buildUi();
    void setActiveNavBtn(int index);
    QWidget* buildSidebar();
};
