#ifndef INVENTORYWIDGET_H
#define INVENTORYWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

class InventoryWidget : public QWidget {
    Q_OBJECT
public:
    explicit InventoryWidget(QWidget* parent = nullptr);
    void refresh();

private:
    void buildUi();
    void buildInventoryTab(QWidget* parent);
    void buildAnnouncementsTab(QWidget* parent);
    void buildBillingTab(QWidget* parent);

    void populateInventory();
    void populateAnnouncements();

    // Inventory
    QTableWidget* m_invTable;
    // Announcements
    QTableWidget* m_annTable;
    QLineEdit* m_annTitle;
    QLineEdit* m_annContent;
    QComboBox* m_annPriority;
    // Billing
    QComboBox* m_billDormCombo;
    QDoubleSpinBox* m_billRent;
    QDoubleSpinBox* m_billMeal;
};

#endif // INVENTORYWIDGET_H