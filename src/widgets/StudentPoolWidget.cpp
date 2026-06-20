#include "StudentPoolWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../AdminMainWindow.h"
#include "../dialogs/AddStudentDialog.h"
#include "../dialogs/StudentDetailDialog.h"
#include "../dialogs/AssignToDormDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QCheckBox>
#include <QGraphicsDropShadowEffect>
#include <QAbstractButton>
#include <QStyledItemDelegate>
#include <QStyleOption>
#include <QPainter>
#include <QPainterPath>
#include <algorithm>

static QGraphicsDropShadowEffect* makeShadow(QObject* parent) {
    auto* e = new QGraphicsDropShadowEffect(parent);
    e->setBlurRadius(18);
    e->setOffset(0, 3);
    e->setColor(QColor(0, 0, 0, 28));
    return e;
}

// Clips children to rounded rect by setting a pixel mask on resize
class ClipFrame : public QFrame {
    int _r;
public:
    explicit ClipFrame(int radius, QWidget* p = nullptr) : QFrame(p), _r(radius) {}
protected:
    void resizeEvent(QResizeEvent* e) override {
        QFrame::resizeEvent(e);
        QBitmap bm(size());
        bm.fill(Qt::color0);
        QPainter p(&bm);
        p.setBrush(Qt::color1);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect(), _r, _r);
        setMask(bm);
    }
};

// Removes the black focus rectangle drawn on clicked cells
class NoFocusDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void initStyleOption(QStyleOptionViewItem* opt, const QModelIndex& idx) const override {
        QStyledItemDelegate::initStyleOption(opt, idx);
        opt->state &= ~QStyle::State_HasFocus;
    }
};

// Icon-only button that paints an eye or trash icon
class IconBtn : public QAbstractButton {
    enum Type { Eye, Trash } type;
    bool _hov = false;
    explicit IconBtn(Type t, QWidget* p) : QAbstractButton(p), type(t) {
        setFixedSize(30, 30);
        setCursor(Qt::PointingHandCursor);
    }
public:
    static IconBtn* eye  (QWidget* p) { return new IconBtn(Eye,   p); }
    static IconBtn* trash(QWidget* p) { return new IconBtn(Trash, p); }

protected:
    void enterEvent(QEnterEvent* e) override { _hov = true;  update(); QAbstractButton::enterEvent(e); }
    void leaveEvent(QEvent*       e) override { _hov = false; update(); QAbstractButton::leaveEvent(e); }

    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QColor c = _hov ? QColor("#F7931E") : QColor("#9CA3AF");
        double cx = width() / 2.0, cy = height() / 2.0;

