#include "DormDetailWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../constants.h"
#include "../AdminMainWindow.h"
#include "../dialogs/StudentPickerDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QProgressBar>
#include <QDialog>
#include <QMessageBox>

// ═══════════════════════════════════════════════════════════════
// RoomSquare
// ═══════════════════════════════════════════════════════════════

RoomSquare::RoomSquare(const std::string& blk, int fl, int num, QWidget* parent)
    : QFrame(parent), blockName(blk), floor(fl), roomNum(num)
{
    setObjectName("roomSq");
    setFixedSize(68, 56);
    setCursor(Qt::PointingHandCursor);
    refreshColor();
}

void RoomSquare::refreshColor() {
    if (dormNum <= 0) return;
    Dormitory* dorm = AppState::instance().university.getDormitory(dormNum);
    if (!dorm) return;
    Block* b = dorm->getBlock(blockName);
    if (!b) return;
    Room* r = b->getRoom(floor, roomNum);
    if (!r) return;

    int occ = r->getCurrentOccupancy();
    int cap = Room::getCapacity();

    QString bg, border, text;
    if (occ == 0)         { bg="#D1FAE5"; border="#6EE7B7"; text="#065F46"; }
    else if (occ < cap)   { bg="#FEF3C7"; border="#FCD34D"; text="#92400E"; }
    else                  { bg="#FEE2E2"; border="#FCA5A5"; text="#991B1B"; }

    setStyleSheet(QString(
        "QFrame#roomSq { background:%1; border:1.5px solid %2; border-radius:8px; }"
        "QFrame#roomSq:hover { border:2px solid %2; }"
        "QLabel { border:none; background:transparent; }")
        .arg(bg, border));

    auto* lbl = findChild<QLabel*>("roomLabel");
    if (!lbl) {
        lbl = new QLabel(QString::number(roomNum), this);
        lbl->setObjectName("roomLabel");
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setGeometry(0, 4, 68, 28);
    }
    lbl->setStyleSheet(QString("QLabel { color:%1; font-size:12px; font-weight:bold;"
                               "  background:transparent; }").arg(text));
    lbl->setText(QString::number(roomNum));

    auto* occLbl = findChild<QLabel*>("occLabel");
    if (!occLbl) {
        occLbl = new QLabel("", this);
        occLbl->setObjectName("occLabel");
        occLbl->setAlignment(Qt::AlignCenter);
        occLbl->setGeometry(0, 32, 68, 18);
    }
    occLbl->setStyleSheet(QString("QLabel { color:%1; font-size:9px; background:transparent; }").arg(text));
    occLbl->setText(QString("%1/%2").arg(occ).arg(cap));
}

void RoomSquare::mousePressEvent(QMouseEvent*) {
    emit clicked(blockName, floor, roomNum);
}

void RoomSquare::enterEvent(QEnterEvent*) { setStyleSheet(styleSheet()); }
void RoomSquare::leaveEvent(QEvent*)      { refreshColor(); }

// ═══════════════════════════════════════════════════════════════
// RoomInfoPanel
// ═══════════════════════════════════════════════════════════════

RoomInfoPanel::RoomInfoPanel(QWidget* parent) : QFrame(parent) {
    setObjectName("roomInfoPanel");
    setStyleSheet("QFrame#roomInfoPanel { background:white; border-left:1px solid #E9ECEF; }");
    setMinimumWidth(0);
    setMaximumWidth(0);
    buildUi();

    anim = new QPropertyAnimation(this, "maximumWidth");
    anim->setDuration(200);
}

