#include "AssignToDormDialog.h"
#include "../appstate.h"
#include "../stylehelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QAbstractItemView>
#include <QPainter>
#include <QPainterPath>

// Custom-painted combo — same approach as StudentDetailDialog / AddStudentDialog
class StyledDropdown : public QComboBox {
    bool _hov = false;
public:
    explicit StyledDropdown(QWidget* p = nullptr) : QComboBox(p) {
        view()->setStyleSheet(
            "QListView { outline:none; padding:4px; background:white; }"
            "QListView::item { padding:9px 16px; color:#212529; border-radius:7px; margin:1px 4px; }"
            "QListView::item:hover { background:#FFF3E0; color:#F7931E; }"
            "QListView::item:selected { background:#F7931E; color:white; }");
        setStyleSheet("QComboBox QAbstractItemView { border:1px solid #E9ECEF; background:white; }");
    }
    QSize sizeHint()        const override { return {160, 38}; }
    QSize minimumSizeHint() const override { return {100, 38}; }
protected:
    void enterEvent(QEnterEvent* e) override { _hov = true;  update(); QComboBox::enterEvent(e); }
    void leaveEvent(QEvent*       e) override { _hov = false; update(); QComboBox::leaveEvent(e); }
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        bool hot = view()->isVisible() || _hov;
        p.setPen(QPen(QColor(hot ? "#F7931E" : "#DEE2E6"), hot ? 1.5 : 1.0));
        p.setBrush(Qt::white);
        p.drawRoundedRect(QRectF(rect()).adjusted(0.5,0.5,-0.5,-0.5), 8, 8);
        p.setPen(QColor("#212529"));
        QFont f = font(); f.setPixelSize(13); p.setFont(f);
        p.drawText(rect().adjusted(12, 0, -30, 0), Qt::AlignVCenter | Qt::AlignLeft, currentText());
        const int cx = rect().right() - 15;
        const int cy = rect().center().y();
        p.setPen(QPen(hot ? QColor("#F7931E") : QColor("#9CA3AF"),
                      1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        QPolygon ch;
        ch << QPoint(cx-4, cy-2) << QPoint(cx, cy+3) << QPoint(cx+4, cy-2);
        p.drawPolyline(ch);
    }
};


AssignToDormDialog::AssignToDormDialog(int studentId, QWidget* parent)
    : QDialog(parent), m_studentId(studentId)
{
    setWindowTitle("Assign Student to Room");
    setFixedWidth(400);
    buildUi();
}

void AssignToDormDialog::buildUi() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(14);

    auto* title = new QLabel("Assign to Room", this);
    title->setStyleSheet("QLabel { font-size:16px; font-weight:bold; color:#212529; }");
    layout->addWidget(title);

    const Student* s = AppState::instance().findStudentById(m_studentId);
    if (s) {
        auto* nameLbl = new QLabel(
            QString("Student: <b>%1 %2</b>  (ID: %3)")
            .arg(QString::fromStdString(s->getFirstName()))
            .arg(QString::fromStdString(s->getFamilyName()))
            .arg(s->getId()), this);
        nameLbl->setStyleSheet("QLabel { color:#6C757D; font-size:12px; }");
        layout->addWidget(nameLbl);
    }

    auto addCombo = [&](const QString& lbl, QComboBox*& combo) {
        auto* l = new QLabel(lbl, this);
        l->setStyleSheet("QLabel { font-size:11px; font-weight:bold; color:#6C757D;"
                         "  letter-spacing:0.5px; }");
        combo = new StyledDropdown(this);
        layout->addWidget(l);
        layout->addWidget(combo);
    };

    addCombo("DORMITORY", dormCombo);
    addCombo("BLOCK",     blockCombo);
    addCombo("FLOOR",     floorCombo);
    addCombo("ROOM",      roomCombo);

    infoLabel = new QLabel("", this);
    infoLabel->setStyleSheet("QLabel { color:#6C757D; font-size:12px; }");
    layout->addWidget(infoLabel);

