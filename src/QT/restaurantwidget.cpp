#include "restaurantwidget.h"
#include "datamanager.h"
#include "styles.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QMessageBox>
#include <QDateTime>
#include <QHeaderView>

RestaurantWidget::RestaurantWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void RestaurantWidget::buildUi() {
    auto* rootLay = new QVBoxLayout(this);
    rootLay->setContentsMargins(24, 20, 24, 20);
    rootLay->setSpacing(16);

    // ── Header ──────────────────────────────────────────────────────────────
    auto* headerRow = new QHBoxLayout();
    auto* titleLbl = new QLabel("🍽️  Restaurant & Menu Management", this);
    titleLbl->setStyleSheet("font-size: 22px; font-weight: 900; color: #E8EDF5;");
    headerRow->addWidget(titleLbl);
    headerRow->addStretch();

    auto* dormLbl = new QLabel("Dormitory:", this);
    dormLbl->setStyleSheet("color: #8896A8; font-weight: 600; background: transparent;");
    headerRow->addWidget(dormLbl);
    m_dormSelect = new QComboBox(this);
    for (int i = 1; i <= 6; i++) m_dormSelect->addItem(QString("🏠 Dormitory %1").arg(i), i);
    m_dormSelect->setFixedHeight(40);
    connect(m_dormSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RestaurantWidget::onDormChanged);
    headerRow->addWidget(m_dormSelect);
    rootLay->addLayout(headerRow);

    // ── Today's menu card ────────────────────────────────────────────────────
    auto* todayCard = new QWidget(this);
    todayCard->setStyleSheet(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #1A0E05,stop:1 #0D1117);"
        " border-radius: 12px; border: 1px solid #FF6B35;");
    auto* todayLay = new QHBoxLayout(todayCard);
    todayLay->setContentsMargins(20, 16, 20, 16);
    todayLay->setSpacing(20);

    auto* todayTitleCol = new QVBoxLayout();
    auto* todayTitleLbl = new QLabel("📅 Today's Menu", todayCard);
    todayTitleLbl->setStyleSheet("font-size: 16px; font-weight: 800; color: #FF6B35; background: transparent;");
    auto* todayDateLbl = new QLabel(QDateTime::currentDateTime().toString("dddd, MMMM d"), todayCard);
    todayDateLbl->setStyleSheet("font-size: 12px; color: #8896A8; background: transparent;");
    m_restaurantStatusLbl = new QLabel("", todayCard);
    m_restaurantStatusLbl->setStyleSheet("font-size: 11px; background: transparent;");
    todayTitleCol->addWidget(todayTitleLbl);
    todayTitleCol->addWidget(todayDateLbl);
    todayTitleCol->addWidget(m_restaurantStatusLbl);
    todayLay->addLayout(todayTitleCol);

    auto mkMealCard = [&](const QString& mealIcon, const QString& mealName, QLabel** outLbl) {
        auto* col = new QVBoxLayout();
        col->setSpacing(4);
        auto* iconLbl = new QLabel(mealIcon + " " + mealName, todayCard);
        iconLbl->setStyleSheet("font-size: 12px; font-weight: 700; color: #8896A8; background: transparent;");
        *outLbl = new QLabel("Loading…", todayCard);
        (*outLbl)->setStyleSheet("font-size: 14px; font-weight: 800; color: #E8EDF5; background: transparent;");
        (*outLbl)->setWordWrap(true);
        col->addWidget(iconLbl);
        col->addWidget(*outLbl);
        return col;
    };

    auto* sep1 = new QFrame(todayCard); sep1->setFrameShape(QFrame::VLine);
    sep1->setStyleSheet("background: rgba(255,107,53,0.3); max-width: 1px;");
    auto* sep2 = new QFrame(todayCard); sep2->setFrameShape(QFrame::VLine);
    sep2->setStyleSheet("background: rgba(255,107,53,0.3); max-width: 1px;");

    todayLay->addWidget(sep1);
    todayLay->addLayout(mkMealCard("☀️", "BREAKFAST", &m_todayBreakfastLbl), 1);
    todayLay->addWidget(sep2);
    todayLay->addLayout(mkMealCard("🌤️", "LUNCH", &m_todayLunchLbl), 1);
    auto* sep3 = new QFrame(todayCard); sep3->setFrameShape(QFrame::VLine);
    sep3->setStyleSheet("background: rgba(255,107,53,0.3); max-width: 1px;");
    todayLay->addWidget(sep3);
    todayLay->addLayout(mkMealCard("🌙", "DINNER", &m_todayDinnerLbl), 1);
    rootLay->addWidget(todayCard);

    // ── Tabs ─────────────────────────────────────────────────────────────────
    auto* tabs = new QTabWidget(this);

    // Tab 1: Weekly Menu Editor
    auto* menuPage = new QWidget();
    auto* menuLay  = new QVBoxLayout(menuPage);
    menuLay->setContentsMargins(16, 16, 16, 16);
    menuLay->setSpacing(12);
    buildMenuEditor(menuPage, menuLay);
    tabs->addTab(menuPage, "📋  Weekly Menu");

    // Tab 2: Meal Tracking
    auto* trackPage = new QWidget();
    auto* trackLay  = new QVBoxLayout(trackPage);
    trackLay->setContentsMargins(16, 16, 16, 16);
    trackLay->setSpacing(12);
    buildMealTracker(trackPage, trackLay);
    tabs->addTab(trackPage, "📊  Meal Tracking");

    // Tab 3: Opening Hours
    auto* hoursPage = new QWidget();
    auto* hoursLay  = new QVBoxLayout(hoursPage);
    hoursLay->setContentsMargins(16, 16, 16, 16);
    hoursLay->setSpacing(12);
    buildHoursPanel(hoursPage, hoursLay);
    tabs->addTab(hoursPage, "🕐  Opening Hours");

    rootLay->addWidget(tabs, 1);
    refresh();
}

