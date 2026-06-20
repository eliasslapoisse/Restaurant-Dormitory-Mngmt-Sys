#include "DashboardWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../AdminMainWindow.h"
#include "../dialogs/AddStudentDialog.h"
#include "../dialogs/AssignToDormDialog.h"
#include "../dialogs/StudentPickerDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QDateTime>
#include <QScrollBar>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

// ── Helpers ──────────────────────────────────────────────────────────────────
static QFrame* makeCard(QWidget* parent) {
    auto* f = new QFrame(parent);
    f->setObjectName("dashCard");
    f->setStyleSheet("QFrame#dashCard { background:white; border-radius:14px; border:none; }");
    auto* shadow = new QGraphicsDropShadowEffect(f);
    shadow->setBlurRadius(22);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 28));
    f->setGraphicsEffect(shadow);
    return f;
}

DashboardWidget::DashboardWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor("#F8F9FA"));
    setPalette(p);
}

void DashboardWidget::buildUi() {
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* content = new QWidget;
    content->setAutoFillBackground(true);
    { QPalette cp = content->palette(); cp.setColor(QPalette::Window, QColor("#F8F9FA")); content->setPalette(cp); }
    scroll->setWidget(content);

    auto* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(0, 0, 0, 24);
    mainLayout->setSpacing(0);

    // ── Gradient header ────────────────────────────────────────
    auto* header = new QFrame(content);
    header->setFixedHeight(170);
    header->setStyleSheet(
        QString("QFrame { background:%1; border-radius:0; }").arg(Style::gradientCSS("0","0","1","1")));

    auto* hOuter = new QHBoxLayout(header);
    hOuter->setContentsMargins(32, 24, 32, 24);
    hOuter->setSpacing(0);

    auto* hLeft = new QVBoxLayout;
    hLeft->setSpacing(4);
    auto* subTitle = new QLabel("UNIVERSITY HOUSING  ·  ADMINISTRATION", header);
    subTitle->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:11px;"
                            "  letter-spacing:1px; background:transparent; }");
    auto* titleLabel = new QLabel("Dashboard", header);
    titleLabel->setStyleSheet("QLabel { color:white; font-size:38px; font-weight:bold;"
                              "  background:transparent; }");
    dateLabel = new QLabel("", header);
    dateLabel->setStyleSheet("QLabel { color:rgba(255,255,255,0.80); font-size:13px;"
                             "  background:transparent; }");
    hLeft->addStretch();
    hLeft->addWidget(subTitle);
    hLeft->addWidget(titleLabel);
    hLeft->addWidget(dateLabel);
    hLeft->addStretch();

    auto* hRight = new QVBoxLayout;
    hRight->setSpacing(6);
    dormCountLabel = new QLabel("", header);
    dormCountLabel->setStyleSheet(
        "QLabel { color:white; font-size:12px; font-weight:bold;"
        "  background:rgba(255,255,255,0.22); border-radius:16px; padding:6px 16px;"
        "  border:1px solid rgba(255,255,255,0.45); }");
    dormCountLabel->setAlignment(Qt::AlignCenter);
    studentCountLabel = new QLabel("", header);
    studentCountLabel->setStyleSheet(
        "QLabel { color:rgba(255,255,255,0.75); font-size:12px; background:transparent; }");
    studentCountLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    hRight->addStretch();
    hRight->addWidget(dormCountLabel, 0, Qt::AlignRight);
    hRight->addWidget(studentCountLabel, 0, Qt::AlignRight);
    hRight->addStretch();

    hOuter->addLayout(hLeft, 1);
    hOuter->addLayout(hRight);

    mainLayout->addWidget(header);

    // ── Stats cards ────────────────────────────────────────────
    auto* statsRow = new QHBoxLayout;
    statsRow->setContentsMargins(24, 20, 24, 0);
    statsRow->setSpacing(16);

    // Card 1: Occupancy
    auto* occCard = makeCard(content);
    occCard->setMinimumHeight(160);
    auto* occCardLayout = new QVBoxLayout(occCard);
    occCardLayout->setContentsMargins(24, 14, 20, 16);
    occCardLayout->setSpacing(0);

    // Title pill — top right
    auto* occTitleRow = new QHBoxLayout;
    occTitleRow->addStretch();
    auto* occTitle = new QLabel("OVERALL OCCUPANCY", occCard);
    occTitle->setStyleSheet(
        "QLabel { color:#9CA3AF; font-size:10px; font-weight:bold; letter-spacing:1.5px;"
        "  background:transparent; }");
    occTitleRow->addWidget(occTitle);
    occCardLayout->addLayout(occTitleRow);
    occCardLayout->addSpacing(6);

    // Circle + text — vertically centered together
    auto* occMainRow = new QHBoxLayout;
    occMainRow->setSpacing(20);

    occupancyCircle = new CircularProgress(occCard);
    occMainRow->addWidget(occupancyCircle, 0, Qt::AlignVCenter);

    auto* occRight = new QWidget(occCard);
    occRight->setStyleSheet("QWidget { background:transparent; }");
    auto* occRightLayout = new QVBoxLayout(occRight);
    occRightLayout->setContentsMargins(0, 0, 0, 0);
    occRightLayout->setSpacing(4);

    occupancyPctLabel = new QLabel("0%", occRight);
    occupancyPctLabel->setStyleSheet(
        QString("QLabel { color:%1; font-size:34px; font-weight:bold; background:transparent; }")
        .arg(Style::PRIMARY_ORANGE));
    occupancyDetailLabel = new QLabel("0 of 0 beds filled", occRight);
    occupancyDetailLabel->setStyleSheet("QLabel { color:#6B7280; font-size:12px; background:transparent; }");
    occupancyLegendLabel = new QLabel("", occRight);
    occupancyLegendLabel->setStyleSheet("QLabel { color:#6B7280; font-size:12px; background:transparent; }");

    occProgressBar = new QProgressBar(occRight);
    occProgressBar->setRange(0, 100);
    occProgressBar->setValue(0);
    occProgressBar->setFixedHeight(6);
    occProgressBar->setTextVisible(false);
    occProgressBar->setStyleSheet(
        "QProgressBar { background:#F3F4F6; border-radius:3px; border:none; }"
        "QProgressBar::chunk { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "  stop:0 #FF6B35, stop:1 #F7931E); border-radius:3px; }");

    occRightLayout->addStretch();
    occRightLayout->addWidget(occupancyPctLabel);
    occRightLayout->addWidget(occupancyDetailLabel);
    occRightLayout->addWidget(occupancyLegendLabel);
    occRightLayout->addSpacing(6);
    occRightLayout->addWidget(occProgressBar);
    occRightLayout->addStretch();

    occMainRow->addWidget(occRight, 1);
    occCardLayout->addLayout(occMainRow, 1);
    statsRow->addWidget(occCard, 2);

    // Card 2: Students
    auto* stuCard = makeCard(content);
    auto* stuLayout = new QVBoxLayout(stuCard);
    stuLayout->setContentsMargins(24, 14, 24, 20);
    stuLayout->setSpacing(8);

    auto* stuTitleRow = new QHBoxLayout;
    stuTitleRow->addStretch();
    auto* stuTitle = new QLabel("STUDENTS", stuCard);
    stuTitle->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    stuTitle->setStyleSheet(
        "QLabel { color:#9CA3AF; font-size:10px; font-weight:bold; letter-spacing:1.5px;"
        "  background:transparent; }");
    stuTitleRow->addWidget(stuTitle);
    stuLayout->addLayout(stuTitleRow);

    auto* stuIconRow = new QHBoxLayout;
    auto* stuIcon = new QLabel("S", stuCard);
    stuIcon->setStyleSheet(
        "QLabel { font-size:24px; font-weight:bold; color:white;"
        "  background:qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #3B82F6, stop:1 #60A5FA);"
        "  border-radius:12px; }");
    stuIcon->setFixedSize(54, 54);
    stuIcon->setAlignment(Qt::AlignCenter);
    totalStudentsLabel = new QLabel("0", stuCard);
    totalStudentsLabel->setStyleSheet(
        "QLabel { color:#111827; font-size:44px; font-weight:bold; background:transparent; }");
    stuIconRow->addWidget(stuIcon);
    stuIconRow->addWidget(totalStudentsLabel);
    stuIconRow->addStretch();
    auto* stuSep = new QFrame(stuCard);
    stuSep->setFrameShape(QFrame::HLine);
    stuSep->setStyleSheet("QFrame { color:#F3F4F6; }");
    auto* assignRow = new QHBoxLayout;
    auto* assignDot = new QLabel("✓", stuCard);
    assignDot->setStyleSheet("QLabel { color:#10B981; font-size:13px; background:transparent; }");
    auto* assignLbl = new QLabel("Assigned", stuCard);
    assignLbl->setStyleSheet("QLabel { color:#6B7280; font-size:12px; background:transparent; }");
    assignedCountLabel = new QLabel("0", stuCard);
    assignedCountLabel->setStyleSheet("QLabel { color:#10B981; font-size:14px; font-weight:bold; background:transparent; }");
    assignRow->addWidget(assignDot); assignRow->addWidget(assignLbl); assignRow->addStretch();
    assignRow->addWidget(assignedCountLabel);

    auto* poolRow = new QHBoxLayout;
    auto* poolDot = new QLabel("○", stuCard);
    poolDot->setStyleSheet("QLabel { color:#F59E0B; font-size:13px; background:transparent; }");
    auto* poolLbl = new QLabel("In Pool", stuCard);
    poolLbl->setStyleSheet("QLabel { color:#6B7280; font-size:12px; background:transparent; }");
    poolCountLabel = new QLabel("0", stuCard);
    poolCountLabel->setStyleSheet("QLabel { color:#F59E0B; font-size:14px; font-weight:bold; background:transparent; }");
    poolRow->addWidget(poolDot); poolRow->addWidget(poolLbl); poolRow->addStretch();
    poolRow->addWidget(poolCountLabel);

    stuLayout->addLayout(stuIconRow);
    stuLayout->addWidget(stuSep);
    stuLayout->addLayout(assignRow);
    stuLayout->addLayout(poolRow);
    stuLayout->addStretch();
    statsRow->addWidget(stuCard, 1);

    // Card 3: Issues
    auto* issCard = makeCard(content);
    issCard->setStyleSheet("QFrame#dashCard { background:#FFF1F2; border-radius:14px; border:none; }");
    auto* issLayout = new QVBoxLayout(issCard);
    issLayout->setContentsMargins(24, 14, 24, 20);
    issLayout->setSpacing(8);

    auto* issTitleRow = new QHBoxLayout;
    issTitleRow->addStretch();
    auto* issTitle = new QLabel("ISSUES", issCard);
    issTitle->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    issTitle->setStyleSheet(
        "QLabel { color:#9CA3AF; font-size:10px; font-weight:bold; letter-spacing:1.5px;"
        "  background:transparent; }");
    issTitleRow->addWidget(issTitle);
    issLayout->addLayout(issTitleRow);

    auto* issIconRow = new QHBoxLayout;
    issIconRow->setSpacing(12);
    auto* issIconLbl = new QLabel("!", issCard);
    issIconLbl->setStyleSheet(
        "QLabel { font-size:28px; font-weight:bold; color:white;"
        "  background:#EF4444; border-radius:12px; }");
    issIconLbl->setFixedSize(54, 54);
    issIconLbl->setAlignment(Qt::AlignCenter);
    issuesCountLabel = new QLabel("0", issCard);
    issuesCountLabel->setStyleSheet(
        "QLabel { color:#EF4444; font-size:44px; font-weight:bold; background:transparent; }");
    issIconRow->addWidget(issIconLbl);
    issIconRow->addWidget(issuesCountLabel);
    issIconRow->addStretch();
    issLayout->addLayout(issIconRow);

    solvedIssuesLabel  = new QLabel(issCard); solvedIssuesLabel->hide();
    pendingIssuesLabel = new QLabel(issCard); pendingIssuesLabel->hide();

    auto* issPendingLbl = new QLabel("Pending complaints", issCard);
    issPendingLbl->setStyleSheet("QLabel { color:#9CA3AF; font-size:12px; background:transparent; }");
    issLayout->addWidget(issPendingLbl);

    auto* issViewAll = new QPushButton("View all  →", issCard);
    issViewAll->setStyleSheet(
        "QPushButton { color:#EF4444; font-size:12px; font-weight:bold;"
        "  background:transparent; border:none; text-align:left; padding:0; }");
    issViewAll->setCursor(Qt::PointingHandCursor);
    connect(issViewAll, &QPushButton::clicked, this, [] {
        auto* admin = AdminMainWindow::instance();
        if (admin) admin->navigateTo(6);
    });
    issLayout->addWidget(issViewAll);

    issLayout->addStretch();
    statsRow->addWidget(issCard, 1);

    mainLayout->addLayout(statsRow);

    // ── Info tiles ─────────────────────────────────────────────
    auto* tilesRow = new QHBoxLayout;
    tilesRow->setContentsMargins(24, 16, 24, 0);
    tilesRow->setSpacing(16);

    struct TileInfo { QString iconGrad; QString numColor; QString icon; QLabel** numRef; QString sub; QString cardBg; };
    dormsTileLabel = new QLabel("6");
    bedsTileLabel  = new QLabel("0");
    availTileLabel = new QLabel("0");
    TileInfo tiles[] = {
        { "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #FF6B35, stop:1 #F7931E)",
          "#111827", "D", &dormsTileLabel, "DORMITORIES",   "#FFF7ED" },
        { "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #059669, stop:1 #10B981)",
          "#111827", "T", &bedsTileLabel,  "TOTAL BEDS",    "#F0FDF4" },
        { "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #7C3AED, stop:1 #8B5CF6)",
          "#111827", "B", &availTileLabel, "AVAILABLE BEDS","#F5F3FF" },
    };
    for (auto& t : tiles) {
        auto* tile = makeCard(content);
        tile->setStyleSheet(QString("QFrame#dashCard { background:%1; border-radius:14px; border:none; }").arg(t.cardBg));
        auto* tileLayout = new QHBoxLayout(tile);
        tileLayout->setContentsMargins(20, 16, 20, 16);
        tileLayout->setSpacing(16);
        auto* iconBox = new QLabel(t.icon, tile);
        iconBox->setFixedSize(52, 52);
        iconBox->setAlignment(Qt::AlignCenter);
        iconBox->setStyleSheet(
            QString("QLabel { background:%1; border-radius:14px; font-size:22px; font-weight:bold; color:white; }").arg(t.iconGrad));
        auto* vbox = new QWidget(tile);
        vbox->setStyleSheet("QWidget { background:transparent; }");
        auto* vboxLayout = new QVBoxLayout(vbox);
        vboxLayout->setContentsMargins(0,0,0,0); vboxLayout->setSpacing(2);
        (*t.numRef)->setParent(tile);
        (*t.numRef)->setStyleSheet(
            QString("QLabel { color:%1; font-size:30px; font-weight:bold; background:transparent; }")
            .arg(t.numColor));
        auto* subLbl = new QLabel(t.sub, tile);
        subLbl->setStyleSheet("QLabel { color:#9CA3AF; font-size:10px; font-weight:bold;"
                              "  letter-spacing:1.5px; background:transparent; }");
        vboxLayout->addWidget(*t.numRef);
        vboxLayout->addWidget(subLbl);
        tileLayout->addWidget(iconBox);
        tileLayout->addWidget(vbox);
        tileLayout->addStretch();
        tilesRow->addWidget(tile, 1);
    }
    mainLayout->addLayout(tilesRow);

    // ── Quick actions ──────────────────────────────────────────
    auto* qaSection = new QWidget(content);
    qaSection->setStyleSheet("QWidget { background:transparent; }");
    auto* qaLayout = new QVBoxLayout(qaSection);
    qaLayout->setContentsMargins(24, 20, 24, 0);
    qaLayout->setSpacing(12);
    auto* qaTitleRow = new QHBoxLayout;
    qaTitleRow->setSpacing(14);

    auto* lineL = new QWidget(qaSection);
    lineL->setFixedHeight(1);
    lineL->setStyleSheet("QWidget { background:#E5E7EB; }");

    auto* qaTitle = new QLabel("QUICK ACTIONS", qaSection);
    qaTitle->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    qaTitle->setStyleSheet(
        QString("QLabel { color:white; font-size:10px; font-weight:bold; letter-spacing:1.5px;"
                "  background:%1;"
                "  border-radius:10px; padding:5px 14px; }").arg(Style::gradientCSS()));

    auto* lineR = new QWidget(qaSection);
    lineR->setFixedHeight(1);
    lineR->setStyleSheet("QWidget { background:#E5E7EB; }");

    qaTitleRow->addWidget(lineL, 1, Qt::AlignVCenter);
    qaTitleRow->addWidget(qaTitle, 0, Qt::AlignVCenter);
    qaTitleRow->addWidget(lineR, 1, Qt::AlignVCenter);
    auto* qaRow = new QHBoxLayout;
    qaRow->setSpacing(12);

    struct QA { QString icon; QString text; QString sub; };
    QA actions[] = {
        { "+",  "Add New Student",    "Register a new resident" },
        { "→",  "Assign to Dorm",     "Move student to a room"   },
        { "≡",  "View Complaints",    "Review pending issues"    },
        { "#",  "Manage Dormitories", "Rooms & configuration"   },
    };
    for (int i = 0; i < 4; i++) {
        auto& a = actions[i];
        auto* btn = new QPushButton(qaSection);
        btn->setMinimumHeight(64);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            QString("QPushButton { background:%1; border:none; border-radius:10px; }"
                    "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                    "  stop:0 #E65C2A,stop:1 #E8A522); }").arg(Style::gradientCSS()));

        auto* btnLayout = new QHBoxLayout(btn);
        btnLayout->setContentsMargins(16, 14, 16, 14);
        btnLayout->setSpacing(12);

        auto* iconBox = new QWidget(btn);
        iconBox->setFixedSize(40, 40);
        iconBox->setStyleSheet("QWidget { background:rgba(255,255,255,0.22); border-radius:10px; }");
        iconBox->setAttribute(Qt::WA_TransparentForMouseEvents);
        auto* iconBoxL = new QHBoxLayout(iconBox);
        iconBoxL->setContentsMargins(0,0,0,0);
        auto* iconLbl = new QLabel(a.icon, iconBox);
        iconLbl->setStyleSheet("QLabel { font-size:18px; font-weight:bold; color:white; background:transparent; }");
        iconLbl->setAlignment(Qt::AlignCenter);
        iconLbl->setAttribute(Qt::WA_TransparentForMouseEvents);
        iconBoxL->addWidget(iconLbl);

        auto* textVBox = new QWidget(btn);
        textVBox->setStyleSheet("QWidget { background:transparent; }");
        textVBox->setAttribute(Qt::WA_TransparentForMouseEvents);
        auto* tvLayout = new QVBoxLayout(textVBox);
        tvLayout->setContentsMargins(0,0,0,0); tvLayout->setSpacing(2);

        auto* textLbl = new QLabel(a.text, textVBox);
        textLbl->setStyleSheet("QLabel { color:white; font-weight:bold; font-size:13px; background:transparent; }");
        textLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

        auto* subLbl = new QLabel(a.sub, textVBox);
        subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.8); font-size:11px; background:transparent; }");
        subLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

        tvLayout->addWidget(textLbl);
        tvLayout->addWidget(subLbl);
        btnLayout->addWidget(iconBox);
        btnLayout->addWidget(textVBox, 1);
        qaRow->addWidget(btn, 1);

        auto* btnShadow = new QGraphicsDropShadowEffect(btn);
        btnShadow->setBlurRadius(20);
        btnShadow->setOffset(0, 5);
        btnShadow->setColor(QColor(247, 147, 30, 80));
        btn->setGraphicsEffect(btnShadow);

        int capturedI = i;
        connect(btn, &QPushButton::clicked, this, [this, capturedI] {
            auto* admin = AdminMainWindow::instance();
            if (!admin) return;
            switch (capturedI) {
            case 0: { AddStudentDialog d(this); if (d.exec() == QDialog::Accepted) admin->refreshAll(); break; }
            case 1: {
                StudentPickerDialog picker(false, this);
                if (picker.exec() != QDialog::Accepted) break;
                int sid = picker.selectedStudentId();
                if (sid < 0) break;
                AssignToDormDialog d(sid, this);
                if (d.exec() == QDialog::Accepted) admin->refreshAll();
                break;
            }
            case 2: admin->navigateTo(6); break;
            case 3: admin->navigateTo(2); break;
            }
        });
    }

    qaLayout->addLayout(qaTitleRow);
    qaLayout->addLayout(qaRow);
    mainLayout->addWidget(qaSection);

    // ── Bottom: occupancy list + activity ──────────────────────
    auto* bottomRow = new QHBoxLayout;
    bottomRow->setContentsMargins(24, 16, 24, 0);
    bottomRow->setSpacing(16);
    bottomRow->addWidget(buildOccupancyList(), 1);
    bottomRow->addWidget(buildActivityFeed(),  1);
    mainLayout->addLayout(bottomRow);

    // Main layout for this widget
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->addWidget(scroll);
}