    auto* btnRow = new QHBoxLayout;
    auto* cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setFixedHeight(40);
    cancelBtn->setStyleSheet(Style::secondaryBtnSS());
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    confirmBtn = new QPushButton("Assign", this);
    confirmBtn->setFixedHeight(40);
    confirmBtn->setStyleSheet(Style::primaryBtnSS());
    confirmBtn->setCursor(Qt::PointingHandCursor);
    confirmBtn->setEnabled(false);
    connect(confirmBtn, &QPushButton::clicked, this, &AssignToDormDialog::onConfirm);

    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(confirmBtn);
    layout->addLayout(btnRow);

    // Populate dormitory combo
    auto& uni = AppState::instance().university;
    for (const auto& dorm : uni.getDormitories()) {
        dormCombo->addItem(
            QString("Dorm %1  (%2 available)")
            .arg(dorm.getNumber())
            .arg(dorm.getAvailableSpace()),
            dorm.getNumber());
    }

    blockCombo->addItems({ "A", "B", "C", "D", "E" });

    for (int f = 0; f <= 5; f++)
        floorCombo->addItem(QString("Floor %1").arg(f), f);

    connect(dormCombo,  qOverload<int>(&QComboBox::currentIndexChanged), this, &AssignToDormDialog::onDormChanged);
    connect(blockCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &AssignToDormDialog::onBlockChanged);
    connect(floorCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &AssignToDormDialog::onFloorChanged);
    connect(roomCombo,  qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int idx) {
        confirmBtn->setEnabled(idx >= 0 && roomCombo->count() > 0);
    });

    refreshRoomCombo();
}

void AssignToDormDialog::onDormChanged(int)  { refreshRoomCombo(); }
void AssignToDormDialog::onBlockChanged(int) { refreshRoomCombo(); }
void AssignToDormDialog::onFloorChanged(int) { refreshRoomCombo(); }

void AssignToDormDialog::refreshRoomCombo() {
    roomCombo->clear();
    confirmBtn->setEnabled(false);

    int dormNum  = dormCombo->currentData().toInt();
    QString block = blockCombo->currentText();
    int floor    = floorCombo->currentData().toInt();

    auto& uni = AppState::instance().university;
    Dormitory* dorm = uni.getDormitory(dormNum);
    if (!dorm) return;

    Block* b = dorm->getBlock(block.toStdString());
    if (!b) return;

    auto rooms = b->getRoomsByFloor(floor);
    for (Room* r : rooms) {
        if (!r) continue;
        int occ = r->getCurrentOccupancy();
        int cap = Room::getCapacity();
        if (occ < cap) {
            roomCombo->addItem(
                QString("Room %1  (%2/%3)").arg(r->getNumber()).arg(occ).arg(cap),
                r->getNumber());
        }
    }

    if (roomCombo->count() == 0) {
        infoLabel->setText("No available rooms on this floor.");
        infoLabel->setStyleSheet("QLabel { color:#DC3545; font-size:12px; }");
    } else {
        infoLabel->setText(QString("%1 available room(s)").arg(roomCombo->count()));
        infoLabel->setStyleSheet("QLabel { color:#28A745; font-size:12px; }");
        confirmBtn->setEnabled(true);
    }
}

void AssignToDormDialog::onConfirm() {
    int dormNum   = dormCombo->currentData().toInt();
    QString block = blockCombo->currentText();
    int floor     = floorCombo->currentData().toInt();
    int roomNum   = roomCombo->currentData().toInt();

    if (AppState::instance().assignStudentToRoom(m_studentId, dormNum,
                                                  block.toStdString(), floor, roomNum)) {
        const Student* s = AppState::instance().findStudentById(m_studentId);
        AppState::instance().logActivity("STUDENT_ASSIGNED",
            s ? QString("%1 %2 assigned to Dorm %3, Block %4, Floor %5, Room %6")
                    .arg(QString::fromStdString(s->getFirstName()))
                    .arg(QString::fromStdString(s->getFamilyName()))
                    .arg(dormNum).arg(block).arg(floor).arg(roomNum)
              : QString("Student assigned to Dorm %1").arg(dormNum));
        accept();
    } else {
        QMessageBox::critical(this, "Error",
            "Failed to assign student. Room may be full or already assigned.");
    }
}
