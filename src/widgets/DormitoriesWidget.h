#pragma once
#include <QWidget>
#include <QLabel>
#include <QProgressBar>

class DormitoriesWidget : public QWidget {
    Q_OBJECT
public:
    explicit DormitoriesWidget(QWidget* parent = nullptr);
    void refresh();

signals:
    void manageDorm(int dormNum);

private:
    struct DormCard {
        QLabel*       occupancyLabel;
        QProgressBar* bar;
        QLabel*       detailLabel;
        QLabel*       availLabel;
    };
    DormCard cards[6];

    void buildUi();
    QWidget* makeDormCard(int dormNum, int index);
};
