#pragma once
#include <QDialog>
#include <QComboBox>

class MenuEditorDialog : public QDialog {
    Q_OBJECT
public:
    explicit MenuEditorDialog(int dormNum, int dayIndex, QWidget* parent = nullptr);

private slots:
    void onSave();

private:
    int        m_dormNum;
    int        m_dayIndex;
    QComboBox* breakfastCombo;
    QComboBox* lunchCombo;
    QComboBox* dinnerCombo;

    void buildUi();
};