QWidget* DashboardWidget::buildOccupancyList() {
    auto* card = makeCard(this);
    // Stronger shadow to match the compact top cards
    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(32); shadow->setOffset(0, 6); shadow->setColor(QColor(0,0,0,38));
    card->setGraphicsEffect(shadow);
    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 18, 22, 18);
    layout->setSpacing(14);

    auto* hdr = new QHBoxLayout;
    auto* title = new QLabel("OCCUPANCY BY DORMITORY", card);
    title->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    title->setStyleSheet(
        QString("QLabel { color:white; font-size:10px; font-weight:bold; letter-spacing:1.5px;"
                "  background:%1; border-radius:10px; padding:4px 10px; }").arg(Style::gradientCSS()));
    auto* sub = new QLabel("Students assigned per building", card);
    sub->setStyleSheet("QLabel { color:#9CA3AF; font-size:11px; background:transparent; }");
    hdr->addWidget(title);
    hdr->addSpacing(10);
    hdr->addWidget(sub);
    hdr->addStretch();
    layout->addLayout(hdr);

    auto* divider = new QFrame(card);
    divider->setFrameShape(QFrame::HLine);
    divider->setFixedHeight(1);
    divider->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    layout->addWidget(divider);

    occupancyListContainer = new QWidget(card);
    occupancyListContainer->setStyleSheet("QWidget { background:transparent; }");
    layout->addWidget(occupancyListContainer);

    return card;
}

