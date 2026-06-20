#include "RestaurantDetailWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../dialogs/MenuEditorDialog.h"
#include "../restaurant.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QAbstractItemView>
#include <QInputDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>

RestaurantDetailWidget::RestaurantDetailWidget(QWidget* parent) : QWidget(parent) {
    setAutoFillBackground(true);
    QPalette p = palette(); p.setColor(QPalette::Window, QColor("#F8F9FA")); setPalette(p);
    buildUi();
}

void RestaurantDetailWidget::buildUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    auto* topSection = new QWidget(this);
    topSection->setStyleSheet("QWidget { background:#F8F9FA; }");
    auto* topL = new QVBoxLayout(topSection);
    topL->setContentsMargins(28, 18, 28, 0);
    topL->setSpacing(2);

    auto* topBar = new QHBoxLayout;
    auto* backBtn = new QPushButton("← Back", topSection);
    backBtn->setFlat(true);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet(
        QString("QPushButton { background:transparent; border:none; color:%1;"
                "  font-size:13px; font-weight:bold; padding:0; }"
                "QPushButton:hover { color:#E65C2A; }").arg(Style::PRIMARY_ORANGE));
    connect(backBtn, &QPushButton::clicked, this, &RestaurantDetailWidget::backClicked);

    titleLabel = new QLabel("Restaurant", topSection);
    titleLabel->setStyleSheet("QLabel { font-size:22px; font-weight:bold; color:#111827; background:transparent; }");

    openStatusLabel = new QLabel("● Closed", topSection);
    openStatusLabel->setStyleSheet(
        "QLabel { background:#FEE2E2; color:#991B1B; font-size:11px; font-weight:600;"
        "  border-radius:10px; padding:4px 12px; }");

    topBar->addWidget(backBtn);
    topBar->addSpacing(12);
    topBar->addWidget(titleLabel);
    topBar->addSpacing(12);
    topBar->addWidget(openStatusLabel);
    topBar->addStretch();
    topL->addLayout(topBar);
    topL->addSpacing(12);

    mainLayout->addWidget(topSection);


    tabs = new QTabWidget(this);
    tabs->setStyleSheet(
        "QTabWidget::pane { border:none; background:#F8F9FA; }"
        "QTabBar::tab { background:transparent; color:#9CA3AF; font-size:13px;"
        "  padding:10px 20px; border:none; border-bottom:2px solid transparent; }"
        "QTabBar::tab:selected { color:#111827; font-weight:bold;"
        "  border-bottom:2px solid #F7931E; }"
        "QTabBar::tab:hover:!selected { color:#374151; }");


    auto* menuTab = new QWidget;
    menuTab->setStyleSheet("QWidget { background:#F8F9FA; }");
    auto* menuLayout = new QVBoxLayout(menuTab);
    menuLayout->setContentsMargins(24, 16, 24, 16);
    menuLayout->setSpacing(0);

    auto* menuCard = new QFrame(menuTab);
    menuCard->setObjectName("menuCard");
    menuCard->setStyleSheet("QFrame#menuCard { background:white; border-radius:16px; border:none; }");
    auto* cardShadow = new QGraphicsDropShadowEffect(menuCard);
    cardShadow->setBlurRadius(32); cardShadow->setOffset(0, 6); cardShadow->setColor(QColor(0,0,0,22));
    menuCard->setGraphicsEffect(cardShadow);
    auto* menuCardLayout = new QVBoxLayout(menuCard);
    menuCardLayout->setContentsMargins(0, 0, 0, 0);
    menuCardLayout->setSpacing(0);


    auto* hdrWidget = new QWidget(menuCard);
    hdrWidget->setStyleSheet("QWidget { background:#F9FAFB; border-radius:16px 16px 0 0; }");
    hdrWidget->setFixedHeight(54);
    auto* hdrLayout = new QHBoxLayout(hdrWidget);
    hdrLayout->setContentsMargins(20, 0, 20, 0);
    hdrLayout->setSpacing(0);

    auto* dayHdr = new QLabel("DAY", hdrWidget);
    dayHdr->setFixedWidth(140);
    dayHdr->setStyleSheet("QLabel { color:#9CA3AF; font-size:10px; font-weight:bold;"
                          "  letter-spacing:1px; background:transparent; }");
    hdrLayout->addWidget(dayHdr);

    // Meal headers: emoji + label
    auto addEmojiHdr = [&](const QString& emoji, const QString& text) {
        auto* w = new QWidget(hdrWidget);
        w->setStyleSheet("QWidget { background:transparent; }");
        auto* hl = new QHBoxLayout(w);
        hl->setContentsMargins(0, 0, 0, 0); hl->setSpacing(7);
        auto* emojiLbl = new QLabel(emoji, w);
        emojiLbl->setStyleSheet("QLabel { font-size:17px; background:transparent; }");
        auto* textLbl = new QLabel(text, w);
        textLbl->setStyleSheet("QLabel { color:#6B7280; font-size:12px; font-weight:bold; background:transparent; }");
        hl->addWidget(emojiLbl); hl->addWidget(textLbl); hl->addStretch();
        hdrLayout->addWidget(w, 1);
    };
    addEmojiHdr("🌅", "Breakfast");
    addEmojiHdr("☀️", "Lunch");
    addEmojiHdr("🌙", "Dinner");

    auto* editHdr = new QLabel("Edit", hdrWidget);
    editHdr->setFixedWidth(60);
    editHdr->setAlignment(Qt::AlignCenter);
    editHdr->setStyleSheet("QLabel { color:#9CA3AF; font-size:10px; font-weight:bold;"
                           "  letter-spacing:1px; background:transparent; }");
    hdrLayout->addWidget(editHdr);
    menuCardLayout->addWidget(hdrWidget);

    // Header/body divider
    auto makeSep = [&](QWidget* parent) {
        auto* s = new QFrame(parent);
        s->setFrameShape(QFrame::HLine);
        s->setFixedHeight(1);
        s->setStyleSheet("QFrame { background:#E5E7EB; border:none; }");
        return s;
    };
    menuCardLayout->addWidget(makeSep(menuCard));


    const char* dayNames[] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };
    for (int d = 0; d < 7; d++) {
        if (d > 0) menuCardLayout->addWidget(makeSep(menuCard));

        auto* row = new QFrame(menuCard);
        row->setObjectName(QString("dayRow_%1").arg(d));
        row->setFrameShape(QFrame::NoFrame);
        row->setFixedHeight(60);
        dayRows[d].rowWidget = row;

        auto* rl = new QHBoxLayout(row);
        rl->setContentsMargins(20, 0, 20, 0);
        rl->setSpacing(0);

        // Day cell
        auto* dayCell = new QWidget(row);
        dayCell->setStyleSheet("QWidget { background:transparent; }");
        dayCell->setFixedWidth(140);
        auto* dcl = new QHBoxLayout(dayCell);
        dcl->setContentsMargins(0,0,0,0); dcl->setSpacing(6);
        auto* dnLbl = new QLabel(dayNames[d], dayCell);
        dnLbl->setStyleSheet("QLabel { font-size:13px; font-weight:bold; color:#111827; background:transparent; }");
        dayRows[d].todayBadge = new QLabel("TODAY", dayCell);
        dayRows[d].todayBadge->setStyleSheet(
            QString("QLabel { color:%1; font-size:9px; font-weight:bold;"
                    "  letter-spacing:0.5px; background:transparent; }").arg(Style::PRIMARY_ORANGE));
        dayRows[d].todayBadge->hide();
        dcl->addWidget(dnLbl);
        dcl->addWidget(dayRows[d].todayBadge);
        dcl->addStretch();
        rl->addWidget(dayCell);

        // Food cells — subtle per-meal color
        struct FoodColor { const char* normal; };
        const FoodColor fc[] = { {"#92400E"}, {"#B45309"}, {"#374151"} };
        auto addFood = [&](QLabel*& lbl, int ci) {
            lbl = new QLabel("–", row);
            lbl->setStyleSheet(
                QString("QLabel { color:%1; font-size:13px; background:transparent; }").arg(fc[ci].normal));
            rl->addWidget(lbl, 1);
        };
        addFood(dayRows[d].bfLabel, 0);
        addFood(dayRows[d].luLabel, 1);
        addFood(dayRows[d].diLabel, 2);

        // Edit cell — visible orange outlined pill
        auto* editCell = new QWidget(row);
        editCell->setFixedWidth(60);
        editCell->setStyleSheet("background:transparent;");
        auto* ecl = new QHBoxLayout(editCell);
        ecl->setContentsMargins(0,0,4,0);
        auto* editBtn = new QPushButton("Edit", row);
        editBtn->setFixedSize(48, 28);
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setStyleSheet(
            QString("QPushButton { background:%1; color:white; border:none; outline:none;"
                    "  border-radius:8px; font-size:11px; font-weight:bold; }"
                    "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                    "  stop:0 #E65C2A, stop:1 #E8A522); }"
                    "QPushButton:focus { outline:none; }")
            .arg(Style::gradientCSS()));
        connect(editBtn, &QPushButton::clicked, this, [this, d]() {
            MenuEditorDialog dlg(m_dormNum, d, this);
            if (dlg.exec() == QDialog::Accepted) refreshMenuTab();
        });
        ecl->addStretch(); ecl->addWidget(editBtn);
        rl->addWidget(editCell);

        menuCardLayout->addWidget(row);
    }
    menuCardLayout->addSpacing(8);
    menuLayout->addWidget(menuCard);
    tabs->addTab(menuTab, "Weekly Menu");


    auto* recordsTab = new QWidget;
    recordsTab->setStyleSheet("QWidget { background:#F8F9FA; }");
    auto* recOuterLayout = new QVBoxLayout(recordsTab);
    recOuterLayout->setContentsMargins(24, 16, 24, 16);
    recOuterLayout->setSpacing(0);

    // Header: date label + Record Meal button
    auto* recHeader = new QHBoxLayout;
    recordsDateLabel = new QLabel("Records for today", recordsTab);
    recordsDateLabel->setStyleSheet("QLabel { color:#6B7280; font-size:13px; background:transparent; }");

    auto* recMealBtn = new QPushButton("  +   Record Meal", recordsTab);
    recMealBtn->setFixedHeight(40);
    recMealBtn->setCursor(Qt::PointingHandCursor);
    recMealBtn->setStyleSheet(
        QString("QPushButton { background:%1; color:white; border:none; border-radius:10px;"
                "  font-size:13px; font-weight:bold; padding:0 20px; }"
                "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "  stop:0 #E65C2A, stop:1 #E8A522); }").arg(Style::gradientCSS()));
    connect(recMealBtn, &QPushButton::clicked, this, [this] {
        // Custom dialog — no QInputDialog (it breaks with our app stylesheet)
        auto* dlg = new QDialog(this);
        dlg->setWindowTitle("Record Meal");
        dlg->setFixedWidth(360);
        auto* dl = new QVBoxLayout(dlg);
        dl->setContentsMargins(24, 20, 24, 20);
        dl->setSpacing(10);

        auto* dlgTitle = new QLabel("Record Meal", dlg);
        dlgTitle->setStyleSheet("QLabel { font-size:15px; font-weight:bold; color:#111827; }");
        dl->addWidget(dlgTitle);
        dl->addSpacing(4);

        auto* mealLbl = new QLabel("MEAL", dlg);
        mealLbl->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF; letter-spacing:1px; }");
        dl->addWidget(mealLbl);

        auto* mealCombo = new QComboBox(dlg);
        mealCombo->addItem(QString("🌅  Breakfast  (%1:00 – %2:00)").arg(Restaurant::getBR_OPEN()).arg(Restaurant::getBR_CLOSE()), 0);
        mealCombo->addItem(QString("☀️  Lunch  (%1:00 – %2:00)").arg(Restaurant::getLU_OPEN()).arg(Restaurant::getLU_CLOSE()), 1);
        mealCombo->addItem(QString("🌙  Dinner  (%1:00 – %2:00)").arg(Restaurant::getDI_OPEN()).arg(Restaurant::getDI_CLOSE()), 2);
        mealCombo->setFixedHeight(42);
        mealCombo->setStyleSheet(
            "QComboBox { border:1.5px solid #E5E7EB; border-radius:8px; padding:8px 12px;"
            "  background:white; color:#111827; font-size:13px; }"
            "QComboBox:focus { border:1.5px solid #FF6B35; }"
            "QComboBox::drop-down { border:none; width:20px; }");
        mealCombo->view()->setStyleSheet(
            "QListView { outline:none; padding:4px; background:white; }"
            "QListView::item { padding:9px 14px; color:#212529; border-radius:7px; margin:1px 4px; }"
            "QListView::item:hover    { background:#FFF3E0; color:#F7931E; }"
            "QListView::item:selected { background:#FFF3E0; color:#F7931E; }");
        dl->addWidget(mealCombo);
        dl->addSpacing(4);

        auto* idLbl = new QLabel("STUDENT ID", dlg);
        idLbl->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF; letter-spacing:1px; }");
        dl->addWidget(idLbl);

        auto* idInput = new QLineEdit(dlg);
        idInput->setPlaceholderText("Enter student ID…");
        idInput->setFixedHeight(42);
        idInput->setStyleSheet(
            "QLineEdit { border:1.5px solid #E5E7EB; border-radius:8px;"
            "  padding:8px 12px; font-size:13px; color:#111827; background:white; }"
            "QLineEdit:focus { border:1.5px solid #FF6B35; }");
        dl->addWidget(idInput);
        dl->addSpacing(12);

        auto* dlgBtnRow = new QHBoxLayout;
        auto* dlgCancel = new QPushButton("Cancel", dlg);
        dlgCancel->setFixedHeight(40);
        dlgCancel->setStyleSheet(Style::secondaryBtnSS());
        dlgCancel->setCursor(Qt::PointingHandCursor);
        auto* dlgRecord = new QPushButton("Record", dlg);
        dlgRecord->setFixedHeight(40);
        dlgRecord->setStyleSheet(Style::primaryBtnSS());
        dlgRecord->setCursor(Qt::PointingHandCursor);
        connect(dlgCancel, &QPushButton::clicked, dlg, &QDialog::reject);
        connect(dlgRecord, &QPushButton::clicked, dlg, [dlg, idInput] {
            if (!idInput->text().trimmed().isEmpty()) dlg->accept();
        });
        dlgBtnRow->addWidget(dlgCancel);
        dlgBtnRow->addWidget(dlgRecord);
        dl->addLayout(dlgBtnRow);

        if (dlg->exec() != QDialog::Accepted) return;

        // Helper: styled error dialog (QMessageBox is broken by app stylesheet)
        auto warn = [this](const QString& title, const QString& msg) {
            auto* dlg = new QDialog(this);
            dlg->setWindowTitle(title);
            dlg->setFixedWidth(320);
            auto* dl = new QVBoxLayout(dlg);
            dl->setContentsMargins(24, 20, 24, 20); dl->setSpacing(14);
            auto* lbl = new QLabel(msg, dlg);
            lbl->setWordWrap(true);
            lbl->setStyleSheet("QLabel { color:#111827; font-size:13px; }");
            auto* okBtn = new QPushButton("OK", dlg);
            okBtn->setFixedHeight(38);
            okBtn->setStyleSheet(Style::primaryBtnSS());
            okBtn->setCursor(Qt::PointingHandCursor);
            connect(okBtn, &QPushButton::clicked, dlg, &QDialog::accept);
            dl->addWidget(lbl); dl->addWidget(okBtn);
            dlg->exec();
        };

        bool ok;
        int studentId = idInput->text().trimmed().toInt(&ok);
        if (!ok) {
            warn("Invalid ID", "Please enter a valid numeric student ID.");
            return;
        }

        int mealType = mealCombo->currentData().toInt();
        Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
        if (!dorm) return;
        Restaurant& rest = dorm->getRestaurant();
        int hour = QTime::currentTime().hour();
        bool recorded = false;
        switch (mealType) {
        case 0: recorded = rest.recordBreakfast(studentId, hour); break;
        case 1: recorded = rest.recordLunch(studentId, hour);     break;
        case 2: recorded = rest.recordDinner(studentId, hour);    break;
        }
        if (!recorded) {
            warn("Failed",
                 "Could not record meal.\n\n"
                 "• Student already recorded for this meal\n"
                 "• Outside meal service hours\n"
                 "• Student ID not found");
        } else {
            refreshRecordsTab();
        }
    });
    recHeader->addWidget(recordsDateLabel, 1);
    recHeader->addWidget(recMealBtn);
    recOuterLayout->addLayout(recHeader);
    recOuterLayout->addSpacing(16);

    // Scroll area
    auto* recScroll = new QScrollArea(recordsTab);
    recScroll->setWidgetResizable(true);
    recScroll->setFrameShape(QFrame::NoFrame);
    recScroll->setStyleSheet("QScrollArea { background:transparent; }");

    auto* recContent = new QWidget;
    recContent->setStyleSheet("QWidget { background:transparent; }");
    auto* recLayout = new QVBoxLayout(recContent);
    recLayout->setContentsMargins(0, 0, 0, 0);
    recLayout->setSpacing(14);

    struct MealDef {
        QString      emoji;
        QString      name;
        QString      hours;
        QListWidget*& list;
        QLabel*&      count;
    };
    MealDef mealDefs[] = {
        { "🌅", "Breakfast", "", breakfastList, bfCountLabel },
        { "☀️", "Lunch",     "", lunchList,     luCountLabel },
        { "🌙", "Dinner",    "", dinnerList,    diCountLabel },
    };

    for (int i = 0; i < 3; i++) {
        auto& m = mealDefs[i];

        auto* card = new QFrame(recContent);
        card->setObjectName("recCard");
        card->setStyleSheet(
            "QFrame#recCard { background:white; border-radius:14px;"
            "  border:1px solid #E5E7EB; }");
        auto* cShadow = new QGraphicsDropShadowEffect(card);
        cShadow->setBlurRadius(16); cShadow->setOffset(0, 3); cShadow->setColor(QColor(0,0,0,14));
        card->setGraphicsEffect(cShadow);

        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(20, 18, 20, 18);
        cl->setSpacing(12);

        // Card header row
        auto* cardHdr = new QHBoxLayout;
        cardHdr->setSpacing(12);

        // Emoji icon
        auto* iconLbl = new QLabel(m.emoji, card);
        iconLbl->setFixedSize(40, 40);
        iconLbl->setAlignment(Qt::AlignCenter);
        iconLbl->setStyleSheet("QLabel { font-size:26px; background:transparent; }");

        // Meal name + hours inline
        auto* mealNameLbl = new QLabel(m.name, card);
        mealNameLbl->setStyleSheet(
            "QLabel { font-size:15px; font-weight:bold; color:#111827; background:transparent; }");

        // Hours label — stored so refresh can update it from backend
        mealHoursLabels[i] = new QLabel("", card);
        mealHoursLabels[i]->setStyleSheet(
            "QLabel { color:#9CA3AF; font-size:13px; background:transparent; }");

        // Count badge — orange rounded square
        m.count = new QLabel("0", card);
        m.count->setFixedSize(30, 30);
        m.count->setAlignment(Qt::AlignCenter);
        m.count->setStyleSheet(
            QString("QLabel { background:%1; color:white; border-radius:8px;"
                    "  font-size:13px; font-weight:bold; }").arg(Style::gradientCSS()));

        cardHdr->addWidget(iconLbl);
        cardHdr->addWidget(mealNameLbl);
        cardHdr->addSpacing(8);
        cardHdr->addWidget(mealHoursLabels[i]);
        cardHdr->addStretch();
        cardHdr->addWidget(m.count);
        cl->addLayout(cardHdr);

        // Student list
        m.list = new QListWidget(card);
        m.list->setFixedHeight(72);
        m.list->setStyleSheet(
            "QListWidget { border:none; background:transparent; }"
            "QListWidget::item { padding:4px 0; font-size:12px; color:#374151; }");
        m.list->setFrameShape(QFrame::NoFrame);
        m.list->hide();
        cl->addWidget(m.list);

        // Empty placeholder
        auto* emptyLbl = new QLabel("No records yet", card);
        emptyLbl->setObjectName("emptyLbl_" + m.name);
        emptyLbl->setStyleSheet(
            "QLabel { color:#9CA3AF; font-size:13px; background:transparent; }");
        cl->addWidget(emptyLbl);

        recLayout->addWidget(card);
    }
    recLayout->addStretch();

    recScroll->setWidget(recContent);
    recOuterLayout->addWidget(recScroll, 1);

    tabs->addTab(recordsTab, "Meal Records");
    mainLayout->addWidget(tabs, 1);
}