        if (type == Eye) {
            // Lens outline
            QPainterPath path;
            path.moveTo(cx - 7, cy);
            path.cubicTo(cx - 4, cy - 5, cx + 4, cy - 5, cx + 7, cy);
            path.cubicTo(cx + 4, cy + 5, cx - 4, cy + 5, cx - 7, cy);
            p.setPen(QPen(c, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(Qt::NoBrush);
            p.drawPath(path);
            // Iris
            p.setPen(Qt::NoPen);
            p.setBrush(c);
            p.drawEllipse(QPointF(cx, cy), 2.2, 2.2);
        } else {
            p.setPen(QPen(c, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(Qt::NoBrush);
            // Body
            p.drawRoundedRect(QRectF(cx - 5, cy - 3, 10, 9), 1, 1);
            // Lid
            p.drawLine(QPointF(cx - 7, cy - 3), QPointF(cx + 7, cy - 3));
            // Handle
            p.drawRoundedRect(QRectF(cx - 2.5, cy - 6.5, 5, 3.5), 1, 1);
            // Inner lines
            p.drawLine(QPointF(cx - 2, cy - 0.5), QPointF(cx - 2, cy + 4.5));
            p.drawLine(QPointF(cx + 2, cy - 0.5), QPointF(cx + 2, cy + 4.5));
        }
    }
};

// Custom combo box: fully self-painted so we control border + arrow exactly
class StyledCombo : public QComboBox {
    bool _hovered = false;
public:
    explicit StyledCombo(QWidget* parent = nullptr) : QComboBox(parent) {
        view()->setStyleSheet(
            "QListView { outline: none; padding: 4px; background: white; }"
            "QListView::item {"
            "  padding: 9px 16px; color: #212529;"
            "  border-radius: 7px; margin: 1px 4px; }"
            "QListView::item:hover {"
            "  background: #FFF3E0; color: #F7931E; }"
            "QListView::item:selected,"
            "QListView::item:selected:hover {"
            "  background: #F7931E; color: white; }");
        setStyleSheet(
            "QComboBox QAbstractItemView {"
            "  border: 1px solid #E9ECEF; background: white; outline: none; }");
    }

    QSize sizeHint()        const override { return {140, 34}; }
    QSize minimumSizeHint() const override { return {90,  34}; }

protected:
    void enterEvent(QEnterEvent* e) override { _hovered = true;  update(); QComboBox::enterEvent(e); }
    void leaveEvent(QEvent*       e) override { _hovered = false; update(); QComboBox::leaveEvent(e); }

    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        bool hot = view()->isVisible() || _hovered;

        // Border — always light gray, no colour change on hover
        p.setPen(QPen(QColor("#DEE2E6"), 1.0));
        p.setBrush(Qt::white);
        p.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 10, 10);

        // Text
        p.setPen(QColor("#212529"));
        QFont f = font();
        f.setPixelSize(13);
        p.setFont(f);
        p.drawText(rect().adjusted(12, 0, -30, 0),
                   Qt::AlignVCenter | Qt::AlignLeft, currentText());

        // Chevron — only the arrow turns orange on hover/open
        const int cx = rect().right() - 16;
        const int cy = rect().center().y();
        p.setPen(QPen(hot ? QColor("#F7931E") : QColor("#9CA3AF"),
                      1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        QPolygon ch;
        ch << QPoint(cx - 4, cy - 2) << QPoint(cx, cy + 3) << QPoint(cx + 4, cy - 2);
        p.drawPolyline(ch);
    }
};

static bool confirmExpel(QWidget* parent, const QString& msg) {
    QDialog dlg(parent);
    dlg.setWindowTitle("Confirm Expulsion");
    dlg.setFixedWidth(420);
    auto* vl = new QVBoxLayout(&dlg);
    vl->setContentsMargins(28, 28, 28, 22);
    vl->setSpacing(22);

    auto* lbl = new QLabel(msg, &dlg);
    lbl->setWordWrap(true);
    lbl->setStyleSheet("QLabel { color:#212529; font-size:14px; background:transparent; }");
    vl->addWidget(lbl);

    auto* hl = new QHBoxLayout;
    hl->setSpacing(10);
    hl->addStretch();

    auto* cancelBtn  = new QPushButton("Cancel", &dlg);
    auto* expelBtn   = new QPushButton("Expel",  &dlg);

    cancelBtn->setFixedHeight(36);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(Style::secondaryBtnSS());

    expelBtn->setFixedHeight(36);
    expelBtn->setCursor(Qt::PointingHandCursor);
    expelBtn->setStyleSheet(Style::dangerBtnSS());

    QObject::connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
    QObject::connect(expelBtn,  &QPushButton::clicked, &dlg, &QDialog::accept);

    hl->addWidget(cancelBtn);
    hl->addWidget(expelBtn);
    vl->addLayout(hl);

    return dlg.exec() == QDialog::Accepted;
}

StudentPoolWidget::StudentPoolWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor("#F8F9FA"));
    setPalette(p);
}

void StudentPoolWidget::buildUi() {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // ── Gradient header ─────────────────────────────────────────
    auto* header = new QFrame(this);
    header->setFixedHeight(130);
    header->setStyleSheet(
        QString("QFrame { background:%1; border-radius:0; }").arg(Style::gradientCSS("0","0","1","1")));
    auto* hLayout = new QVBoxLayout(header);
    hLayout->setContentsMargins(32, 18, 32, 18);
    hLayout->setSpacing(4);

    auto* topRow = new QHBoxLayout;
    auto* sectionTag = new QLabel("UNIVERSITY HOUSING  ·  ADMINISTRATION", header);
    sectionTag->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:11px;"
                              "  letter-spacing:1px; background:transparent; }");
    topRow->addWidget(sectionTag);
    topRow->addStretch();
    hLayout->addLayout(topRow);
    countLabel = nullptr;

