#include <QApplication>
#include <QStackedWidget>
#include <QDir>
#include "loginwindow.h"
#include "mainwindow.h"
#include "studentportalwindow.h"
#include "datamanager.h"

int main(int argc, char *argv[]) {
    // Enable High DPI scaling
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationName("UDRMS");
    app.setStyle("Fusion"); // Base style, overridden by our custom Styles::APP_STYLE

    // Ensure DataManager is initialized
    DataManager::instance();

    // Central stack to manage main views
    QStackedWidget mainStack;
    mainStack.setWindowTitle("UDRMS — ENSIA");
    mainStack.setMinimumSize(1200, 750);
    
    // Windows
    LoginWindow loginWin;
    MainWindow adminWin;
    StudentPortalWindow* studentWin = nullptr;

    mainStack.addWidget(&loginWin);
    mainStack.addWidget(&adminWin);

    // Transitions
    QObject::connect(&loginWin, &LoginWindow::adminLoggedIn, [&]() {
        adminWin.refreshAll();
        mainStack.setCurrentWidget(&adminWin);
    });

    QObject::connect(&loginWin, &LoginWindow::studentLoggedIn, [&](int studentId) {
        if(studentWin) { mainStack.removeWidget(studentWin); delete studentWin; }
        studentWin = new StudentPortalWindow(studentId, &mainStack);
        mainStack.addWidget(studentWin);
        
        QObject::connect(studentWin, &StudentPortalWindow::loggedOut, [&]() {
            mainStack.setCurrentWidget(&loginWin);
            if(studentWin) { mainStack.removeWidget(studentWin); studentWin->deleteLater(); studentWin = nullptr; }
        });
        
        mainStack.setCurrentWidget(studentWin);
    });

    QObject::connect(&adminWin, &MainWindow::loggedOut, [&]() {
        mainStack.setCurrentWidget(&loginWin);
    });

    // Start App
    mainStack.show();

    return app.exec();
}