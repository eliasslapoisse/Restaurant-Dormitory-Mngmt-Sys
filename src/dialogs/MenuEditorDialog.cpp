#include "MenuEditorDialog.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "restaurant.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QAbstractItemView>

MenuEditorDialog::MenuEditorDialog(int dormNum, int dayIndex, QWidget* parent)
    : QDialog(parent), m_dormNum(dormNum), m_dayIndex(dayIndex)
{
    setWindowTitle("Edit Weekly Menu");
    setFixedWidth(420);
    buildUi();
}

void MenuEditorDialog::buildUi() {
    const char* dayNames[] = { "Sunday","Monday","Tuesday","Wednesday",
                                "Thursday","Friday","Saturday" };

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(0);

    auto* title = new QLabel(QString("Edit Menu — Dorm %1").arg(m_dormNum), this);
    title->setStyleSheet("QLabel { font-size:16px; font-weight:bold; color:#111827; }");
    layout->addWidget(title);

    auto* dayLbl = new QLabel(dayNames[m_dayIndex], this);
    dayLbl->setStyleSheet(
        QString("QLabel { font-size:13px; color:%1; font-weight:600; }").arg(Style::PRIMARY_ORANGE));
    layout->addWidget(dayLbl);
    layout->addSpacing(20);

    // Fetch current menu for this day
    Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
    Restaurant::dailyMenu currentMenu{};
    if (dorm)
        currentMenu = dorm->getRestaurant().getDailyMenu(
            static_cast<Restaurant::day>(m_dayIndex));

    auto makeMealSection = [&](const QString& dotColor, const QString& labelText, QComboBox*& combo) {
        auto* row = new QHBoxLayout;
        auto* dot = new QLabel(this);
        dot->setFixedSize(10, 10);
        dot->setStyleSheet(QString("QLabel { background:%1; border-radius:5px; }").arg(dotColor));
        auto* lbl = new QLabel(labelText, this);
        lbl->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#6B7280; letter-spacing:1px; }");
        row->addWidget(dot);
        row->addSpacing(6);
        row->addWidget(lbl);
        row->addStretch();
        layout->addLayout(row);
        layout->addSpacing(6);
        combo = new QComboBox(this);
        combo->setFixedHeight(42);
        combo->setStyleSheet(
            "QComboBox { border:1.5px solid #E5E7EB; border-radius:8px; padding:8px 12px;"
            "  background:white; color:#111827; font-size:13px; }"
            "QComboBox:focus { border:1.5px solid #FF6B35; }"
            "QComboBox::drop-down { border:none; width:20px; }");
        combo->view()->setStyleSheet(
            "QListView { outline:none; padding:4px; background:white; }"
            "QListView::item { padding:9px 14px; color:#212529; border-radius:7px; margin:1px 4px; }"
            "QListView::item:hover    { background:#FFF3E0; color:#F7931E; }"
            "QListView::item:selected { background:#FFF3E0; color:#F7931E; }");
        layout->addWidget(combo);
        layout->addSpacing(16);
    };

    const char* dishes[] = {
        "Couscous","Rechta","Spaghetti","Sardines","Chorba",
        "Lentil Soup","Chakhchoukha","Loubia","Rice","Tajjine",
        "Tlitli","Mtewem","Jwaz","Fried Chicken"
    };

    makeMealSection(Style::PRIMARY_ORANGE, "BREAKFAST", breakfastCombo);
    breakfastCombo->addItem("Coffee Milk & Croissant", 0);
    breakfastCombo->addItem("Yogurt & Chocolate Bread", 1);
    breakfastCombo->addItem("Tea & Chocolate Bar", 2);
    breakfastCombo->setCurrentIndex(static_cast<int>(currentMenu.breakfast));

    makeMealSection("#F59E0B", "LUNCH", lunchCombo);
    for (int i = 0; i < 14; i++) lunchCombo->addItem(dishes[i], i);
    lunchCombo->setCurrentIndex(static_cast<int>(currentMenu.lunch));

    makeMealSection("#374151", "DINNER", dinnerCombo);
    for (int i = 0; i < 14; i++) dinnerCombo->addItem(dishes[i], i);
    dinnerCombo->setCurrentIndex(static_cast<int>(currentMenu.dinner));

    auto* btnRow = new QHBoxLayout;
    auto* cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setFixedHeight(42);
    cancelBtn->setStyleSheet(Style::secondaryBtnSS());
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    auto* saveBtn = new QPushButton("Save", this);
    saveBtn->setFixedHeight(42);
    saveBtn->setStyleSheet(Style::primaryBtnSS());
    saveBtn->setCursor(Qt::PointingHandCursor);
    connect(saveBtn, &QPushButton::clicked, this, &MenuEditorDialog::onSave);

    btnRow->addWidget(cancelBtn);
    btnRow->addSpacing(12);
    btnRow->addWidget(saveBtn);
    layout->addLayout(btnRow);
}

void MenuEditorDialog::onSave() {
    Dormitory* dorm = AppState::instance().university.getDormitory(m_dormNum);
    if (!dorm) { reject(); return; }

    Restaurant& rest = dorm->getRestaurant();
    Restaurant::day d = static_cast<Restaurant::day>(m_dayIndex);

    auto breakfast = static_cast<Restaurant::morningDish>(breakfastCombo->currentData().toInt());
    auto lunch     = static_cast<Restaurant::dish>(lunchCombo->currentData().toInt());
    auto dinner    = static_cast<Restaurant::dish>(dinnerCombo->currentData().toInt());

    rest.setDailyMenu(d, breakfast, lunch, dinner);
    AppState::instance().logActivity("MENU_UPDATED",
        QString("Dorm %1 restaurant menu updated").arg(m_dormNum));
    accept();
}