void RestaurantDetailWidget::loadRestaurant(int dormNum) {
    m_dormNum = dormNum;
    titleLabel->setText(QString("Dormitory %1 Restaurant").arg(dormNum));
    recordsDateLabel->setText(QString("Records for %1").arg(QDate::currentDate().toString("MMMM d, yyyy")));

    // Open/closed status based on meal hours (from backend, not hardcoded)
    QTime now = QTime::currentTime();
    bool isOpen = false;
    QString statusText = "● Closed";
    if (now >= QTime(Restaurant::getBR_OPEN(),0) && now < QTime(Restaurant::getBR_CLOSE(),0))
        { isOpen = true; statusText = "● Open · Breakfast"; }
    if (now >= QTime(Restaurant::getLU_OPEN(),0) && now < QTime(Restaurant::getLU_CLOSE(),0))
        { isOpen = true; statusText = "● Open · Lunch"; }
    if (now >= QTime(Restaurant::getDI_OPEN(),0) && now < QTime(Restaurant::getDI_CLOSE(),0))
        { isOpen = true; statusText = "● Open · Dinner"; }
    openStatusLabel->setText(statusText);
    openStatusLabel->setStyleSheet(isOpen
        ? "QLabel { background:#D1FAE5; color:#065F46; font-size:11px; font-weight:600;"
          "  border-radius:10px; padding:4px 12px; }"
        : "QLabel { background:#FEE2E2; color:#991B1B; font-size:11px; font-weight:600;"
          "  border-radius:10px; padding:4px 12px; }");

    refresh();
}

