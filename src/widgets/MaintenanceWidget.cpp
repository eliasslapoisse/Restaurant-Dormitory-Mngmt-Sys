#include "MaintenanceWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../dormitory.h"
#include "../block.h"
#include "../room.h"
#include "../dialogs/ComplaintDetailDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QDialog>
#include <QAbstractButton>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QBitmap>
#include <QGraphicsDropShadowEffect>
#include <algorithm>

// ── Helpers (identical conventions to StudentPoolWidget) ──────────────────────

static QGraphicsDropShadowEffect* makeShadow(QObject* parent) {
    auto* e = new QGraphicsDropShadowEffect(parent);
    e->setBlurRadius(18);
    e->setOffset(0, 3);
    e->setColor(QColor(0, 0, 0, 28));
    return e;
}

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

class NoFocusDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void initStyleOption(QStyleOptionViewItem* opt, const QModelIndex& idx) const override {
        QStyledItemDelegate::initStyleOption(opt, idx);
        opt->state &= ~QStyle::State_HasFocus;
    }
};

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
            QPainterPath path;
            path.moveTo(cx - 7, cy);
            path.cubicTo(cx - 4, cy - 5, cx + 4, cy - 5, cx + 7, cy);
            path.cubicTo(cx + 4, cy + 5, cx - 4, cy + 5, cx - 7, cy);
            p.setPen(QPen(c, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(Qt::NoBrush);
            p.drawPath(path);
            p.setPen(Qt::NoPen);
            p.setBrush(c);
            p.drawEllipse(QPointF(cx, cy), 2.2, 2.2);
        } else {
            p.setPen(QPen(c, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(QRectF(cx - 5, cy - 3, 10, 9), 1, 1);
            p.drawLine(QPointF(cx - 7, cy - 3), QPointF(cx + 7, cy - 3));
            p.drawRoundedRect(QRectF(cx - 2.5, cy - 6.5, 5, 3.5), 1, 1);
            p.drawLine(QPointF(cx - 2, cy - 0.5), QPointF(cx - 2, cy + 4.5));
            p.drawLine(QPointF(cx + 2, cy - 0.5), QPointF(cx + 2, cy + 4.5));
        }
    }
};

class StyledCombo : public QComboBox {
    bool _hovered = false;
public:
    explicit StyledCombo(QWidget* parent = nullptr) : QComboBox(parent) {
        view()->setStyleSheet(
            "QListView { outline: none; padding: 4px; background: white; }"
            "QListView::item {"
            "  padding: 9px 16px; color: #212529;"
            "  border-radius: 7px; margin: 1px 4px; }"
            "QListView::item:hover { background: #FFF3E0; color: #F7931E; }"
            "QListView::item:selected,"
            "QListView::item:selected:hover { background: #F7931E; color: white; }");
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
        p.setPen(QPen(QColor("#DEE2E6"), 1.0));
        p.setBrush(Qt::white);
        p.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 10, 10);
        p.setPen(QColor("#212529"));
        QFont f = font();
        f.setPixelSize(13);
        p.setFont(f);
        p.drawText(rect().adjusted(12, 0, -30, 0),
                   Qt::AlignVCenter | Qt::AlignLeft, currentText());
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

static QString findStudentRoom(int studentId) {
    auto& uni = AppState::instance().university;
    for (int d = 1; d <= 6; d++) {
        Dormitory* dorm = uni.getDormitory(d);
        if (!dorm) continue;
        for (const auto& block : dorm->getBlocks()) {
            Room* room = const_cast<Block&>(block).findRoomByStudentID(studentId);
            if (room) {
                return QString("Dorm %1  ·  Block %2  ·  Floor %3  ·  Room %4")
                    .arg(d)
                    .arg(QString::fromStdString(block.getName()))
                    .arg(room->getFloor())
                    .arg(room->getNumber());
            }
        }
    }
    return "Not assigned";
}

static QString categoryIcon(const QString& cat) {
    if (cat == "Cleanliness")  return "🧹";
    if (cat == "Noise")        return "🔊";
    if (cat == "Maintenance")  return "🔧";
    if (cat == "Food")         return "🍽️";
    return "📋";
}

// ── MaintenanceWidget ─────────────────────────────────────────────────────────

MaintenanceWidget::MaintenanceWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor("#F8F9FA"));
    setPalette(p);
}

