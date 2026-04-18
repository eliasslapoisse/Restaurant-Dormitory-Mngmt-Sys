QT += core gui widgets

CONFIG += c++17
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UDRMS
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    student.cpp \
    room.cpp \
    block.cpp \
    dormitory.cpp \
    restaurant.cpp \
    studentswidget.cpp \
    university.cpp \
    datamanager.cpp \
    loginwindow.cpp \
    mainwindow.cpp \
    dashboardwidget.cpp \
    dormitorywidget.cpp \
    restaurantwidget.cpp \
    maintenancewidget.cpp \
    inventorywidget.cpp \
    studentportalwindow.cpp

HEADERS += \
    student.h \
    room.h \
    block.h \
    dormitory.h \
    restaurant.h \
    studentswidget.h \
    university.h \
    constants.h \
    styles.h \
    datamanager.h \
    loginwindow.h \
    mainwindow.h \
    dashboardwidget.h \
    studentswidget.h \
    dormitorywidget.h \
    restaurantwidget.h \
    maintenancewidget.h \
    inventorywidget.h \
    studentportalwindow.h

QMAKE_CXXFLAGS += -std=c++17