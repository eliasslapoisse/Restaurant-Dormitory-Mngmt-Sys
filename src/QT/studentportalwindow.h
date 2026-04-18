#ifndef STUDENTPORTALWINDOW_H
#define STUDENTPORTALWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTabWidget>
#include <QLineEdit>
#include <QComboBox>

class StudentPortalWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit StudentPortalWindow(int studentId, QWidget* parent = nullptr);

signals:
    void loggedOut();

private slots:
    void onSubmitMaintenance();

private:
    void buildUi();
    void loadStudentData();

    int m_studentId;
    int m_dormNum = -1;
    QString m_blockName;
    int m_floor = -1;
    int m_room = -1;
    QString m_fullName;

    QLabel* m_welcomeLbl;
    QLabel* m_roomInfoLbl;
    QLabel* m_roommatesLbl;
    QLabel* m_menuLbl;

    // Maintenance Request Form
    QComboBox* m_maintCategory;
    QComboBox* m_maintPriority;
    QLineEdit* m_maintDesc;
};

#endif // STUDENTPORTALWINDOW_H