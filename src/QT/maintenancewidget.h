#ifndef MAINTENANCEWIDGET_H
#define MAINTENANCEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class MaintenanceWidget : public QWidget {
    Q_OBJECT
public:
    explicit MaintenanceWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onFilterChanged();
    void onUpdateStatus();
    void onSelectionChanged();

private:
    void buildUi();
    void populateTable();

    QTableWidget* m_table;
    QComboBox* m_filterStatus;
    QComboBox* m_filterDorm;
    
    QComboBox* m_actionStatus;
    QLineEdit* m_adminNotesInput;
    QPushButton* m_updateBtn;
    
    QLabel* m_statsLbl;
};

#endif // MAINTENANCEWIDGET_H