#include "dormitorywidget.h"
#include "datamanager.h"
#include "styles.h"
#include "constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPainter>
#include <QFrame>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>

// ── RoomCell ───────────────────────────────────────────────────────────────────
RoomCell::RoomCell(int floor, int number, int occ, int cap, QWidget* parent)
    : QPushButton(parent), m_floor(floor), m_number(number), m_occ(occ), m_cap(cap)
{
    setFixedSize(52, 48);
    setCursor(Qt::PointingHandCursor);
    setText(QString::number(number));
    setToolTip(QString("Floor %1, Room %2\n%3/%4 occupied").arg(floor).arg(number).arg(occ).arg(cap));
}

void RoomCell::setOccupancy(int occ) { m_occ = occ; update(); }

void RoomCell::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor bg, border, text;
    if (m_occ == 0) {
        bg = QColor("#0D1A12"); border = QColor("#2ECC71"); text = QColor("#2ECC71");
    } else if (m_occ < m_cap) {
        bg = QColor("#1A1200"); border = QColor("#F7931E"); text = QColor("#F7931E");
    } else {
        bg = QColor("#1A0A08"); border = QColor("#E74C3C"); text = QColor("#E74C3C");
    }

    p.setBrush(bg);
    p.setPen(QPen(border, 1.5));
    p.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 6, 6);

    // Occupancy dots
    for (int i = 0; i < m_cap; i++) {
        QColor dotColor = (i < m_occ) ? border : QColor(border.red(), border.green(), border.blue(), 60);
        p.setBrush(dotColor);
        p.setPen(Qt::NoPen);
        p.drawEllipse(8 + i * 12, height() - 12, 8, 8);
    }

    // Room number
    p.setPen(text);
    QFont f; f.setPointSize(9); f.setWeight(QFont::Bold);
    p.setFont(f);
    p.drawText(QRect(0, 6, width(), 20), Qt::AlignCenter, text.name() == "#e74c3c" ? "🔴" : text.name() == "#f7931e" ? "🟡" : "🟢");
    p.setPen(text);
    QFont f2; f2.setPointSize(8); f2.setWeight(QFont::DemiBold);
    p.setFont(f2);
    p.drawText(QRect(0, 22, width(), 16), Qt::AlignCenter, QString::number(m_number));
}