void RoomInfoPanel::buildUi() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(0);

    // ── Header ────────────────────────────────────────────────
    auto* detailsTag = new QLabel("ROOM DETAILS", this);
    detailsTag->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF;"
                              "  letter-spacing:1.5px; background:transparent; }");

    auto* headerRow = new QHBoxLayout;
    titleLabel = new QLabel("Room", this);
    titleLabel->setStyleSheet("QLabel { font-size:15px; font-weight:bold; color:#111827; background:transparent; }");
    titleLabel->setWordWrap(true);
    auto* closeBtn = new QPushButton("✕", this);
    closeBtn->setFixedSize(28, 28);
    closeBtn->setStyleSheet(
        "QPushButton { background:#F3F4F6; border:none; border-radius:14px; color:#6B7280; font-size:11px; }"
        "QPushButton:hover { background:#E5E7EB; color:#111827; }");
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &RoomInfoPanel::close);
    headerRow->addWidget(titleLabel, 1);
    headerRow->addWidget(closeBtn);

    layout->addWidget(detailsTag);
    layout->addSpacing(6);
    layout->addLayout(headerRow);
    layout->addSpacing(20);

    // ── Separator ────────────────────────────────────────────
    auto* sep1 = new QFrame(this);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setFixedHeight(1);
    sep1->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    layout->addWidget(sep1);
    layout->addSpacing(16);

    // ── Capacity section ──────────────────────────────────────
    auto* capTitle = new QLabel("Capacity", this);
    capTitle->setStyleSheet("QLabel { font-size:13px; font-weight:bold; color:#111827; background:transparent; }");
    layout->addWidget(capTitle);
    layout->addSpacing(10);

    // Bed icons container
    capacityIconRow = new QWidget(this);
    capacityIconRow->setStyleSheet("QWidget { background:transparent; }");
    capacityIconLayout = new QHBoxLayout(capacityIconRow);
    capacityIconLayout->setContentsMargins(0,0,0,0);
    capacityIconLayout->setSpacing(8);
    layout->addWidget(capacityIconRow);
    layout->addSpacing(6);

    capacityOccLabel = new QLabel("0/2 occupied", this);
    capacityOccLabel->setStyleSheet("QLabel { font-size:12px; color:#9CA3AF; background:transparent; }");
    layout->addWidget(capacityOccLabel);
    layout->addSpacing(20);

    // ── Separator ────────────────────────────────────────────
    auto* sep2 = new QFrame(this);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setFixedHeight(1);
    sep2->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    layout->addWidget(sep2);
    layout->addSpacing(16);

    // ── Residents section ─────────────────────────────────────
    residentsContainer = new QWidget(this);
    residentsContainer->setStyleSheet("QWidget { background:transparent; }");
    auto* resLayout = new QVBoxLayout(residentsContainer);
    resLayout->setContentsMargins(0, 0, 0, 0);
    resLayout->setSpacing(8);

    auto* resTitle = new QLabel("Residents", residentsContainer);
    resTitle->setStyleSheet("QLabel { font-size:13px; font-weight:bold; color:#111827; background:transparent; }");
    resLayout->addWidget(resTitle);

    residentList = new QWidget(residentsContainer);
    residentList->setStyleSheet("QWidget { background:transparent; }");
    resLayout->addWidget(residentList);
    layout->addWidget(residentsContainer);

    layout->addStretch();

    // ── Add Student button ────────────────────────────────────
    addBtn = new QPushButton("+ Add Student", this);
    addBtn->setFixedHeight(46);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(
        QString("QPushButton { background:%1; color:white; border:none; border-radius:10px;"
                "  font-size:14px; font-weight:bold; }"
                "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "  stop:0 #E65C2A, stop:1 #E8A522); }"
                "QPushButton:disabled { background:#D1D5DB; color:#9CA3AF; }").arg(Style::gradientCSS()));
    connect(addBtn, &QPushButton::clicked, this, &RoomInfoPanel::onAddStudent);

    clearBtn = new QPushButton("Remove All", this);
    clearBtn->setFixedHeight(32);
    clearBtn->setCursor(Qt::PointingHandCursor);
    clearBtn->setStyleSheet(
        "QPushButton { background:transparent; border:1px solid #FCA5A5; color:#EF4444;"
        "  border-radius:8px; font-size:12px; }"
        "QPushButton:hover { background:#FEF2F2; }"
        "QPushButton:disabled { color:#D1D5DB; border-color:#E5E7EB; }");
    connect(clearBtn, &QPushButton::clicked, this, &RoomInfoPanel::onClearRoom);

    layout->addWidget(addBtn);
    layout->addSpacing(8);
    layout->addWidget(clearBtn);
}

void RoomInfoPanel::loadRoom(int dormNum, const std::string& block, int floor, int roomNum) {
    m_dormNum = dormNum;
    m_block   = block;
    m_floor   = floor;
    m_roomNum = roomNum;

    titleLabel->setText(
        QString("Block %1  ·  Floor %2  ·  Room %3")
        .arg(QString::fromStdString(block)).arg(floor).arg(roomNum));

    refreshResidents();

    anim->stop();
    anim->setStartValue(maximumWidth());
    anim->setEndValue(300);
    anim->start();
}

