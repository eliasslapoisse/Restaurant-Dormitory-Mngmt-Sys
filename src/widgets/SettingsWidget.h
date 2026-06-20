#pragma once
#include <QWidget>
#include <QLabel>
#include <QSpinBox>

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onSaveData();
    void onLoadData();
    void onResetData();
    void onApplyCapacity();
    void onApplyHours();

private:
    QLabel*   lastPathLabel;
    QSpinBox* capacitySpin;
    QSpinBox* brOpenSpin;
    QSpinBox* brCloseSpin;
    QSpinBox* luOpenSpin;
    QSpinBox* luCloseSpin;
    QSpinBox* diOpenSpin;
    QSpinBox* diCloseSpin;

    void buildUi();
};
