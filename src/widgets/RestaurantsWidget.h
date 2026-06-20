#pragma once
#include <QWidget>
#include <QLabel>

class RestaurantsWidget : public QWidget {
    Q_OBJECT
public:
    explicit RestaurantsWidget(QWidget* parent = nullptr);
    void refresh();

signals:
    void manageRestaurant(int dormNum);

private:
    struct RestCard {
        QLabel* breakfastLabel;
        QLabel* lunchLabel;
        QLabel* dinnerLabel;
    };
    RestCard cards[6];

    void buildUi();
    QWidget* makeRestCard(int dormNum, int index);
};
