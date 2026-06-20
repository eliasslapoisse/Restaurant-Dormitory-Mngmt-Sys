#include "StudentMainWindow.h"
#include "appstate.h"
#include "stylehelper.h"
#include "widgets/StudentHomeWidget.h"
#include "LoginDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QGuiApplication>
#include <QScreen>

StudentMainWindow* StudentMainWindow::s_instance = nullptr;

StudentMainWindow::StudentMainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    s_instance = this;
    setWindowTitle("UDRMS — Student Portal");
    resize(1280, 800);
    if (auto* scr = QGuiApplication::primaryScreen())
        move((scr->availableGeometry().width()  - 1280) / 2,
             (scr->availableGeometry().height() - 800)  / 2);
    buildUi();
}

StudentMainWindow::~StudentMainWindow() {
    if (s_instance == this) s_instance = nullptr;
}

void StudentMainWindow::showEvent(QShowEvent* e) {
    QMainWindow::showEvent(e);
    refresh();
}

void StudentMainWindow::buildUi() {
    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);


    auto* nav = new QFrame(this);
    nav->setFixedHeight(56);
    nav->setStyleSheet("QFrame { background:white; border-bottom:1px solid #E9ECEF; }");
    auto* nl = new QHBoxLayout(nav);
    nl->setContentsMargins(24, 0, 24, 0);
    nl->setSpacing(0);

    auto* logo = new QLabel("UDRMS", nav);
    logo->setFixedSize(88, 34);
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet(
        QString("QLabel { background:%1; color:white; border-radius:8px;"
                "  font-size:15px; font-weight:bold; }").arg(Style::gradientCSS()));
    nl->addWidget(logo);
    nl->addSpacing(20);

    studentNameLabel = new QLabel("", nav);
    studentNameLabel->setStyleSheet(
        "QLabel { color:#6B7280; font-size:13px; background:transparent; }");
    nl->addWidget(studentNameLabel);
    nl->addStretch();

    auto* logoutBtn = new QPushButton("Logout", nav);
    logoutBtn->setFixedHeight(36);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(
        "QPushButton { background:#DC3545; color:white; border:none;"
        "  border-radius:8px; padding:0 22px; font-size:13px; font-weight:bold; }"
        "QPushButton:hover { background:#C82333; }");
    connect(logoutBtn, &QPushButton::clicked, this, &StudentMainWindow::onLogout);
    nl->addWidget(logoutBtn);

    root->addWidget(nav);


    dashWidget = new StudentHomeWidget(this);
    root->addWidget(dashWidget, 1);
}

void StudentMainWindow::refresh() {
    auto& app = AppState::instance();
    if (const Student* s = app.findStudentById(app.currentStudentId))
        studentNameLabel->setText(
            QString("Logged in as  %1 %2")
            .arg(QString::fromStdString(s->getFirstName()))
            .arg(QString::fromStdString(s->getFamilyName())));
    dashWidget->refresh();
}

void StudentMainWindow::onLogout() {
    s_instance = nullptr;
    close();
    auto* login = new LoginDialog;
    login->show();
}
