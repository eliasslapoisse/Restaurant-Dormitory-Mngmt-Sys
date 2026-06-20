#include "RestaurantsWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QPushButton>
#include <QScrollArea>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>

RestaurantsWidget::RestaurantsWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
    setAutoFillBackground(true);
    QPalette p = palette(); p.setColor(QPalette::Window, QColor("#F8F9FA")); setPalette(p);
}

void RestaurantsWidget::buildUi() {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // ── Gradient header ────────────────────────────────────────
    auto* header = new QFrame(this);
    header->setFixedHeight(130);
    header->setStyleSheet(
        QString("QFrame { background:%1; border-radius:0; }").arg(Style::gradientCSS("0","0","1","1")));
    auto* hLayout = new QVBoxLayout(header);
    hLayout->setContentsMargins(32, 18, 32, 18);
    hLayout->setSpacing(4);

    auto* sectionTag = new QLabel("UNIVERSITY HOUSING  ·  ADMINISTRATION", header);
    sectionTag->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:11px;"
                              "  letter-spacing:1px; background:transparent; }");
    hLayout->addWidget(sectionTag);

    auto* titleLbl = new QLabel("Restaurants", header);
    titleLbl->setStyleSheet("QLabel { color:white; font-size:34px; font-weight:bold; background:transparent; }");
    hLayout->addWidget(titleLbl);

    auto* subLbl = new QLabel("Daily menus and meal records for each dormitory", header);
    subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.8); font-size:13px; background:transparent; }");
    hLayout->addWidget(subLbl);

    outer->addWidget(header);

    // ── Body: grid of restaurant cards ────────────────────────
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background:#F8F9FA; }");

    auto* gridContainer = new QWidget;
    gridContainer->setAutoFillBackground(true);
    { QPalette gp = gridContainer->palette(); gp.setColor(QPalette::Window, QColor("#F8F9FA")); gridContainer->setPalette(gp); }

    auto* grid = new QGridLayout(gridContainer);
    grid->setContentsMargins(20, 16, 20, 16);
    grid->setSpacing(14);

    for (int i = 0; i < 6; i++) {
        auto* card = makeRestCard(i + 1, i);
        static_cast<QFrame*>(card)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        grid->addWidget(card, i / 3, i % 3);
    }
    grid->setRowStretch(0, 1);
    grid->setRowStretch(1, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 1);
    gridContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    scroll->setWidget(gridContainer);
    outer->addWidget(scroll, 1);
}