// ── DormitoryWidget ────────────────────────────────────────────────────────────
DormitoryWidget::DormitoryWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void DormitoryWidget::buildUi() {
    auto* rootLay = new QVBoxLayout(this);
    rootLay->setContentsMargins(24, 20, 24, 20);
    rootLay->setSpacing(16);

    // ── Header ──────────────────────────────────────────────────────────────
    auto* headerRow = new QHBoxLayout();
    auto* titleLbl = new QLabel("🏠  Dormitory & Room Management", this);
    titleLbl->setStyleSheet("font-size: 22px; font-weight: 900; color: #E8EDF5;");
    headerRow->addWidget(titleLbl);
    headerRow->addStretch();
    rootLay->addLayout(headerRow);

    // ── Controls Row ─────────────────────────────────────────────────────────
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(12);

    auto* dormLbl = new QLabel("Dormitory:", this);
    dormLbl->setStyleSheet("color: #8896A8; font-weight: 600; background: transparent;");
    ctrlRow->addWidget(dormLbl);
    m_dormSelect = new QComboBox(this);
    for (int i = 1; i <= 6; i++) m_dormSelect->addItem(QString("Dormitory %1").arg(i), i);
    m_dormSelect->setFixedHeight(40);
    connect(m_dormSelect, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DormitoryWidget::onDormChanged);
    ctrlRow->addWidget(m_dormSelect);

    auto* blockLbl = new QLabel("Block:", this);
    blockLbl->setStyleSheet("color: #8896A8; font-weight: 600; background: transparent;");
    ctrlRow->addWidget(blockLbl);
    m_blockSelect = new QComboBox(this);
    m_blockSelect->addItems({"A", "B", "C", "D", "E"});
    m_blockSelect->setFixedHeight(40);
    connect(m_blockSelect, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DormitoryWidget::onBlockChanged);
    ctrlRow->addWidget(m_blockSelect);

    auto* floorLbl = new QLabel("Floor:", this);
    floorLbl->setStyleSheet("color: #8896A8; font-weight: 600; background: transparent;");
    ctrlRow->addWidget(floorLbl);
    m_floorSelect = new QSpinBox(this);
    m_floorSelect->setRange(0, LAST_FLOOR);
    m_floorSelect->setFixedHeight(40);
    m_floorSelect->setPrefix("Floor ");
    connect(m_floorSelect, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &DormitoryWidget::onFloorChanged);
    ctrlRow->addWidget(m_floorSelect);

    ctrlRow->addStretch();

    // Legend
    struct Legend { QString color; QString label; };
    for (const auto& lg : QList<Legend>{{"#2ECC71","Available"},{"#F7931E","Partial"},{"#E74C3C","Full"}}) {
        auto* dot = new QLabel("●", this);
        dot->setStyleSheet(QString("color: %1; font-size: 14px; background: transparent;").arg(lg.color));
        auto* lbl = new QLabel(lg.label, this);
        lbl->setStyleSheet("color: #8896A8; font-size: 12px; background: transparent;");
        ctrlRow->addWidget(dot);
        ctrlRow->addWidget(lbl);
        ctrlRow->addSpacing(6);
    }
    rootLay->addLayout(ctrlRow);

    // ── Main body (splitter) ──────────────────────────────────────────────────
    auto* bodyRow = new QHBoxLayout();
    bodyRow->setSpacing(16);

    // Left: Stats panel
    auto* statsPanel = new QWidget(this);
    statsPanel->setFixedWidth(220);
    statsPanel->setStyleSheet("background: #161B27; border-radius: 12px; border: 1px solid #21293A;");
    auto* statsLay = new QVBoxLayout(statsPanel);
    statsLay->setContentsMargins(16, 16, 16, 16);
    statsLay->setSpacing(12);
    buildDormStats(statsPanel, statsLay);
    bodyRow->addWidget(statsPanel);

    // Center: Room Grid
    auto* gridPanel = new QWidget(this);
    gridPanel->setStyleSheet("background: #161B27; border-radius: 12px; border: 1px solid #21293A;");
    auto* gridPanelLay = new QVBoxLayout(gridPanel);
    gridPanelLay->setContentsMargins(16, 16, 16, 16);
    gridPanelLay->setSpacing(10);

    auto* gridTitleLbl = new QLabel("Room Grid", gridPanel);
    gridTitleLbl->setStyleSheet("font-size: 14px; font-weight: 800; color: #E8EDF5; background: transparent;");
    gridPanelLay->addWidget(gridTitleLbl);
    auto* gridSubLbl = new QLabel("Click a room to view residents and manage assignments", gridPanel);
    gridSubLbl->setStyleSheet("font-size: 11px; color: #8896A8; background: transparent;");
    gridPanelLay->addWidget(gridSubLbl);

    auto* scroll = new QScrollArea(gridPanel);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background: transparent;");
    m_roomGridWidget = new QWidget();
    m_roomGridWidget->setStyleSheet("background: transparent;");
    m_roomGrid = new QGridLayout(m_roomGridWidget);
    m_roomGrid->setSpacing(6);
    m_roomGrid->setContentsMargins(8, 8, 8, 8);
    scroll->setWidget(m_roomGridWidget);
    gridPanelLay->addWidget(scroll, 1);
    bodyRow->addWidget(gridPanel, 2);

    // Right: Room detail + actions
    auto* detailPanel = new QWidget(this);
    detailPanel->setFixedWidth(260);
    detailPanel->setStyleSheet("background: #161B27; border-radius: 12px; border: 1px solid #21293A;");
    auto* detailLay = new QVBoxLayout(detailPanel);
    detailLay->setContentsMargins(16, 16, 16, 16);
    detailLay->setSpacing(12);

    auto* detailTitleLbl = new QLabel("Room Details", detailPanel);
    detailTitleLbl->setStyleSheet("font-size: 14px; font-weight: 800; color: #E8EDF5; background: transparent;");
    detailLay->addWidget(detailTitleLbl);

    m_selectedRoomLbl = new QLabel("Click a room to view details", detailPanel);
    m_selectedRoomLbl->setStyleSheet(
        "background: #1C2333; border-radius: 8px; padding: 12px; color: #8896A8; font-size: 12px;");
    m_selectedRoomLbl->setWordWrap(true);
    detailLay->addWidget(m_selectedRoomLbl);

    // Residents table
    auto* resTitleLbl = new QLabel("Residents:", detailPanel);
    resTitleLbl->setStyleSheet("font-size: 12px; font-weight: 700; color: #8896A8; background: transparent;");
    detailLay->addWidget(resTitleLbl);

    m_residentsTable = new QTableWidget(0, 2, detailPanel);
    m_residentsTable->setHorizontalHeaderLabels({"ID", "Name"});
    m_residentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_residentsTable->setMaximumHeight(120);
    m_residentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_residentsTable->verticalHeader()->setVisible(false);
    m_residentsTable->setShowGrid(false);
    m_residentsTable->setAlternatingRowColors(true);
    detailLay->addWidget(m_residentsTable);

    detailLay->addSpacing(8);
    auto* actionsLbl = new QLabel("Actions:", detailPanel);
    actionsLbl->setStyleSheet("font-size: 12px; font-weight: 700; color: #8896A8; background: transparent;");
    detailLay->addWidget(actionsLbl);

    m_moveBtn = new QPushButton("📦 Move Student", detailPanel);
    m_moveBtn->setProperty("class", "info");
    m_moveBtn->setFixedHeight(38);
    m_moveBtn->setEnabled(false);
    connect(m_moveBtn, &QPushButton::clicked, this, &DormitoryWidget::onMoveStudent);
    detailLay->addWidget(m_moveBtn);

    m_swapBtn = new QPushButton("🔄 Swap Students", detailPanel);
    m_swapBtn->setProperty("class", "warning");
    m_swapBtn->setFixedHeight(38);
    m_swapBtn->setEnabled(false);
    connect(m_swapBtn, &QPushButton::clicked, this, &DormitoryWidget::onSwapStudents);
    detailLay->addWidget(m_swapBtn);

    m_unassignRoomBtn = new QPushButton("↩ Remove from Room", detailPanel);
    m_unassignRoomBtn->setProperty("class", "danger");
    m_unassignRoomBtn->setFixedHeight(38);
    m_unassignRoomBtn->setEnabled(false);
    connect(m_unassignRoomBtn, &QPushButton::clicked, this, &DormitoryWidget::onUnassignFromRoom);
    detailLay->addWidget(m_unassignRoomBtn);

    detailLay->addStretch();

    // Block stats
    auto* blockStatsGroup = new QGroupBox("Block Summary", detailPanel);
    auto* bsLay = new QVBoxLayout(blockStatsGroup);
    m_blockOccLbl = new QLabel("—", blockStatsGroup);
    m_blockOccLbl->setStyleSheet("color: #E8EDF5; font-size: 12px; background: transparent;");
    bsLay->addWidget(m_blockOccLbl);
    detailLay->addWidget(blockStatsGroup);

    bodyRow->addWidget(detailPanel);
    rootLay->addLayout(bodyRow, 1);

    buildRoomGrid();
    updateDormStats();
}