QWidget* DashboardWidget::buildActivityFeed() {
    auto* card = makeCard(this);
    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(32); shadow->setOffset(0, 6); shadow->setColor(QColor(0,0,0,38));
    card->setGraphicsEffect(shadow);
    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 18, 22, 18);
    layout->setSpacing(14);

    auto* hdr = new QHBoxLayout;
    auto* title = new QLabel("RECENT ACTIVITY", card);
    title->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    title->setStyleSheet(
        QString("QLabel { color:white; font-size:10px; font-weight:bold; letter-spacing:1.5px;"
                "  background:%1; border-radius:10px; padding:4px 10px; }").arg(Style::gradientCSS()));
    auto* sub = new QLabel("Latest system events", card);
    sub->setStyleSheet("QLabel { color:#9CA3AF; font-size:11px; background:transparent; }");
    hdr->addWidget(title);
    hdr->addSpacing(10);
    hdr->addWidget(sub);
    hdr->addStretch();
    layout->addLayout(hdr);

    auto* divider = new QFrame(card);
    divider->setFrameShape(QFrame::HLine);
    divider->setFixedHeight(1);
    divider->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    layout->addWidget(divider);

    activityList = new QListWidget(card);
    activityList->setStyleSheet(
        "QListWidget { border:none; background:transparent; outline:none; }"
        "QListWidget::item { padding:10px 6px; border-bottom:1px solid #F9FAFB; color:#374151; font-size:12px; }"
        "QListWidget::item:hover    { background:#FFF8F0; border-radius:6px; border-bottom:1px solid #F9FAFB; }"
        "QListWidget::item:selected { background:#FFF8F0; border-radius:6px; border:none; outline:none; }");
    activityList->setFrameShape(QFrame::NoFrame);
    activityList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(activityList, 1);

    return card;
}

