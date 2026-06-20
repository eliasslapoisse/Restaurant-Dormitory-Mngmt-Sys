#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>

class StudentPoolWidget : public QWidget {
    Q_OBJECT
public:
    explicit StudentPoolWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onAddStudent();
    void onEditRow(int studentId);
    void onAssignRow(int studentId);
    void onExpelRow(int studentId);
    void onCellDoubleClicked(int row, int col);
    void onHeaderClicked(int col);
    void onSelectionChanged();
    void onExpelSelected();

private:
    QLineEdit*    searchEdit;
    QComboBox*    yearCombo;
    QComboBox*    genderCombo;
    QTableWidget* table;
    QLabel*       countLabel;
    QCheckBox*    selectAllCheck;
    QPushButton*  expelSelectedBtn;
    QLabel*       selectionLabel;
    QPushButton*  tabBtns[3];   // 0=All, 1=Pool, 2=Assigned

    int  currentTab = 0;
    int  sortCol    = -1;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;

    void buildUi();
    void applyFilters();
    void populateTable(const QString& search, int year, const QString& gender);
    void addTableRow(int row, const class Student& s, bool assigned);
    void updateTabStyles();
    void updateBulkBar();
};