void DormitoryWidget::buildDormStats(QWidget* parent, QVBoxLayout* lay) {
    auto* titleLbl = new QLabel("📊 Dorm Statistics", parent);
    titleLbl->setStyleSheet("font-size: 13px; font-weight: 800; color: #E8EDF5; background: transparent;");
    lay->addWidget(titleLbl);

    auto* sep = new QFrame(parent);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("background: #21293A; border: none;");
    lay->addWidget(sep);

    m_dormNameLbl = new QLabel("Dormitory 1", parent);
    m_dormNameLbl->setStyleSheet("font-size: 14px; font-weight: 900; color: #FF6B35; background: transparent;");
    lay->addWidget(m_dormNameLbl);

    auto mkStat = [&](const QString& l, QLabel** out, const QString& color = "#E8EDF5") {
        auto* row = new QHBoxLayout();
        auto* lbl = new QLabel(l + ":", parent);
        lbl->setStyleSheet("font-size: 11px; color: #8896A8; background: transparent;");
        *out = new QLabel("—", parent);
        (*out)->setStyleSheet(QString("font-size: 12px; font-weight: 700; color: %1; background: transparent;").arg(color));
        row->addWidget(lbl);
        row->addStretch();
        row->addWidget(*out);
        lay->addLayout(row);
    };
    mkStat("Occupancy",  &m_dormOccLbl,  "#2ECC71");
    mkStat("Capacity",   &m_dormCapLbl,  "#8896A8");

    m_dormBar = new QProgressBar(parent);
    m_dormBar->setRange(0, 100);
    m_dormBar->setValue(0);
    lay->addWidget(m_dormBar);

    // Per-block stats labels (added dynamically in updateDormStats)
    lay->addSpacing(8);
    auto* blocksLbl = new QLabel("Blocks A-E Overview:", parent);
    blocksLbl->setStyleSheet("font-size: 11px; font-weight: 700; color: #8896A8; background: transparent;");
    lay->addWidget(blocksLbl);

    QStringList blockNames = {"A", "B", "C", "D", "E"};
    QStringList colors = {"#FF6B35", "#2ECC71", "#3498DB", "#9B59B6", "#F1C40F"};
    auto* DM = DataManager::instance();

    for (int bi = 0; bi < 5; bi++) {
        auto* bRow = new QHBoxLayout();
        auto* bNameLbl = new QLabel("Block " + blockNames[bi] + ":", parent);
        bNameLbl->setStyleSheet(QString("font-size: 11px; color: %1; background: transparent;").arg(colors[bi]));
        bRow->addWidget(bNameLbl);
        bRow->addStretch();

        auto* bBar = new QProgressBar(parent);
        bBar->setRange(0, 100);
        bBar->setValue(0);
        bBar->setFixedHeight(8);
        bBar->setStyleSheet(QString(
            "QProgressBar { background: #1C2333; border: none; border-radius: 4px; }"
            "QProgressBar::chunk { background: %1; border-radius: 4px; }").arg(colors[bi]));
        bRow->addWidget(bBar, 1);
        lay->addLayout(bRow);
    }

    lay->addStretch();
}