    auto* titleLbl = new QLabel("Student Pool", header);
    titleLbl->setStyleSheet("QLabel { color:white; font-size:34px; font-weight:bold; background:transparent; }");
    hLayout->addWidget(titleLbl);

    auto* subLbl = new QLabel("Manage enrolled and assigned students", header);
    subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.8); font-size:13px; background:transparent; }");
    hLayout->addWidget(subLbl);

    outer->addWidget(header);

    // ── Body ─────────────────────────────────────────────────────
    auto* body = new QWidget(this);
    body->setAutoFillBackground(true);
    { QPalette bp = body->palette(); bp.setColor(QPalette::Window, QColor("#F8F9FA")); body->setPalette(bp); }

    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(28, 18, 28, 28);
    bodyLayout->setSpacing(16);

    // ── Tab bar + Add Student button (same row) ───────────────────
    auto* tabCard = new QFrame(body);
    tabCard->setStyleSheet("QFrame { background:white; border-radius:14px; border:1px solid #E9ECEF; }");
    tabCard->setGraphicsEffect(makeShadow(tabCard));
    auto* tabLayout = new QHBoxLayout(tabCard);
    tabLayout->setContentsMargins(16, 6, 16, 6);
    tabLayout->setSpacing(0);

    const char* tabLabels[] = { "All Students", "Pool", "Assigned" };
    for (int i = 0; i < 3; i++) {
        tabBtns[i] = new QPushButton(tabLabels[i], tabCard);
        tabBtns[i]->setFixedHeight(44);
        tabBtns[i]->setCursor(Qt::PointingHandCursor);
        tabBtns[i]->setFlat(true);
        tabLayout->addWidget(tabBtns[i]);
        connect(tabBtns[i], &QPushButton::clicked, this, [this, i]() {
            currentTab = i;
            updateTabStyles();
            applyFilters();
        });
    }
    tabLayout->addStretch();

    auto* addBtn = new QPushButton("+ Add Student", tabCard);
    addBtn->setFixedHeight(32);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(Style::primaryBtnSS());
    connect(addBtn, &QPushButton::clicked, this, &StudentPoolWidget::onAddStudent);
    tabLayout->addWidget(addBtn);

    bodyLayout->addWidget(tabCard);
    updateTabStyles();

    // ── Filter bar ────────────────────────────────────────────────
    auto* filterCard = new QFrame(body);
    filterCard->setStyleSheet("QFrame { background:white; border-radius:14px; border:1px solid #E9ECEF; }");
    filterCard->setGraphicsEffect(makeShadow(filterCard));
    auto* filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(20, 14, 20, 14);
    filterLayout->setSpacing(14);

    searchEdit = new QLineEdit(filterCard);
    searchEdit->setPlaceholderText("Search by name, email or ID...");
    searchEdit->setFixedHeight(40);
    searchEdit->setStyleSheet(Style::inputSS());
    filterLayout->addWidget(searchEdit, 2);

    yearCombo = new StyledCombo(filterCard);
    yearCombo->addItem("All Years", 0);
    for (int i = 1; i <= 7; i++) yearCombo->addItem(QString("Year %1").arg(i), i);
    filterLayout->addWidget(yearCombo);

    genderCombo = new StyledCombo(filterCard);
    genderCombo->addItems({ "All Genders", "Male", "Female" });
    filterLayout->addWidget(genderCombo);

    bodyLayout->addWidget(filterCard);

    connect(searchEdit,  &QLineEdit::textChanged,      this, [this](const QString&){ applyFilters(); });
    connect(yearCombo,   &QComboBox::currentTextChanged, this, [this](const QString&){ applyFilters(); });
    connect(genderCombo, &QComboBox::currentTextChanged, this, [this](const QString&){ applyFilters(); });

    // ── Bulk action bar ────────────────────────────────────────────
    auto* bulkFrame = new QFrame(body);
    bulkFrame->setObjectName("bulkBar");
    bulkFrame->setStyleSheet("QFrame#bulkBar { background:white; border-radius:12px; border:1px solid #E9ECEF; }");
    bulkFrame->setGraphicsEffect(makeShadow(bulkFrame));
    auto* bulkLayout = new QHBoxLayout(bulkFrame);
    bulkLayout->setContentsMargins(16, 8, 16, 8);
    bulkLayout->setSpacing(12);

    selectAllCheck = new QCheckBox("Select All", bulkFrame);
    selectAllCheck->setTristate(true);
    selectAllCheck->setStyleSheet("QCheckBox { font-size:13px; color:#495057; }");
    bulkLayout->addWidget(selectAllCheck);

    selectionLabel = new QLabel("", bulkFrame);
    selectionLabel->setStyleSheet("QLabel { color:#6C757D; font-size:13px; }");
    selectionLabel->setVisible(false);
    bulkLayout->addWidget(selectionLabel);

    bulkLayout->addStretch();

    expelSelectedBtn = new QPushButton("Expel Selected", bulkFrame);
    expelSelectedBtn->setFixedHeight(32);
    expelSelectedBtn->setCursor(Qt::PointingHandCursor);
    expelSelectedBtn->setStyleSheet(
        "QPushButton { background:white; color:#DC3545; border:1px solid #DC3545;"
        "  border-radius:6px; padding:4px 14px; font-size:12px; font-weight:bold; }"
        "QPushButton:hover { background:#F8D7DA; }");
    expelSelectedBtn->setVisible(false);
    bulkLayout->addWidget(expelSelectedBtn);

    bodyLayout->addWidget(bulkFrame);

    // Use clicked (not stateChanged) to avoid feedback loops with programmatic state changes
    connect(selectAllCheck, &QCheckBox::clicked, this, [this](bool checked) {
        if (checked) table->selectAll();
        else         table->clearSelection();
    });
    connect(expelSelectedBtn, &QPushButton::clicked, this, &StudentPoolWidget::onExpelSelected);

    // ── Table ─────────────────────────────────────────────────────
    // Wrapper gives real rounded corners + shadow (table border-radius alone doesn't clip cells)
    auto* tableWrap = new ClipFrame(16, body);
    tableWrap->setStyleSheet("QFrame { background:white; border-radius:16px; border:1px solid #E9ECEF; }");
    tableWrap->setGraphicsEffect(makeShadow(tableWrap));
    auto* twLayout = new QVBoxLayout(tableWrap);
    twLayout->setContentsMargins(0, 0, 0, 0);
    twLayout->setSpacing(0);

    table = new QTableWidget(tableWrap);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels(
        { "ID", "Full Name", "Email", "Year", "Gender", "Status", "Actions" });
    table->setCornerButtonEnabled(false);
    table->setStyleSheet(
        "QTableWidget { border:none; background:white; outline:0; }"
        "QTableWidget::item { padding:0px 10px; border:none; border-bottom:1px solid #F3F4F6; }"
        "QTableWidget::item:selected { background:#FFF3E0; color:#212529;"
        "  border-bottom:1px solid #F3F4F6; outline:0; }"
        "QTableWidget::item:focus { outline:0; border:none; }"
        "QHeaderView::section { background:white; color:#9CA3AF; font-weight:600;"
        "  font-size:11px; padding:0px 10px; border:none;"
        "  border-bottom:1px solid #F0F0F0; letter-spacing:0.4px; }"
        "QTableCornerButton::section { background:white; border:none; }"
        "QScrollBar:vertical { width:5px; background:transparent; margin:0; border:none; }"
        "QScrollBar::handle:vertical { background:#DEE2E6; border-radius:3px; min-height:24px; }"
        "QScrollBar::handle:vertical:hover { background:#adb5bd; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; border:none; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background:none; }"
        "QScrollBar:horizontal { height:5px; background:transparent; margin:0; border:none; }"
        "QScrollBar::handle:horizontal { background:#DEE2E6; border-radius:3px; min-width:24px; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width:0; border:none; }"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background:none; }");
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(false);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(false);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    table->horizontalHeader()->setMinimumHeight(46);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    table->setColumnWidth(0, 72);
    table->setColumnWidth(3, 72);
    table->setColumnWidth(4, 80);
    table->setColumnWidth(5, 120);
    table->setColumnWidth(6, 80);
    table->setItemDelegate(new NoFocusDelegate(table));
    table->setShowGrid(false);
    table->setSortingEnabled(false); // manual pre-sort to keep cell widgets intact
    table->horizontalHeader()->setSortIndicatorShown(false);
    table->horizontalHeader()->setSectionsClickable(true);

    connect(table, &QTableWidget::cellDoubleClicked,
            this, &StudentPoolWidget::onCellDoubleClicked);
    connect(table->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &StudentPoolWidget::onHeaderClicked);
    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &StudentPoolWidget::onSelectionChanged);

    twLayout->addWidget(table);
    bodyLayout->addWidget(tableWrap, 1);
    outer->addWidget(body, 1);
}

