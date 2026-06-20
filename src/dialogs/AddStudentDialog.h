#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>

class AddStudentDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddStudentDialog(QWidget* parent = nullptr);

private slots:
    void onFieldChanged();
    void onSubmit();

private:
    QLineEdit*    firstNameEdit;
    QLineEdit*    familyNameEdit;
    QLineEdit*    emailEdit;
    QLineEdit*    phoneEdit;
    QRadioButton* maleRadio;
    QRadioButton* femaleRadio;
    QComboBox*    yearCombo;

    QLabel* fnStatus;
    QLabel* lnStatus;
    QLabel* emailStatus;
    QLabel* phoneStatus;

    QPushButton* submitBtn;

    void buildUi();
    void updateFieldStatus(QLabel* label, bool valid, const QString& msg);
    bool allValid();
};