void RestaurantWidget::buildMenuEditor(QWidget* parent, QVBoxLayout* lay) {
    auto* descLbl = new QLabel("Set the weekly menu for each dormitory restaurant. Changes apply to all residents.", parent);
    descLbl->setStyleSheet("font-size: 12px; color: #8896A8; background: transparent;");
    lay->addWidget(descLbl);

    // Grid: rows = days, cols = breakfast/lunch/dinner
    auto* grid = new QGridLayout();
    grid->setSpacing(10);

    QStringList dayNames = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    QStringList mealNames = {"☀️ Breakfast", "🌤️ Lunch", "🌙 Dinner"};

    // Header row
    grid->addWidget(new QLabel("Day", parent), 0, 0);
    for (int m = 0; m < 3; m++) {
        auto* hdr = new QLabel(mealNames[m], parent);
        hdr->setStyleSheet("font-size: 12px; font-weight: 700; color: #FF6B35; background: transparent;");
        hdr->setAlignment(Qt::AlignCenter);
        grid->addWidget(hdr, 0, m + 1);
    }

    // Morning dish options
    QStringList morningDishes = {"Coffee Milk & Croissant", "Yogurt & Chocolate Bread", "Tea & Chocolate Bar"};
    QStringList mainDishes = {"Couscous","Rechta","Spaghetti","Sardines","Chorba","Lentil Soup",
                              "Chakhchoukha","Loubia","Rice","Tajjine","Tlitli","Mtewem","Jwaz","Fried Chicken"};

    for (int d = 0; d < 7; d++) {
        auto* dayLbl = new QLabel(dayNames[d], parent);
        dayLbl->setStyleSheet("font-size: 12px; font-weight: 700; color: #E8EDF5; background: transparent; min-width: 90px;");
        grid->addWidget(dayLbl, d + 1, 0);

        for (int m = 0; m < 3; m++) {
            m_menuCombos[d][m] = new QComboBox(parent);
            m_menuCombos[d][m]->setFixedHeight(36);
            if (m == 0) {
                for (const auto& dish : morningDishes) m_menuCombos[d][m]->addItem(dish);
            } else {
                for (const auto& dish : mainDishes) m_menuCombos[d][m]->addItem(dish);
            }
            grid->addWidget(m_menuCombos[d][m], d + 1, m + 1);
        }
    }

    auto* scrollWidget = new QWidget(parent);
    scrollWidget->setLayout(grid);
    auto* scroll = new QScrollArea(parent);
    scroll->setWidget(scrollWidget);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background: transparent;");
    lay->addWidget(scroll, 1);

    auto* saveBtn = new QPushButton("💾  Save Weekly Menu", parent);
    saveBtn->setFixedHeight(42);
    connect(saveBtn, &QPushButton::clicked, this, &RestaurantWidget::onSaveWeeklyMenu);
    lay->addWidget(saveBtn);
}