void RoomInfoPanel::close() {
    anim->stop();
    anim->setStartValue(maximumWidth());
    anim->setEndValue(0);
    anim->start();
}

void RoomInfoPanel::refreshResidents() {
    delete residentList->layout();
    qDeleteAll(residentList->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));

    auto* rll = new QVBoxLayout(residentList);
    rll->setContentsMargins(0, 0, 0, 0);
    rll->setSpacing(8);

    Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
    if (!dorm) return;
    Block* b = dorm->getBlock(m_block);
    if (!b) return;
    Room* r = b->getRoom(m_floor, m_roomNum);
    if (!r) return;

    int cap = Room::getCapacity();
    int occ = r->getCurrentOccupancy();

    // Update capacity display
    capacityOccLabel->setText(QString("%1/%2 occupied").arg(occ).arg(cap));

    // Rebuild bed icons
    while (QLayoutItem* item = capacityIconLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    for (int i = 0; i < cap; i++) {
        auto* bedIcon = new QLabel("🛏", capacityIconRow);
        bedIcon->setFixedSize(36, 36);
        bedIcon->setAlignment(Qt::AlignCenter);
        bedIcon->setStyleSheet(
            QString("QLabel { background:%1; border-radius:8px; font-size:16px; }")
            .arg(i < occ ? "#FEF3C7" : "#F3F4F6"));
        capacityIconLayout->addWidget(bedIcon);
    }
    capacityIconLayout->addStretch();

    if (r->getResidents().empty()) {
        auto* emptyLbl = new QLabel("No residents", residentList);
        emptyLbl->setStyleSheet("QLabel { color:#9CA3AF; font-size:13px;"
                                "  font-style:italic; background:transparent; }");
        rll->addWidget(emptyLbl);
    }

    for (const auto& s : r->getResidents()) {
        auto* card = new QFrame(residentList);
        card->setObjectName("resCard");
        card->setStyleSheet("QFrame#resCard { background:#F9FAFB; border-radius:10px; border:1px solid #E5E7EB; }"
                            "QLabel { border:none; background:transparent; }");
        auto* cl = new QHBoxLayout(card);
        cl->setContentsMargins(10, 8, 10, 8);
        cl->setSpacing(10);

        auto* avatar = new QLabel(
            QString::fromStdString(s.getFirstName()).left(1).toUpper(), card);
        avatar->setFixedSize(36, 36);
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setStyleSheet(
            QString("QLabel { background:%1; border-radius:18px; color:white;"
                    "  font-weight:bold; font-size:14px; }").arg(Style::gradientCSS()));

        auto* infoW = new QWidget(card);
        infoW->setStyleSheet("QWidget { background:transparent; }");
        auto* infoL = new QVBoxLayout(infoW);
        infoL->setContentsMargins(0,0,0,0); infoL->setSpacing(1);

        auto* nameLbl = new QLabel(
            QString::fromStdString(s.getFirstName() + " " + s.getFamilyName()), card);
        nameLbl->setStyleSheet("QLabel { font-size:12px; font-weight:bold; color:#111827;"
                               "  background:transparent; }");
        auto* detLbl = new QLabel(
            QString("ID %1  ·  Year %2").arg(s.getId()).arg(s.getAcademicYear()), card);
        detLbl->setStyleSheet("QLabel { font-size:11px; color:#9CA3AF; background:transparent; }");
        infoL->addWidget(nameLbl);
        infoL->addWidget(detLbl);

        auto* removeBtn = new QPushButton("✕", card);
        removeBtn->setFixedSize(24, 24);
        removeBtn->setStyleSheet(
            "QPushButton { background:#FEE2E2; border:none; border-radius:12px; color:#EF4444; font-size:10px; }"
            "QPushButton:hover { background:#EF4444; color:white; }");
        removeBtn->setCursor(Qt::PointingHandCursor);
        int sid = s.getId();
        connect(removeBtn, &QPushButton::clicked, this, [this, sid]{ onRemoveStudent(sid); });

        cl->addWidget(avatar);
        cl->addWidget(infoW, 1);
        cl->addWidget(removeBtn);
        rll->addWidget(card);
    }

    addBtn->setEnabled(occ < cap);
    clearBtn->setEnabled(occ > 0);
}

