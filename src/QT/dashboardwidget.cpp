#include "dashboardwidget.h"
#include "datamanager.h"
#include "styles.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPainter>
#include <QLinearGradient>
#include <QDateTime>
#include <QFont>
#include <QFrame>

// ── StatCard ───────────────────────────────────────────────────────────────────
StatCard::StatCard(const QString& title, const QString& value, const QString& sub,
                   const QString& icon, const QString& color, QWidget* parent)
    : QWidget(parent), m_title(title), m_value(value), m_sub(sub), m_icon(icon), m_color(color)
{
    setFixedHeight(120);
    setMinimumWidth(160);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(20, 16, 20, 16);
    lay->setSpacing(4);

    auto* topRow = new QHBoxLayout();
    auto* titleLbl = new QLabel(m_title, this);
    titleLbl->setStyleSheet("font-size: 11px; font-weight: 800; color: #8896A8; letter-spacing: 1.5px; background: transparent;");
    auto* iconLbl = new QLabel(m_icon, this);
    iconLbl->setStyleSheet("font-size: 20px; background: transparent;");
    topRow->addWidget(titleLbl);
    topRow->addStretch();
    topRow->addWidget(iconLbl);
    lay->addLayout(topRow);

    m_valueLbl = new QLabel(m_value, this);
    m_valueLbl->setStyleSheet(
        QString("font-size: 36px; font-weight: 900; color: %1; background: transparent;").arg(m_color));
    lay->addWidget(m_valueLbl);

    m_subLbl = new QLabel(m_sub, this);
    m_subLbl->setStyleSheet("font-size: 11px; color: #4A5568; background: transparent;");
    lay->addWidget(m_subLbl);
}

void StatCard::setValue(const QString& v) { m_valueLbl->setText(v); }
void StatCard::setSub(const QString& s)   { m_subLbl->setText(s); }

void StatCard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Card background
    p.setBrush(QColor("#161B27"));
    p.setPen(QPen(QColor("#21293A"), 1));
    p.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 12, 12);

    // Colored top bar
    QColor c(m_color);
    c.setAlpha(200);
    p.setBrush(QBrush(c));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, width(), 4, 2, 2);

    // Subtle glow bottom-right
    QRadialGradient glow(width(), height(), width() * 0.7);
    QColor gc(m_color); gc.setAlpha(18);
    glow.setColorAt(0, gc);
    glow.setColorAt(1, Qt::transparent);
    p.setBrush(glow);
    p.drawRoundedRect(rect(), 12, 12);
}

// ── DashboardWidget ────────────────────────────────────────────────────────────
DashboardWidget::DashboardWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void DashboardWidget::buildUi() {
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background: transparent;");

    auto* content = new QWidget();
    content->setStyleSheet("background: transparent;");

    auto* mainLay = new QVBoxLayout(content);
    mainLay->setContentsMargins(28, 24, 28, 28);
    mainLay->setSpacing(24);

    // ── Page Header ──────────────────────────────────────────────────────────
    auto* headerRow = new QHBoxLayout();
    auto* titleArea = new QVBoxLayout();
    auto* pageTitleLbl = new QLabel("📊  Dashboard", content);
    pageTitleLbl->setStyleSheet("font-size: 24px; font-weight: 900; color: #E8EDF5; background: transparent;");
    auto* dateStr = QDateTime::currentDateTime().toString("dddd, MMMM d yyyy");
    auto* pageSubLbl = new QLabel("Sidi Abdollah NSAI  •  " + dateStr, content);
    pageSubLbl->setStyleSheet("font-size: 13px; color: #8896A8; background: transparent;");
    titleArea->addWidget(pageTitleLbl);
    titleArea->addWidget(pageSubLbl);
    titleArea->setSpacing(4);
    headerRow->addLayout(titleArea);
    headerRow->addStretch();
    mainLay->addLayout(headerRow);

    // ── Stat Cards ───────────────────────────────────────────────────────────
    auto* cardsRow = new QHBoxLayout();
    cardsRow->setSpacing(16);
    buildStatCards(content, cardsRow);
    mainLay->addLayout(cardsRow);

    // ── Middle Section ───────────────────────────────────────────────────────
    auto* midRow = new QHBoxLayout();
    midRow->setSpacing(20);

    // Left: Dorm Occupancy
    auto* dormPanel = new QWidget(content);
    dormPanel->setStyleSheet("background:#161B27; border-radius:12px; border:1px solid #21293A;");
    auto* dormLay = new QVBoxLayout(dormPanel);
    dormLay->setContentsMargins(20, 18, 20, 18);
    dormLay->setSpacing(14);
    auto* dormTitleLbl = new QLabel("🏠  Dormitory Occupancy Overview", dormPanel);
    dormTitleLbl->setStyleSheet("font-size: 15px; font-weight: 800; color: #E8EDF5; background: transparent;");
    dormLay->addWidget(dormTitleLbl);

    auto* dormSubLbl = new QLabel("Real-time student distribution across all 6 complexes", dormPanel);
    dormSubLbl->setStyleSheet("font-size: 12px; color: #8896A8; background: transparent;");
    dormLay->addWidget(dormSubLbl);

    auto* sepH = new QFrame(dormPanel);
    sepH->setFrameShape(QFrame::HLine);
    sepH->setStyleSheet("background: #21293A; border: none;");
    dormLay->addWidget(sepH);

    buildDormGrid(dormPanel, dormLay);
    midRow->addWidget(dormPanel, 3);

    // Right: Quick Actions + Activity
    auto* rightCol = new QVBoxLayout();
    rightCol->setSpacing(16);

    auto* actPanel = new QWidget(content);
    actPanel->setStyleSheet("background:#161B27; border-radius:12px; border:1px solid #21293A;");
    auto* actLay = new QVBoxLayout(actPanel);
    actLay->setContentsMargins(18, 16, 18, 16);
    actLay->setSpacing(10);
    buildActivityFeed(actPanel, actLay);

    auto* qaPanel = new QWidget(content);
    qaPanel->setStyleSheet("background:#161B27; border-radius:12px; border:1px solid #21293A;");
    auto* qaLay = new QVBoxLayout(qaPanel);
    qaLay->setContentsMargins(18, 16, 18, 16);
    qaLay->setSpacing(10);
    buildQuickActions(qaPanel, qaLay);

    rightCol->addWidget(qaPanel);
    rightCol->addWidget(actPanel, 1);
    midRow->addLayout(rightCol, 2);
    mainLay->addLayout(midRow, 1);

    scroll->setWidget(content);

    auto* rootLay = new QVBoxLayout(this);
    rootLay->setContentsMargins(0, 0, 0, 0);
    rootLay->addWidget(scroll);
}

