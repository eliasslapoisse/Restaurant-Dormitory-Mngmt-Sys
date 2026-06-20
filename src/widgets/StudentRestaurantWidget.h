#pragma once
#include <QWidget>
#include <QLabel>

class StudentRestaurantWidget : public QWidget {
    Q_OBJECT
public:
    explicit StudentRestaurantWidget(QWidget* parent = nullptr);
    void refresh();

private:
    QLabel* notAssignedLabel;
    QWidget* contentWidget;

    // 7 day cards — just labels we update
    QLabel* dayMeals[7][3]; // [day][0=breakfast,1=lunch,2=dinner]
    QLabel* dayHeaders[7];

    // Today's meal status + time labels (updated from backend on refresh)
    QLabel* mealStatus[3];   // 0=breakfast, 1=lunch, 2=dinner
    QLabel* mealTimeLabels[3];

    void buildUi();
};
