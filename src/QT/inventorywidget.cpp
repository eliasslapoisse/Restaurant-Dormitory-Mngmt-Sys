#include "inventorywidget.h"
#include "datamanager.h"
#include "styles.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>

InventoryWidget::InventoryWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void InventoryWidget::buildUi() {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(24, 20, 24, 20);
    
    auto* titleLbl = new QLabel("📦  Administration & Inventory", this);
    titleLbl->setStyleSheet("font-size: 22px; font-weight: 900; color: #E8EDF5; margin-bottom: 10px;");
    lay->addWidget(titleLbl);

    auto* tabs = new QTabWidget(this);
    
    auto* invPage = new QWidget(); buildInventoryTab(invPage);
    tabs->addTab(invPage, "📦 Inventory Items");

    auto* annPage = new QWidget(); buildAnnouncementsTab(annPage);
    tabs->addTab(annPage, "📢 Announcements");

    auto* billPage = new QWidget(); buildBillingTab(billPage);
    tabs->addTab(billPage, "💳 Billing Settings");

    lay->addWidget(tabs);
    refresh();
}

void InventoryWidget::buildInventoryTab(QWidget* parent) {
    auto* lay = new QVBoxLayout(parent);
    m_invTable = new QTableWidget(0, 6, parent);
    m_invTable->setHorizontalHeaderLabels({"ID", "Item Name", "Category", "Quantity", "Min Qty", "Status"});
    m_invTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_invTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_invTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_invTable->verticalHeader()->setVisible(false);
    lay->addWidget(m_invTable);
}

void InventoryWidget::buildAnnouncementsTab(QWidget* parent) {
    auto* lay = new QVBoxLayout(parent);
    
    auto* formGroup = new QGroupBox("Post New Announcement", parent);
    auto* formLay = new QHBoxLayout(formGroup);
    m_annTitle = new QLineEdit(); m_annTitle->setPlaceholderText("Title...");
    m_annContent = new QLineEdit(); m_annContent->setPlaceholderText("Message content...");
    m_annPriority = new QComboBox(); m_annPriority->addItems({"Info", "Warning", "Urgent"});
    
    auto* postBtn = new QPushButton("Post");
    connect(postBtn, &QPushButton::clicked, this, [this](){
        if(m_annTitle->text().isEmpty()) return;
        Announcement a;
        a.title = m_annTitle->text();
        a.content = m_annContent->text();
        a.priority = m_annPriority->currentText();
        a.targetGroup = "All";
        DataManager::instance()->addAnnouncement(a);
        m_annTitle->clear(); m_annContent->clear();
        populateAnnouncements();
    });

    formLay->addWidget(m_annTitle, 1);
    formLay->addWidget(m_annContent, 2);
    formLay->addWidget(m_annPriority);
    formLay->addWidget(postBtn);
    lay->addWidget(formGroup);

    m_annTable = new QTableWidget(0, 4, parent);
    m_annTable->setHorizontalHeaderLabels({"Date", "Priority", "Title", "Content"});
    m_annTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_annTable->verticalHeader()->setVisible(false);
    lay->addWidget(m_annTable);
}

void InventoryWidget::buildBillingTab(QWidget* parent) {
    auto* lay = new QVBoxLayout(parent);
    auto* form = new QFormLayout();
    
    m_billDormCombo = new QComboBox();
    for(int i=1; i<=6; i++) m_billDormCombo->addItem(QString("Dorm %1").arg(i), i);
    
    m_billRent = new QDoubleSpinBox(); m_billRent->setRange(0, 50000); m_billRent->setSuffix(" DZD");
    m_billMeal = new QDoubleSpinBox(); m_billMeal->setRange(0, 50000); m_billMeal->setSuffix(" DZD");

    form->addRow("Select Dormitory:", m_billDormCombo);
    form->addRow("Monthly Rent Fee:", m_billRent);
    form->addRow("Meal Plan Fee:", m_billMeal);
    
    auto* saveBtn = new QPushButton("Save Billing Settings");
    connect(saveBtn, &QPushButton::clicked, this, [this](){
        BillingSettings b;
        b.dormNumber = m_billDormCombo->currentData().toInt();
        b.monthlyRent = m_billRent->value();
        b.mealPlanFee = m_billMeal->value();
        b.maintenanceFee = 250.0;
        b.mealPlanEnabled = true;
        DataManager::instance()->updateBilling(b.dormNumber, b);
        QMessageBox::information(this, "Saved", "Billing settings updated!");
    });
    
    lay->addLayout(form);
    lay->addWidget(saveBtn);
    lay->addStretch();
}

void InventoryWidget::populateInventory() {
    m_invTable->setRowCount(0);
    auto items = DataManager::instance()->inventory();
    for (const auto& it : items) {
        int r = m_invTable->rowCount();
        m_invTable->insertRow(r);
        m_invTable->setItem(r, 0, new QTableWidgetItem(QString::number(it.id)));
        m_invTable->setItem(r, 1, new QTableWidgetItem(it.name));
        m_invTable->setItem(r, 2, new QTableWidgetItem(it.category));
        m_invTable->setItem(r, 3, new QTableWidgetItem(QString::number(it.quantity)));
        m_invTable->setItem(r, 4, new QTableWidgetItem(QString::number(it.minQuantity)));
        
        auto* stat = new QTableWidgetItem(it.quantity <= it.minQuantity ? "LOW STOCK" : "OK");
        if(it.quantity <= it.minQuantity) stat->setForeground(QColor("#E74C3C"));
        m_invTable->setItem(r, 5, stat);
    }
}

void InventoryWidget::populateAnnouncements() {
    m_annTable->setRowCount(0);
    auto anns = DataManager::instance()->announcements();
    for (const auto& a : anns) {
        int r = m_annTable->rowCount();
        m_annTable->insertRow(r);
        m_annTable->setItem(r, 0, new QTableWidgetItem(a.date));
        auto* prio = new QTableWidgetItem(a.priority);
        if(a.priority == "Urgent") prio->setForeground(QColor("#E74C3C"));
        m_annTable->setItem(r, 1, prio);
        m_annTable->setItem(r, 2, new QTableWidgetItem(a.title));
        m_annTable->setItem(r, 3, new QTableWidgetItem(a.content));
    }
}

void InventoryWidget::refresh() {
    populateInventory();
    populateAnnouncements();
    // Refresh billing based on combo
    int dorm = m_billDormCombo->currentData().toInt();
    auto* b = DataManager::instance()->billingForDorm(dorm);
    if(b) {
        m_billRent->setValue(b->monthlyRent);
        m_billMeal->setValue(b->mealPlanFee);
    }
}