QWidget* RestaurantsWidget::makeRestCard(int dormNum, int index) {
    auto* card = new QFrame(this);
    card->setObjectName("restCard");
    card->setStyleSheet("QFrame#restCard { background:white; border-radius:16px; border:none; }");
    card->setMinimumHeight(220);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 20));
    card->setGraphicsEffect(shadow);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 20, 22, 18);
    layout->setSpacing(0);

    // ── Header ────────────────────────────────────────────────
    auto* headerRow = new QHBoxLayout;
    headerRow->setSpacing(12);

    auto* leftHeader = new QVBoxLayout;
    leftHeader->setSpacing(4);

    auto* dormTag = new QLabel(QString("DORM %1").arg(dormNum), card);
    dormTag->setStyleSheet("QLabel { color:#9CA3AF; font-size:10px; font-weight:bold;"
                           "  letter-spacing:1.5px; background:transparent; }");

    auto* titleLbl = new QLabel(QString("Dormitory %1 Restaurant").arg(dormNum), card);
    titleLbl->setStyleSheet("QLabel { font-size:15px; font-weight:bold; color:#111827;"
                            "  background:transparent; }");

    leftHeader->addWidget(dormTag);
    leftHeader->addWidget(titleLbl);

    // ≡ menu icon — represents a restaurant menu
    auto* iconBox = new QLabel("≡", card);
    iconBox->setFixedSize(42, 42);
    iconBox->setAlignment(Qt::AlignCenter);
    iconBox->setStyleSheet(
        "QLabel { background:qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #FF6B35, stop:1 #F7931E);"
        "  border-radius:12px; color:white; font-size:22px; font-weight:bold; }");

    headerRow->addLayout(leftHeader, 1);
    headerRow->addWidget(iconBox, 0, Qt::AlignTop);
    layout->addLayout(headerRow);
    layout->addSpacing(22);

    // ── Meal rows ─────────────────────────────────────────────
    Dormitory* dorm = AppState::instance().university.getDormitory(dormNum);
    if (dorm) {
        const Restaurant& rest = dorm->getRestaurant();
        int restDay = (QDateTime::currentDateTime().date().dayOfWeek() == 7) ? 0
                      : QDateTime::currentDateTime().date().dayOfWeek();
        Restaurant::day d = static_cast<Restaurant::day>(restDay);
        auto menu = rest.getDailyMenu(d);

        // dot color, label
        struct MealStyle { const char* dot; const char* label; };
        const MealStyle styles[] = {
            { "#FF6B35", "Breakfast" },  // warm orange  — morning
            { "#F59E0B", "Lunch"     },  // amber        — midday
            { "#374151", "Dinner"    },  // dark slate   — evening
        };

        auto addMealRow = [&](int si, const QString& dishName, QLabel*& lbl) {
            auto* row = new QHBoxLayout;
            row->setSpacing(10);

            auto* dot = new QLabel(card);
            dot->setFixedSize(12, 12);
            dot->setStyleSheet(QString("QLabel { background:%1; border-radius:6px; }").arg(styles[si].dot));

            auto* mealLbl = new QLabel(QString(styles[si].label) + ":", card);
            mealLbl->setStyleSheet("QLabel { color:#6B7280; font-size:13px; font-weight:600;"
                                   "  background:transparent; }");
            mealLbl->setFixedWidth(76);

            lbl = new QLabel(dishName, card);
            lbl->setStyleSheet("QLabel { color:#374151; font-size:13px; background:transparent; }");

            row->addWidget(dot, 0, Qt::AlignVCenter);
            row->addWidget(mealLbl, 0, Qt::AlignVCenter);
            row->addWidget(lbl, 1, Qt::AlignVCenter);
            layout->addLayout(row);
            layout->addSpacing(14);
        };

        addMealRow(0, QString::fromStdString(Restaurant::getMorningDishName(menu.breakfast)),
                   cards[index].breakfastLabel);
        addMealRow(1, QString::fromStdString(Restaurant::getDishName(menu.lunch)),
                   cards[index].lunchLabel);
        addMealRow(2, QString::fromStdString(Restaurant::getDishName(menu.dinner)),
                   cards[index].dinnerLabel);
    }

    layout->addStretch();

    // ── Footer ────────────────────────────────────────────────
    auto* sep = new QFrame(card);
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    sep->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    layout->addWidget(sep);
    layout->addSpacing(10);

    auto* manageBtn = new QPushButton("Manage  >", card);
    manageBtn->setFixedHeight(40);
    manageBtn->setStyleSheet(
        QString("QPushButton { color:#F7931E; font-size:13px; font-weight:bold;"
                "  background:white; border:2px solid #F7931E; border-radius:10px; }"
                "QPushButton:hover { background:%1; color:white; border:none; }").arg(Style::gradientCSS()));
    manageBtn->setCursor(Qt::PointingHandCursor);
    connect(manageBtn, &QPushButton::clicked, this, [this, dormNum]{
        emit manageRestaurant(dormNum);
    });
    layout->addWidget(manageBtn);

    return card;
}

void RestaurantsWidget::refresh() {
    int dayIdx = (QDateTime::currentDateTime().date().dayOfWeek() == 7) ? 0
                 : QDateTime::currentDateTime().date().dayOfWeek();
    Restaurant::day d = static_cast<Restaurant::day>(dayIdx);

    auto& uni = AppState::instance().university;
    for (int i = 0; i < 6; i++) {
        Dormitory* dorm = uni.getDormitory(i + 1);
        if (!dorm) continue;
        auto menu = dorm->getRestaurant().getDailyMenu(d);
        if (cards[i].breakfastLabel)
            cards[i].breakfastLabel->setText(
                QString::fromStdString(Restaurant::getMorningDishName(menu.breakfast)));
        if (cards[i].lunchLabel)
            cards[i].lunchLabel->setText(
                QString::fromStdString(Restaurant::getDishName(menu.lunch)));
        if (cards[i].dinnerLabel)
            cards[i].dinnerLabel->setText(
                QString::fromStdString(Restaurant::getDishName(menu.dinner)));
    }
}