void RestaurantWidget::buildMealTracker(QWidget* parent, QVBoxLayout* lay) {
    // Stats row
    auto* statsRow = new QHBoxLayout();
    statsRow->setSpacing(12);

    auto mkMealStat = [&](const QString& title, const QString& icon, QLabel** out) {
        auto* card = new QWidget(parent);
        card->setStyleSheet("background: #1C2333; border-radius: 10px; border: 1px solid #21293A;");
        auto* cardLay = new QVBoxLayout(card);
        cardLay->setContentsMargins(16, 12, 16, 12);
        auto* t = new QLabel(icon + " " + title, card);
        t->setStyleSheet("font-size: 11px; font-weight: 700; color: #8896A8; background: transparent;");
        *out = new QLabel("0", card);
        (*out)->setStyleSheet("font-size: 28px; font-weight: 900; color: #FF6B35; background: transparent;");
        cardLay->addWidget(t);
        cardLay->addWidget(*out);
        statsRow->addWidget(card, 1);
    };

    mkMealStat("BREAKFAST", "☀️", &m_breakfastCountLbl);
    mkMealStat("LUNCH",     "🌤️", &m_lunchCountLbl);
    mkMealStat("DINNER",    "🌙", &m_dinnerCountLbl);
    lay->addLayout(statsRow);

    // Record meal form
    auto* formGroup = new QGroupBox("Record Meal Access", parent);
    auto* formLay = new QHBoxLayout(formGroup);
    formLay->setSpacing(12);

    auto* idLbl = new QLabel("Student ID:", formGroup);
    idLbl->setStyleSheet("color: #8896A8; background: transparent;");
    m_trackStudentId = new QSpinBox(formGroup);
    m_trackStudentId->setRange(1, 99999);
    m_trackStudentId->setFixedHeight(40);

    auto* mealLbl = new QLabel("Meal:", formGroup);
    mealLbl->setStyleSheet("color: #8896A8; background: transparent;");
    m_trackMealType = new QComboBox(formGroup);
    m_trackMealType->addItems({"☀️ Breakfast", "🌤️ Lunch", "🌙 Dinner"});
    m_trackMealType->setFixedHeight(40);

    auto* hourLbl = new QLabel("Current Hour:", formGroup);
    hourLbl->setStyleSheet("color: #8896A8; background: transparent;");
    m_trackHour = new QSpinBox(formGroup);
    m_trackHour->setRange(0, 23);
    m_trackHour->setValue(QDateTime::currentDateTime().time().hour());
    m_trackHour->setFixedHeight(40);
    m_trackHour->setSuffix(":00");

    auto* recordBtn = new QPushButton("✓ Record", formGroup);
    recordBtn->setFixedHeight(40);
    connect(recordBtn, &QPushButton::clicked, this, &RestaurantWidget::onRecordMeal);

    auto* resetBtn = new QPushButton("🔄 Reset Day", formGroup);
    resetBtn->setProperty("class", "danger");
    resetBtn->setFixedHeight(40);
    connect(resetBtn, &QPushButton::clicked, this, &RestaurantWidget::onResetDailyMeals);

    formLay->addWidget(idLbl); formLay->addWidget(m_trackStudentId);
    formLay->addWidget(mealLbl); formLay->addWidget(m_trackMealType);
    formLay->addWidget(hourLbl); formLay->addWidget(m_trackHour);
    formLay->addWidget(recordBtn);
    formLay->addStretch();
    formLay->addWidget(resetBtn);
    lay->addWidget(formGroup);

    // Meal log
    auto* logLbl = new QLabel("Today's Meal Log:", parent);
    logLbl->setStyleSheet("font-size: 13px; font-weight: 700; color: #E8EDF5; background: transparent;");
    lay->addWidget(logLbl);

    m_mealLogList = new QListWidget(parent);
    m_mealLogList->setStyleSheet(
        "QListWidget { background: #0D1117; border: 1px solid #21293A; border-radius: 10px; }"
        "QListWidget::item { padding: 10px 14px; border-bottom: 1px solid #1A2030; }"
        "QListWidget::item:selected { background: rgba(255,107,53,0.1); }");
    lay->addWidget(m_mealLogList, 1);
}