void RoomInfoPanel::onAddStudent() {
    StudentPickerDialog d(false, this);
    if (d.exec() != QDialog::Accepted) return;
    int pickedId = d.selectedStudentId();
    if (pickedId < 0) return;

    if (AppState::instance().assignStudentToRoom(
            pickedId, m_dormNum, m_block, m_floor, m_roomNum)) {
        const Student* s = AppState::instance().findStudentById(pickedId);
        AppState::instance().logActivity("STUDENT_ASSIGNED",
            s ? QString("%1 %2 assigned to Block %3, Floor %4, Room %5")
                .arg(QString::fromStdString(s->getFirstName()))
                .arg(QString::fromStdString(s->getFamilyName()))
                .arg(QString::fromStdString(m_block)).arg(m_floor).arg(m_roomNum)
              : "Student assigned to room");
        refreshResidents();
        emit dataChanged();
    } else {
        QMessageBox::critical(this, "Error", "Could not assign student to this room.");
    }
}

void RoomInfoPanel::onRemoveStudent(int studentId) {
    const Student* s = AppState::instance().findStudentById(studentId);
    QString name = s ? QString::fromStdString(s->getFirstName() + " " + s->getFamilyName()) : "student";

    if (AppState::instance().university.unassignStudent(studentId)) {
        AppState::instance().logActivity("STUDENT_REMOVED",
            QString("%1 removed from Block %2, Floor %3, Room %4")
            .arg(name).arg(QString::fromStdString(m_block)).arg(m_floor).arg(m_roomNum));
        refreshResidents();
        emit dataChanged();
    }
}