void DashboardWidget::buildStatCards(QWidget* parent, QHBoxLayout* lay) {
    auto* DM = DataManager::instance();

    m_cardTotal    = new StatCard("TOTAL STUDENTS",    QString::number(DM->totalStudents()),
                                   "Enrolled in system", "👨‍🎓", "#FF6B35", parent);
    m_cardAssigned = new StatCard("ASSIGNED",           QString::number(DM->totalAssigned()),
                                   "Living in dorms",    "🏠", "#2ECC71", parent);
    m_cardPool     = new StatCard("STUDENT POOL",       QString::number(DM->totalInPool()),
                                   "Awaiting assignment","⏳", "#F7931E", parent);
    m_cardOccupancy= new StatCard("OCCUPANCY RATE",     QString::number((int)DM->overallOccupancyRate()) + "%",
                                   "Total dorm capacity","📈", "#3498DB", parent);
    m_cardMaint    = new StatCard("MAINTENANCE",        QString::number(DM->pendingMaintenanceCount()),
                                   "Pending requests",  "🔧", "#E74C3C", parent);
    m_cardStock    = new StatCard("LOW STOCK",          QString::number(DM->lowStockCount()),
                                   "Items need restock","📦", "#9B59B6", parent);

    lay->addWidget(m_cardTotal);
    lay->addWidget(m_cardAssigned);
    lay->addWidget(m_cardPool);
    lay->addWidget(m_cardOccupancy);
    lay->addWidget(m_cardMaint);
    lay->addWidget(m_cardStock);
}

void DashboardWidget::buildDormGrid(QWidget* parent, QLayout* parentLay) {
    auto* DM = DataManager::instance();
    QStringList dormNames = { "Dorm 1 – Block A–E", "Dorm 2 – Block A–E", "Dorm 3 – Block A–E",
                              "Dorm 4 – Block A–E", "Dorm 5 – Block A–E", "Dorm 6 – Block A–E" };
    QStringList colors = { "#FF6B35", "#2ECC71", "#3498DB", "#9B59B6", "#F1C40F", "#E74C3C" };

    for (int i = 1; i <= 6; i++) {
        auto* row = new QWidget(parent);
        row->setStyleSheet("background: transparent;");
        auto* rowLay = new QHBoxLayout(row);
        rowLay->setContentsMargins(0, 0, 0, 0);
        rowLay->setSpacing(12);

        // Dorm label
        auto* nameLbl = new QLabel(dormNames[i-1], row);
        nameLbl->setFixedWidth(160);
        nameLbl->setStyleSheet("font-size: 12px; font-weight: 600; color: #E8EDF5; background: transparent;");
        rowLay->addWidget(nameLbl);

        // Progress bar
        auto* bar = new QProgressBar(row);
        bar->setMinimum(0);
        bar->setMaximum(100);
        int cap = DM->dormCapacity(i);
        int occ = DM->dormOccupancy(i);
        int pct = (cap > 0) ? (occ * 100 / cap) : 0;
        bar->setValue(pct);
        bar->setStyleSheet(QString(
            "QProgressBar { background: #1C2333; border: none; border-radius: 6px; height: 14px; }"
            "QProgressBar::chunk { background: %1; border-radius: 6px; }").arg(colors[i-1]));
        rowLay->addWidget(bar, 1);

        // Count label
        auto* countLbl = new QLabel(QString("%1 / %2").arg(occ).arg(cap), row);
        countLbl->setFixedWidth(90);
        countLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        countLbl->setStyleSheet(QString("font-size: 12px; font-weight: 700; color: %1; background: transparent;").arg(colors[i-1]));
        rowLay->addWidget(countLbl);

        // Pct label
        auto* pctLbl = new QLabel(QString("%1%").arg(pct), row);
        pctLbl->setFixedWidth(40);
        pctLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        pctLbl->setStyleSheet("font-size: 12px; color: #8896A8; background: transparent;");
        rowLay->addWidget(pctLbl);

        m_dormBars.append(bar);
        m_dormLabels.append(countLbl);
        parentLay->addWidget(row);
    }
}

