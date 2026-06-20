#pragma once
#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

class AssignToDormDialog : public QDialog {
    Q_OBJECT
public:
    explicit AssignToDormDialog(int studentId, QWidget* parent = nullptr);

private slots:
    void onDormChanged(int index);
    void onBlockChanged(int index);
    void onFloorChanged(int index);
    void onConfirm();

private:
    int        m_studentId;
    QComboBox* dormCombo;
    QComboBox* blockCombo;
    QComboBox* floorCombo;
    QComboBox* roomCombo;
    QLabel*    infoLabel;
    QPushButton* confirmBtn;

    void buildUi();
    void refreshRoomCombo();
};
