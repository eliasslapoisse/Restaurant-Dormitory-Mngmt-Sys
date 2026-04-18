#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>
#include <QPaintEvent>

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget* parent = nullptr);

signals:
    void adminLoggedIn();
    void studentLoggedIn(int studentId);

protected:
    void paintEvent(QPaintEvent* e) override;

private slots:
    void onAdminLogin();
    void onStudentLogin();
    void togglePasswordVisibility();
    void updateClock();
    void switchToAdmin();
    void switchToStudent();

private:
    void buildUi();
    void buildLeftPanel(QWidget* parent);
    void buildRightPanel(QWidget* parent);
    void showError(const QString& msg);

    // Right panel stacked (Admin / Student)
    QStackedWidget* m_formStack;

    // Admin form
    QLineEdit* m_adminUser;
    QLineEdit* m_adminPass;
    QPushButton* m_adminLoginBtn;

    // Student form
    QLineEdit* m_stuId;
    QLineEdit* m_stuPass;
    QPushButton* m_stuLoginBtn;

    // Tab buttons
    QPushButton* m_tabAdmin;
    QPushButton* m_tabStudent;

    // Clock
    QLabel* m_clockLabel;
    QTimer* m_clockTimer;

    QLabel* m_errorLabel;
};

#endif // LOGINWINDOW_H