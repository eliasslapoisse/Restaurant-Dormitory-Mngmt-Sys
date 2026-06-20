#include "StudentDetailDialog.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "AssignToDormDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QAbstractItemView>
#include <QPainter>
#include <QPainterPath>
#include <QDialog>

// Custom-painted dropdown — same approach as filter combos, no Qt6 arrow bug
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

static bool confirmAction(QWidget* parent, const QString& title, const QString& msg,
                           const QString& confirmLabel, const QString& confirmStyle) {
    QDialog dlg(parent);
    dlg.setWindowTitle(title);
    dlg.setFixedWidth(400);
    auto* vl = new QVBoxLayout(&dlg);
    vl->setContentsMargins(28, 28, 28, 22);
    vl->setSpacing(20);
    auto* lbl = new QLabel(msg, &dlg);
    lbl->setWordWrap(true);
    lbl->setStyleSheet("QLabel { color:#212529; font-size:14px; background:transparent; }");
    vl->addWidget(lbl);
    auto* hl = new QHBoxLayout;
    hl->setSpacing(10);
    hl->addStretch();
    auto* cancelBtn  = new QPushButton("Cancel",      &dlg);
    auto* confirmBtn = new QPushButton(confirmLabel,  &dlg);
    cancelBtn->setFixedHeight(36);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(Style::secondaryBtnSS());
    confirmBtn->setFixedHeight(36);
    confirmBtn->setCursor(Qt::PointingHandCursor);
    confirmBtn->setStyleSheet(confirmStyle);
    QObject::connect(cancelBtn,  &QPushButton::clicked, &dlg, &QDialog::reject);
    QObject::connect(confirmBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    hl->addWidget(cancelBtn);
    hl->addWidget(confirmBtn);
    vl->addLayout(hl);
    return dlg.exec() == QDialog::Accepted;
}


StudentDetailDialog::StudentDetailDialog(int studentId, QWidget* parent)
    : QDialog(parent), m_studentId(studentId)
{
    setWindowTitle("Student Details");
    setFixedWidth(700);
    buildUi();
    loadStudent();
}

void StudentDetailDialog::buildUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    auto* header = new QFrame(this);
    header->setFixedHeight(100);
    header->setStyleSheet(
        QString("QFrame { background:%1; }").arg(Style::gradientCSS("0","0","1","1")));
    auto* hLayout = new QVBoxLayout(header);
    hLayout->setContentsMargins(28, 22, 28, 22);
    hLayout->setSpacing(4);

    auto* nameTitle = new QLabel("Student", header);
    nameTitle->setObjectName("headerNameLbl");
    nameTitle->setStyleSheet("QLabel { color:white; font-size:22px; font-weight:bold;"
                             "  background:transparent; }");
    hLayout->addWidget(nameTitle);

    idLabel = new QLabel("", header);
    idLabel->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:12px;"
                           "  background:transparent; }");
    hLayout->addWidget(idLabel);

    mainLayout->addWidget(header);


    auto* body = new QWidget(this);
    body->setStyleSheet("QWidget { background:white; }");
    auto* bodyRow = new QHBoxLayout(body);
    bodyRow->setContentsMargins(0, 0, 0, 0);
    bodyRow->setSpacing(0);


    auto* leftCol = new QWidget(body);
    leftCol->setStyleSheet("QWidget { background:white; }");
    auto* leftLayout = new QVBoxLayout(leftCol);
    leftLayout->setContentsMargins(28, 22, 22, 24);
    leftLayout->setSpacing(14);

    auto* infoHdr = new QLabel("INFORMATION", leftCol);
    infoHdr->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF;"
                           "  letter-spacing:1.5px; background:transparent; }");
    leftLayout->addWidget(infoHdr);

    auto* formGrid = new QGridLayout;
    formGrid->setHorizontalSpacing(12);
    formGrid->setVerticalSpacing(6);
    formGrid->setColumnStretch(0, 1);
    formGrid->setColumnStretch(1, 1);

    auto addField = [&](int row, int col, const QString& lbl, QLineEdit*& edit) {
        auto* l = new QLabel(lbl, leftCol);
        l->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF;"
                         "  letter-spacing:0.5px; background:transparent; }");
        edit = new QLineEdit(leftCol);
        edit->setFixedHeight(38);
        edit->setStyleSheet(Style::inputSS());
        formGrid->addWidget(l,    row * 2,     col);
        formGrid->addWidget(edit, row * 2 + 1, col);
    };

    addField(0, 0, "FIRST NAME",  firstNameEdit);
    addField(0, 1, "FAMILY NAME", familyNameEdit);
    addField(1, 0, "EMAIL",       emailEdit);
    addField(1, 1, "PHONE",       phoneEdit);

    // Gender dropdown
    auto* gLbl = new QLabel("GENDER", leftCol);
    gLbl->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF;"
                        "  letter-spacing:0.5px; background:transparent; }");
    genderCombo = new StyledDropdown(leftCol);
    genderCombo->addItem("Male",   "M");
    genderCombo->addItem("Female", "F");
    formGrid->addWidget(gLbl,        4, 0);
    formGrid->addWidget(genderCombo, 5, 0);

    // Academic Year dropdown
    auto* yLbl = new QLabel("ACADEMIC YEAR", leftCol);
    yLbl->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF;"
                        "  letter-spacing:0.5px; background:transparent; }");
    yearCombo = new StyledDropdown(leftCol);
    for (int y = 1; y <= 7; y++)
        yearCombo->addItem(QString("Year %1").arg(y), y);
    formGrid->addWidget(yLbl,      4, 1);
    formGrid->addWidget(yearCombo, 5, 1);

    leftLayout->addLayout(formGrid);

    errorLabel = new QLabel("", leftCol);
    errorLabel->setStyleSheet("QLabel { color:#DC3545; font-size:12px; background:transparent; }");
    errorLabel->hide();
    leftLayout->addWidget(errorLabel);

    leftLayout->addStretch();

    auto* saveBtn = new QPushButton("Save Changes", leftCol);
    saveBtn->setFixedHeight(44);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        QString("QPushButton { background:%1; color:white; border:none; border-radius:10px;"
                "  font-size:14px; font-weight:bold; }"
                "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "  stop:0 #E65C2A, stop:1 #E8A522); }").arg(Style::gradientCSS()));
    connect(saveBtn, &QPushButton::clicked, this, &StudentDetailDialog::onSave);
    leftLayout->addWidget(saveBtn);

    bodyRow->addWidget(leftCol, 55);


    auto* div = new QFrame(body);
    div->setFrameShape(QFrame::VLine);
    div->setFixedWidth(1);
    div->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    bodyRow->addWidget(div);


    auto* rightCol = new QWidget(body);
    rightCol->setStyleSheet("QWidget { background:#FAFAFA; }");
    auto* rightLayout = new QVBoxLayout(rightCol);
    rightLayout->setContentsMargins(20, 22, 24, 24);
    rightLayout->setSpacing(12);

    auto* assignHdr = new QLabel("STATUS & ROOM ASSIGNMENT", rightCol);
    assignHdr->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF;"
                             "  letter-spacing:1.5px; background:transparent; }");
    rightLayout->addWidget(assignHdr);

    // Status card — styled dynamically in loadStudent()
    auto* statusCard = new QFrame(rightCol);
    statusCard->setObjectName("statusCard");
    auto* scl = new QVBoxLayout(statusCard);
    scl->setContentsMargins(14, 12, 14, 12);
    scl->setSpacing(4);
    statusLabel = new QLabel("", statusCard);
    statusLabel->setStyleSheet("QLabel { font-size:13px; font-weight:bold; background:transparent; }");
    locationLabel = new QLabel("", statusCard);
    locationLabel->setStyleSheet("QLabel { font-size:12px; color:#6C757D; background:transparent; }");
    locationLabel->setWordWrap(true);
    scl->addWidget(statusLabel);
    scl->addWidget(locationLabel);
    rightLayout->addWidget(statusCard);

    // Dormitory selector
    auto* assignLbl = new QLabel("ASSIGN TO DORMITORY", rightCol);
    assignLbl->setStyleSheet("QLabel { font-size:10px; font-weight:bold; color:#9CA3AF;"
                             "  letter-spacing:0.5px; background:transparent; }");
    rightLayout->addWidget(assignLbl);

    assignDormCombo = new StyledDropdown(rightCol);
    for (int i = 1; i <= 6; i++)
        assignDormCombo->addItem(QString("Dormitory %1").arg(i), i);
    rightLayout->addWidget(assignDormCombo);

    assignBtn = new QPushButton("Assign to Room", rightCol);
    assignBtn->setFixedHeight(40);
    assignBtn->setCursor(Qt::PointingHandCursor);
    assignBtn->setStyleSheet(Style::secondaryBtnSS());
    connect(assignBtn, &QPushButton::clicked, this, &StudentDetailDialog::onAssignToRoom);
    rightLayout->addWidget(assignBtn);

    auto* poolBtn = new QPushButton("Move to Pool", rightCol);
    poolBtn->setFixedHeight(40);
    poolBtn->setCursor(Qt::PointingHandCursor);
    poolBtn->setStyleSheet(Style::smallBtnSS());
    connect(poolBtn, &QPushButton::clicked, this, &StudentDetailDialog::onMoveToPool);
    rightLayout->addWidget(poolBtn);

    rightLayout->addStretch();

    auto* expelBtn = new QPushButton("Expel Student", rightCol);
    expelBtn->setFixedHeight(40);
    expelBtn->setCursor(Qt::PointingHandCursor);
    expelBtn->setStyleSheet(Style::dangerBtnSS());
    connect(expelBtn, &QPushButton::clicked, this, &StudentDetailDialog::onExpel);
    rightLayout->addWidget(expelBtn);

    bodyRow->addWidget(rightCol, 45);

    mainLayout->addWidget(body, 1);
}

