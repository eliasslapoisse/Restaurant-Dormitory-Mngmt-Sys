#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

class StudentDetailDialog : public QDialog {
    Q_OBJECT
public:
    explicit StudentDetailDialog(int studentId, QWidget* parent = nullptr);

private slots:
    void onSave();
    void onAssignToRoom();
    void onMoveToPool();
    void onExpel();

private:
    int m_studentId;

    QLineEdit* firstNameEdit;
    QLineEdit* familyNameEdit;
    QLineEdit* emailEdit;
    QLineEdit* phoneEdit;
    QComboBox* genderCombo;
    QComboBox* yearCombo;
    QComboBox* assignDormCombo;
    QPushButton* assignBtn;
    QLabel*    idLabel;
    QLabel*    statusLabel;
    QLabel*    locationLabel;
    QLabel*    errorLabel;

    void buildUi();
    void loadStudent();
};
