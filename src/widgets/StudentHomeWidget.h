#pragma once
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QScrollArea>

class StudentHomeWidget : public QWidget {
    Q_OBJECT
public:
    explicit StudentHomeWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onSaveInfo();
    void onSubmitComplaint();

private:
    // Banner
    QLabel* avatarLabel;
    QLabel* nameLabel;
    QLabel* idBadge, *yearBadge, *genderBadge;
    QLabel* assignBannerLabel;   // shows dorm info or "In Pool"

    // My Profile
    QLineEdit* emailEdit;
    QLineEdit* phoneEdit;
    QLabel*    saveStatusLabel;

    // Today's Meals
    QLabel* mealDishLabels[3];
    QLabel* mealTimeLabels[3];
    QLabel* mealStatusBadges[3];
    QWidget* mealsContent;      // shown when assigned
    QLabel*  mealsEmptyLabel;   // shown when not assigned

    // Weekly menu
    QLabel* dayMeals[7][3];
    QLabel* dayHeaders[7];
    QWidget* weeklyContent;     // shown when assigned
    QLabel*  weeklyEmptyLabel;  // shown when not assigned

    // Complaints
    QComboBox*   categoryCombo;
    QTextEdit*   descEdit;
    QLabel*      submitStatusLabel;
    QLabel*      complaintsCountLabel;
    QWidget*     complaintsListContent;
    QVBoxLayout* complaintsListLayout;

    void buildUi();
    void populateComplaints();
};
