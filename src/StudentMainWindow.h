#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QShowEvent>

class StudentHomeWidget;

class StudentMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit StudentMainWindow(QWidget* parent = nullptr);
    ~StudentMainWindow() override;

    static StudentMainWindow* instance() { return s_instance; }
    void refresh();

protected:
    void showEvent(QShowEvent* e) override;

private slots:
    void onLogout();

private:
    static StudentMainWindow* s_instance;
    QLabel*            studentNameLabel;
    StudentHomeWidget* dashWidget;
    void buildUi();
};