void DormitoryWidget::buildRoomGrid() {
    // Clear existing cells
    for (auto* cell : m_roomCells) { delete cell; }
    m_roomCells.clear();

    // Clear grid layout
    QLayoutItem* item;
    while ((item = m_roomGrid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    Block* block = dorm->getBlock(m_currentBlock.toStdString());
    if (!block) return;

    int floor = m_floorSelect->value();

    // Column headers (room numbers)
    int cols = 12; // rooms per row in grid
    for (int col = 0; col < cols; col++) {
        int roomStart = col * (LAST_NUMBER / cols) + 1;
        auto* colLbl = new QLabel(QString::number(roomStart) + "–" + QString::number(roomStart + LAST_NUMBER / cols - 1), m_roomGridWidget);
        colLbl->setStyleSheet("font-size: 9px; color: #4A5568; background: transparent;");
        colLbl->setAlignment(Qt::AlignCenter);
        m_roomGrid->addWidget(colLbl, 0, col);
    }

    // Room cells: display all rooms on selected floor
    for (int r = 1; r <= LAST_NUMBER; r++) {
        const Room* room = block->getRoom(floor, r);
        int occ = room ? room->getCurrentOccupancy() : 0;
        int cap = Room::getCapacity();

        auto* cell = new RoomCell(floor, r, occ, cap, m_roomGridWidget);
        int col = (r - 1) % cols;
        int row = (r - 1) / cols + 1;
        m_roomGrid->addWidget(cell, row, col);
        m_roomCells.append(cell);

        connect(cell, &RoomCell::clicked, this, [this, floor, r]() {
            onRoomCellClicked(floor, r);
        });
    }
}

void DormitoryWidget::updateDormStats() {
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    m_dormNameLbl->setText(QString("Dormitory %1").arg(m_currentDorm));
    int occ = dorm->getTotalOccupancy();
    int cap = dorm->getTotalCapacity();
    m_dormOccLbl->setText(QString("%1 students").arg(occ));
    m_dormCapLbl->setText(QString("%1 total").arg(cap));
    m_dormBar->setValue(cap > 0 ? occ * 100 / cap : 0);

    // Block summary
    Block* block = dorm->getBlock(m_currentBlock.toStdString());
    if (block) {
        int blockOcc = block->getTotalOccupancy();
        int blockCap = Room::getCapacity() * (LAST_FLOOR + 1) * LAST_NUMBER;
        m_blockOccLbl->setText(
            QString("Block %1: %2 / %3 students\n%4% occupied")
                .arg(m_currentBlock)
                .arg(blockOcc).arg(blockCap)
                .arg(blockCap > 0 ? blockOcc * 100 / blockCap : 0));
    }
}

void DormitoryWidget::onDormChanged(int) {
    m_currentDorm = m_dormSelect->currentData().toInt();
    m_selectedFloor = -1; m_selectedRoom = -1;
    m_moveBtn->setEnabled(false);
    m_swapBtn->setEnabled(false);
    m_unassignRoomBtn->setEnabled(false);
    buildRoomGrid();
    updateDormStats();
}

void DormitoryWidget::onBlockChanged(int) {
    m_currentBlock = m_blockSelect->currentText();
    buildRoomGrid();
    updateDormStats();
}

void DormitoryWidget::onFloorChanged(int floor) {
    m_currentFloor = floor;
    buildRoomGrid();
}

void DormitoryWidget::onRoomCellClicked(int floor, int number) {
    m_selectedFloor = floor;
    m_selectedRoom  = number;
    showRoomDetails(floor, number);
}

void DormitoryWidget::showRoomDetails(int floor, int number) {
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;
    Block* block = dorm->getBlock(m_currentBlock.toStdString());
    if (!block) return;
    const Room* room = block->getRoom(floor, number);
    if (!room) return;

    int occ = room->getCurrentOccupancy();
    int cap = Room::getCapacity();
    QString status = occ == 0 ? "🟢 Available" : occ < cap ? "🟡 Partially Occupied" : "🔴 Full";

    m_selectedRoomLbl->setText(
        QString("🏠 Room Details\n\n"
                "Dorm:    %1  |  Block: %2\n"
                "Floor:   %3  |  Room:  %4\n"
                "Status:  %5\n"
                "Occupancy: %6 / %7")
            .arg(m_currentDorm).arg(m_currentBlock)
            .arg(floor).arg(number)
            .arg(status).arg(occ).arg(cap));
    m_selectedRoomLbl->setStyleSheet(
        "background: #1C2333; border-radius: 8px; padding: 12px; color: #E8EDF5; font-size: 12px; border: 1px solid #FF6B35;");

    // Populate residents table
    m_residentsTable->setRowCount(0);
    for (const auto& s : room->getResidents()) {
        int row = m_residentsTable->rowCount();
        m_residentsTable->insertRow(row);
        m_residentsTable->setItem(row, 0, new QTableWidgetItem(QString::number(s.getId())));
        m_residentsTable->setItem(row, 1, new QTableWidgetItem(
            QString::fromStdString(s.getFirstName() + " " + s.getFamilyName())));
        m_residentsTable->setRowHeight(row, 36);
    }

    bool hasResidents = occ > 0;
    m_moveBtn->setEnabled(hasResidents);
    m_swapBtn->setEnabled(hasResidents);
    m_unassignRoomBtn->setEnabled(hasResidents);
}

void DormitoryWidget::onMoveStudent() {
    if (m_selectedFloor < 0 || m_selectedRoom < 0) return;
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    bool ok;
    int studentId = QInputDialog::getInt(this, "Move Student", "Enter Student ID to move:", 0, 1, 99999, 1, &ok);
    if (!ok) return;

    QString dstBlock = QInputDialog::getItem(this, "Destination Block", "Block:", {"A","B","C","D","E"}, 0, false, &ok);
    if (!ok) return;
    int dstFloor = QInputDialog::getInt(this, "Destination Floor", "Floor:", 0, 0, LAST_FLOOR, 1, &ok);
    if (!ok) return;
    int dstRoom = QInputDialog::getInt(this, "Destination Room", "Room Number:", 1, 1, LAST_NUMBER, 1, &ok);
    if (!ok) return;

    if (dorm->moveStudent(studentId, dstBlock.toStdString(), dstFloor, dstRoom)) {
        DM->logActivity("student", QString("Student #%1 moved → Block %2, Floor %3, Room %4")
            .arg(studentId).arg(dstBlock).arg(dstFloor).arg(dstRoom));
        refresh();
        QMessageBox::information(this, "Success", "✅ Student moved successfully.");
    } else {
        QMessageBox::warning(this, "Failed", "Move failed. Check that the destination room exists and is not full.");
    }
}

void DormitoryWidget::onSwapStudents() {
    if (m_selectedFloor < 0 || m_selectedRoom < 0) return;
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    bool ok;
    int id1 = QInputDialog::getInt(this, "Swap Students", "First Student ID:", 0, 1, 99999, 1, &ok);
    if (!ok) return;
    int id2 = QInputDialog::getInt(this, "Swap Students", "Second Student ID:", 0, 1, 99999, 1, &ok);
    if (!ok) return;

    if (dorm->swapStudents(id1, id2)) {
        DM->logActivity("student", QString("Students #%1 and #%2 swapped rooms").arg(id1).arg(id2));
        refresh();
        QMessageBox::information(this, "Success", "✅ Students swapped successfully.");
    } else {
        QMessageBox::warning(this, "Failed", "Swap failed. Both students must be in this dormitory.");
    }
}

void DormitoryWidget::onUnassignFromRoom() {
    if (m_selectedFloor < 0 || m_selectedRoom < 0) return;
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    Block* block = dorm->getBlock(m_currentBlock.toStdString());
    if (!block) return;
    Room* room = block->getRoom(m_selectedFloor, m_selectedRoom);
    if (!room || !room->isOccupied()) return;

    bool ok;
    int studentId = QInputDialog::getInt(this, "Remove from Room", "Student ID to remove:", 0, 1, 99999, 1, &ok);
    if (!ok) return;

    if (dorm->unassignStudent(studentId)) {
        DM->uni().unassignStudent(studentId); // Moves back to pool
        DM->logActivity("student", QString("Student #%1 removed from room → pool").arg(studentId));
        refresh();
        QMessageBox::information(this, "Success", "✅ Student removed from room and returned to pool.");
    } else {
        QMessageBox::warning(this, "Failed", "Could not remove student. Check the ID.");
    }
}

void DormitoryWidget::onShowFloorStats() { /* used for filter */ }

void DormitoryWidget::refresh() {
    buildRoomGrid();
    updateDormStats();
    if (m_selectedFloor >= 0 && m_selectedRoom >= 0) {
        showRoomDetails(m_selectedFloor, m_selectedRoom);
    }
}