void StudentDetailDialog::loadStudent() {
    const Student* s = AppState::instance().findStudentById(m_studentId);
    if (!s) { reject(); return; }

    if (auto* nl = findChild<QLabel*>("headerNameLbl"))
        nl->setText(QString::fromStdString(s->getFirstName() + " " + s->getFamilyName()));
    idLabel->setText(QString("Student ID: %1").arg(s->getId()));

    firstNameEdit->setText(QString::fromStdString(s->getFirstName()));
    familyNameEdit->setText(QString::fromStdString(s->getFamilyName()));
    emailEdit->setText(QString::fromStdString(s->getEmail()));
    phoneEdit->setText(QString::fromStdString(s->getPhoneNumber()));

    yearCombo->setCurrentIndex(std::max(0, s->getAcademicYear() - 1));

    QString g = QString::fromStdString(s->getGender());
    genderCombo->setCurrentIndex((g == "F" || g.toLower() == "female") ? 1 : 0);

    auto* statusCard = findChild<QFrame*>("statusCard");
    Dormitory* dorm = AppState::instance().findStudentDorm(m_studentId);
    if (dorm) {
        if (statusCard)
            statusCard->setStyleSheet("QFrame#statusCard { background:#F0FFF4; border-radius:10px;"
                                      "  border:1.5px solid #86EFAC; }");
        statusLabel->setStyleSheet("QLabel { color:#166534; font-size:13px;"
                                   "  font-weight:bold; background:transparent; }");
        statusLabel->setText("● Assigned");
        locationLabel->setText(
            QString::fromStdString(AppState::instance().university.locateStudent(m_studentId)));
    } else {
        if (statusCard)
            statusCard->setStyleSheet("QFrame#statusCard { background:#FFFBEB; border-radius:10px;"
                                      "  border:1.5px solid #FCD34D; }");
        statusLabel->setStyleSheet("QLabel { color:#92400E; font-size:13px;"
                                   "  font-weight:bold; background:transparent; }");
        statusLabel->setText("● In Pool");
        locationLabel->setText("Waiting for room assignment");
    }
}

