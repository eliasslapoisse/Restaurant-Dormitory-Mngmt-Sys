#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

class DashboardWidget;
class StudentsWidget;
class DormitoryWidget;
class RestaurantWidget;
class MaintenanceWidget;
class InventoryWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

public slots:
    void refreshAll();

signals:
    void loggedOut();

private slots:
    void setPage(int index);
    void updateClock();
    void onSave();
    void onLoad();

private:
    void buildUi();
    void buildSidebar(QWidget* parent);
    void buildHeader(QWidget* parent);
    void setSidebarActive(int index);

    QWidget*       m_sidebar;
    QStackedWidget* m_stack;

    // Sidebar nav buttons
    QList<QPushButton*> m_navBtns;

    // Header widgets
    QLabel* m_clockLabel;
    QLabel* m_statsBar;
    QTimer* m_clockTimer;
    QTimer* m_refreshTimer;

    // Pages
    DashboardWidget*   m_dashboard;
    StudentsWidget*    m_students;
    DormitoryWidget*   m_dorm;
    RestaurantWidget*  m_restaurant;
    MaintenanceWidget* m_maintenance;
    InventoryWidget*   m_inventory;

    int m_activePage = 0;
};

#endif // MAINWINDOW_H