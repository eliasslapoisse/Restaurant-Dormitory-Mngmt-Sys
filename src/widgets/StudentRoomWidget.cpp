#include "StudentRoomWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QScrollArea>

StudentRoomWidget::StudentRoomWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void StudentRoomWidget::buildUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);

    stack = new QStackedWidget(this);


    auto* unassignedPage = new QWidget;
    auto* uLayout = new QVBoxLayout(unassignedPage);
    uLayout->setAlignment(Qt::AlignCenter);

    auto* waitFrame = new QFrame(unassignedPage);
    waitFrame->setStyleSheet("QFrame { background:white; border-radius:16px; }");
    waitFrame->setFixedSize(360, 240);
    auto* wl = new QVBoxLayout(waitFrame);
    wl->setAlignment(Qt::AlignCenter);
    wl->setSpacing(12);
    auto* waitIcon = new QLabel("🏠", waitFrame);
    waitIcon->setAlignment(Qt::AlignCenter);
    waitIcon->setStyleSheet("QLabel { font-size:48px; background:transparent; }");
    auto* waitTitle = new QLabel("No Room Assigned", waitFrame);
    waitTitle->setAlignment(Qt::AlignCenter);
    waitTitle->setStyleSheet("QLabel { font-size:18px; font-weight:bold; color:#212529; background:transparent; }");
    auto* waitSub = new QLabel("You are currently in the waiting pool.\nA room will be assigned soon.", waitFrame);
    waitSub->setAlignment(Qt::AlignCenter);
    waitSub->setStyleSheet("QLabel { color:#6C757D; font-size:13px; background:transparent; }");
    wl->addWidget(waitIcon);
    wl->addWidget(waitTitle);
    wl->addWidget(waitSub);
    uLayout->addWidget(waitFrame, 0, Qt::AlignCenter);

    stack->addWidget(unassignedPage); // index 0


    auto* assignedPage = new QWidget;
    auto* scroll = new QScrollArea(assignedPage);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto* sc = new QWidget;
    auto* aLayout = new QVBoxLayout(sc);
    aLayout->setContentsMargins(32, 28, 32, 28);
    aLayout->setSpacing(20);

    // Room card
    auto* roomCard = new QFrame(sc);
    roomCard->setStyleSheet(
        QString("QFrame { background:%1; border-radius:14px; }").arg(Style::gradientCSS()));
    roomCard->setFixedHeight(110);
    auto* rcl = new QHBoxLayout(roomCard);
    rcl->setContentsMargins(28, 20, 28, 20);

    auto* roomIcon = new QLabel("🛏", roomCard);
    roomIcon->setStyleSheet("QLabel { font-size:36px; background:transparent; }");

    auto* roomInfo = new QWidget(roomCard);
    roomInfo->setStyleSheet("QWidget { background:transparent; }");
    auto* ril = new QVBoxLayout(roomInfo);
    ril->setContentsMargins(0,0,0,0); ril->setSpacing(4);
    roomTitle = new QLabel("Block — · Floor — · Room —", roomInfo);
    roomTitle->setStyleSheet("QLabel { color:white; font-size:18px; font-weight:bold;"
                             "  background:transparent; }");
    occupancyLabel = new QLabel("— / — occupants", roomInfo);
    occupancyLabel->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:13px;"
                                  "  background:transparent; }");
    ril->addWidget(roomTitle);
    ril->addWidget(occupancyLabel);

    rcl->addWidget(roomIcon);
    rcl->addSpacing(12);
    rcl->addWidget(roomInfo, 1);
    aLayout->addWidget(roomCard);

    // Roommate card
    auto* rmCard = new QFrame(sc);
    rmCard->setStyleSheet("QFrame { background:white; border-radius:12px; }");
    auto* rml = new QVBoxLayout(rmCard);
    rml->setContentsMargins(24, 20, 24, 20);
    rml->setSpacing(12);

    auto* rmTitle = new QLabel("Roommate", rmCard);
    rmTitle->setStyleSheet("QLabel { font-size:15px; font-weight:bold; color:#212529; background:transparent; }");
    rml->addWidget(rmTitle);

    roommateCard = new QLabel("No roommate yet", rmCard);
    roommateCard->setStyleSheet("QLabel { color:#6C757D; font-size:13px; background:transparent; }");
    roommateDetail = new QLabel("", rmCard);
    roommateDetail->setStyleSheet("QLabel { color:#6C757D; font-size:12px; background:transparent; }");
    rml->addWidget(roommateCard);
    rml->addWidget(roommateDetail);

    aLayout->addWidget(rmCard);
    aLayout->addStretch();

    scroll->setWidget(sc);
    auto* alOuter = new QVBoxLayout(assignedPage);
    alOuter->setContentsMargins(0,0,0,0);
    alOuter->addWidget(scroll);

    stack->addWidget(assignedPage); // index 1

    mainLayout->addWidget(stack);
}

void StudentRoomWidget::refresh() {
    auto& app = AppState::instance();
    Dormitory* dorm = app.findStudentDorm(app.currentStudentId);

    if (!dorm) {
        stack->setCurrentIndex(0);
        return;
    }

    stack->setCurrentIndex(1);

    Room* r = dorm->findRoomByStudentID(app.currentStudentId);
    if (!r) return;

    // Find block
    QString blockName;
    for (const auto& b : dorm->getBlocks()) {
        if (b.isStudentIn(app.currentStudentId)) {
            blockName = QString::fromStdString(b.getName());
            break;
        }
    }

    roomTitle->setText(
        QString("Block %1  ·  Floor %2  ·  Room %3")
        .arg(blockName).arg(r->getFloor()).arg(r->getNumber()));
    occupancyLabel->setText(
        QString("%1 / %2 occupant%3")
        .arg(r->getCurrentOccupancy())
        .arg(Room::getCapacity())
        .arg(Room::getCapacity() != 1 ? "s" : ""));

    // Roommate
    QStringList mates;
    for (const auto& s : r->getResidents()) {
        if (s.getId() != app.currentStudentId) {
            mates << QString("• %1 %2  (Year %3, %4)")
                .arg(QString::fromStdString(s.getFirstName()))
                .arg(QString::fromStdString(s.getFamilyName()))
                .arg(s.getAcademicYear())
                .arg(QString::fromStdString(s.getGender()) == "M" ? "Male" : "Female");
        }
    }

    if (mates.isEmpty()) {
        roommateCard->setText("No roommate yet");
        roommateDetail->clear();
    } else {
        roommateCard->setText("You have a roommate:");
        roommateDetail->setText(mates.join("\n"));
    }
}