void DashboardWidget::buildActivityFeed(QWidget* parent, QVBoxLayout* lay) {
    auto* titleLbl = new QLabel("⚡  Recent Activity", parent);
    titleLbl->setStyleSheet("font-size: 15px; font-weight: 800; color: #E8EDF5; background: transparent;");
    lay->addWidget(titleLbl);

    m_activityList = new QListWidget(parent);
    m_activityList->setMaximumHeight(220);
    m_activityList->setStyleSheet(
        "QListWidget { background: transparent; border: none; }"
        "QListWidget::item { padding: 8px 4px; border-bottom: 1px solid #1A2030; border-radius: 0; }"
        "QListWidget::item:selected { background: transparent; color: #E8EDF5; }"
        "QListWidget::item:hover { background: rgba(255,107,53,0.05); }");
    m_activityList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    lay->addWidget(m_activityList, 1);
}

void DashboardWidget::buildQuickActions(QWidget* parent, QVBoxLayout* lay) {
    auto* titleLbl = new QLabel("⚡  Quick Actions", parent);
    titleLbl->setStyleSheet("font-size: 15px; font-weight: 800; color: #E8EDF5; background: transparent;");
    lay->addWidget(titleLbl);

    auto* grid = new QGridLayout();
    grid->setSpacing(8);

    struct QA { QString icon; QString label; QString color; };
    QList<QA> actions = {
        {"➕", "New Student",  "#FF6B35"},
        {"🏠", "Assign Room",  "#2ECC71"},
        {"🔧", "Maintenance",  "#E74C3C"},
        {"📢", "Announce",     "#3498DB"},
        {"📊", "Reports",      "#9B59B6"},
        {"💾", "Export Data",  "#F7931E"}
    };

    for (int i = 0; i < actions.size(); i++) {
        auto& a = actions[i];
        auto* btn = new QPushButton(a.icon + "  " + a.label, parent);
        btn->setFixedHeight(40);
        btn->setStyleSheet(QString(
            "QPushButton { background: rgba(%1); border: 1px solid rgba(%2); border-radius: 8px;"
            " color: #E8EDF5; font-size: 12px; font-weight: 600; }"
            "QPushButton:hover { background: %3; color: white; border-color: %3; }")
            .arg("28, 35, 51, 255").arg("33, 41, 58, 255").arg(a.color));
        grid->addWidget(btn, i / 2, i % 2);
    }
    lay->addLayout(grid);
}

void DashboardWidget::refresh() {
    auto* DM = DataManager::instance();

    // Update stat cards
    m_cardTotal->setValue(QString::number(DM->totalStudents()));
    m_cardAssigned->setValue(QString::number(DM->totalAssigned()));
    m_cardPool->setValue(QString::number(DM->totalInPool()));
    m_cardOccupancy->setValue(QString::number((int)DM->overallOccupancyRate()) + "%");
    m_cardMaint->setValue(QString::number(DM->pendingMaintenanceCount()));
    m_cardStock->setValue(QString::number(DM->lowStockCount()));

    // Update dorm bars
    for (int i = 0; i < m_dormBars.size(); i++) {
        int dormNum = i + 1;
        int cap = DM->dormCapacity(dormNum);
        int occ = DM->dormOccupancy(dormNum);
        int pct = (cap > 0) ? (occ * 100 / cap) : 0;
        m_dormBars[i]->setValue(pct);
        m_dormLabels[i]->setText(QString("%1 / %2").arg(occ).arg(cap));
    }

    // Update activity feed
    m_activityList->clear();
    const auto& logs = DM->activityLog();
    int count = qMin(logs.size(), 30);
    for (int i = 0; i < count; i++) {
        const auto& log = logs[i];
        QString typeIcon;
        if (log.type == "student")     typeIcon = "🎓";
        else if (log.type == "maintenance") typeIcon = "🔧";
        else if (log.type == "restaurant")  typeIcon = "🍽️";
        else typeIcon = "⚙️";

        auto* item = new QListWidgetItem(
            QString("[%1] %2 %3").arg(log.timestamp).arg(typeIcon).arg(log.message));
        item->setForeground(QColor("#8896A8"));
        QFont f; f.setPointSize(11);
        item->setFont(f);
        m_activityList->addItem(item);
    }
}