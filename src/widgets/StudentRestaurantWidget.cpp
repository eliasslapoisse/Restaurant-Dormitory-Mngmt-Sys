#include "StudentRestaurantWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../restaurant.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QStackedWidget>
#include <QGraphicsDropShadowEffect>
#include <QDate>

StudentRestaurantWidget::StudentRestaurantWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

static QGraphicsDropShadowEffect* restShadow(QWidget* w) {
    auto* s = new QGraphicsDropShadowEffect(w);
    s->setBlurRadius(28);
    s->setOffset(0, 5);
    s->setColor(QColor(0, 0, 0, 32));
    return s;
}

void StudentRestaurantWidget::buildUi() {
    setAutoFillBackground(true);
    QPalette p = palette(); p.setColor(QPalette::Window, QColor("#F5F5F5")); setPalette(p);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);


    auto* header = new QFrame(this);
    header->setFixedHeight(130);
    header->setStyleSheet(
        QString("QFrame { background:%1; border-radius:0; }")
        .arg(Style::gradientCSS("0","0","1","1")));
    auto* hl = new QVBoxLayout(header);
    hl->setContentsMargins(32, 18, 32, 18);
    hl->setSpacing(4);
    auto* tagLbl = new QLabel("STUDENT  ·  DINING", header);
    tagLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:11px;"
                          "  letter-spacing:1.5px; font-weight:bold; background:transparent; }");
    auto* titleLbl = new QLabel("Restaurant", header);
    titleLbl->setStyleSheet("QLabel { color:white; font-size:34px; font-weight:bold; background:transparent; }");
    auto* subLbl = new QLabel("Weekly menu and today's meal status", header);
    subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.80); font-size:13px; background:transparent; }");
    hl->addWidget(tagLbl);
    hl->addWidget(titleLbl);
    hl->addWidget(subLbl);
    outerLayout->addWidget(header);


    auto* stack = new QStackedWidget(this);
    stack->setStyleSheet("QStackedWidget { background:#F5F5F5; }");


    notAssignedLabel = new QLabel(
        "You must be assigned to a dormitory\nto view restaurant information.", stack);
    notAssignedLabel->setAlignment(Qt::AlignCenter);
    notAssignedLabel->setStyleSheet(
        "QLabel { color:#9CA3AF; font-size:15px; padding:40px; background:transparent; }");
    stack->addWidget(notAssignedLabel);


    auto* scroll = new QScrollArea(stack);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background:#F5F5F5; border:none; }");

    contentWidget = new QWidget;
    contentWidget->setAutoFillBackground(true);
    { QPalette cp = contentWidget->palette(); cp.setColor(QPalette::Window, QColor("#F5F5F5")); contentWidget->setPalette(cp); }

    auto* cl = new QVBoxLayout(contentWidget);
    cl->setContentsMargins(28, 24, 28, 28);
    cl->setSpacing(20);


    auto makePill = [&](const QString& text, QWidget* parent) {
        auto* lbl = new QLabel(text, parent);
        lbl->setStyleSheet(
            QString("QLabel { color:%1; font-size:10px; font-weight:bold; letter-spacing:1.5px; }")
            .arg(Style::PRIMARY_MID));
        lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        return lbl;
    };


    auto* weekCard = new QFrame(contentWidget);
    weekCard->setStyleSheet("QFrame { background:white; border-radius:16px; border:none; }");
    weekCard->setGraphicsEffect(restShadow(weekCard));

    auto* wl = new QVBoxLayout(weekCard);
    wl->setContentsMargins(24, 20, 24, 24);
    wl->setSpacing(16);

    // Header row
    auto* wHdr = new QHBoxLayout;
    wHdr->addWidget(makePill("THIS WEEK'S MENU", weekCard));
    wHdr->addStretch();
    auto* todayTag = new QLabel("", weekCard);
    todayTag->setStyleSheet("QLabel { color:#9CA3AF; font-size:11px; background:transparent; }");
    todayTag->setText(QString("Today: %1").arg(QDate::currentDate().toString("dddd, MMM d")));
    wHdr->addWidget(todayTag);
    wl->addLayout(wHdr);

    auto* wSep = new QFrame(weekCard);
    wSep->setFrameShape(QFrame::HLine);
    wSep->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    wSep->setFixedHeight(1);
    wl->addWidget(wSep);

    auto* grid = new QHBoxLayout;
    grid->setSpacing(8);

    const char* dayNames[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
    const char* mealInitials[] = { "B","L","D" };
    int todayIdx = (QDate::currentDate().dayOfWeek() == 7) ? 0 : QDate::currentDate().dayOfWeek();

    for (int d = 0; d < 7; d++) {
        auto* dayCard = new QFrame(weekCard);
        bool isToday = (d == todayIdx);
        if (isToday) {
            dayCard->setStyleSheet(
                QString("QFrame { background:%1; border-radius:12px; border:none; }")
                .arg(Style::gradientCSS("0","0","0","1")));
        } else {
            dayCard->setStyleSheet(
                "QFrame { background:#F9FAFB; border-radius:12px; }");
        }

        auto* dl = new QVBoxLayout(dayCard);
        dl->setContentsMargins(8, 10, 8, 10);
        dl->setSpacing(5);

        dayHeaders[d] = new QLabel(dayNames[d], dayCard);
        dayHeaders[d]->setAlignment(Qt::AlignCenter);
        dayHeaders[d]->setStyleSheet(isToday
            ? "QLabel { color:white; font-weight:bold; font-size:11px; background:transparent; }"
            : "QLabel { color:#374151; font-weight:bold; font-size:11px; background:transparent; }");
        dl->addWidget(dayHeaders[d]);

        auto* daySep = new QFrame(dayCard);
        daySep->setFrameShape(QFrame::HLine);
        daySep->setStyleSheet(isToday
            ? "QFrame { background:rgba(255,255,255,0.3); border:none; }"
            : "QFrame { background:#E5E7EB; border:none; }");
        daySep->setFixedHeight(1);
        dl->addWidget(daySep);

        for (int m = 0; m < 3; m++) {
            auto* mRow = new QHBoxLayout;
            mRow->setSpacing(5);
            mRow->setContentsMargins(0,0,0,0);

            auto* dot = new QLabel(mealInitials[m], dayCard);
            dot->setAlignment(Qt::AlignCenter);
            dot->setStyleSheet(isToday
                ? "QLabel { color:rgba(255,255,255,0.70);"
                  "  font-size:9px; font-weight:bold; }"
                : "QLabel { color:#6B7280;"
                  "  font-size:9px; font-weight:bold; }");

            dayMeals[d][m] = new QLabel("—", dayCard);
            dayMeals[d][m]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            dayMeals[d][m]->setWordWrap(true);
            dayMeals[d][m]->setStyleSheet(isToday
                ? "QLabel { color:rgba(255,255,255,0.90); font-size:9px; background:transparent; }"
                : "QLabel { color:#6B7280; font-size:9px; background:transparent; }");
            mRow->addWidget(dot);
            mRow->addWidget(dayMeals[d][m], 1);
            dl->addLayout(mRow);
        }
        grid->addWidget(dayCard, 1);
    }
    wl->addLayout(grid);
    cl->addWidget(weekCard);

    auto* todayCard = new QFrame(contentWidget);
    todayCard->setStyleSheet("QFrame { background:white; border-radius:16px; border:none; }");
    todayCard->setGraphicsEffect(restShadow(todayCard));

    auto* tl = new QVBoxLayout(todayCard);
    tl->setContentsMargins(24, 20, 24, 24);
    tl->setSpacing(16);

    auto* tHdr = new QHBoxLayout;
    tHdr->addWidget(makePill("TODAY'S MEALS", todayCard));
    tHdr->addStretch();
    wl->addSpacing(0);
    tl->addLayout(tHdr);

    auto* tSep = new QFrame(todayCard);
    tSep->setFrameShape(QFrame::HLine);
    tSep->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    tSep->setFixedHeight(1);
    tl->addWidget(tSep);

    auto* mealRow = new QHBoxLayout;
    mealRow->setSpacing(14);

    const char* mealNames[] = { "Breakfast", "Lunch", "Dinner" };

    for (int m = 0; m < 3; m++) {
        auto* mc = new QFrame(todayCard);
        mc->setStyleSheet(
            "QFrame { background:#F9FAFB; border-radius:14px; }");
        auto* mcl = new QVBoxLayout(mc);
        mcl->setContentsMargins(20, 20, 20, 20);
        mcl->setSpacing(8);
        mcl->setAlignment(Qt::AlignTop);

        // Monochrome icon box
        auto* iconBox = new QLabel(mealInitials[m], mc);
        iconBox->setFixedSize(42, 42);
        iconBox->setAlignment(Qt::AlignCenter);
        iconBox->setStyleSheet(
            "QLabel { background:#1F2937; border-radius:11px;"
            "  color:white; font-size:16px; font-weight:bold; }");

        auto* nameL = new QLabel(mealNames[m], mc);
        nameL->setStyleSheet(
            "QLabel { font-size:14px; font-weight:bold; color:#111827; background:transparent; }");

        mealTimeLabels[m] = new QLabel("", mc);
        mealTimeLabels[m]->setStyleSheet(
            "QLabel { font-size:11px; color:#9CA3AF; background:transparent; }");

        auto* divL = new QFrame(mc);
        divL->setFrameShape(QFrame::HLine);
        divL->setStyleSheet("QFrame { background:#E5E7EB; border:none; }");
        divL->setFixedHeight(1);

        mealStatus[m] = new QLabel("—", mc);
        mealStatus[m]->setAlignment(Qt::AlignCenter);
        mealStatus[m]->setStyleSheet(
            "QLabel { font-size:12px; color:#9CA3AF; }");

        mcl->addWidget(iconBox);
        mcl->addWidget(nameL);
        mcl->addWidget(mealTimeLabels[m]);
        mcl->addWidget(divL);
        mcl->addWidget(mealStatus[m]);
        mealRow->addWidget(mc, 1);
    }
    tl->addLayout(mealRow);
    cl->addWidget(todayCard);
    cl->addStretch();

    scroll->setWidget(contentWidget);
    stack->addWidget(scroll);

    notAssignedLabel->setProperty("stack", QVariant::fromValue(stack));
    contentWidget->setProperty("stack",    QVariant::fromValue(stack));

    outerLayout->addWidget(stack, 1);
}

