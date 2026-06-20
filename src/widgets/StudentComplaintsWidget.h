#pragma once
#include <QWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>

class StudentComplaintsWidget : public QWidget {
    Q_OBJECT
public:
    explicit StudentComplaintsWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onSubmit();

private:
    QComboBox*   categoryCombo;
    QTextEdit*   descEdit;
    QLabel*      charCountLabel;
    QLabel*      submitStatusLabel;

    QLabel*      complaintsTitle;
    QWidget*     complaintsListContent;
    QVBoxLayout* complaintsListLayout;

    void buildUi();
    void populateHistory();
};