void RestaurantDetailWidget::refresh() {
    refreshMenuTab();
    refreshRecordsTab();
}

void RestaurantDetailWidget::refreshMenuTab() {
    Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
    if (!dorm) return;
    const Restaurant& rest = dorm->getRestaurant();
    const Restaurant::dailyMenu* weekly = rest.getWeeklyMenu();
    int today = (QDate::currentDate().dayOfWeek() == 7) ? 0 : QDate::currentDate().dayOfWeek();

    for (int d = 0; d < 7; d++) {
        bool isToday = (d == today);
        dayRows[d].todayBadge->setVisible(isToday);
        dayRows[d].rowWidget->setStyleSheet(
            isToday ? "QFrame { background:#FFF7ED; }"
                    : "QFrame { background:white; }");
        dayRows[d].bfLabel->setText(QString::fromStdString(Restaurant::getMorningDishName(weekly[d].breakfast)));
        dayRows[d].luLabel->setText(QString::fromStdString(Restaurant::getDishName(weekly[d].lunch)));
        dayRows[d].diLabel->setText(QString::fromStdString(Restaurant::getDishName(weekly[d].dinner)));
    }
}

void RestaurantDetailWidget::refreshRecordsTab() {
    Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
    if (!dorm) return;
    const Restaurant& rest = dorm->getRestaurant();

    // Update hour labels from backend (reflects any Settings changes)
    mealHoursLabels[0]->setText(QString("%1:00 – %2:00").arg(Restaurant::getBR_OPEN()).arg(Restaurant::getBR_CLOSE()));
    mealHoursLabels[1]->setText(QString("%1:00 – %2:00").arg(Restaurant::getLU_OPEN()).arg(Restaurant::getLU_CLOSE()));
    mealHoursLabels[2]->setText(QString("%1:00 – %2:00").arg(Restaurant::getDI_OPEN()).arg(Restaurant::getDI_CLOSE()));

    auto fillList = [](QListWidget* list, QLabel* countLbl, QLabel* emptyLbl,
                       const std::vector<int>& ids) {
        list->clear();
        for (int id : ids) {
            const Student* s = AppState::instance().findStudentById(id);
            QString text = s ? QString("%1 %2  (ID: %3)")
                .arg(QString::fromStdString(s->getFirstName()))
                .arg(QString::fromStdString(s->getFamilyName()))
                .arg(id) : QString("Student ID: %1").arg(id);
            list->addItem(text);
        }
        countLbl->setText(QString::number(ids.size()));
        if (ids.empty()) {
            list->hide();
            if (emptyLbl) emptyLbl->show();
        } else {
            list->show();
            if (emptyLbl) emptyLbl->hide();
        }
    };

    // Find the empty labels by name
    QLabel* bfEmpty = tabs->widget(1)->findChild<QLabel*>("emptyLbl_Breakfast");
    QLabel* luEmpty = tabs->widget(1)->findChild<QLabel*>("emptyLbl_Lunch");
    QLabel* diEmpty = tabs->widget(1)->findChild<QLabel*>("emptyLbl_Dinner");

    fillList(breakfastList, bfCountLabel, bfEmpty, rest.getAteBreakfastList());
    fillList(lunchList,     luCountLabel, luEmpty, rest.getAteLunchList());
    fillList(dinnerList,    diCountLabel, diEmpty, rest.getAteDinnerList());
}