void MaintenanceWidget::buildUi() {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // ── Gradient header ───────────────────────────────────────────────────────
    auto* header = new QFrame(this);
    header->setFixedHeight(130);
    header->setStyleSheet(
        QString("QFrame { background:%1; border-radius:0; }").arg(Style::gradientCSS("0","0","1","1")));
    auto* hLayout = new QVBoxLayout(header);
    hLayout->setContentsMargins(32, 18, 32, 18);
    hLayout->setSpacing(4);

    auto* topRow = new QHBoxLayout;
    auto* sectionTag = new QLabel("SUPPORT", header);
    sectionTag->setStyleSheet("QLabel { color:rgba(255,255,255,0.85); font-size:11px;"
                              "  letter-spacing:1.5px; font-weight:bold; background:transparent; }");
    topRow->addWidget(sectionTag);
    topRow->addStretch();
    hLayout->addLayout(topRow);

    auto* titleLbl = new QLabel("Maintenance & Complaints", header);
    titleLbl->setStyleSheet("QLabel { color:white; font-size:30px; font-weight:bold; background:transparent; }");
    hLayout->addWidget(titleLbl);

    auto* subLbl = new QLabel("Student requests, repairs, and issue resolution", header);
    subLbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.8); font-size:13px; background:transparent; }");
    hLayout->addWidget(subLbl);

    outer->addWidget(header);

    // ── Body ──────────────────────────────────────────────────────────────────
    auto* body = new QWidget(this);
    body->setAutoFillBackground(true);
    { QPalette bp = body->palette(); bp.setColor(QPalette::Window, QColor("#F8F9FA")); body->setPalette(bp); }

    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(28, 18, 28, 28);
    bodyLayout->setSpacing(16);

    // ── Filter bar ────────────────────────────────────────────────────────────
    auto* filterCard = new QFrame(body);
    filterCard->setStyleSheet("QFrame { background:white; border-radius:14px; border:1px solid #E9ECEF; }");
    filterCard->setGraphicsEffect(makeShadow(filterCard));
    auto* filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(20, 14, 20, 14);
    filterLayout->setSpacing(14);

    searchEdit = new QLineEdit(filterCard);
    searchEdit->setPlaceholderText("Search by student name...");
    searchEdit->setFixedHeight(40);
    searchEdit->setStyleSheet(Style::inputSS());
    filterLayout->addWidget(searchEdit, 2);

    statusCombo = new StyledCombo(filterCard);
    statusCombo->addItems({ "All", "Pending", "Resolved" });
    filterLayout->addWidget(statusCombo);

    bodyLayout->addWidget(filterCard);

    connect(searchEdit,  &QLineEdit::textChanged,
            this, [this](const QString&){ applyFilters(); });
    connect(statusCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, [this](int){ applyFilters(); });

    // ── Table ─────────────────────────────────────────────────────────────────
    auto* tableWrap = new ClipFrame(16, body);
    tableWrap->setStyleSheet("QFrame { background:white; border-radius:16px; border:1px solid #E9ECEF; }");
    tableWrap->setGraphicsEffect(makeShadow(tableWrap));
    auto* twLayout = new QVBoxLayout(tableWrap);
    twLayout->setContentsMargins(0, 0, 0, 0);
    twLayout->setSpacing(0);

    table = new QTableWidget(tableWrap);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({ "ID", "Student", "Category", "Date", "Status", "Actions" });
    table->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    table->setCornerButtonEnabled(false);
    table->setStyleSheet(
        "QTableWidget { border:none; background:white; outline:0; }"
        "QTableWidget::item { padding:0px 8px; border:none; border-bottom:1px solid #F3F4F6; }"
        "QTableWidget::item:selected { background:#FFF3E0; color:#212529;"
        "  border-bottom:1px solid #F3F4F6; outline:0; }"
        "QTableWidget::item:focus { outline:0; border:none; }"
        "QHeaderView::section { background:white; color:#9CA3AF; font-weight:600;"
        "  font-size:11px; padding:0px 8px; border:none;"
        "  border-bottom:1px solid #F0F0F0; letter-spacing:0.4px; }"
        "QTableCornerButton::section { background:white; border:none; }"
        "QScrollBar:vertical { width:5px; background:transparent; margin:0; border:none; }"
        "QScrollBar::handle:vertical { background:#DEE2E6; border-radius:3px; min-height:24px; }"
        "QScrollBar::handle:vertical:hover { background:#adb5bd; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; border:none; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background:none; }");
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(false);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(false);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    table->horizontalHeader()->setMinimumHeight(46);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    table->setColumnWidth(0, 72);
    table->setColumnWidth(1, 220);
    table->setColumnWidth(3, 130);
    table->setColumnWidth(4, 110);
    table->setColumnWidth(5, 90);
    table->setItemDelegate(new NoFocusDelegate(table));
    table->setShowGrid(false);
    table->setFocusPolicy(Qt::NoFocus);

    connect(table, &QTableWidget::cellDoubleClicked,
            this, &MaintenanceWidget::onCellDoubleClicked);

    twLayout->addWidget(table);
    bodyLayout->addWidget(tableWrap, 1);
    outer->addWidget(body, 1);
}