void RoomInfoPanel::onClearRoom() {
    QDialog dlg(this);
    dlg.setWindowTitle("Clear Room");
    dlg.setFixedWidth(360);
    auto* ml = new QVBoxLayout(&dlg);
    ml->setContentsMargins(0, 0, 0, 0);
    ml->setSpacing(0);

    auto* hdr = new QFrame(&dlg);
    hdr->setFixedHeight(76);
    hdr->setStyleSheet("QFrame { background:#FEF2F2; }");
    auto* hl = new QHBoxLayout(hdr);
    hl->setContentsMargins(24, 0, 24, 0);
    hl->setSpacing(12);
    auto* iconLbl = new QLabel("⚠", &dlg);
    iconLbl->setStyleSheet("QLabel { font-size:26px; background:transparent; border:none; }");
    auto* hdText = new QWidget(&dlg);
    hdText->setStyleSheet("QWidget { background:transparent; }");
    auto* hdl = new QVBoxLayout(hdText);
    hdl->setContentsMargins(0, 0, 0, 0);
    hdl->setSpacing(2);
    auto* hTitle = new QLabel("Clear Room", hdText);
    hTitle->setStyleSheet("QLabel { font-size:15px; font-weight:bold; color:#991B1B; background:transparent; border:none; }");
    auto* hSub = new QLabel(QString("Floor %1  ·  Room %2").arg(m_floor).arg(m_roomNum), hdText);
    hSub->setStyleSheet("QLabel { font-size:11px; color:#EF4444; background:transparent; border:none; }");
    hdl->addWidget(hTitle);
    hdl->addWidget(hSub);
    hl->addWidget(iconLbl);
    hl->addWidget(hdText, 1);
    ml->addWidget(hdr);

    auto* body = new QWidget(&dlg);
    body->setStyleSheet("QWidget { background:white; }");
    auto* bl = new QVBoxLayout(body);
    bl->setContentsMargins(24, 20, 24, 20);
    bl->setSpacing(16);
    auto* msgLbl = new QLabel("Remove all students from this room?\nThis action cannot be undone.", body);
    msgLbl->setStyleSheet("QLabel { color:#374151; font-size:13px; background:transparent; border:none; }");
    msgLbl->setWordWrap(true);
    bl->addWidget(msgLbl);

    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(10);
    auto* cancelBtn = new QPushButton("Cancel", body);
    cancelBtn->setFixedHeight(40);
    cancelBtn->setStyleSheet(Style::secondaryBtnSS());
    cancelBtn->setCursor(Qt::PointingHandCursor);
    QObject::connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
    auto* confirmBtn = new QPushButton("Remove All", body);
    confirmBtn->setFixedHeight(40);
    confirmBtn->setMinimumWidth(120);
    confirmBtn->setCursor(Qt::PointingHandCursor);
    confirmBtn->setStyleSheet(
        "QPushButton { background:#EF4444; color:white; border:none; border-radius:9px;"
        "  font-size:13px; font-weight:bold; padding:0 24px; }"
        "QPushButton:hover { background:#DC2626; }"
        "QPushButton:pressed { background:#B91C1C; }");
    QObject::connect(confirmBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    btnRow->addStretch();
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(confirmBtn);
    bl->addLayout(btnRow);
    ml->addWidget(body);

    if (dlg.exec() != QDialog::Accepted) return;

    Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
    if (!dorm) return;
    Block* b = dorm->getBlock(m_block);
    if (!b) return;
    Room* r = b->getRoom(m_floor, m_roomNum);
    if (!r) return;

    std::vector<int> ids;
    for (const auto& s : r->getResidents()) ids.push_back(s.getId());
    for (int id : ids) AppState::instance().university.unassignStudent(id);

    AppState::instance().logActivity("ROOM_CLEARED",
        QString("Block %1, Floor %2, Room %3 cleared")
        .arg(QString::fromStdString(m_block)).arg(m_floor).arg(m_roomNum));
    refreshResidents();
    emit dataChanged();
}

// ═══════════════════════════════════════════════════════════════
// DormDetailWidget
// ═══════════════════════════════════════════════════════════════

DormDetailWidget::DormDetailWidget(QWidget* parent) : QWidget(parent) {
    memset(roomSquares, 0, sizeof(roomSquares));
    setAutoFillBackground(true);
    QPalette p = palette(); p.setColor(QPalette::Window, QColor("#F8F9FA")); setPalette(p);
    buildUi();
}

void DormDetailWidget::buildUi() {
    auto* outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    auto* mainContent = new QWidget(this);
    mainContent->setStyleSheet("QWidget { background:#F8F9FA; }");
    auto* mainLayout = new QVBoxLayout(mainContent);
    mainLayout->setContentsMargins(28, 20, 28, 20);
    mainLayout->setSpacing(14);

    // ── Top bar ───────────────────────────────────────────────
    auto* topBar = new QHBoxLayout;
    auto* backBtn = new QPushButton("← Back", mainContent);
    backBtn->setFlat(true);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet(
        QString("QPushButton { background:transparent; border:none; color:%1;"
                "  font-size:13px; font-weight:bold; padding:0; }"
                "QPushButton:hover { color:#E65C2A; }").arg(Style::PRIMARY_ORANGE));
    connect(backBtn, &QPushButton::clicked, this, [this]{
        infoPanel->close();
        emit backClicked();
    });

    titleLabel = new QLabel("Dormitory", mainContent);
    titleLabel->setStyleSheet("QLabel { font-size:22px; font-weight:bold; color:#111827; background:transparent; }");

    topBar->addWidget(backBtn);
    topBar->addSpacing(12);
    topBar->addWidget(titleLabel);
    topBar->addStretch();

    residentsLbl = new QLabel("0");
    availLbl     = new QLabel("0");
    capLbl       = new QLabel("0");

    auto makeStatWidget = [&](QLabel*& numOut, const QString& sub, const QString& color) -> QWidget* {
        auto* w = new QWidget(mainContent);
        w->setStyleSheet("QWidget { background:transparent; }");
        auto* wl = new QVBoxLayout(w);
        wl->setContentsMargins(0,0,0,0); wl->setSpacing(0);
        numOut = new QLabel("0", w);
        numOut->setAlignment(Qt::AlignCenter);
        numOut->setStyleSheet(QString("QLabel { color:%1; font-size:22px; font-weight:bold;"
                                      "  background:transparent; }").arg(color));
        auto* sl = new QLabel(sub, w);
        sl->setAlignment(Qt::AlignCenter);
        sl->setStyleSheet("QLabel { color:#9CA3AF; font-size:10px; background:transparent; }");
        wl->addWidget(numOut);
        wl->addWidget(sl);
        return w;
    };

    topBar->addWidget(makeStatWidget(residentsLbl, "Residents", Style::PRIMARY_ORANGE));
    topBar->addSpacing(8);
    auto* vDivA = new QFrame(mainContent);
    vDivA->setFixedWidth(1); vDivA->setFixedHeight(36);
    vDivA->setStyleSheet("QFrame { background:#E5E7EB; }");
    topBar->addWidget(vDivA);
    topBar->addSpacing(8);
    topBar->addWidget(makeStatWidget(availLbl, "Available", "#10B981"));
    topBar->addSpacing(8);
    auto* vDivB = new QFrame(mainContent);
    vDivB->setFixedWidth(1); vDivB->setFixedHeight(36);
    vDivB->setStyleSheet("QFrame { background:#E5E7EB; }");
    topBar->addWidget(vDivB);
    topBar->addSpacing(8);
    topBar->addWidget(makeStatWidget(capLbl, "Capacity", "#111827"));

    mainLayout->addLayout(topBar);

    // ── Block selector (underline tabs) ───────────────────────
    blockButtonBar = new QWidget(mainContent);
    blockButtonBar->setStyleSheet("QWidget { background:transparent; }");
    auto* blockLayout = new QHBoxLayout(blockButtonBar);
    blockLayout->setContentsMargins(0, 0, 0, 0);
    blockLayout->setSpacing(0);

    const char* blockNames[] = { "A", "B", "C", "D", "E" };
    for (int i = 0; i < 5; i++) {
        blockBtns[i] = new QPushButton(QString("Block %1").arg(blockNames[i]), blockButtonBar);
        blockBtns[i]->setFixedHeight(40);
        blockBtns[i]->setCursor(Qt::PointingHandCursor);
        blockBtns[i]->setFlat(true);
        QString bn = blockNames[i];
        connect(blockBtns[i], &QPushButton::clicked, this, [this, bn]{
            onBlockSelected(bn);
        });
        blockLayout->addWidget(blockBtns[i]);
    }
    blockLayout->addStretch();
    mainLayout->addWidget(blockButtonBar);

    // ── Block stats card ──────────────────────────────────────────
    auto* statsCard = new QFrame(mainContent);
    statsCard->setObjectName("blockStatsCard");
    statsCard->setStyleSheet(
        "QFrame#blockStatsCard { background:white; border-radius:10px; border:1px solid #EAECF0; }"
        "QLabel { border:none; background:transparent; }");
    auto* statsRow = new QHBoxLayout(statsCard);
    statsRow->setContentsMargins(20, 10, 20, 10);
    statsRow->setSpacing(0);

    auto addBlockStat = [&](QLabel*& out, const QString& label, const QString& color) {
        auto* col = new QVBoxLayout;
        col->setSpacing(1);
        out = new QLabel("0", statsCard);
        out->setStyleSheet(
            QString("QLabel { font-size:18px; font-weight:bold; color:%1; }").arg(color));
        auto* sub = new QLabel(label, statsCard);
        sub->setStyleSheet("QLabel { font-size:10px; color:#9CA3AF; letter-spacing:0.5px; }");
        col->addWidget(out);
        col->addWidget(sub);
        statsRow->addLayout(col);
    };

    auto addVDiv = [&]() {
        auto* d = new QFrame(statsCard);
        d->setFixedWidth(1); d->setFixedHeight(30);
        d->setStyleSheet("QFrame { background:#E5E7EB; border:none; }");
        statsRow->addSpacing(20);
        statsRow->addWidget(d);
        statsRow->addSpacing(20);
    };

    addBlockStat(blockOccLbl,   "OCCUPIED",   Style::PRIMARY_ORANGE);
    addVDiv();
    addBlockStat(blockAvailLbl, "AVAILABLE",  "#10B981");
    addVDiv();
    addBlockStat(blockRateLbl,  "OCCUPANCY",  "#6B7280");

    statsRow->addStretch();

    auto* barWrap = new QVBoxLayout;
    barWrap->setSpacing(0);
    barWrap->addStretch();
    blockBar = new QProgressBar(statsCard);
    blockBar->setRange(0, 100);
    blockBar->setValue(0);
    blockBar->setFixedSize(110, 6);
    blockBar->setTextVisible(false);
    blockBar->setStyleSheet(
        QString("QProgressBar { background:#F3F4F6; border-radius:3px; border:none; }"
                "QProgressBar::chunk { background:%1; border-radius:3px; }").arg(Style::gradientCSS()));
    barWrap->addWidget(blockBar);
    barWrap->addStretch();
    statsRow->addLayout(barWrap);

    mainLayout->addWidget(statsCard);

    // ── Floor selector (pill buttons) ─────────────────────────
    floorButtonBar = new QWidget(mainContent);
    floorButtonBar->setStyleSheet("QWidget { background:transparent; }");
    auto* floorLayout = new QHBoxLayout(floorButtonBar);
    floorLayout->setContentsMargins(0, 0, 0, 0);
    floorLayout->setSpacing(8);

    for (int i = 0; i <= 5; i++) {
        floorBtns[i] = new QPushButton(QString("Floor %1").arg(i), floorButtonBar);
        floorBtns[i]->setFixedHeight(34);
        floorBtns[i]->setCursor(Qt::PointingHandCursor);
        int fi = i;
        connect(floorBtns[i], &QPushButton::clicked, this, [this, fi]{
            onFloorSelected(fi);
        });
        floorLayout->addWidget(floorBtns[i]);
    }
    floorLayout->addStretch();
    mainLayout->addWidget(floorButtonBar);

    // Legend
    auto* legendRow = new QHBoxLayout;
    struct { QString bg; QString border; QString label; } legend[] = {
        { "#D1FAE5", "#6EE7B7", "Empty"   },
        { "#FEF3C7", "#FCD34D", "Partial" },
        { "#FEE2E2", "#FCA5A5", "Full"    },
    };
    for (auto& lg : legend) {
        auto* box = new QFrame(mainContent);
        box->setFixedSize(12, 12);
        box->setStyleSheet(QString("QFrame { background:%1; border:1.5px solid %2; border-radius:3px; }")
                           .arg(lg.bg, lg.border));
        auto* lbl = new QLabel(lg.label, mainContent);
        lbl->setStyleSheet("QLabel { color:#6B7280; font-size:11px; background:transparent; }");
        legendRow->addWidget(box);
        legendRow->addSpacing(4);
        legendRow->addWidget(lbl);
        legendRow->addSpacing(14);
    }
    legendRow->addStretch();
    mainLayout->addLayout(legendRow);

    // ── Room grid ──────────────────────────────────────────────
    auto* gridScroll = new QScrollArea(mainContent);
    gridScroll->setWidgetResizable(true);
    gridScroll->setFrameShape(QFrame::NoFrame);
    gridScroll->setStyleSheet("QScrollArea { background:transparent; }");

    roomGrid = new QWidget;
    roomGrid->setStyleSheet("QWidget { background:transparent; }");
    auto* roomLayout = new QGridLayout(roomGrid);
    roomLayout->setSpacing(8);
    roomLayout->setContentsMargins(0, 0, 0, 0);

    for (int f = 0; f <= 5; f++) {
        for (int n = 1; n <= 48; n++) {
            roomSquares[f][n-1] = new RoomSquare("A", f, n, roomGrid);
            roomSquares[f][n-1]->hide();
            connect(roomSquares[f][n-1], &RoomSquare::clicked,
                    this, &DormDetailWidget::onRoomClicked);
        }
    }

    gridScroll->setWidget(roomGrid);
    mainLayout->addWidget(gridScroll, 1);

    outerLayout->addWidget(mainContent, 1);

    // ── Info panel (right) ─────────────────────────────────────
    infoPanel = new RoomInfoPanel(this);
    connect(infoPanel, &RoomInfoPanel::dataChanged, this, &DormDetailWidget::refresh);
    outerLayout->addWidget(infoPanel);
}

void DormDetailWidget::loadDorm(int dormNum) {
    m_dormNum  = dormNum;
    m_curBlock = "A";
    m_curFloor = 0;

    Dormitory* dorm = AppState::instance().university.getDormitory(dormNum);
    if (!dorm) return;

    titleLabel->setText(QString("Dormitory %1 Management").arg(dormNum));

    setActiveBlockBtn(0);
    setActiveFloorBtn(0);

    for (int f = 0; f <= 5; f++)
        for (int n = 0; n < 48; n++)
            if (roomSquares[f][n]) roomSquares[f][n]->dormNum = dormNum;

    refresh();
}

void DormDetailWidget::refresh() {
    Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
    if (!dorm) return;

    int cap = dorm->getTotalCapacity();
    int occ = dorm->getTotalOccupancy();

    residentsLbl->setText(QString::number(occ));
    availLbl->setText(QString::number(cap - occ));
    capLbl->setText(QString::number(cap));

    refreshBlockStats();
    refreshRoomGrid();
}

void DormDetailWidget::refreshBlockStats() {
    Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
    if (!dorm) return;
    Block* b = dorm->getBlock(m_curBlock.toStdString());
    if (!b) return;

    int occ      = b->getTotalOccupancy();
    int totalCap = (LAST_FLOOR + 1) * LAST_NUMBER * Room::getCapacity();
    int avail    = totalCap - occ;
    int rate     = totalCap > 0 ? (occ * 100 / totalCap) : 0;

    blockOccLbl->setText(QString::number(occ));
    blockAvailLbl->setText(QString::number(avail));
    blockRateLbl->setText(QString("%1%").arg(rate));
    blockBar->setValue(rate);
}

void DormDetailWidget::refreshRoomGrid() {
    // Remove layout items without destroying widgets or changing their parent
    QLayoutItem* item;
    while ((item = roomGrid->layout()->takeAt(0)) != nullptr)
        delete item;

    // Hide all squares; we'll only show the current floor's set
    for (int f = 0; f <= 5; f++)
        for (int n = 0; n < 48; n++)
            if (roomSquares[f][n]) roomSquares[f][n]->hide();

    auto* gridLayout = qobject_cast<QGridLayout*>(roomGrid->layout());
    if (!gridLayout) return;

    int f = m_curFloor;
    for (int n = 0; n < 48; n++) {
        if (!roomSquares[f][n]) continue;
        roomSquares[f][n]->blockName = m_curBlock.toStdString();
        roomSquares[f][n]->floor     = f;
        roomSquares[f][n]->roomNum   = n + 1;
        roomSquares[f][n]->dormNum   = m_dormNum;
        roomSquares[f][n]->refreshColor();
        gridLayout->addWidget(roomSquares[f][n], n / 8, n % 8);
        roomSquares[f][n]->show();
    }
}

void DormDetailWidget::setActiveBlockBtn(int idx) {
    QString active =
        QString("QPushButton { color:#111827; font-size:13px; font-weight:bold;"
                "  background:transparent; border:none; border-bottom:2px solid %1;"
                "  padding:8px 16px; }").arg(Style::PRIMARY_ORANGE);
    QString inactive =
        "QPushButton { color:#6B7280; font-size:13px; font-weight:normal;"
        "  background:transparent; border:none; border-bottom:2px solid transparent;"
        "  padding:8px 16px; }"
        "QPushButton:hover { color:#374151; }";
    for (int i = 0; i < 5; i++)
        blockBtns[i]->setStyleSheet(i == idx ? active : inactive);
}

void DormDetailWidget::setActiveFloorBtn(int idx) {
    QString active =
        QString("QPushButton { background:%1; color:white; border:none; border-radius:17px;"
                "  padding:6px 18px; font-size:12px; font-weight:bold; }"
                "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "  stop:0 #E65C2A, stop:1 #E8A522); }").arg(Style::gradientCSS());
    QString inactive =
        "QPushButton { background:white; border:1px solid #E5E7EB; border-radius:17px;"
        "  padding:6px 18px; color:#6B7280; font-size:12px; }"
        "QPushButton:hover { border-color:#F7931E; color:#F7931E; }";
    for (int i = 0; i <= 5; i++)
        floorBtns[i]->setStyleSheet(i == idx ? active : inactive);
}

void DormDetailWidget::onBlockSelected(const QString& block) {
    m_curBlock = block;
    const char* bnames[] = { "A", "B", "C", "D", "E" };
    for (int i = 0; i < 5; i++)
        if (QString(bnames[i]) == block) { setActiveBlockBtn(i); break; }
    refreshBlockStats();
    refreshRoomGrid();
}

void DormDetailWidget::onFloorSelected(int floor) {
    m_curFloor = floor;
    setActiveFloorBtn(floor);
    refreshRoomGrid();
}

void DormDetailWidget::onRoomClicked(const std::string& block, int floor, int roomNum) {
    infoPanel->loadRoom(m_dormNum, block, floor, roomNum);
}