void StudentPoolWidget::refresh() { applyFilters(); }

void StudentPoolWidget::applyFilters() {
    populateTable(searchEdit->text(),
                  yearCombo->currentData().toInt(),
                  genderCombo->currentText());
}

void StudentPoolWidget::updateTabStyles() {
    for (int i = 0; i < 3; i++) {
        bool active = (i == currentTab);
        tabBtns[i]->setStyleSheet(
            active ?
            "QPushButton { border:none; border-bottom:2px solid #F7931E; color:#F7931E;"
            "  background:transparent; font-weight:bold; padding:10px 24px; font-size:13px; }" :
            "QPushButton { border:none; border-bottom:2px solid transparent; color:#6C757D;"
            "  background:transparent; padding:10px 24px; font-size:13px; }"
            "QPushButton:hover { color:#F7931E; }");
    }
}

void StudentPoolWidget::updateBulkBar() {
    int n     = table->selectionModel()->selectedRows().count();
    int total = table->rowCount();

    // Update tristate checkbox visually without triggering clicked
    if (n == 0 || total == 0)
        selectAllCheck->setCheckState(Qt::Unchecked);
    else if (n == total)
        selectAllCheck->setCheckState(Qt::Checked);
    else
        selectAllCheck->setCheckState(Qt::PartiallyChecked);

    selectionLabel->setVisible(n > 0);
    expelSelectedBtn->setVisible(n > 0);
    if (n > 0)
        selectionLabel->setText(
            QString("%1 student%2 selected").arg(n).arg(n != 1 ? "s" : ""));
}