void StudentRestaurantWidget::refresh() {
    auto& app = AppState::instance();
    auto* stack = notAssignedLabel->property("stack").value<QStackedWidget*>();
    if (!stack) return;

    Dormitory* dorm = app.findStudentDorm(app.currentStudentId);
    if (!dorm) { stack->setCurrentIndex(0); return; }
    stack->setCurrentIndex(1);

    const Restaurant& rest = dorm->getRestaurant();
    const Restaurant::dailyMenu* weekly = rest.getWeeklyMenu();

    for (int d = 0; d < 7; d++) {
        dayMeals[d][0]->setText(
            QString::fromStdString(Restaurant::getMorningDishName(weekly[d].breakfast)));
        dayMeals[d][1]->setText(
            QString::fromStdString(Restaurant::getDishName(weekly[d].lunch)));
        dayMeals[d][2]->setText(
            QString::fromStdString(Restaurant::getDishName(weekly[d].dinner)));
    }

    mealTimeLabels[0]->setText(
        QString("%1:00 – %2:00").arg(Restaurant::getBR_OPEN()).arg(Restaurant::getBR_CLOSE()));
    mealTimeLabels[1]->setText(
        QString("%1:00 – %2:00").arg(Restaurant::getLU_OPEN()).arg(Restaurant::getLU_CLOSE()));
    mealTimeLabels[2]->setText(
        QString("%1:00 – %2:00").arg(Restaurant::getDI_OPEN()).arg(Restaurant::getDI_CLOSE()));

    bool ate[] = {
        rest.didStudentEatBreakfast(app.currentStudentId),
        rest.didStudentEatLunch(app.currentStudentId),
        rest.didStudentEatDinner(app.currentStudentId)
    };
    for (int m = 0; m < 3; m++) {
        if (ate[m]) {
            mealStatus[m]->setText("✓  Eaten");
            mealStatus[m]->setStyleSheet(
                "QLabel { font-size:12px; font-weight:bold;"
                "  color:#065F46; }");
        } else {
            mealStatus[m]->setText("Not yet");
            mealStatus[m]->setStyleSheet(
                "QLabel { font-size:12px;"
                "  color:#9CA3AF; }");
        }
    }
}
