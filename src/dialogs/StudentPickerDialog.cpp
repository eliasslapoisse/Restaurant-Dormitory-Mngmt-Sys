#include "StudentPickerDialog.h"
#include "../appstate.h"
#include "../stylehelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <functional>

// ── Clickable student card (no Q_OBJECT — uses std::function callbacks) ────
class StudentCard : public QFrame {
public:
    int  studentId;
    std::function<void(int)> onClicked;
    std::function<void(int)> onDoubleClicked;

    StudentCard(int id, const QString& name, const QString& detail,
                bool assigned, QWidget* parent = nullptr)
        : QFrame(parent), studentId(id)
    {
        setObjectName("stuCard");
        setFixedHeight(68);
        setCursor(Qt::PointingHandCursor);
        applyStyle(false);

        auto* lay = new QHBoxLayout(this);
        lay->setContentsMargins(14, 10, 14, 10);
        lay->setSpacing(12);

        // Avatar circle with first letter
        auto* avatar = new QLabel(name.left(1).toUpper(), this);
        avatar->setFixedSize(40, 40);
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setStyleSheet(
            QString("QLabel { background:%1; border-radius:20px; color:white;"
                    "  font-weight:bold; font-size:16px; border:none; }").arg(Style::gradientCSS()));

        // Name + detail
        auto* infoW = new QWidget(this);
        infoW->setStyleSheet("QWidget { background:transparent; }");
        auto* infoL = new QVBoxLayout(infoW);
        infoL->setContentsMargins(0, 0, 0, 0);
        infoL->setSpacing(2);
        auto* nameLbl = new QLabel(name, infoW);
        nameLbl->setStyleSheet("QLabel { font-size:13px; font-weight:bold;"
                               "  color:#111827; border:none; background:transparent; }");
        auto* detLbl = new QLabel(detail, infoW);
        detLbl->setStyleSheet("QLabel { font-size:11px; color:#9CA3AF;"
                              "  border:none; background:transparent; }");
        infoL->addWidget(nameLbl);
        infoL->addWidget(detLbl);

        // Status badge
        auto* badge = new QLabel(assigned ? "Assigned" : "Pool", this);
        badge->setFixedHeight(20);
        badge->setStyleSheet(assigned
            ? "QLabel { background:#FEF3C7; color:#92400E; border-radius:6px; padding:2px 8px;"
              "  font-size:10px; font-weight:bold; border:none; }"
            : "QLabel { background:#D1FAE5; color:#065F46; border-radius:6px; padding:2px 8px;"
              "  font-size:10px; font-weight:bold; border:none; }");

        lay->addWidget(avatar);
        lay->addWidget(infoW, 1);
        lay->addWidget(badge);
    }

    void setSelected(bool sel) { applyStyle(sel); }

protected:
    void mousePressEvent(QMouseEvent* e) override {
        QFrame::mousePressEvent(e);
        if (onClicked) onClicked(studentId);
    }
    void mouseDoubleClickEvent(QMouseEvent* e) override {
        QFrame::mouseDoubleClickEvent(e);
        if (onDoubleClicked) onDoubleClicked(studentId);
    }

private:
    void applyStyle(bool sel) {
        setStyleSheet(sel
            ? "QFrame#stuCard { background:#FFF7ED; border-radius:10px; border:2px solid #F7931E; }"
              "QLabel { border:none; background:transparent; }"
              "QWidget { background:transparent; }"
            : "QFrame#stuCard { background:#F9FAFB; border-radius:10px; border:1px solid #E5E7EB; }"
              "QLabel { border:none; background:transparent; }"
              "QWidget { background:transparent; }");
    }
};

// ── StudentPickerDialog ─────────────────────────────────────────────────────

StudentPickerDialog::StudentPickerDialog(bool showAssigned, QWidget* parent)
    : QDialog(parent), m_showAssigned(showAssigned)
{
    setWindowTitle("Select Student");
    setFixedSize(480, 560);
    buildUi();
}

