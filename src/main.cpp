#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "appstate.h"
#include "LoginDialog.h"
#include "AdminMainWindow.h"
#include "StudentMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("UDRMS");

    QFile f(":/styles/styles.qss");
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&f);
        app.setStyleSheet(ts.readAll());
    }


    AppState::instance();

    while (true) {
        LoginDialog login;
        if (login.exec() != QDialog::Accepted)
            break;

        if (AppState::instance().role == AppState::ADMIN) {
            AdminMainWindow w;
            w.show();
            app.exec();
        } else {
            StudentMainWindow w;
            w.show();
            app.exec();
        }
    }

    return 0;
}
