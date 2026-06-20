#include "AddStudentDialog.h"
#include "../appstate.h"
#include "../stylehelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QComboBox>
#include <QButtonGroup>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>

// Styled year dropdown — same custom-paint approach as the filter combos
class YearCombo : public QComboBox {
    bool _hov = false;
public:
    explicit YearCombo(QWidget* p = nullptr) : QComboBox(p) {
        view()->setStyleSheet(
            "QListView { outline:none; padding:4px; background:white; }"
            "QListView::item { padding:9px 16px; color:#212529; border-radius:7px; margin:1px 4px; }"
            "QListView::item:hover { background:#FFF3E0; color:#F7931E; }"
            "QListView::item:selected { background:#F7931E; color:white; }");
        setStyleSheet("QComboBox QAbstractItemView { border:1px solid #E9ECEF; background:white; }");
    }
    QSize sizeHint()        const override { return {160, 42}; }
    QSize minimumSizeHint() const override { return {120, 42}; }
protected:
    void enterEvent(QEnterEvent* e) override { _hov = true;  update(); QComboBox::enterEvent(e); }
    void leaveEvent(QEvent*       e) override { _hov = false; update(); QComboBox::leaveEvent(e); }
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        bool hot = view()->isVisible() || _hov;
        p.setPen(QPen(QColor(hot ? "#F7931E" : "#DEE2E6"), hot ? 1.5 : 1.0));
        p.setBrush(Qt::white);
        p.drawRoundedRect(QRectF(rect()).adjusted(0.5,0.5,-0.5,-0.5), 10, 10);
        p.setPen(QColor("#212529"));
        QFont f = font(); f.setPixelSize(13); p.setFont(f);
        p.drawText(rect().adjusted(14, 0, -32, 0), Qt::AlignVCenter | Qt::AlignLeft, currentText());
        const int cx = rect().right() - 16;
        const int cy = rect().center().y();
        p.setPen(QPen(hot ? QColor("#F7931E") : QColor("#9CA3AF"),
                      1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        QPolygon ch;
        ch << QPoint(cx-4, cy-2) << QPoint(cx, cy+3) << QPoint(cx+4, cy-2);
        p.drawPolyline(ch);
    }
};

AddStudentDialog::AddStudentDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Add New Student");
    setFixedWidth(480);
    buildUi();
}