void StudentPickerDialog::buildUi() {
    auto* ml = new QVBoxLayout(this);
    ml->setContentsMargins(0, 0, 0, 0);
    ml->setSpacing(0);

    // ── Gradient header ──────────────────────────────────────────────
    auto* header = new QFrame(this);
    header->setFixedHeight(96);
    header->setStyleSheet(
        QString("QFrame { background:%1; }").arg(Style::gradientCSS("0","0","1","1")));
    auto* hl = new QVBoxLayout(header);
    hl->setContentsMargins(24, 20, 24, 20);
    hl->setSpacing(4);
    auto* titleLbl = new QLabel("Select a Student", header);
    titleLbl->setStyleSheet("QLabel { color:white; font-size:18px; font-weight:bold;"
                            "  background:transparent; }");
    auto* subLbl = new QLabel("Choose from students available in the pool", header);
    subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.80); font-size:12px;"
                          "  background:transparent; }");
    hl->addWidget(titleLbl);
    hl->addWidget(subLbl);
    ml->addWidget(header);

    // ── Body ─────────────────────────────────────────────────────────
    auto* body = new QWidget(this);
    body->setStyleSheet("QWidget { background:white; }");
    auto* bl = new QVBoxLayout(body);
    bl->setContentsMargins(20, 16, 20, 16);
    bl->setSpacing(12);

    searchEdit = new QLineEdit(body);
    searchEdit->setPlaceholderText("Search by name or ID...");
    searchEdit->setFixedHeight(40);
    searchEdit->setStyleSheet(Style::inputSS());
    bl->addWidget(searchEdit);

    // ── Scrollable cards area ────────────────────────────────────────
    auto* scroll = new QScrollArea(body);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background:transparent; border:none; }");

    cardsContainer = new QWidget;
    cardsContainer->setStyleSheet("QWidget { background:transparent; }");
    cardsLayout = new QVBoxLayout(cardsContainer);
    cardsLayout->setContentsMargins(0, 2, 4, 2);
    cardsLayout->setSpacing(6);
    cardsLayout->setAlignment(Qt::AlignTop);

    scroll->setWidget(cardsContainer);
    bl->addWidget(scroll, 1);

    // ── Separator + buttons ──────────────────────────────────────────
    auto* sep = new QFrame(body);
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    sep->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    bl->addWidget(sep);

    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(10);
    auto* cancelBtn = new QPushButton("Cancel", body);
    cancelBtn->setFixedHeight(42);
    cancelBtn->setStyleSheet(Style::secondaryBtnSS());
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    selectBtn = new QPushButton("Select Student", body);
    selectBtn->setFixedHeight(42);
    selectBtn->setEnabled(false);
    selectBtn->setStyleSheet(Style::primaryBtnSS());
    selectBtn->setCursor(Qt::PointingHandCursor);
    connect(selectBtn, &QPushButton::clicked, this, &StudentPickerDialog::onConfirm);

    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(selectBtn);
    bl->addLayout(btnRow);

    ml->addWidget(body, 1);

    connect(searchEdit, &QLineEdit::textChanged, this, &StudentPickerDialog::onSearch);
    populate();
}

void StudentPickerDialog::populate(const QString& filter) {
    // Deselect and disable before clearing
    m_cards.clear();
    m_selectedId = -1;
    selectBtn->setEnabled(false);

    // Delete old card widgets
    while (cardsLayout->count() > 0) {
        QLayoutItem* li = cardsLayout->takeAt(0);
        if (li->widget()) delete li->widget();
        delete li;
    }

    auto& app = AppState::instance();
    QVector<Student> students;

    for (const auto& s : app.university.getStudentPool())
        students.append(s);
    if (m_showAssigned) {
        for (const auto& dorm : app.university.getDormitories())
            for (const auto& s : dorm.getAllAssignedStudents())
                students.append(s);
    }

    bool any = false;
    for (const auto& s : students) {
        QString name = QString::fromStdString(s.getFirstName() + " " + s.getFamilyName());
        QString idStr = QString::number(s.getId());
        if (!filter.isEmpty()) {
            if (!name.toLower().contains(filter.toLower()) && !idStr.contains(filter))
                continue;
        }

        bool assigned = app.findStudentDorm(s.getId()) != nullptr;
        QString detail = QString("ID %1  ·  Year %2  ·  %3")
                         .arg(s.getId())
                         .arg(s.getAcademicYear())
                         .arg(QString::fromStdString(s.getGender()) == "M" ? "Male" : "Female");

        auto* card = new StudentCard(s.getId(), name, detail, assigned, cardsContainer);
        card->onClicked      = [this](int id){ selectCard(id); };
        card->onDoubleClicked = [this](int id){ selectCard(id); onConfirm(); };

        cardsLayout->addWidget(card);
        m_cards[s.getId()] = card;
        any = true;
    }

    if (!any) {
        auto* emptyLbl = new QLabel(
            filter.isEmpty() ? "No students in the pool" : "No matches found", cardsContainer);
        emptyLbl->setAlignment(Qt::AlignCenter);
        emptyLbl->setStyleSheet(
            "QLabel { color:#9CA3AF; font-size:13px; font-style:italic; background:transparent; }");
        cardsLayout->addWidget(emptyLbl);
    }
}

void StudentPickerDialog::selectCard(int studentId) {
    if (m_selectedId >= 0 && m_cards.contains(m_selectedId))
        m_cards[m_selectedId]->setSelected(false);
    m_selectedId = studentId;
    if (m_cards.contains(studentId)) {
        m_cards[studentId]->setSelected(true);
        selectBtn->setEnabled(true);
    }
}

void StudentPickerDialog::onSearch(const QString& text) { populate(text); }

void StudentPickerDialog::onConfirm() {
    if (m_selectedId < 0) return;
    accept();
}