void StudentPoolWidget::populateTable(const QString& search, int year, const QString& gender) {
    table->setRowCount(0);

    auto all = AppState::instance().allStudents();

    struct Entry { Student s; bool assigned; };
    QVector<Entry> base;

    for (const auto& s : all) {
        bool assigned = AppState::instance().findStudentDorm(s.getId()) != nullptr;

        if (year > 0 && s.getAcademicYear() != year) continue;

        if (gender != "All Genders") {
            QString g = QString::fromStdString(s.getGender());
            if (gender == "Male"   && g != "M" && g.toLower() != "male")   continue;
            if (gender == "Female" && g != "F" && g.toLower() != "female") continue;
        }

        if (!search.isEmpty()) {
            QString fullName = QString::fromStdString(s.getFirstName() + " " + s.getFamilyName()).toLower();
            QString email    = QString::fromStdString(s.getEmail()).toLower();
            QString idStr    = QString::number(s.getId());
            if (!fullName.contains(search.toLower()) &&
                !email.contains(search.toLower()) &&
                !idStr.contains(search))
                continue;
        }
        base.append({s, assigned});
    }

    // Compute tab badge counts from the base-filtered set
    int allCount = base.size(), poolCount = 0, assignedCount = 0;
    for (const auto& e : base) {
        if (e.assigned) assignedCount++;
        else            poolCount++;
    }
    tabBtns[0]->setText(QString("All Students (%1)").arg(allCount));
    tabBtns[1]->setText(QString("Pool (%1)").arg(poolCount));
    tabBtns[2]->setText(QString("Assigned (%1)").arg(assignedCount));

    // Apply tab filter
    QVector<Entry> filtered;
    for (const auto& e : base) {
        if (currentTab == 1 && e.assigned)  continue;
        if (currentTab == 2 && !e.assigned) continue;
        filtered.append(e);
    }

    // Pre-sort the data so cell widgets remain in the right rows
    if (sortCol >= 0 && sortCol < 6) {
        auto key = [this](const Entry& e) -> QString {
            const auto& s = e.s;
            switch (sortCol) {
                case 0: return QString::number(s.getId()).rightJustified(10, '0');
                case 1: return QString::fromStdString(s.getFirstName() + " " + s.getFamilyName()).toLower();
                case 2: return QString::fromStdString(s.getEmail()).toLower();
                case 3: return QString::number(s.getAcademicYear()).rightJustified(2, '0');
                case 4: return QString::fromStdString(s.getGender()).toLower();
                case 5: return e.assigned ? "0" : "1";
                default: return "";
            }
        };
        std::stable_sort(filtered.begin(), filtered.end(),
            [&](const Entry& a, const Entry& b) {
                return sortOrder == Qt::AscendingOrder ? key(a) < key(b) : key(a) > key(b);
            });
    }

    int tabCount = filtered.size();
    table->setRowCount(tabCount);
    for (int row = 0; row < tabCount; row++)
        addTableRow(row, filtered[row].s, filtered[row].assigned);

    if (sortCol >= 0) {
        table->horizontalHeader()->setSortIndicatorShown(true);
        table->horizontalHeader()->setSortIndicator(sortCol, sortOrder);
    } else {
        table->horizontalHeader()->setSortIndicatorShown(false);
    }

    updateBulkBar();
}