void RestaurantWidget::buildHoursPanel(QWidget* parent, QVBoxLayout* lay) {
    auto* descLbl = new QLabel("Set meal service hours for all dormitory restaurants (24-hour format).", parent);
    descLbl->setStyleSheet("font-size: 12px; color: #8896A8; background: transparent;");
    lay->addWidget(descLbl);

    auto* grid = new QGridLayout();
    grid->setSpacing(14);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);

    auto mkHourSpin = [&]() {
        auto* s = new QSpinBox(parent);
        s->setRange(0, 24);
        s->setFixedHeight(40);
        s->setSuffix(":00");
        return s;
    };

    struct MealHour { QString name; QString icon; QSpinBox** open; QSpinBox** close; };

    m_brOpen = mkHourSpin(); m_brClose = mkHourSpin();
    m_brOpen->setValue(7);   m_brClose->setValue(9);

    m_luOpen = mkHourSpin(); m_luClose = mkHourSpin();
    m_luOpen->setValue(12);  m_luClose->setValue(14);

    m_diOpen = mkHourSpin(); m_diClose = mkHourSpin();
    m_diOpen->setValue(19);  m_diClose->setValue(21);

    int row = 0;
    for (const auto& [name, icon, open, close] : QList<std::tuple<QString,QString,QSpinBox**,QSpinBox**>>{
        {"Breakfast", "☀️", &m_brOpen, &m_brClose},
        {"Lunch",     "🌤️",&m_luOpen,  &m_luClose},
        {"Dinner",    "🌙",&m_diOpen,  &m_diClose}
    }) {
        auto* nameLbl = new QLabel(icon + " " + name, parent);
        nameLbl->setStyleSheet("font-size: 14px; font-weight: 700; color: #E8EDF5; background: transparent;");
        grid->addWidget(nameLbl, row, 0);
        auto* openLbl = new QLabel("Opens:", parent);
        openLbl->setStyleSheet("color: #8896A8; background: transparent;");
        grid->addWidget(openLbl, row, 1);
        grid->addWidget(*open, row, 2);
        auto* closeLbl = new QLabel("Closes:", parent);
        closeLbl->setStyleSheet("color: #8896A8; background: transparent;");
        grid->addWidget(closeLbl, row, 3);
        grid->addWidget(*close, row, 4);
        row++;
    }
    lay->addLayout(grid);
    lay->addSpacing(10);

    auto* saveHrsBtn = new QPushButton("💾 Save Opening Hours", parent);
    saveHrsBtn->setFixedHeight(42);
    saveHrsBtn->setFixedWidth(220);
    connect(saveHrsBtn, &QPushButton::clicked, this, &RestaurantWidget::onUpdateHours);
    lay->addWidget(saveHrsBtn);
    lay->addStretch();
}

void RestaurantWidget::onDormChanged(int) {
    m_currentDorm = m_dormSelect->currentData().toInt();
    loadMenuToEditor();
    updateTodayCard();
    refresh();
}

void RestaurantWidget::loadMenuToEditor() {
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    const auto* menu = dorm->getRestaurant().getWeeklyMenu();
    for (int d = 0; d < 7; d++) {
        m_menuCombos[d][0]->setCurrentIndex(static_cast<int>(menu[d].breakfast));
        m_menuCombos[d][1]->setCurrentIndex(static_cast<int>(menu[d].lunch));
        m_menuCombos[d][2]->setCurrentIndex(static_cast<int>(menu[d].dinner));
    }
}

void RestaurantWidget::onSaveWeeklyMenu() {
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    Restaurant::dailyMenu days[7];
    for (int d = 0; d < 7; d++) {
        days[d].breakfast = static_cast<Restaurant::morningDish>(m_menuCombos[d][0]->currentIndex());
        days[d].lunch     = static_cast<Restaurant::dish>(m_menuCombos[d][1]->currentIndex());
        days[d].dinner    = static_cast<Restaurant::dish>(m_menuCombos[d][2]->currentIndex());
    }
    dorm->getRestaurant().setWeeklyMenu(days[0],days[1],days[2],days[3],days[4],days[5],days[6]);
    DM->logActivity("restaurant", QString("Weekly menu updated for Dorm %1").arg(m_currentDorm));
    updateTodayCard();
    QMessageBox::information(this, "Saved", "✅ Weekly menu saved for Dormitory " + QString::number(m_currentDorm));
}