void StudentDetailDialog::onSave() {
    errorLabel->hide();
    QString fn     = firstNameEdit->text().trimmed();
    QString ln     = familyNameEdit->text().trimmed();
    QString email  = emailEdit->text().trimmed();
    QString phone  = phoneEdit->text().trimmed();
    QString gender = genderCombo->currentData().toString();
    int     year   = yearCombo->currentData().toInt();

    Student tmp;
    if (!tmp.verifyInput(fn.toStdString(), ln.toStdString(),
                         email.toStdString(), phone.toStdString(),
                         gender.toStdString(), year)) {
        errorLabel->setText("Invalid input — check all fields.");
        errorLabel->show();
        return;
    }

    auto& uni = AppState::instance().university;
    const Student* cur = AppState::instance().findStudentById(m_studentId);
    if (cur) {
        if (cur->getEmail() != email.toStdString() && uni.isEmailDuplicate(email.toStdString())) {
            errorLabel->setText("Email already in use by another student.");
            errorLabel->show(); return;
        }
        if (cur->getPhoneNumber() != phone.toStdString() && uni.isPhoneDuplicate(phone.toStdString())) {
            errorLabel->setText("Phone already in use by another student.");
            errorLabel->show(); return;
        }
    }

    if (AppState::instance().updateStudentData(m_studentId,
            fn.toStdString(), ln.toStdString(),
            email.toStdString(), phone.toStdString(),
            gender.toStdString(), year)) {
        AppState::instance().logActivity("STUDENT_UPDATED",
            QString("Student %1 updated").arg(m_studentId));
        accept();
    } else {
        errorLabel->setText("Failed to save changes.");
        errorLabel->show();
    }
}