void StudentPoolWidget::addTableRow(int row, const Student& s, bool assigned) {
    table->setRowHeight(row, 58);

    auto mkItem = [&](int col, const QString& text, QColor fg, bool bold = false) {
        auto* item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        item->setForeground(fg);
        if (bold) {
            QFont f = item->font();
            f.setBold(true);
            item->setFont(f);
        }
        table->setItem(row, col, item);
    };

    // col 0 — ID: orange, bold
    mkItem(0, QString::number(s.getId()), QColor("#F7931E"), true);

    // col 1 — Full Name: dark, bold
    mkItem(1, QString::fromStdString(s.getFirstName() + " " + s.getFamilyName()),
           QColor("#111827"), true);

    // col 2 — Email: muted gray
    mkItem(2, QString::fromStdString(s.getEmail()), QColor("#6B7280"));

    // col 3 — Year: muted gray
    mkItem(3, QString("Year %1").arg(s.getAcademicYear()), QColor("#6B7280"));

    // col 4 — Gender: muted gray
    QString gRaw = QString::fromStdString(s.getGender());
    mkItem(4, (gRaw == "M" || gRaw.toLower() == "male") ? "Male" : "Female",
           QColor("#6B7280"));

    // col 5 — Status badge
    auto* sw = new QWidget;
    sw->setStyleSheet("background:transparent;");
    auto* sl = new QHBoxLayout(sw);
    sl->setContentsMargins(0, 0, 0, 0);
    auto* badge = new QLabel(assigned ? "Assigned" : "In Pool");
    badge->setStyleSheet(assigned ? Style::badgeGreenSS() : Style::badgeYellowSS());
    badge->setAlignment(Qt::AlignCenter);
    badge->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sl->addWidget(badge);
    sl->addStretch();
    table->setCellWidget(row, 5, sw);

    // col 6 — Actions: eye (view/edit) + trash (expel)
    auto* aw = new QWidget;
    aw->setStyleSheet("background:transparent;");
    auto* al = new QHBoxLayout(aw);
    al->setContentsMargins(0, 0, 0, 0);
    al->setSpacing(12);

    auto* eyeBtn   = IconBtn::eye(aw);
    auto* trashBtn = IconBtn::trash(aw);

    int id = s.getId();
    connect(eyeBtn,   &IconBtn::clicked, this, [this, id]{ onEditRow(id);  });
    connect(trashBtn, &IconBtn::clicked, this, [this, id]{ onExpelRow(id); });

    al->addWidget(eyeBtn);
    al->addWidget(trashBtn);
    al->addStretch();
    table->setCellWidget(row, 6, aw);
}