void DashboardWidget::refresh() {
    auto& uni = AppState::instance().university;

    dateLabel->setText(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy"));

    // Count students
    int poolCount = (int)uni.getStudentPool().size();
    int assignedCount = 0;
    for (const auto& d : uni.getDormitories())
        assignedCount += d.getTotalOccupancy();
    int total = poolCount + assignedCount;

    totalStudentsLabel->setText(QString::number(total));
    assignedCountLabel->setText(QString::number(assignedCount));
    poolCountLabel->setText(QString::number(poolCount));
    studentCountLabel->setText(QString("%1 student%2 registered").arg(total).arg(total != 1 ? "s" : ""));

    // Occupancy
    int capacity = uni.getTotalCapacity();
    int occ = assignedCount;
    double rate = (capacity > 0) ? (occ * 100.0 / capacity) : 0.0;
    int ratePct = (int)rate;
    occupancyCircle->setValue(ratePct);
    occupancyPctLabel->setText(QString("%1%").arg(ratePct));
    occupancyDetailLabel->setText(QString("%1 of %2 beds filled").arg(occ).arg(capacity));
    occupancyLegendLabel->setText(QString("● %1 assigned   ○ %2 free").arg(occ).arg(capacity - occ));
    occProgressBar->setValue(ratePct);

    int numDorms = (int)uni.getDormitories().size();
    dormCountLabel->setText(QString("↗  %1 Active Dormitor%2")
                            .arg(numDorms).arg(numDorms == 1 ? "y" : "ies"));

    // Tiles
    bedsTileLabel->setText(QString::number(capacity));
    availTileLabel->setText(QString::number(capacity - occ));
    dormsTileLabel->setText(QString::number(numDorms));

    // Issues
    int pending = 0, solved = 0;
    for (const auto& c : AppState::instance().complaints) {
        if (c.status == "Pending") pending++;
        else solved++;
    }
    issuesCountLabel->setText(QString::number(pending));
    solvedIssuesLabel->setText(QString::number(solved));
    pendingIssuesLabel->setText(QString::number(pending));

    // Occupancy list — clear old child widgets first (deleting only the layout leaves them orphaned but still visible)
    for (auto* w : occupancyListContainer->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
        delete w;
    delete occupancyListContainer->layout();
    auto* occLayout = new QVBoxLayout(occupancyListContainer);
    occLayout->setContentsMargins(0, 0, 0, 0);
    occLayout->setSpacing(12);
    for (const auto& dorm : uni.getDormitories()) {
        int dormCap = dorm.getTotalCapacity();
        int dormOcc = dorm.getTotalOccupancy();
        double dormRate = (dormCap > 0) ? (dormOcc * 100.0 / dormCap) : 0.0;

        auto* rowWidget = new QWidget(occupancyListContainer);
        rowWidget->setStyleSheet("QWidget { background:transparent; }");
        auto* rowLayout = new QVBoxLayout(rowWidget);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(5);

        auto* topRow2 = new QHBoxLayout;
        auto* nameL = new QLabel(QString("Dormitory %1").arg(dorm.getNumber()), rowWidget);
        nameL->setStyleSheet("QLabel { color:#111827; font-weight:bold; font-size:13px; background:transparent; }");
        auto* bedsL = new QLabel(QString("%1 / %2").arg(dormOcc).arg(dormCap), rowWidget);
        bedsL->setStyleSheet("QLabel { color:#9CA3AF; font-size:11px; background:transparent; }");
        auto* pctL = new QLabel(QString("%1%").arg((int)dormRate), rowWidget);
        pctL->setStyleSheet(QString("QLabel { color:%1; font-weight:bold; font-size:12px; background:transparent; }")
                            .arg(Style::PRIMARY_ORANGE));
        topRow2->addWidget(nameL);
        topRow2->addSpacing(8);
        topRow2->addWidget(bedsL);
        topRow2->addStretch();
        topRow2->addWidget(pctL);

        auto* bar = new QProgressBar(rowWidget);
        bar->setRange(0, 100);
        bar->setValue((int)dormRate);
        bar->setFixedHeight(6);
        bar->setTextVisible(false);
        bar->setStyleSheet(
            "QProgressBar { background:#F3F4F6; border-radius:3px; border:none; }"
            "QProgressBar::chunk { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "  stop:0 #FF6B35, stop:1 #FDB827); border-radius:3px; }");

        rowLayout->addLayout(topRow2);
        rowLayout->addWidget(bar);
        occLayout->addWidget(rowWidget);
    }
    occLayout->addStretch();

    // Activity feed
    activityList->clear();
    for (const auto& log : AppState::instance().activityLog) {
        QString dot;
        QColor fg;
        if      (log.action.contains("ADDED"))      { dot = "●"; fg = QColor("#FF6B35"); }
        else if (log.action.contains("ASSIGNED"))   { dot = "●"; fg = QColor("#374151"); }
        else if (log.action.contains("UNASSIGNED")) { dot = "●"; fg = QColor("#F59E0B"); }
        else if (log.action.contains("EXPELLED"))   { dot = "●"; fg = QColor("#EF4444"); }
        else if (log.action.contains("UPDATED"))    { dot = "●"; fg = QColor("#6B7280"); }
        else                                        { dot = "●"; fg = QColor("#9CA3AF"); }

        auto* item = new QListWidgetItem;
        item->setText(QString("%1  %2\n    %3")
            .arg(dot)
            .arg(log.description)
            .arg(log.timestamp.toString("MMM d  ·  hh:mm")));
        item->setForeground(fg);
        activityList->addItem(item);
    }
    if (activityList->count() == 0) {
        auto* item = new QListWidgetItem("   No recent activity");
        item->setForeground(QColor("#D1D5DB"));
        activityList->addItem(item);
    }
}
