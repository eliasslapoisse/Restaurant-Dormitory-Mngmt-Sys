#pragma once
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>

class StudentRoomWidget : public QWidget {
    Q_OBJECT
public:
    explicit StudentRoomWidget(QWidget* parent = nullptr);
    void refresh();

private:
    QStackedWidget* stack;       // 0=not assigned, 1=assigned
    QLabel*         roomTitle;
    QLabel*         occupancyLabel;
    QLabel*         roommateCard;
    QLabel*         roommateDetail;

    void buildUi();
};
