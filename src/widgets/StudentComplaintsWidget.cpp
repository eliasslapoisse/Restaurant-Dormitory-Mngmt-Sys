#include "StudentComplaintsWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../dialogs/ComplaintDetailDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QScrollArea>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QAbstractItemView>

StudentComplaintsWidget::StudentComplaintsWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

static QGraphicsDropShadowEffect* cmpShadow(QWidget* w) {
    auto* s = new QGraphicsDropShadowEffect(w);
    s->setBlurRadius(20);
    s->setOffset(0, 4);
    s->setColor(QColor(0, 0, 0, 18));
    return s;
}

void StudentComplaintsWidget::buildUi() {
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background:#F5F5F5; border:none; }");

    auto* content = new QWidget;
    content->setAutoFillBackground(true);
    { QPalette p = content->palette(); p.setColor(QPalette::Window, QColor("#F5F5F5")); content->setPalette(p); }

    auto* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(28, 28, 28, 28);
    mainLayout->setSpacing(20);

    auto* pageTitle = new QLabel("Complaints", content);
    pageTitle->setStyleSheet(
        "QLabel { font-size:26px; font-weight:bold; color:#111827; background:transparent; }");
    mainLayout->addWidget(pageTitle);

    auto* colRow = new QHBoxLayout;
    colRow->setSpacing(16);
    colRow->setAlignment(Qt::AlignTop);

    auto* submitCard = new QFrame(content);
    submitCard->setStyleSheet("QFrame { background:white; border-radius:14px; border:none; }");
    submitCard->setGraphicsEffect(cmpShadow(submitCard));

    auto* sl = new QVBoxLayout(submitCard);
    sl->setContentsMargins(24, 22, 24, 24);
    sl->setSpacing(14);

    auto* submitTitle = new QLabel("Submit New Complaint", submitCard);
    submitTitle->setStyleSheet(
        "QLabel { font-size:15px; font-weight:bold; color:#111827; background:transparent; }");
    sl->addWidget(submitTitle);

    auto* sep = new QFrame(submitCard);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    sep->setFixedHeight(1);
    sl->addWidget(sep);

    // Category
    auto* catLbl = new QLabel("Category", submitCard);
    catLbl->setStyleSheet(
        "QLabel { font-size:12px; font-weight:600; color:#374151; background:transparent; }");
    sl->addWidget(catLbl);

    categoryCombo = new QComboBox(submitCard);
    categoryCombo->addItem("Select category");
    categoryCombo->addItems({ "Maintenance", "Cleanliness", "Noise", "Food", "Other" });
    categoryCombo->setFixedHeight(42);
    categoryCombo->setStyleSheet(
        "QComboBox { border-radius:8px; padding:0 12px;"
        "  background:#F9FAFB; color:#6B7280; font-size:13px; }"
        "QComboBox:focus { background:#FFFBF7; color:#111827; }"
        "QComboBox::drop-down { width:24px; }"
        "QComboBox::down-arrow { image:none; width:0; }");
    categoryCombo->view()->setStyleSheet(
        "QListView { outline:none; background:white; padding:4px; }"
        "QListView::item { padding:9px 14px; color:#212529; border-radius:6px; margin:1px 4px; }"
        "QListView::item:hover    { background:#FFF3E0; color:#F7931E; }"
        "QListView::item:selected { background:#FFF3E0; color:#F7931E; }");
    sl->addWidget(categoryCombo);

    // Description
    auto* descLbl = new QLabel("Description", submitCard);
    descLbl->setStyleSheet(
        "QLabel { font-size:12px; font-weight:600; color:#374151; background:transparent; }");
    sl->addWidget(descLbl);

    descEdit = new QTextEdit(submitCard);
    descEdit->setFixedHeight(140);
    descEdit->setPlaceholderText("Describe your issue in detail...");
    descEdit->setStyleSheet(
        "QTextEdit { border-radius:8px; padding:10px 12px;"
        "  font-size:13px; color:#111827; background:#F9FAFB; }"
        "QTextEdit:focus { background:#FFFBF7; }");
    sl->addWidget(descEdit);

    // Char counter
    charCountLabel = new QLabel("0/500", submitCard);
    charCountLabel->setAlignment(Qt::AlignRight);
    charCountLabel->setStyleSheet(
        "QLabel { font-size:11px; color:#9CA3AF; background:transparent; }");
    sl->addWidget(charCountLabel);

    connect(descEdit, &QTextEdit::textChanged, this, [this] {
        int len = qMin(descEdit->toPlainText().length(), 500);
        charCountLabel->setText(QString("%1/500").arg(len));
        if (descEdit->toPlainText().length() > 500) {
            QTextCursor cursor = descEdit->textCursor();
            descEdit->setPlainText(descEdit->toPlainText().left(500));
            descEdit->setTextCursor(cursor);
        }
    });

    submitStatusLabel = new QLabel("", submitCard);
    submitStatusLabel->setStyleSheet("QLabel { font-size:12px; background:transparent; }");
    sl->addWidget(submitStatusLabel);

    auto* submitBtn = new QPushButton("✈   Submit Complaint", submitCard);
    submitBtn->setFixedHeight(46);
    submitBtn->setCursor(Qt::PointingHandCursor);
    submitBtn->setStyleSheet(
        QString("QPushButton { background:%1; color:white; border:none;"
                "  border-radius:10px; font-size:14px; font-weight:bold; }"
                "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "  stop:0 #E65C2A, stop:1 #E8A522); }").arg(Style::gradientCSS()));
    connect(submitBtn, &QPushButton::clicked, this, &StudentComplaintsWidget::onSubmit);
    sl->addWidget(submitBtn);

    colRow->addWidget(submitCard, 1);

    auto* histCard = new QFrame(content);
    histCard->setStyleSheet("QFrame { background:white; border-radius:14px; border:none; }");
    histCard->setGraphicsEffect(cmpShadow(histCard));
    histCard->setMinimumHeight(300);

    auto* hl = new QVBoxLayout(histCard);
    hl->setContentsMargins(24, 22, 24, 16);
    hl->setSpacing(14);

    complaintsTitle = new QLabel("My Complaints (0)", histCard);
    complaintsTitle->setStyleSheet(
        "QLabel { font-size:15px; font-weight:bold; color:#111827; background:transparent; }");
    hl->addWidget(complaintsTitle);

    auto* sep2 = new QFrame(histCard);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    sep2->setFixedHeight(1);
    hl->addWidget(sep2);

    // Scrollable list area
    auto* listScroll = new QScrollArea(histCard);
    listScroll->setWidgetResizable(true);
    listScroll->setFrameShape(QFrame::NoFrame);
    listScroll->setStyleSheet("QScrollArea { background:transparent; border:none; }");

    complaintsListContent = new QWidget;
    complaintsListContent->setAutoFillBackground(true);
    { QPalette p = complaintsListContent->palette();
      p.setColor(QPalette::Window, Qt::white); complaintsListContent->setPalette(p); }

    complaintsListLayout = new QVBoxLayout(complaintsListContent);
    complaintsListLayout->setContentsMargins(0, 0, 0, 0);
    complaintsListLayout->setSpacing(10);
    complaintsListLayout->addStretch();

    listScroll->setWidget(complaintsListContent);
    hl->addWidget(listScroll, 1);

    colRow->addWidget(histCard, 1);
    mainLayout->addLayout(colRow);
    mainLayout->addStretch();

    scroll->setWidget(content);
    outerLayout->addWidget(scroll);
}

void StudentComplaintsWidget::refresh() {
    populateHistory();
}

void StudentComplaintsWidget::populateHistory() {
    auto& app = AppState::instance();

    // Clear old items (remove everything except the stretch at the end)
    while (complaintsListLayout->count() > 1) {
        auto* item = complaintsListLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int count = 0;
    for (auto& c : app.complaints) {
        if (c.studentId != app.currentStudentId) continue;

        // Complaint card
        auto* card = new QFrame(complaintsListContent);
        card->setStyleSheet(
            "QFrame { background:#F9FAFB; border-radius:10px; }");
        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(16, 12, 16, 12);
        cl->setSpacing(6);

        // Top row: category + status
        auto* topRow = new QHBoxLayout;
        auto* catLbl = new QLabel(c.category, card);
        catLbl->setStyleSheet(
            "QLabel { font-size:12px; font-weight:bold; color:#F7931E; background:transparent; }");
        auto* dateLbl = new QLabel(c.dateTime.toString("yyyy-MM-dd"), card);
        dateLbl->setStyleSheet(
            "QLabel { font-size:11px; color:#9CA3AF; background:transparent; }");
        auto* statusLbl = new QLabel(c.status, card);
        statusLbl->setAlignment(Qt::AlignCenter);
        statusLbl->setStyleSheet(c.status == "Resolved"
            ? "QLabel { color:#065F46; font-size:11px; font-weight:bold; }"
            : "QLabel { color:#854D0E; font-size:11px; font-weight:bold; }");
        topRow->addWidget(catLbl);
        topRow->addSpacing(8);
        topRow->addWidget(dateLbl);
        topRow->addStretch();
        topRow->addWidget(statusLbl);
        cl->addLayout(topRow);

        // Description preview
        QString shortDesc = c.description.length() > 80
            ? c.description.left(77) + "..."
            : c.description;
        auto* descLbl = new QLabel(shortDesc, card);
        descLbl->setWordWrap(true);
        descLbl->setStyleSheet(
            "QLabel { font-size:12px; color:#6B7280; background:transparent; }");
        cl->addWidget(descLbl);

        // If admin replied, show a "View Reply" hint
        if (!c.adminReply.isEmpty()) {
            auto* replyHint = new QLabel("💬 Admin replied — click to view", card);
            replyHint->setStyleSheet(
                "QLabel { font-size:11px; color:#F7931E; background:transparent; font-weight:600; }");
            cl->addWidget(replyHint);
        }

        // Overlay button for click
        auto* btn = new QPushButton(card);
        btn->setFlat(true);
        btn->setStyleSheet("QPushButton { background:transparent; border:none; }");
        btn->setCursor(Qt::PointingHandCursor);
        btn->setGeometry(0, 0, 2000, 200);
        btn->raise();

        int cid = c.id;
        connect(btn, &QPushButton::clicked, this, [this, cid] {
            ComplaintDetailDialog dlg(cid, false, this);
            dlg.exec();
            populateHistory();
        });

        complaintsListLayout->insertWidget(complaintsListLayout->count() - 1, card);
        count++;
    }

    complaintsTitle->setText(QString("My Complaints (%1)").arg(count));

    if (count == 0) {
        auto* emptyLbl = new QLabel("No complaints submitted yet.", complaintsListContent);
        emptyLbl->setAlignment(Qt::AlignCenter);
        emptyLbl->setStyleSheet(
            "QLabel { color:#9CA3AF; font-size:13px; background:transparent; padding:20px; }");
        complaintsListLayout->insertWidget(0, emptyLbl);
    }
}

void StudentComplaintsWidget::onSubmit() {
    auto& app = AppState::instance();

    if (categoryCombo->currentIndex() == 0) {
        submitStatusLabel->setStyleSheet(
            "QLabel { font-size:12px; color:#DC3545; background:transparent; }");
        submitStatusLabel->setText("Please select a category.");
        return;
    }

    QString desc = descEdit->toPlainText().trimmed();
    if (desc.length() < 10) {
        submitStatusLabel->setStyleSheet(
            "QLabel { font-size:12px; color:#DC3545; background:transparent; }");
        submitStatusLabel->setText("Description must be at least 10 characters.");
        return;
    }

    const Student* s = app.findStudentById(app.currentStudentId);
    if (!s) return;

    Complaint c;
    c.id          = ++app.complaintIdCounter;
    c.studentId   = app.currentStudentId;
    c.studentName = QString::fromStdString(s->getFirstName() + " " + s->getFamilyName());
    c.dateTime    = QDateTime::currentDateTime();
    c.category    = categoryCombo->currentText();
    c.description = desc;
    c.status      = "Pending";
    app.complaints.append(c);

    app.logActivity("COMPLAINT_SUBMITTED",
        QString("Student %1 submitted a %2 complaint")
        .arg(app.currentStudentId).arg(c.category));

    descEdit->clear();
    categoryCombo->setCurrentIndex(0);
    submitStatusLabel->setStyleSheet(
        "QLabel { font-size:12px; color:#28A745; background:transparent; }");
    submitStatusLabel->setText("✓ Complaint submitted successfully.");
    populateHistory();
}