void AddStudentDialog::buildUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    auto* header = new QFrame(this);
    header->setFixedHeight(106);
    header->setStyleSheet(
        QString("QFrame { background:%1; }").arg(Style::gradientCSS("0","0","1","1")));
    auto* hLayout = new QVBoxLayout(header);
    hLayout->setContentsMargins(28, 22, 28, 22);
    hLayout->setSpacing(5);

    auto* titleLbl = new QLabel("Register New Student", header);
    titleLbl->setStyleSheet("QLabel { color:white; font-size:20px; font-weight:bold;"
                            "  background:transparent; }");
    hLayout->addWidget(titleLbl);

    auto* subLbl = new QLabel("All fields below are required", header);
    subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:12px;"
                          "  background:transparent; }");
    hLayout->addWidget(subLbl);

    mainLayout->addWidget(header);


    auto* body = new QWidget(this);
    body->setStyleSheet("QWidget { background:white; }");
    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(28, 20, 28, 24);
    bodyLayout->setSpacing(4);

    // Helper: label + input + status line
    auto addField = [&](const QString& labelText, QLineEdit*& edit,
                        QLabel*& statusLbl, const QString& placeholder) {
        auto* lbl = new QLabel(labelText, body);
        lbl->setStyleSheet("QLabel { font-size:12px; font-weight:bold; color:#374151;"
                           "  background:transparent; margin-top:8px; }");
        edit = new QLineEdit(body);
        edit->setPlaceholderText(placeholder);
        edit->setFixedHeight(42);
        edit->setStyleSheet(Style::inputSS());
        statusLbl = new QLabel("", body);
        statusLbl->setStyleSheet("QLabel { font-size:11px; color:#6C757D; background:transparent; }");
        bodyLayout->addWidget(lbl);
        bodyLayout->addWidget(edit);
        bodyLayout->addWidget(statusLbl);
        connect(edit, &QLineEdit::textChanged, this, &AddStudentDialog::onFieldChanged);
    };

    addField("First Name",    firstNameEdit, fnStatus,    "e.g. Ahmed");
    addField("Family Name",   familyNameEdit, lnStatus,   "e.g. Boudiaf");
    addField("Email Address", emailEdit,     emailStatus,  "student@university.edu");
    addField("Phone Number",  phoneEdit,     phoneStatus,  "0X-XXXXXXXX (starts with 05/06/07)");

    // Gender
    auto* gLbl = new QLabel("Gender", body);
    gLbl->setStyleSheet("QLabel { font-size:12px; font-weight:bold; color:#374151;"
                        "  background:transparent; margin-top:8px; }");
    bodyLayout->addWidget(gLbl);

    auto* gRow = new QHBoxLayout;
    gRow->setSpacing(20);
    maleRadio   = new QRadioButton("Male",   body);
    femaleRadio = new QRadioButton("Female", body);
    maleRadio->setChecked(true);

    const QString radioSS =
        "QRadioButton { color:#212529; font-size:13px; background:transparent; }"
        "QRadioButton::indicator { width:13px; height:13px; border-radius:7px;"
        "  border:2px solid #DEE2E6; background:white; }"
        "QRadioButton::indicator:checked { border:2px solid #F7931E; background:#F7931E; }"
        "QRadioButton::indicator:hover   { border:2px solid #F7931E; }";
    maleRadio->setStyleSheet(radioSS);
    femaleRadio->setStyleSheet(radioSS);

    gRow->addWidget(maleRadio);
    gRow->addWidget(femaleRadio);
    gRow->addStretch();
    bodyLayout->addSpacing(4);
    bodyLayout->addLayout(gRow);

    // Academic Year — styled combo
    auto* yLbl = new QLabel("Academic Year", body);
    yLbl->setStyleSheet("QLabel { font-size:12px; font-weight:bold; color:#374151;"
                        "  background:transparent; margin-top:8px; }");
    yearCombo = new YearCombo(body);
    for (int i = 1; i <= 7; i++)
        yearCombo->addItem(QString("Year %1").arg(i), i);

    bodyLayout->addWidget(yLbl);
    bodyLayout->addSpacing(2);
    bodyLayout->addWidget(yearCombo, 0, Qt::AlignLeft);

    // Divider
    bodyLayout->addSpacing(12);
    auto* sep = new QFrame(body);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("QFrame { background:#F3F4F6; border:none; max-height:1px; }");
    bodyLayout->addWidget(sep);
    bodyLayout->addSpacing(8);

    // Buttons
    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(12);

    auto* cancelBtn = new QPushButton("Cancel", body);
    cancelBtn->setFixedHeight(42);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(Style::secondaryBtnSS());
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    submitBtn = new QPushButton("Register Student", body);
    submitBtn->setFixedHeight(42);
    submitBtn->setEnabled(false);
    submitBtn->setCursor(Qt::PointingHandCursor);
    submitBtn->setStyleSheet(Style::primaryBtnSS());
    connect(submitBtn, &QPushButton::clicked, this, &AddStudentDialog::onSubmit);

    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(submitBtn);
    bodyLayout->addLayout(btnRow);

    mainLayout->addWidget(body);

    onFieldChanged();
}

void AddStudentDialog::updateFieldStatus(QLabel* label, bool valid, const QString& msg) {
    label->setText(msg);
    label->setStyleSheet(valid
        ? "QLabel { font-size:11px; color:#28A745; background:transparent; }"
        : "QLabel { font-size:11px; color:#DC3545; background:transparent; }");
}

