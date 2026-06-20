#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMap>

class StudentCard;

class StudentPickerDialog : public QDialog {
    Q_OBJECT
public:
    explicit StudentPickerDialog(bool showAssigned = true, QWidget* parent = nullptr);
    int selectedStudentId() const { return m_selectedId; }

private slots:
    void onSearch(const QString& text);
    void onConfirm();

private:
    int           m_selectedId   = -1;
    bool          m_showAssigned;
    QLineEdit*    searchEdit;
    QWidget*      cardsContainer;
    QVBoxLayout*  cardsLayout;
    QPushButton*  selectBtn;
    QMap<int, StudentCard*> m_cards;

    void buildUi();
    void populate(const QString& filter = "");
    void selectCard(int studentId);
};
