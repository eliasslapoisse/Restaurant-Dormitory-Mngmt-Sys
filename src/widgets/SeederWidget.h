#pragma once
#include <QWidget>
#include <QSpinBox>
#include <QLabel>

class SeederWidget : public QWidget {
    Q_OBJECT
public:
    explicit SeederWidget(QWidget* parent = nullptr);

private slots:
    void onSeedStudents();
    void onAssignRandom();
    void onClearAll();

private:
    QSpinBox* seedCountSpin;
    QSpinBox* assignCountSpin;
    QLabel*   statusLabel;

    void buildUi();
};