void MaintenanceWidget::refresh()      { applyFilters(); }
void MaintenanceWidget::applyFilters() { populateTable(searchEdit->text(), statusCombo->currentText()); }

void MaintenanceWidget::populateTable(const QString& search, const QString& status) {
    table->setRowCount(0);
    auto& complaints = AppState::instance().complaints;
    int row = 0;

    for (const auto& c : complaints) {
        if (status != "All" && c.status != status) continue;
        if (!search.isEmpty()) {
            if (!c.studentName.toLower().contains(search.toLower()) &&
                !QString::number(c.studentId).contains(search))
                continue;
        }
        table->setRowCount(row + 1);
        addTableRow(row, c);
        row++;
    }
}

void MaintenanceWidget::addTableRow(int row, const Complaint& c) {
    table->setRowHeight(row, 64);

    auto mkItem = [&](int col, const QString& text, QColor fg, bool bold = false) {
        auto* item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        item->setForeground(fg);
        item->setData(Qt::UserRole, c.id);
        if (bold) { QFont f = item->font(); f.setBold(true); item->setFont(f); }
        table->setItem(row, col, item);
    };

    // col 0 — ID: centered, orange, bold
    mkItem(0, QString::number(c.id), QColor("#F7931E"), true);
    table->item(row, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    // col 1 — Student: name + room info
    auto* stuW = new QWidget;
    stuW->setStyleSheet("background:transparent;");
    auto* stuL = new QVBoxLayout(stuW);
    stuL->setContentsMargins(8, 0, 8, 0);
    stuL->setSpacing(2);
    auto* nameLbl = new QLabel(c.studentName, stuW);
    nameLbl->setStyleSheet("QLabel { font-size:13px; font-weight:bold; color:#111827; background:transparent; border:none; }");
    auto* roomLbl = new QLabel(findStudentRoom(c.studentId), stuW);
    roomLbl->setStyleSheet("QLabel { font-size:11px; color:#9CA3AF; background:transparent; border:none; }");
    stuL->addStretch();
    stuL->addWidget(nameLbl);
    stuL->addWidget(roomLbl);
    stuL->addStretch();
    table->setCellWidget(row, 1, stuW);
    auto* stuItem = new QTableWidgetItem;
    stuItem->setData(Qt::UserRole, c.id);
    table->setItem(row, 1, stuItem);

    // col 2 — Category: emoji + text as plain item
    mkItem(2, categoryIcon(c.category) + "  " + c.category, QColor("#6B7280"));

    // col 3 — Date: muted
    mkItem(3, c.dateTime.toString("MMM d, yyyy"), QColor("#6B7280"));

    // col 4 — Status badge
    auto* sw = new QWidget;
    sw->setStyleSheet("background:transparent;");
    auto* sl = new QHBoxLayout(sw);
    sl->setContentsMargins(4, 0, 0, 0);
    auto* badge = new QLabel(c.status);
    badge->setStyleSheet(c.status == "Resolved" ? Style::badgeGreenSS() : Style::badgeYellowSS());
    badge->setAlignment(Qt::AlignCenter);
    badge->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sl->addWidget(badge);
    sl->addStretch();
    table->setCellWidget(row, 4, sw);
    auto* statusItem = new QTableWidgetItem;
    statusItem->setData(Qt::UserRole, c.id);
    table->setItem(row, 4, statusItem);

    // col 5 — Actions: eye + trash
    auto* aw = new QWidget;
    aw->setStyleSheet("background:transparent;");
    auto* al = new QHBoxLayout(aw);
    al->setContentsMargins(4, 0, 0, 0);
    al->setSpacing(12);
    auto* eyeBtn   = IconBtn::eye(aw);
    auto* trashBtn = IconBtn::trash(aw);
    int cid = c.id;
    connect(eyeBtn,   &IconBtn::clicked, this, [this, cid]{ onViewRow(cid);   });
    connect(trashBtn, &IconBtn::clicked, this, [this, cid]{ onDeleteRow(cid); });
    al->addWidget(eyeBtn);
    al->addWidget(trashBtn);
    al->addStretch();
    table->setCellWidget(row, 5, aw);
}

void MaintenanceWidget::onViewRow(int complaintId) {
    ComplaintDetailDialog d(complaintId, true, this);
    if (d.exec() == QDialog::Accepted) applyFilters();
}

void MaintenanceWidget::onDeleteRow(int complaintId) {
    auto& complaints = AppState::instance().complaints;
    auto it = std::find_if(complaints.begin(), complaints.end(),
        [complaintId](const Complaint& c){ return c.id == complaintId; });
    if (it == complaints.end()) return;

    QDialog dlg(this);
    dlg.setWindowTitle("Delete Complaint");
    dlg.setFixedWidth(420);
    auto* vl = new QVBoxLayout(&dlg);
    vl->setContentsMargins(28, 28, 28, 22);
    vl->setSpacing(22);

    auto* lbl = new QLabel(
        QString("Delete complaint from <b>%1</b>?<br>"
                "<span style='color:#6C757D;font-size:12px;'>This action cannot be undone.</span>")
        .arg(it->studentName), &dlg);
    lbl->setWordWrap(true);
    lbl->setStyleSheet("QLabel { color:#212529; font-size:14px; background:transparent; }");
    vl->addWidget(lbl);

    auto* hl = new QHBoxLayout;
    hl->setSpacing(10);
    hl->addStretch();

    auto* cancelBtn = new QPushButton("Cancel", &dlg);
    cancelBtn->setFixedHeight(36);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(Style::secondaryBtnSS());

    auto* deleteBtn = new QPushButton("Delete", &dlg);
    deleteBtn->setFixedHeight(36);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setStyleSheet(Style::dangerBtnSS());

    QObject::connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
    QObject::connect(deleteBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    hl->addWidget(cancelBtn);
    hl->addWidget(deleteBtn);
    vl->addLayout(hl);

    if (dlg.exec() != QDialog::Accepted) return;

    complaints.erase(it);
    applyFilters();
}

void MaintenanceWidget::onCellDoubleClicked(int row, int col) {
    if (col == 5) return;
    auto* item = table->item(row, 0);
    if (!item) return;
    int cid = item->data(Qt::UserRole).toInt();
    onViewRow(cid);
}
