#include "DormitoriesWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QResizeEvent>

// Re-tiles children into 1/2/3 columns based on available width
class ResponsiveDormGrid : public QWidget {
    QGridLayout* m_grid;
    QList<QWidget*> m_cards;
    int m_cols = 3;
public:
    explicit ResponsiveDormGrid(QWidget* p = nullptr) : QWidget(p) {
        m_grid = new QGridLayout(this);
        m_grid->setContentsMargins(24, 20, 24, 24);
        m_grid->setSpacing(18);
        setAutoFillBackground(true);
        QPalette pal = palette();
        pal.setColor(QPalette::Window, QColor("#F8F9FA"));
        setPalette(pal);
    }
    void addCard(QWidget* card) {
        m_cards.append(card);
        relayout(m_cols);
    }
private:
    void relayout(int cols) {
        m_cols = cols;
        for (auto* c : m_cards) m_grid->removeWidget(c);
        for (int i = 0; i < m_cards.size(); i++)
            m_grid->addWidget(m_cards[i], i / cols, i % cols);
        // stretch the last row
        for (int r = 0; r <= m_cards.size() / cols; r++)
            m_grid->setRowStretch(r, 0);
    }
protected:
    void resizeEvent(QResizeEvent* e) override {
        QWidget::resizeEvent(e);
        int w = e->size().width();
        int cols = w >= 860 ? 3 : w >= 560 ? 2 : 1;
        if (cols != m_cols) relayout(cols);
    }
};

DormitoriesWidget::DormitoriesWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
    setAutoFillBackground(true);
    QPalette p = palette(); p.setColor(QPalette::Window, QColor("#F8F9FA")); setPalette(p);
}

void DormitoriesWidget::buildUi() {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);


    auto* header = new QFrame(this);
    header->setFixedHeight(150);
    header->setStyleSheet(
        QString("QFrame { background:%1; border-radius:0; }").arg(Style::gradientCSS("0","0","1","1")));
    auto* hLayout = new QVBoxLayout(header);
    hLayout->setContentsMargins(32, 22, 32, 22);
    hLayout->setSpacing(4);

    auto* topRow = new QHBoxLayout;
    auto* sectionTag = new QLabel("HOUSING", header);
    sectionTag->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:10px;"
                              "  letter-spacing:2px; background:transparent; }");
    topRow->addWidget(sectionTag);
    topRow->addStretch();

    hLayout->addLayout(topRow);

    auto* titleLbl = new QLabel("Dormitories", header);
    titleLbl->setStyleSheet("QLabel { color:white; font-size:38px; font-weight:bold; background:transparent; }");
    hLayout->addWidget(titleLbl);

    auto* subLbl = new QLabel("6 residential buildings under management", header);
    subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.80); font-size:13px; background:transparent; }");
    hLayout->addWidget(subLbl);

    outer->addWidget(header);


    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background:#F8F9FA; }");

    auto* dormGrid = new ResponsiveDormGrid;
    for (int i = 0; i < 6; i++)
        dormGrid->addCard(makeDormCard(i + 1, i));

    scroll->setWidget(dormGrid);
    outer->addWidget(scroll, 1);
}

