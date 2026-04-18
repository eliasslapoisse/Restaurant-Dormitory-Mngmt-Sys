#include "maintenancewidget.h"
#include "datamanager.h"
#include "styles.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>

MaintenanceWidget::MaintenanceWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void MaintenanceWidget::buildUi() {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(24, 20, 24, 20);
    lay->setSpacing(16);

    // Header
    auto* headerRow = new QHBoxLayout();
    auto* titleLbl = new QLabel("🔧  Maintenance & Complaints", this);
    titleLbl->setStyleSheet("font-size: 22px; font-weight: 900; color: #E8EDF5;");
    headerRow->addWidget(titleLbl);
    headerRow->addStretch();
    lay->addLayout(headerRow);

    // Toolbar
    auto* toolRow = new QHBoxLayout();
    toolRow->setSpacing(12);
    
    m_filterStatus = new QComboBox(this);
    m_filterStatus->addItems({"All Statuses", "Pending", "In Progress", "Resolved", "Rejected"});
    m_filterStatus->setFixedHeight(40);
    connect(m_filterStatus, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MaintenanceWidget::onFilterChanged);
    
    m_filterDorm = new QComboBox(this);
    m_filterDorm->addItem("All Dormitories");
    for (int i=1; i<=6; i++) m_filterDorm->addItem(QString("Dorm %1").arg(i), i);
    m_filterDorm->setFixedHeight(40);
    connect(m_filterDorm, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MaintenanceWidget::onFilterChanged);

    toolRow->addWidget(new QLabel("Filter:", this));
    toolRow->addWidget(m_filterStatus);
    toolRow->addWidget(m_filterDorm);
    toolRow->addStretch();
    
    m_statsLbl = new QLabel("", this);
    m_statsLbl->setStyleSheet("color: #FF6B35; font-weight: 700;");
    toolRow->addWidget(m_statsLbl);
    lay->addLayout(toolRow);

    // Table
    m_table = new QTableWidget(0, 9, this);
    m_table->setHorizontalHeaderLabels({"ID", "Date", "Student", "Location", "Category", "Priority", "Status", "Description", "Admin Notes"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, &MaintenanceWidget::onSelectionChanged);
    lay->addWidget(m_table, 1);

    // Action Panel
    auto* actionGroup = new QGroupBox("Update Request Status", this);
    auto* actionLay = new QHBoxLayout(actionGroup);
    actionLay->setSpacing(12);

    m_actionStatus = new QComboBox(actionGroup);
    m_actionStatus->addItems({"In Progress", "Resolved", "Rejected"});
    m_actionStatus->setFixedHeight(40);
    
    m_adminNotesInput = new QLineEdit(actionGroup);
    m_adminNotesInput->setPlaceholderText("Add notes or resolution details here...");
    m_adminNotesInput->setFixedHeight(40);
    
    m_updateBtn = new QPushButton("Update Request", actionGroup);
    m_updateBtn->setFixedHeight(40);
    m_updateBtn->setEnabled(false);
    connect(m_updateBtn, &QPushButton::clicked, this, &MaintenanceWidget::onUpdateStatus);

    actionLay->addWidget(new QLabel("New Status:", actionGroup));
    actionLay->addWidget(m_actionStatus);
    actionLay->addWidget(m_adminNotesInput, 1);
    actionLay->addWidget(m_updateBtn);
    lay->addWidget(actionGroup);

    refresh();
}

void MaintenanceWidget::populateTable() {
    m_table->setRowCount(0);
    QString statFilter = m_filterStatus->currentText();
    int dormFilter = m_filterDorm->currentIndex() > 0 ? m_filterDorm->currentData().toInt() : -1;

    int count = 0;
    auto reqs = DataManager::instance()->maintenance();
    for (const auto& r : reqs) {
        if (statFilter != "All Statuses" && r.status != statFilter) continue;
        if (dormFilter != -1 && r.dormNumber != dormFilter) continue;

        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(r.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(r.dateSubmitted));
        m_table->setItem(row, 2, new QTableWidgetItem(r.studentName));
        m_table->setItem(row, 3, new QTableWidgetItem(QString("Dorm %1, Blk %2, Rm %3").arg(r.dormNumber).arg(r.blockName).arg(r.roomNumber)));
        m_table->setItem(row, 4, new QTableWidgetItem(r.category));
        
        auto* prioItem = new QTableWidgetItem(r.priority);
        if (r.priority == "Urgent") prioItem->setForeground(QColor("#E74C3C"));
        m_table->setItem(row, 5, prioItem);
        
        auto* statItem = new QTableWidgetItem(r.status);
        if (r.status == "Pending") statItem->setForeground(QColor("#F7931E"));
        else if (r.status == "Resolved") statItem->setForeground(QColor("#2ECC71"));
        m_table->setItem(row, 6, statItem);
        
        m_table->setItem(row, 7, new QTableWidgetItem(r.description));
        m_table->setItem(row, 8, new QTableWidgetItem(r.adminNotes));
        m_table->setRowHeight(row, 40);
        count++;
    }
    m_statsLbl->setText(QString("%1 Requests Found").arg(count));
}

void MaintenanceWidget::onFilterChanged() { populateTable(); }

void MaintenanceWidget::onSelectionChanged() {
    m_updateBtn->setEnabled(m_table->selectedItems().count() > 0);
}

void MaintenanceWidget::onUpdateStatus() {
    if (m_table->selectedItems().isEmpty()) return;
    int id = m_table->item(m_table->currentRow(), 0)->text().toInt();
    QString status = m_actionStatus->currentText();
    QString notes = m_adminNotesInput->text();

    if (DataManager::instance()->updateMaintenanceStatus(id, status, notes)) {
        m_adminNotesInput->clear();
        refresh();
    }
}

void MaintenanceWidget::refresh() {
    populateTable();
}