bool RestaurantDetailWidget::recordMeal(int mealType) {
    bool ok;
    QString input = QInputDialog::getText(this, "Record Meal",
        "Enter Student ID:", QLineEdit::Normal, "", &ok);
    if (!ok || input.isEmpty()) return false;

    int studentId = input.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Invalid", "Please enter a valid numeric ID.");
        return false;
    }

    Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
    if (!dorm) return false;
    Restaurant& rest = dorm->getRestaurant();

    int hour = QTime::currentTime().hour();
    bool recorded = false;
    switch (mealType) {
    case 0: recorded = rest.recordBreakfast(studentId, hour); break;
    case 1: recorded = rest.recordLunch(studentId, hour);     break;
    case 2: recorded = rest.recordDinner(studentId, hour);    break;
    }

    if (!recorded) {
        auto* dlg = new QDialog(this);
        dlg->setWindowTitle("Failed");
        dlg->setFixedWidth(320);
        auto* dl = new QVBoxLayout(dlg);
        dl->setContentsMargins(24,20,24,20); dl->setSpacing(14);
        auto* lbl = new QLabel("Could not record meal.\n\n"
            "• Student already recorded for this meal\n"
            "• Outside meal service hours\n"
            "• Student ID not found", dlg);
        lbl->setWordWrap(true);
        lbl->setStyleSheet("QLabel { color:#111827; font-size:13px; }");
        auto* okBtn = new QPushButton("OK", dlg);
        okBtn->setFixedHeight(38);
        okBtn->setStyleSheet(Style::primaryBtnSS());
        connect(okBtn, &QPushButton::clicked, dlg, &QDialog::accept);
        dl->addWidget(lbl); dl->addWidget(okBtn);
        dlg->exec();
    }
    return recorded;
}

void RestaurantDetailWidget::onRecordBreakfast() {
    if (recordMeal(0)) refreshRecordsTab();
}
void RestaurantDetailWidget::onRecordLunch() {
    if (recordMeal(1)) refreshRecordsTab();
}
void RestaurantDetailWidget::onRecordDinner() {
    if (recordMeal(2)) refreshRecordsTab();
}