QWidget* DormitoriesWidget::makeDormCard(int dormNum, int index) {
    auto* card = new QFrame(this);
    card->setObjectName("dormCard");
    card->setStyleSheet("QFrame#dormCard { background:white; border-radius:16px; border:1px solid #EAECF0; }");
    card->setMinimumHeight(230);
    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 5);
    shadow->setColor(QColor(0, 0, 0, 22));
    card->setGraphicsEffect(shadow);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 18, 22, 18);
    layout->setSpacing(0);


    auto* topRow = new QHBoxLayout;
    auto* dormTag = new QLabel("DORMITORY", card);
    dormTag->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF;"
                           "  letter-spacing:1.5px; background:transparent; }");
    topRow->addWidget(dormTag, 1);

    auto* numBadge = new QLabel(QString::number(dormNum), card);
    numBadge->setFixedSize(36, 36);
    numBadge->setAlignment(Qt::AlignCenter);
    numBadge->setStyleSheet(
        QString("QLabel { background:%1; color:white; border-radius:10px;"
                "  font-size:15px; font-weight:bold; }").arg(Style::gradientCSS()));
    topRow->addWidget(numBadge);
    layout->addLayout(topRow);
    layout->addSpacing(6);


    auto* numLbl = new QLabel(QString::number(dormNum), card);
    numLbl->setStyleSheet(
        QString("QLabel { font-size:42px; font-weight:bold; color:%1; background:transparent; }")
        .arg(Style::PRIMARY_ORANGE));
    layout->addWidget(numLbl);

    layout->addSpacing(14);


    Dormitory* dorm = AppState::instance().university.getDormitory(dormNum);
    int cap  = dorm ? dorm->getTotalCapacity()  : 0;
    int occ  = dorm ? dorm->getTotalOccupancy() : 0;
    double rate = cap > 0 ? (occ * 100.0 / cap) : 0.0;

    auto* occRow = new QHBoxLayout;
    auto* occTitle = new QLabel("Occupancy", card);
    occTitle->setStyleSheet("QLabel { color:#9CA3AF; font-size:11px; background:transparent; }");
    cards[index].occupancyLabel = new QLabel(QString("%1%").arg((int)rate), card);
    cards[index].occupancyLabel->setStyleSheet(
        "QLabel { color:#6B7280; font-size:11px; font-weight:bold; background:transparent; }");
    occRow->addWidget(occTitle);
    occRow->addStretch();
    occRow->addWidget(cards[index].occupancyLabel);
    layout->addLayout(occRow);
    layout->addSpacing(5);

    cards[index].bar = new QProgressBar(card);
    cards[index].bar->setRange(0, 100);
    cards[index].bar->setValue((int)rate);
    cards[index].bar->setFixedHeight(7);
    cards[index].bar->setTextVisible(false);
    cards[index].bar->setStyleSheet(
        QString("QProgressBar { background:#F3F4F6; border-radius:4px; border:none; }"
                "QProgressBar::chunk { background:%1; border-radius:4px; }").arg(Style::gradientCSS()));
    layout->addWidget(cards[index].bar);
    layout->addSpacing(14);


    auto* statsRow = new QHBoxLayout;
    statsRow->setSpacing(0);

    auto* resBlock = new QVBoxLayout;
    resBlock->setSpacing(2);
    cards[index].detailLabel = new QLabel(QString("%1/%2").arg(occ).arg(cap), card);
    cards[index].detailLabel->setStyleSheet(
        "QLabel { color:#111827; font-size:15px; font-weight:bold; background:transparent; }");
    auto* resLbl = new QLabel("residents", card);
    resLbl->setStyleSheet("QLabel { color:#9CA3AF; font-size:11px; background:transparent; }");
    resBlock->addWidget(cards[index].detailLabel);
    resBlock->addWidget(resLbl);
    statsRow->addLayout(resBlock);
    statsRow->addStretch();

    auto availRooms = dorm ? dorm->getAvailableRooms() : std::vector<Room*>{};
    auto* roomBlock = new QVBoxLayout;
    roomBlock->setSpacing(2);
    cards[index].availLabel = new QLabel(QString::number(availRooms.size()), card);
    cards[index].availLabel->setStyleSheet(
        "QLabel { color:#10B981; font-size:15px; font-weight:bold; background:transparent; }");
    auto* roomLbl = new QLabel("available", card);
    roomLbl->setStyleSheet("QLabel { color:#9CA3AF; font-size:11px; background:transparent; }");
    roomBlock->addWidget(cards[index].availLabel);
    roomBlock->addWidget(roomLbl);
    statsRow->addLayout(roomBlock);

    layout->addLayout(statsRow);
    layout->addSpacing(16);


    auto* sep = new QFrame(card);
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    sep->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    layout->addWidget(sep);
    layout->addSpacing(12);


    auto* manageBtn = new QPushButton("Manage  ›", card);
    manageBtn->setFixedHeight(36);
    manageBtn->setCursor(Qt::PointingHandCursor);
    manageBtn->setStyleSheet(
        QString("QPushButton { background:transparent; border:2px solid #F7931E;"
                "  border-radius:9px; color:#F7931E; font-size:13px; font-weight:bold;"
                "  padding:0 16px; text-align:left; }"
                "QPushButton:hover { background:%1; color:white; border:none; padding:0 18px; }"
                "QPushButton:pressed { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "  stop:0 #E65C2A, stop:1 #E8A522); color:white; border:none; }")
        .arg(Style::gradientCSS()));
    connect(manageBtn, &QPushButton::clicked, this, [this, dormNum]{
        emit manageDorm(dormNum);
    });
    layout->addWidget(manageBtn);

    return card;
}

void DormitoriesWidget::refresh() {
    auto& uni = AppState::instance().university;
    for (int i = 0; i < 6; i++) {
        int dormNum = i + 1;
        Dormitory* dorm = uni.getDormitory(dormNum);
        if (!dorm) continue;

        int cap = dorm->getTotalCapacity();
        int occ = dorm->getTotalOccupancy();
        double rate = cap > 0 ? (occ * 100.0 / cap) : 0.0;

        cards[i].occupancyLabel->setText(QString("%1%").arg((int)rate));
        cards[i].bar->setValue((int)rate);
        cards[i].detailLabel->setText(QString("%1/%2 residents").arg(occ).arg(cap));
        cards[i].availLabel->setText(QString("%1 rooms available").arg(dorm->getAvailableRooms().size()));
    }
}