void AddStudentDialog::onFieldChanged() {
    Student tmp;
    auto& uni = AppState::instance().university;

    QString fn    = firstNameEdit->text().trimmed();
    QString ln    = familyNameEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();

    bool fnOk    = !fn.isEmpty()    && tmp.verifyInput(fn.toStdString(),    "X", "x@x.com", "0500000000", "M", 1);
    bool lnOk    = !ln.isEmpty()    && tmp.verifyInput("X", ln.toStdString(), "x@x.com", "0500000000", "M", 1);
    bool emailOk = !email.isEmpty() && tmp.verifyInput("X", "X", email.toStdString(), "0500000000", "M", 1);
    bool phoneOk = !phone.isEmpty() && tmp.verifyInput("X", "X", "x@x.com", phone.toStdString(), "M", 1);

    bool emailDup = emailOk && uni.isEmailDuplicate(email.toStdString());
    bool phoneDup = phoneOk && uni.isPhoneDuplicate(phone.toStdString());

    if (fn.isEmpty())    updateFieldStatus(fnStatus, false, "");
    else if (fnOk)       updateFieldStatus(fnStatus, true,  "✓ Valid");
    else                 updateFieldStatus(fnStatus, false, "Letters and hyphens only, max 30 chars");

    if (ln.isEmpty())    updateFieldStatus(lnStatus, false, "");
    else if (lnOk)       updateFieldStatus(lnStatus, true,  "✓ Valid");
    else                 updateFieldStatus(lnStatus, false, "Letters and hyphens only, max 30 chars");

    if (email.isEmpty()) updateFieldStatus(emailStatus, false, "");
    else if (emailDup)   updateFieldStatus(emailStatus, false, "Email already registered");
    else if (emailOk)    updateFieldStatus(emailStatus, true,  "✓ Valid");
    else                 updateFieldStatus(emailStatus, false, "Must contain @ and be ≤254 chars");

    if (phone.isEmpty()) updateFieldStatus(phoneStatus, false, "");
    else if (phoneDup)   updateFieldStatus(phoneStatus, false, "Phone already registered");
    else if (phoneOk)    updateFieldStatus(phoneStatus, true,  "✓ Valid");
    else                 updateFieldStatus(phoneStatus, false, "10 digits, starts with 05/06/07");

    submitBtn->setEnabled(allValid());
}

bool AddStudentDialog::allValid() {
    Student tmp;
    auto& uni = AppState::instance().university;
    QString fn    = firstNameEdit->text().trimmed();
    QString ln    = familyNameEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();
    return !fn.isEmpty() && !ln.isEmpty() && !email.isEmpty() && !phone.isEmpty()
        && tmp.verifyInput(fn.toStdString(), ln.toStdString(),
                           email.toStdString(), phone.toStdString(), "M", 1)
        && !uni.isEmailDuplicate(email.toStdString())
        && !uni.isPhoneDuplicate(phone.toStdString());
}

void AddStudentDialog::onSubmit() {
    QString fn     = firstNameEdit->text().trimmed();
    QString ln     = familyNameEdit->text().trimmed();
    QString email  = emailEdit->text().trimmed();
    QString phone  = phoneEdit->text().trimmed();
    QString gender = maleRadio->isChecked() ? "M" : "F";
    int     year   = yearCombo->currentData().toInt();

    Student newStudent(fn.toStdString(), ln.toStdString(),
                       email.toStdString(), phone.toStdString(),
                       gender.toStdString(), year);

    if (!newStudent.isInitialized()) {
        QMessageBox::critical(this, "Error", "Failed to create student. Check all fields.");
        return;
    }

    if (AppState::instance().university.enrollStudent(newStudent)) {
        AppState::instance().logActivity("STUDENT_ADDED",
            QString("%1 %2 added to student pool").arg(fn, ln));
        accept();
    } else {
        QMessageBox::critical(this, "Error",
            "Failed to enroll student. Email or phone may already be registered.");
    }
}
