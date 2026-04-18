#ifndef RESTAURANTWIDGET_H
#define RESTAURANTWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QSpinBox>
#include <QGroupBox>
#include <QListWidget>

class RestaurantWidget : public QWidget {
    Q_OBJECT
public:
    explicit RestaurantWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onDormChanged(int);
    void onSaveWeeklyMenu();
    void onRecordMeal();
    void onResetDailyMeals();
    void onUpdateHours();

private:
    void buildUi();
    void buildMenuEditor(QWidget* parent, QVBoxLayout* lay);
    void buildMealTracker(QWidget* parent, QVBoxLayout* lay);
    void buildHoursPanel(QWidget* parent, QVBoxLayout* lay);
    void loadMenuToEditor();
    void updateTodayCard();

    QComboBox* m_dormSelect;
    int m_currentDorm = 1;

    // Weekly menu editor: [7 days][3 meals]
    QComboBox* m_menuCombos[7][3]; // [day][meal: 0=breakfast,1=lunch,2=dinner]

    // Today's menu
    QLabel* m_todayBreakfastLbl;
    QLabel* m_todayLunchLbl;
    QLabel* m_todayDinnerLbl;
    QLabel* m_restaurantStatusLbl;

    // Meal tracking
    QSpinBox*  m_trackStudentId;
    QComboBox* m_trackMealType;
    QSpinBox*  m_trackHour;
    QListWidget* m_mealLogList;
    QLabel*    m_breakfastCountLbl;
    QLabel*    m_lunchCountLbl;
    QLabel*    m_dinnerCountLbl;

    // Opening hours
    QSpinBox* m_brOpen; QSpinBox* m_brClose;
    QSpinBox* m_luOpen; QSpinBox* m_luClose;
    QSpinBox* m_diOpen; QSpinBox* m_diClose;
};

#endif // RESTAURANTWIDGET_H