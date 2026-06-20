#pragma once
#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QTabWidget>
#include <QPushButton>

class RestaurantDetailWidget : public QWidget {
    Q_OBJECT
public:
    explicit RestaurantDetailWidget(QWidget* parent = nullptr);
    void loadRestaurant(int dormNum);
    void refresh();

signals:
    void backClicked();

private slots:
    void onRecordBreakfast();
    void onRecordLunch();
    void onRecordDinner();

private:
    int m_dormNum = 1;

    QLabel*     titleLabel;
    QLabel*     openStatusLabel;
    QTabWidget* tabs;

    // Menu tab — one struct per day row
    struct DayRow {
        QWidget*     rowWidget;
        QLabel*      todayBadge;
        QLabel*      bfLabel;
        QLabel*      luLabel;
        QLabel*      diLabel;
    };
    DayRow dayRows[7];

    // Records tab
    QLabel*      recordsDateLabel;
    QListWidget* breakfastList;
    QListWidget* lunchList;
    QListWidget* dinnerList;
    QLabel*      bfCountLabel;
    QLabel*      luCountLabel;
    QLabel*      diCountLabel;
    QLabel*      mealHoursLabels[3]; // updated from backend on refresh

    void buildUi();
    void refreshMenuTab();
    void refreshRecordsTab();
    bool recordMeal(int mealType);
};
