#ifndef DORMITORYWIDGET_H
#define DORMITORYWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QSpinBox>
#include <QGroupBox>
#include <QProgressBar>
#include <QGridLayout>

class RoomCell : public QPushButton {
    Q_OBJECT
public:
    RoomCell(int floor, int number, int occupancy, int capacity, QWidget* parent = nullptr);
    void setOccupancy(int occ);
    int  floor()  const { return m_floor; }
    int  number() const { return m_number; }
protected:
    void paintEvent(QPaintEvent*) override;
private:
    int m_floor, m_number, m_occ, m_cap;
};

class DormitoryWidget : public QWidget {
    Q_OBJECT
public:
    explicit DormitoryWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onDormChanged(int idx);
    void onBlockChanged(int idx);
    void onFloorChanged(int floor);
    void onRoomCellClicked(int floor, int number);
    void onMoveStudent();
    void onSwapStudents();
    void onUnassignFromRoom();
    void onShowFloorStats();

private:
    void buildUi();
    void buildDormStats(QWidget* parent, QVBoxLayout* lay);
    void buildRoomGrid();
    void updateDormStats();
    void showRoomDetails(int floor, int number);

    QComboBox* m_dormSelect;
    QComboBox* m_blockSelect;
    QSpinBox*  m_floorSelect;

    QWidget*   m_roomGridWidget;
    QGridLayout* m_roomGrid;
    QList<RoomCell*> m_roomCells;

    QLabel* m_dormNameLbl;
    QLabel* m_dormOccLbl;
    QLabel* m_dormCapLbl;
    QLabel* m_blockOccLbl;
    QProgressBar* m_dormBar;
    QTableWidget* m_residentsTable;

    QLabel* m_selectedRoomLbl;
    QPushButton* m_moveBtn;
    QPushButton* m_swapBtn;
    QPushButton* m_unassignRoomBtn;

    int m_currentDorm  = 1;
    int m_currentFloor = 0;
    QString m_currentBlock = "A";
    int m_selectedFloor  = -1;
    int m_selectedRoom   = -1;
};

#endif // DORMITORYWIDGET_H