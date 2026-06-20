#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QEnterEvent>
#include <QHBoxLayout>

// Small colored square representing one room
class RoomSquare : public QFrame {
    Q_OBJECT
public:
    RoomSquare(const std::string& block, int floor, int roomNum, QWidget* parent = nullptr);
    void refreshColor();
    std::string blockName;
    int         floor;
    int         roomNum;
    int         dormNum = 0;
signals:
    void clicked(const std::string& block, int floor, int roomNum);
protected:
    void mousePressEvent(QMouseEvent*) override;
    void enterEvent(QEnterEvent*) override;
    void leaveEvent(QEvent*) override;
};

// Right-side slide-in panel
class RoomInfoPanel : public QFrame {
    Q_OBJECT
public:
    explicit RoomInfoPanel(QWidget* parent = nullptr);
    void loadRoom(int dormNum, const std::string& block, int floor, int roomNum);
    void close();

signals:
    void dataChanged();

private slots:
    void onAddStudent();
    void onRemoveStudent(int studentId);
    void onClearRoom();

private:
    QLabel*      titleLabel;
    QWidget*     residentsContainer;
    QWidget*     residentList;
    QWidget*     capacityIconRow;
    QHBoxLayout* capacityIconLayout;
    QLabel*      capacityOccLabel;
    QPushButton* addBtn;
    QPushButton* clearBtn;
    QPropertyAnimation* anim;

    int         m_dormNum = 0;
    std::string m_block;
    int         m_floor   = 0;
    int         m_roomNum = 0;

    void buildUi();
    void refreshResidents();
};

// Main dorm detail widget
class DormDetailWidget : public QWidget {
    Q_OBJECT
public:
    explicit DormDetailWidget(QWidget* parent = nullptr);
    void loadDorm(int dormNum);
    void refresh();

signals:
    void backClicked();

private slots:
    void onBlockSelected(const QString& block);
    void onFloorSelected(int floor);
    void onRoomClicked(const std::string& block, int floor, int roomNum);

private:
    int             m_dormNum    = 1;
    QString         m_curBlock   = "A";
    int             m_curFloor   = 0;

    QLabel*         titleLabel;
    QLabel*         residentsLbl;
    QLabel*         availLbl;
    QLabel*         capLbl;
    QWidget*        blockButtonBar;
    QPushButton*    blockBtns[5];
    QWidget*        floorButtonBar;
    QPushButton*    floorBtns[6];
    QWidget*        roomGrid;
    RoomSquare*     roomSquares[6][48]; // [floor][room-1]
    RoomInfoPanel*  infoPanel;

    QLabel*         blockOccLbl;
    QLabel*         blockAvailLbl;
    QLabel*         blockRateLbl;
    QProgressBar*   blockBar;

    void buildUi();
    void refreshRoomGrid();
    void refreshBlockStats();
    void setActiveBlockBtn(int idx);
    void setActiveFloorBtn(int idx);
};