void RestaurantWidget::onRecordMeal() {
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    int id   = m_trackStudentId->value();
    int hour = m_trackHour->value();
    int meal = m_trackMealType->currentIndex();

    bool success = false;
    QString mealName;
    if (meal == 0)      { success = dorm->getRestaurant().recordBreakfast(id, hour); mealName = "Breakfast"; }
    else if (meal == 1) { success = dorm->getRestaurant().recordLunch(id, hour);     mealName = "Lunch"; }
    else                { success = dorm->getRestaurant().recordDinner(id, hour);    mealName = "Dinner"; }

    if (success) {
        QString msg = QString("✅ [%1:00] Student #%2 — %3 recorded").arg(hour).arg(id).arg(mealName);
        m_mealLogList->insertItem(0, msg);
        m_mealLogList->item(0)->setForeground(QColor("#2ECC71"));
        DM->logActivity("restaurant", msg);
    } else {
        QString msg = QString("❌ [%1:00] Student #%2 — %3 failed (already eaten or outside hours)")
            .arg(hour).arg(id).arg(mealName);
        m_mealLogList->insertItem(0, msg);
        m_mealLogList->item(0)->setForeground(QColor("#E74C3C"));
    }
    refresh();
}

void RestaurantWidget::onResetDailyMeals() {
    if (QMessageBox::question(this, "Confirm Reset",
        "Reset all daily meal records for this dormitory?\nThis cannot be undone.") != QMessageBox::Yes) return;
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (dorm) {
        dorm->getRestaurant().resetDailyMeals();
        m_mealLogList->clear();
        DM->logActivity("restaurant", QString("Daily meals reset for Dorm %1").arg(m_currentDorm));
        refresh();
        QMessageBox::information(this, "Reset", "Daily meal records have been cleared.");
    }
}

void RestaurantWidget::onUpdateHours() {
    Restaurant::setBR_OPEN(m_brOpen->value());  Restaurant::setBR_CLOSE(m_brClose->value());
    Restaurant::setLU_OPEN(m_luOpen->value());  Restaurant::setLU_CLOSE(m_luClose->value());
    Restaurant::setDI_OPEN(m_diOpen->value());  Restaurant::setDI_CLOSE(m_diClose->value());
    DataManager::instance()->logActivity("restaurant", "Opening hours updated");
    QMessageBox::information(this, "Saved", "✅ Restaurant opening hours updated for all dormitories.");
}

void RestaurantWidget::updateTodayCard() {
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    int dayOfWeek = QDateTime::currentDateTime().date().dayOfWeek() % 7; // Qt: 1=Mon, 7=Sun; we want 0=Sun
    auto menu = dorm->getRestaurant().getDailyMenu(static_cast<Restaurant::day>(dayOfWeek));
    m_todayBreakfastLbl->setText(QString::fromStdString(Restaurant::getMorningDishName(menu.breakfast)));
    m_todayLunchLbl->setText(QString::fromStdString(Restaurant::getDishName(menu.lunch)));
    m_todayDinnerLbl->setText(QString::fromStdString(Restaurant::getDishName(menu.dinner)));

    int hour = QDateTime::currentDateTime().time().hour();
    bool open = dorm->getRestaurant().isOpen(hour);
    m_restaurantStatusLbl->setText(open ? "🟢 Restaurant is currently OPEN" : "🔴 Restaurant is currently CLOSED");
    m_restaurantStatusLbl->setStyleSheet(
        QString("font-size: 12px; font-weight: 700; color: %1; background: transparent;")
            .arg(open ? "#2ECC71" : "#E74C3C"));
}

void RestaurantWidget::refresh() {
    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(m_currentDorm);
    if (!dorm) return;

    const auto& rest = dorm->getRestaurant();
    m_breakfastCountLbl->setText(QString::number(rest.getAteBreakfastList().size()));
    m_lunchCountLbl->setText(QString::number(rest.getAteLunchList().size()));
    m_dinnerCountLbl->setText(QString::number(rest.getAteDinnerList().size()));

    updateTodayCard();
    loadMenuToEditor();
}