void StudentDetailDialog::onAssignToRoom() {
    AssignToDormDialog d(m_studentId, this);
    if (d.exec() == QDialog::Accepted) { loadStudent(); accept(); }
}

void StudentDetailDialog::onMoveToPool() {
    if (AppState::instance().findStudentDorm(m_studentId) == nullptr) {
        confirmAction(this, "Info", "Student is already in the pool.", "OK", Style::primaryBtnSS());
        return;
    }
    if (!confirmAction(this, "Move to Pool",
            "Move this student back to the waiting pool?",
            "Move to Pool", Style::primaryBtnSS()))
        return;

    if (AppState::instance().university.unassignStudent(m_studentId)) {
        const Student* s = AppState::instance().findStudentById(m_studentId);
        AppState::instance().logActivity("STUDENT_UNASSIGNED",
            s ? QString("%1 %2 moved to pool")
                    .arg(QString::fromStdString(s->getFirstName()),
                         QString::fromStdString(s->getFamilyName()))
              : "Student moved to pool");
        loadStudent(); accept();
    }
}

void StudentDetailDialog::onExpel() {
    const Student* s = AppState::instance().findStudentById(m_studentId);
    QString name = s ? QString::fromStdString(s->getFirstName() + " " + s->getFamilyName())
                     : "this student";
    if (!confirmAction(this, "Confirm Expulsion",
            QString("Permanently expel <b>%1</b>?<br>"
                    "<span style='color:#6C757D;font-size:12px;'>This cannot be undone.</span>")
                .arg(name),
            "Expel", Style::dangerBtnSS()))
        return;

    if (AppState::instance().university.expelStudent(m_studentId)) {
        AppState::instance().logActivity("EXPELLED", QString("%1 expelled").arg(name));
        accept();
    }
}