void StudentPoolWidget::onHeaderClicked(int col) {
    if (col == 6) return; // Actions column: not sortable

    if (col == sortCol)
        sortOrder = (sortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    else {
        sortCol   = col;
        sortOrder = Qt::AscendingOrder;
    }
    applyFilters();
}

void StudentPoolWidget::onSelectionChanged() { updateBulkBar(); }

void StudentPoolWidget::onExpelSelected() {
    auto rows = table->selectionModel()->selectedRows();
    if (rows.isEmpty()) return;

    QVector<int> ids;
    for (const auto& idx : rows) {
        auto* item = table->item(idx.row(), 0);
        if (item) ids.append(item->text().toInt());
    }

    if (!confirmExpel(this,
            QString("Expel <b>%1</b> selected student%2?<br>"
                    "<span style='color:#6C757D;font-size:12px;'>This action cannot be undone.</span>")
            .arg(ids.size()).arg(ids.size() != 1 ? "s" : "")))
        return;

    int expelled = 0;
    for (int id : ids) {
        if (AppState::instance().university.expelStudent(id))
            expelled++;
    }

    if (expelled > 0) {
        AppState::instance().logActivity("EXPELLED",
            QString("%1 student(s) expelled").arg(expelled));
        if (AdminMainWindow::instance()) AdminMainWindow::instance()->refreshAll();
        refresh();
    }
}

void StudentPoolWidget::onAddStudent() {
    AddStudentDialog d(this);
    if (d.exec() == QDialog::Accepted) {
        if (AdminMainWindow::instance()) AdminMainWindow::instance()->refreshAll();
        refresh();
    }
}

void StudentPoolWidget::onEditRow(int studentId) {
    StudentDetailDialog d(studentId, this);
    if (d.exec() == QDialog::Accepted) {
        if (AdminMainWindow::instance()) AdminMainWindow::instance()->refreshAll();
        refresh();
    }
}

void StudentPoolWidget::onAssignRow(int studentId) {
    AssignToDormDialog d(studentId, this);
    if (d.exec() == QDialog::Accepted) {
        if (AdminMainWindow::instance()) AdminMainWindow::instance()->refreshAll();
        refresh();
    }
}

void StudentPoolWidget::onExpelRow(int studentId) {
    const Student* s = AppState::instance().findStudentById(studentId);
    QString name = s ? QString::fromStdString(s->getFirstName() + " " + s->getFamilyName())
                     : "this student";

    if (!confirmExpel(this,
            QString("Are you sure you want to expel <b>%1</b>?<br>"
                    "<span style='color:#6C757D;font-size:12px;'>This action cannot be undone.</span>")
            .arg(name)))
        return;

    if (AppState::instance().university.expelStudent(studentId)) {
        AppState::instance().logActivity("EXPELLED",
            QString("Student expelled: %1").arg(name));
        if (AdminMainWindow::instance()) AdminMainWindow::instance()->refreshAll();
        refresh();
    } else {
        QMessageBox::critical(this, "Error", "Failed to expel student.");
    }
}

void StudentPoolWidget::onCellDoubleClicked(int row, int) {
    auto* item = table->item(row, 0);
    if (!item) return;
    int id = item->text().toInt();
    StudentDetailDialog d(id, this);
    if (d.exec() == QDialog::Accepted) {
        if (AdminMainWindow::instance()) AdminMainWindow::instance()->refreshAll();
        refresh();
    }
}
