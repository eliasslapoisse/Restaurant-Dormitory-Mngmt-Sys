#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

class MaintenanceWidget : public QWidget {
    Q_OBJECT
public:
    explicit MaintenanceWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onViewRow(int complaintId);
    void onDeleteRow(int complaintId);
    void onCellDoubleClicked(int row, int col);

private:
    QLineEdit*    searchEdit;
    QComboBox*    statusCombo;
    QTableWidget* table;

    void buildUi();
    void applyFilters();
    void populateTable(const QString& search, const QString& status);
    void addTableRow(int row, const struct Complaint& c);
};
