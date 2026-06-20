#include "SettingsWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../AdminMainWindow.h"
#include "../StudentMainWindow.h"
#include "../restaurant.h"
#include "../room.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect> // used by makeCard shadow in buildUi
#include <QDateTime>
#include <fstream>
#include <sstream>

SettingsWidget::SettingsWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor("#F8F9FA"));
    setPalette(p);
}

void SettingsWidget::buildUi() {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // ── Gradient header ────────────────────────────────────────────────────────
    auto* header = new QFrame(this);
    header->setFixedHeight(130);
    header->setStyleSheet(
        QString("QFrame { background:%1; border-radius:0; }")
        .arg(Style::gradientCSS("0","0","1","1")));
    auto* hLayout = new QVBoxLayout(header);
    hLayout->setContentsMargins(32, 18, 32, 18);
    hLayout->setSpacing(4);

    auto* topRow = new QHBoxLayout;
    auto* sectionTag = new QLabel("ADMINISTRATION  ·  CONFIGURATION", header);
    sectionTag->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:11px;"
                              "  letter-spacing:1.5px; font-weight:bold; background:transparent; }");
    topRow->addWidget(sectionTag);
    topRow->addStretch();
    hLayout->addLayout(topRow);

    auto* titleLbl = new QLabel("Settings", header);
    titleLbl->setStyleSheet("QLabel { color:white; font-size:34px; font-weight:bold; background:transparent; }");
    hLayout->addWidget(titleLbl);

    auto* subLbl = new QLabel("System configuration, data management and room settings", header);
    subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.80); font-size:13px; background:transparent; }");
    hLayout->addWidget(subLbl);

    outer->addWidget(header);

    // ── Scrollable body ────────────────────────────────────────────────────────
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background:#F8F9FA; border:none; }");

    auto* content = new QWidget;
    content->setAutoFillBackground(true);
    { QPalette cp = content->palette(); cp.setColor(QPalette::Window, QColor("#F8F9FA")); content->setPalette(cp); }

    auto* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(28, 24, 28, 32);
    mainLayout->setSpacing(20);

    // ── Card shadow helper ─────────────────────────────────────────────────────
    auto makeShadow = [](QWidget* w, QColor c = QColor(0,0,0,18)) {
        auto* s = new QGraphicsDropShadowEffect(w);
        s->setBlurRadius(22);
        s->setOffset(0, 4);
        s->setColor(c);
        return s;
    };

    // ── Shared spinner style ───────────────────────────────────────────────────
    const QString spinSS =
        "QSpinBox { border:1.5px solid #E9ECEF; border-radius:10px;"
        "  padding:6px 14px; background:white; font-size:14px;"
        "  font-weight:bold; color:#111827; }"
        "QSpinBox:hover { border-color:#F7931E; }"
        "QSpinBox:focus { border:2px solid #F7931E; }"
        "QSpinBox::up-button   { width:0; border:none; background:transparent; }"
        "QSpinBox::down-button { width:0; border:none; background:transparent; }"
        "QSpinBox::up-arrow    { width:0; height:0; }"
        "QSpinBox::down-arrow  { width:0; height:0; }";

    // ── Card builder ───────────────────────────────────────────────────────────
    struct CardParts { QFrame* card; QVBoxLayout* body; };
    auto makeCard = [&](const QString& emoji, const QString& iconBg,
                        const QString& title, const QString& sub,
                        bool danger = false) -> CardParts {
        auto* card = new QFrame(content);
        card->setStyleSheet("QFrame { background:white; border-radius:16px; border:none; }");
        card->setGraphicsEffect(makeShadow(card,
            danger ? QColor(220,38,38,25) : QColor(0,0,0,18)));

        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(28, 22, 28, 26);
        cl->setSpacing(0);

        auto* hdr = new QHBoxLayout;
        hdr->setSpacing(16);

        auto* iconBox = new QLabel(emoji, card);
        iconBox->setFixedSize(42, 42);
        iconBox->setAlignment(Qt::AlignCenter);
        iconBox->setStyleSheet(
            danger
            ? "QLabel { background:#7F1D1D; border-radius:11px; font-size:16px;"
              "  font-weight:bold; color:white; border:none; }"
            : "QLabel { background:#1F2937; border-radius:11px; font-size:16px;"
              "  font-weight:bold; color:white; border:none; }");

        auto* tc = new QVBoxLayout;
        tc->setSpacing(2);
        auto* tLbl = new QLabel(title, card);
        tLbl->setStyleSheet(danger
            ? "QLabel { font-size:15px; font-weight:bold; color:#DC2626; background:transparent; }"
            : "QLabel { font-size:15px; font-weight:bold; color:#111827; background:transparent; }");
        tc->addWidget(tLbl);
        if (!sub.isEmpty()) {
            auto* sLbl = new QLabel(sub, card);
            sLbl->setStyleSheet("QLabel { font-size:12px; color:#9CA3AF; background:transparent; }");
            tc->addWidget(sLbl);
        }

        hdr->addWidget(iconBox, 0, Qt::AlignVCenter);
        hdr->addLayout(tc, 1);
        cl->addLayout(hdr);
        cl->addSpacing(16);

        auto* sep = new QFrame(card);
        sep->setFixedHeight(1);
        sep->setStyleSheet(danger
            ? "QFrame { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
              "  stop:0 #DC2626, stop:0.5 #EF4444, stop:1 rgba(239,68,68,0)); border:none; }"
            : "QFrame { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
              "  stop:0 #FF6B35, stop:0.45 #F7931E, stop:1 rgba(253,184,39,0)); border:none; }");
        cl->addWidget(sep);
        cl->addSpacing(20);

        return { card, cl };
    };

    // ══════════════════════════════════════════════════════════════════════════
    // 1. Data Management
    // ══════════════════════════════════════════════════════════════════════════
    {
        auto parts = makeCard("≡", "", "Data Management", "Save and restore your university data");

        lastPathLabel = new QLabel("No file loaded", parts.card);
        lastPathLabel->setStyleSheet(
            "QLabel { color:#9CA3AF; font-size:12px; background:#F9FAFB;"
            "  border-radius:8px; padding:8px 12px; border:none; }");
        parts.body->addWidget(lastPathLabel);
        parts.body->addSpacing(14);

        auto* btnRow = new QHBoxLayout;
        btnRow->setSpacing(12);

        auto* saveBtn = new QPushButton("Save Data", parts.card);
        saveBtn->setFixedHeight(46);
        saveBtn->setCursor(Qt::PointingHandCursor);
        saveBtn->setStyleSheet(
            QString("QPushButton { background:%1; color:white; border:none;"
                    "  border-radius:10px; font-size:14px; font-weight:bold; }"
                    "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                    "  stop:0 #E65C2A, stop:1 #E8A522); }").arg(Style::gradientCSS()));
        connect(saveBtn, &QPushButton::clicked, this, &SettingsWidget::onSaveData);

        auto* loadBtn = new QPushButton("Load Data", parts.card);
        loadBtn->setFixedHeight(46);
        loadBtn->setCursor(Qt::PointingHandCursor);
        loadBtn->setStyleSheet(
            "QPushButton { background:white; color:#F7931E;"
            "  border:2px solid #F7931E; border-radius:10px;"
            "  font-size:14px; font-weight:bold; }"
            "QPushButton:hover { background:#FFF3E0; }");
        connect(loadBtn, &QPushButton::clicked, this, &SettingsWidget::onLoadData);

        btnRow->addWidget(saveBtn, 1);
        btnRow->addWidget(loadBtn, 1);
        parts.body->addLayout(btnRow);
        mainLayout->addWidget(parts.card);
    }

    // ══════════════════════════════════════════════════════════════════════════
    // 2. Room Configuration
    // ══════════════════════════════════════════════════════════════════════════
    {
        auto parts = makeCard("≡", "", "Room Configuration", "Maximum number of students per room");

        auto* row = new QHBoxLayout;
        row->setSpacing(14);

        auto* capLbl = new QLabel("Students per room", parts.card);
        capLbl->setStyleSheet(
            "QLabel { color:#374151; font-size:14px; font-weight:600; background:transparent; }");

        capacitySpin = new QSpinBox(parts.card);
        capacitySpin->setRange(1, 10);
        capacitySpin->setValue(Room::getCapacity());
        capacitySpin->setFixedSize(82, 46);
        capacitySpin->setStyleSheet(spinSS);

        auto* applyCapBtn = new QPushButton("Apply", parts.card);
        applyCapBtn->setFixedSize(110, 46);
        applyCapBtn->setCursor(Qt::PointingHandCursor);
        applyCapBtn->setStyleSheet(
            QString("QPushButton { background:%1; color:white; border:none;"
                    "  border-radius:10px; font-size:13px; font-weight:bold; }"
                    "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                    "  stop:0 #E65C2A, stop:1 #E8A522); }").arg(Style::gradientCSS()));
        connect(applyCapBtn, &QPushButton::clicked, this, &SettingsWidget::onApplyCapacity);

        row->addWidget(capLbl);
        row->addStretch();
        row->addWidget(capacitySpin);
        row->addWidget(applyCapBtn);
        parts.body->addLayout(row);
        mainLayout->addWidget(parts.card);
    }

    // ══════════════════════════════════════════════════════════════════════════
    // 3. Restaurant Hours
    // ══════════════════════════════════════════════════════════════════════════
    {
        auto parts = makeCard("≡", "", "Restaurant Hours", "Opening and closing times for each meal service");

        struct MealRow {
            const char* emoji;
            const char* bg;
            const char* name;
            QSpinBox**  openSpin;
            QSpinBox**  closeSpin;
            int         defOpen;
            int         defClose;
        };

        MealRow meals[] = {
            { "🌅", "#FF6B35", "Breakfast", &brOpenSpin, &brCloseSpin,
              Restaurant::getBR_OPEN(), Restaurant::getBR_CLOSE() },
            { "☀️", "#F59E0B", "Lunch",     &luOpenSpin, &luCloseSpin,
              Restaurant::getLU_OPEN(), Restaurant::getLU_CLOSE() },
            { "🌙", "#6366F1", "Dinner",    &diOpenSpin, &diCloseSpin,
              Restaurant::getDI_OPEN(), Restaurant::getDI_CLOSE() },
        };

        auto* grid = new QGridLayout;
        grid->setHorizontalSpacing(12);
        grid->setVerticalSpacing(12);
        grid->setColumnStretch(1, 1); // spacer between label and spinners

        for (int i = 0; i < 3; i++) {
            auto& m = meals[i];

            // Meal label row (emoji + color dot + name)
            auto* mealW = new QWidget(parts.card);
            mealW->setStyleSheet("background:transparent;");
            auto* ml = new QHBoxLayout(mealW);
            ml->setContentsMargins(0,0,0,0);
            ml->setSpacing(8);

            auto* dot = new QLabel(m.emoji, parts.card);
            dot->setFixedSize(32, 32);
            dot->setAlignment(Qt::AlignCenter);
            dot->setStyleSheet(
                QString("QLabel { background:%1; border-radius:8px; font-size:16px; border:none; }")
                .arg(m.bg));

            auto* nameLbl = new QLabel(m.name, parts.card);
            nameLbl->setFixedWidth(80);
            nameLbl->setStyleSheet(
                "QLabel { color:#374151; font-size:13px; font-weight:600; background:transparent; }");

            ml->addWidget(dot);
            ml->addWidget(nameLbl);
            grid->addWidget(mealW, i, 0);

            // Open spinner
            *m.openSpin = new QSpinBox(parts.card);
            (*m.openSpin)->setRange(0, 23);
            (*m.openSpin)->setValue(m.defOpen);
            (*m.openSpin)->setFixedSize(76, 42);
            (*m.openSpin)->setStyleSheet(spinSS);
            grid->addWidget(*m.openSpin, i, 2);

            auto* dash = new QLabel("—", parts.card);
            dash->setAlignment(Qt::AlignCenter);
            dash->setStyleSheet("QLabel { color:#D1D5DB; font-size:18px; background:transparent; }");
            grid->addWidget(dash, i, 3, Qt::AlignCenter);

            // Close spinner
            *m.closeSpin = new QSpinBox(parts.card);
            (*m.closeSpin)->setRange(0, 24);
            (*m.closeSpin)->setValue(m.defClose);
            (*m.closeSpin)->setFixedSize(76, 42);
            (*m.closeSpin)->setStyleSheet(spinSS);
            grid->addWidget(*m.closeSpin, i, 4);
        }

        parts.body->addLayout(grid);
        parts.body->addSpacing(18);

        auto* applyHrBtn = new QPushButton("Apply Hours", parts.card);
        applyHrBtn->setFixedHeight(46);
        applyHrBtn->setCursor(Qt::PointingHandCursor);
        applyHrBtn->setStyleSheet(
            QString("QPushButton { background:%1; color:white; border:none;"
                    "  border-radius:10px; font-size:14px; font-weight:bold; }"
                    "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                    "  stop:0 #E65C2A, stop:1 #E8A522); }").arg(Style::gradientCSS()));
        connect(applyHrBtn, &QPushButton::clicked, this, &SettingsWidget::onApplyHours);
        parts.body->addWidget(applyHrBtn);
        mainLayout->addWidget(parts.card);
    }

    // ══════════════════════════════════════════════════════════════════════════
    // 4. Danger Zone
    // ══════════════════════════════════════════════════════════════════════════
    {
        auto parts = makeCard("!", "", "Danger Zone", "Destructive actions — cannot be undone", true);

        auto* descLbl = new QLabel(
            "Reset all students, assignments, and complaints.\n"
            "Dormitory structure is preserved.", parts.card);
        descLbl->setStyleSheet(
            "QLabel { color:#6B7280; font-size:13px; background:transparent; line-height:1.5; }");
        parts.body->addWidget(descLbl);
        parts.body->addSpacing(16);

        auto* resetBtn = new QPushButton("Reset All Data", parts.card);
        resetBtn->setFixedHeight(46);
        resetBtn->setCursor(Qt::PointingHandCursor);
        resetBtn->setStyleSheet(
            "QPushButton { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "  stop:0 #DC2626, stop:1 #EF4444);"
            "  color:white; border:none; border-radius:10px;"
            "  font-size:14px; font-weight:bold; }"
            "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "  stop:0 #B91C1C, stop:1 #DC2626); }");
        connect(resetBtn, &QPushButton::clicked, this, &SettingsWidget::onResetData);
        parts.body->addWidget(resetBtn);
        mainLayout->addWidget(parts.card);
    }

    mainLayout->addStretch();

    scroll->setWidget(content);
    outer->addWidget(scroll, 1);
}

void SettingsWidget::refresh() {
    capacitySpin->setValue(Room::getCapacity());
}

// ── Dialog helpers — same pattern as StudentDetailDialog::confirmAction ────────
static void showInfo(QWidget* parent, const QString& title, const QString& msg,
                     bool danger = false)
{
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
    hl->addStretch();
    auto* ok = new QPushButton("OK", &dlg);
    ok->setFixedHeight(36);
    ok->setCursor(Qt::PointingHandCursor);
    ok->setStyleSheet(danger ? Style::dangerBtnSS() : Style::primaryBtnSS());
    QObject::connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);
    hl->addWidget(ok);
    vl->addLayout(hl);
    dlg.exec();
}

static bool showConfirm(QWidget* parent, const QString& title, const QString& msg,
                        const QString& confirmLabel, bool danger = false)
{
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
    auto* cancelBtn  = new QPushButton("Cancel",       &dlg);
    auto* confirmBtn = new QPushButton(confirmLabel,   &dlg);
    cancelBtn->setFixedHeight(36);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(Style::secondaryBtnSS());
    confirmBtn->setFixedHeight(36);
    confirmBtn->setCursor(Qt::PointingHandCursor);
    confirmBtn->setStyleSheet(danger ? Style::dangerBtnSS() : Style::primaryBtnSS());
    QObject::connect(cancelBtn,  &QPushButton::clicked, &dlg, &QDialog::reject);
    QObject::connect(confirmBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    hl->addWidget(cancelBtn);
    hl->addWidget(confirmBtn);
    vl->addLayout(hl);
    return dlg.exec() == QDialog::Accepted;
}

// ── Persistence helpers ────────────────────────────────────────────────────────
static QString escapeField(const QString& s) {
    return QString(s).replace("|", "{PIPE}").replace("\n", "{NL}");
}
static QString unescapeField(const QString& s) {
    return QString(s).replace("{PIPE}", "|").replace("{NL}", "\n");
}

void SettingsWidget::onSaveData() {
    QString path = QFileDialog::getSaveFileName(this, "Save University Data",
        "", "Data Files (*.txt);;All Files (*)");
    if (path.isEmpty()) return;

    if (!AppState::instance().university.saveData(path.toStdString())) {
        showInfo(this, "Save Failed", "Failed to write data to the selected file.", true);
        return;
    }

    std::ofstream out(path.toStdString(), std::ios::app);
    if (out.is_open()) {
        out << "COMPLAINTS_START\n";
        out << "COMPLAINT_ID_COUNTER|" << AppState::instance().complaintIdCounter << "\n";
        for (const auto& c : AppState::instance().complaints) {
            out << c.id << "|"
                << c.studentId << "|"
                << escapeField(c.studentName).toStdString() << "|"
                << c.dateTime.toString(Qt::ISODate).toStdString() << "|"
                << escapeField(c.category).toStdString() << "|"
                << escapeField(c.description).toStdString() << "|"
                << escapeField(c.status).toStdString() << "|"
                << escapeField(c.adminReply).toStdString() << "\n";
        }
        out << "COMPLAINTS_END\n";
        out.close();
    }

    lastPathLabel->setText("✓  Saved: " + path);
    showInfo(this, "Saved", "University data saved successfully.");
    AppState::instance().logActivity("DATA_SAVED", "University data saved to file");
}

void SettingsWidget::onLoadData() {
    QString path = QFileDialog::getOpenFileName(this, "Load University Data",
        "", "Data Files (*.txt);;All Files (*)");
    if (path.isEmpty()) return;

    if (!showConfirm(this, "Load Data",
            "Loading will replace the current state.\nAll unsaved changes will be lost.",
            "Load", false))
        return;

    if (!AppState::instance().university.loadData(path.toStdString())) {
        showInfo(this, "Load Failed", "Failed to load data. Check the file format.", true);
        return;
    }

    AppState::instance().complaints.clear();
    AppState::instance().complaintIdCounter = 0;

    std::ifstream in(path.toStdString());
    if (in.is_open()) {
        std::string line;
        bool inComplaints = false;
        while (std::getline(in, line)) {
            QString qline = QString::fromStdString(line);
            if (qline == "COMPLAINTS_START") { inComplaints = true; continue; }
            if (qline == "COMPLAINTS_END")   { inComplaints = false; continue; }
            if (!inComplaints) continue;
            if (qline.startsWith("COMPLAINT_ID_COUNTER|")) {
                AppState::instance().complaintIdCounter = qline.mid(21).toInt();
                continue;
            }
            QStringList tok = qline.split('|');
            if (tok.size() < 8) continue;
            Complaint c;
            c.id          = tok[0].toInt();
            c.studentId   = tok[1].toInt();
            c.studentName = unescapeField(tok[2]);
            c.dateTime    = QDateTime::fromString(tok[3], Qt::ISODate);
            c.category    = unescapeField(tok[4]);
            c.description = unescapeField(tok[5]);
            c.status      = unescapeField(tok[6]);
            c.adminReply  = unescapeField(tok[7]);
            AppState::instance().complaints.append(c);
        }
        in.close();
    }

    lastPathLabel->setText("✓  Loaded: " + path);
    showInfo(this, "Loaded", "University data loaded successfully.");
    AppState::instance().logActivity("DATA_LOADED", "University data loaded from file");
    if (auto* admin = AdminMainWindow::instance()) admin->refreshAll();
    if (auto* stu  = StudentMainWindow::instance()) stu->refresh();
}

void SettingsWidget::onResetData() {
    if (!showConfirm(this, "Reset All Data",
            "This will permanently erase all students, assignments, and complaints.\n"
            "Dormitory structure is preserved. This cannot be undone.",
            "Yes, Reset Everything", true))
        return;

    AppState::instance().university = University("University");
    for (int i = 1; i <= 6; i++)
        AppState::instance().university.addDormitory(Dormitory(i));
    Student::setIdCount(0);

    AppState::instance().complaints.clear();
    AppState::instance().complaintIdCounter = 0;
    AppState::instance().activityLog.clear();
    AppState::instance().logActivity("DATA_RESET", "All data reset");

    showInfo(this, "Reset Complete", "All data has been cleared successfully.");
    if (auto* admin = AdminMainWindow::instance()) admin->refreshAll();
}

void SettingsWidget::onApplyCapacity() {
    int cap = capacitySpin->value();
    Room::setCapacity(cap);
    if (auto* admin = AdminMainWindow::instance()) admin->refreshAll();
    showInfo(this, "Applied",
        QString("Room capacity set to %1 student%2 per room.")
        .arg(cap).arg(cap != 1 ? "s" : ""));
}

void SettingsWidget::onApplyHours() {
    Restaurant::setBR_OPEN(brOpenSpin->value());
    Restaurant::setBR_CLOSE(brCloseSpin->value());
    Restaurant::setLU_OPEN(luOpenSpin->value());
    Restaurant::setLU_CLOSE(luCloseSpin->value());
    Restaurant::setDI_OPEN(diOpenSpin->value());
    Restaurant::setDI_CLOSE(diCloseSpin->value());
    if (auto* admin = AdminMainWindow::instance()) admin->refreshAll();
    if (auto* stu  = StudentMainWindow::instance()) stu->refresh();
    showInfo(this, "Applied", "Restaurant hours updated successfully.");
}
