#ifndef STUDENTSWIDGET_H
#define STUDENTSWIDGET_H

#include "student.h"
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QSpinBox>

class EnrollDialog : public QDialog {
    Q_OBJECT
public:
    explicit EnrollDialog(QWidget* parent = nullptr);
    QString firstName()    const;
    QString familyName()   const;
    QString email()        const;
    QString phone()        const;
    QString gender()       const;
    int     academicYear() const;

private slots:
    void onAccept();

private:
    QLineEdit* m_firstName;
    QLineEdit* m_familyName;
    QLineEdit* m_email;
    QLineEdit* m_phone;
    QComboBox* m_gender;
    QSpinBox*  m_year;
    QLabel*    m_errorLbl;
};

class AssignDialog : public QDialog {
    Q_OBJECT
public:
    explicit AssignDialog(int studentId, QWidget* parent = nullptr);
    int     dormNumber() const;
    QString blockName()  const;
    int     floor()      const;
    int     roomNumber() const;

private:
    QComboBox* m_dorm;
    QComboBox* m_block;
    QSpinBox*  m_floor;
    QSpinBox*  m_roomNum;
};

class StudentsWidget : public QWidget {
    Q_OBJECT
public:
    explicit StudentsWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onEnroll();
    void onAssignToDorm();
    void onAssignToRoom();
    void onUnassign();
    void onExpel();
    void onLocate();
    void onSearch(const QString& text);
    void onFilterChanged();
    void onTableSelectionChanged();

private:
    void buildUi();
    void populatePool();
    void populateAssigned();
    QString studentToRowData(const Student& s, const QString& location = "");
    void showStudentDetails(int studentId);

    QTabWidget*    m_tabs;
    QTableWidget*  m_poolTable;
    QTableWidget*  m_assignedTable;
    QLineEdit*     m_searchBar;
    QComboBox*     m_filterYear;
    QLabel*        m_poolCountLbl;
    QLabel*        m_assignedCountLbl;

    QPushButton* m_enrollBtn;
    QPushButton* m_assignDormBtn;
    QPushButton* m_assignRoomBtn;
    QPushButton* m_unassignBtn;
    QPushButton* m_expelBtn;
    QPushButton* m_locateBtn;
};

#endif // STUDENTSWIDGET_H