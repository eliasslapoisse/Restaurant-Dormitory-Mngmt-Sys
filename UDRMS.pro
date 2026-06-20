QT += core gui widgets

CONFIG += c++17

TARGET = UDRMS
TEMPLATE = app

# Suppress deprecation warnings from Qt internals
DEFINES += QT_DEPRECATED_WARNINGS

# Make src/ visible to all files for #include resolution
INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/appstate.cpp \
    src/student.cpp \
    src/room.cpp \
    src/block.cpp \
    src/dormitory.cpp \
    src/restaurant.cpp \
    src/university.cpp \
    src/LoginDialog.cpp \
    src/AdminMainWindow.cpp \
    src/StudentMainWindow.cpp \
    src/widgets/DashboardWidget.cpp \
    src/widgets/StudentPoolWidget.cpp \
    src/widgets/DormitoriesWidget.cpp \
    src/widgets/DormDetailWidget.cpp \
    src/widgets/RestaurantsWidget.cpp \
    src/widgets/RestaurantDetailWidget.cpp \
    src/widgets/MaintenanceWidget.cpp \
    src/widgets/SettingsWidget.cpp \
    src/widgets/StudentHomeWidget.cpp \
    src/widgets/StudentRestaurantWidget.cpp \
    src/widgets/StudentComplaintsWidget.cpp \
    src/widgets/StudentRoomWidget.cpp \
    src/dialogs/AddStudentDialog.cpp \
    src/dialogs/StudentDetailDialog.cpp \
    src/dialogs/AssignToDormDialog.cpp \
    src/dialogs/StudentPickerDialog.cpp \
    src/dialogs/ComplaintDetailDialog.cpp \
    src/dialogs/MenuEditorDialog.cpp

HEADERS += \
    src/appstate.h \
    src/stylehelper.h \
    src/constants.h \
    src/student.h \
    src/room.h \
    src/block.h \
    src/dormitory.h \
    src/restaurant.h \
    src/university.h \
    src/LoginDialog.h \
    src/AdminMainWindow.h \
    src/StudentMainWindow.h \
    src/widgets/DashboardWidget.h \
    src/widgets/StudentPoolWidget.h \
    src/widgets/DormitoriesWidget.h \
    src/widgets/DormDetailWidget.h \
    src/widgets/RestaurantsWidget.h \
    src/widgets/RestaurantDetailWidget.h \
    src/widgets/MaintenanceWidget.h \
    src/widgets/SettingsWidget.h \
    src/widgets/StudentHomeWidget.h \
    src/widgets/StudentRestaurantWidget.h \
    src/widgets/StudentComplaintsWidget.h \
    src/widgets/StudentRoomWidget.h \
    src/dialogs/AddStudentDialog.h \
    src/dialogs/StudentDetailDialog.h \
    src/dialogs/AssignToDormDialog.h \
    src/dialogs/StudentPickerDialog.h \
    src/dialogs/ComplaintDetailDialog.h \
    src/dialogs/MenuEditorDialog.h

RESOURCES += resources/resources.qrc
RC_FILE = resources/resources.rc
