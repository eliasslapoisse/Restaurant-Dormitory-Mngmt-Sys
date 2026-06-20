#include "ComplaintDetailDialog.h"
#include "../stylehelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

ComplaintDetailDialog::ComplaintDetailDialog(int complaintId, bool adminView, QWidget* parent)
    : QDialog(parent), m_id(complaintId), m_adminView(adminView)
{
    setWindowTitle("Complaint Details");
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(520);
    buildUi();
}

void ComplaintDetailDialog::buildUi() {
    Complaint* c = findComplaint();
    if (!c) { reject(); return; }

    // No shell margins — card fills the dialog, no glass border
    auto* shell = new QVBoxLayout(this);
    shell->setContentsMargins(0, 0, 0, 0);
    shell->setSpacing(0);

    auto* card = new QFrame(this);
    card->setObjectName("card");
    card->setStyleSheet("QFrame#card { background:white; border-radius:14px; }");
    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(28); shadow->setOffset(0, 6); shadow->setColor(QColor(0,0,0,45));
    card->setGraphicsEffect(shadow);
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);
    shell->addWidget(card);


    auto* header = new QFrame(card);
    header->setFixedHeight(70);
    header->setStyleSheet(
        QString("QFrame { background:%1; border-top-left-radius:14px;"
                "  border-top-right-radius:14px; }").arg(Style::gradientCSS("0","0","1","1")));
    auto* hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(24, 0, 16, 0);

    auto* hdTitle = new QLabel("Complaint Details", header);
    hdTitle->setStyleSheet("QLabel { color:white; font-size:17px; font-weight:bold; background:transparent; }");
    hLayout->addWidget(hdTitle, 1);

    auto* closeBtn = new QPushButton("✕", header);
    closeBtn->setFixedSize(32, 32);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { background:rgba(255,255,255,0.22); color:white; border:none;"
        "  border-radius:16px; font-size:14px; font-weight:bold; }"
        "QPushButton:hover { background:rgba(255,255,255,0.38); }");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    hLayout->addWidget(closeBtn);
    cardLayout->addWidget(header);


    auto* body = new QWidget(card);
    body->setStyleSheet("QWidget { background:transparent; }");
    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(24, 20, 24, 24);
    bodyLayout->setSpacing(14);

    // Student row
    auto* studentRow = new QHBoxLayout;
    studentRow->setSpacing(14);

    QChar initial = c->studentName.isEmpty() ? '?' : c->studentName[0].toUpper();
    auto* avatar = new QLabel(initial, body);
    avatar->setFixedSize(48, 48);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(
        QString("QLabel { background:%1; color:white; border-radius:24px;"
                "  font-size:18px; font-weight:bold; }").arg(Style::PRIMARY_ORANGE));

    auto* infoCol = new QVBoxLayout;
    infoCol->setSpacing(2);
    auto* sNameLbl = new QLabel(c->studentName, body);
    sNameLbl->setStyleSheet("QLabel { font-size:14px; font-weight:bold; color:#111827; background:transparent; }");
    auto* sCatDateLbl = new QLabel(c->category + "  ·  " + c->dateTime.toString("MMM d, yyyy"), body);
    sCatDateLbl->setStyleSheet("QLabel { color:#9CA3AF; font-size:12px; background:transparent; }");
    infoCol->addWidget(sNameLbl);
    infoCol->addWidget(sCatDateLbl);

    statusLabel = new QLabel(c->status, body);
    statusLabel->setFixedHeight(26);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(c->status == "Resolved"
        ? "QLabel { background:#D1FAE5; color:#065F46; border-radius:13px;"
          "  font-size:11px; font-weight:bold; padding:0 12px; }"
        : "QLabel { background:#FEF3C7; color:#92400E; border-radius:13px;"
          "  font-size:11px; font-weight:bold; padding:0 12px; }");

    studentRow->addWidget(avatar);
    studentRow->addLayout(infoCol, 1);
    studentRow->addWidget(statusLabel);
    bodyLayout->addLayout(studentRow);

    // Separator
    auto* sep = new QFrame(body);
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    sep->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    bodyLayout->addWidget(sep);

    // Description
    auto* descHdr = new QLabel("DESCRIPTION", body);
    descHdr->setStyleSheet("QLabel { font-size:11px; font-weight:bold; color:#9CA3AF;"
                           "  letter-spacing:0.8px; background:transparent; }");
    bodyLayout->addWidget(descHdr);

    descriptionEdit = new QTextEdit(body);
    descriptionEdit->setPlainText(c->description);
    descriptionEdit->setReadOnly(true);
    descriptionEdit->setFixedHeight(90);
    descriptionEdit->setStyleSheet(
        "QTextEdit { border:1px solid #E9ECEF; border-radius:8px;"
        "  background:#F9FAFB; padding:10px; font-size:13px; color:#374151; }");
    bodyLayout->addWidget(descriptionEdit);

    // Admin view: reply textarea + Mark Resolved
    if (m_adminView) {
        if (!c->adminReply.isEmpty()) {
            auto* replyBox = new QFrame(body);
            replyBox->setStyleSheet(
                "QFrame { background:#EBF5FB; border-radius:8px; border:none; }"
                "QLabel { border:none; background:transparent; }");
            auto* rbl = new QVBoxLayout(replyBox);
            rbl->setContentsMargins(14, 10, 14, 10); rbl->setSpacing(4);
            auto* replyHdr = new QLabel("Admin Reply", replyBox);
            replyHdr->setStyleSheet("QLabel { font-size:11px; font-weight:bold; color:#1E40AF; }");
            auto* replyTxt = new QLabel(c->adminReply, replyBox);
            replyTxt->setWordWrap(true);
            replyTxt->setStyleSheet("QLabel { font-size:13px; color:#1E3A5F; background:transparent; }");
            rbl->addWidget(replyHdr);
            rbl->addWidget(replyTxt);
            bodyLayout->addWidget(replyBox);
        }

        if (c->status == "Pending") {
            auto* addReplyHdr = new QLabel("ADD REPLY", body);
            addReplyHdr->setStyleSheet("QLabel { font-size:11px; font-weight:bold; color:#9CA3AF;"
                                       "  letter-spacing:0.8px; background:transparent; }");
            replyEdit = new QTextEdit(body);
            replyEdit->setPlaceholderText("Type your reply here…");
            replyEdit->setFixedHeight(70);
            replyEdit->setStyleSheet(
                "QTextEdit { border:1px solid #E9ECEF; border-radius:8px;"
                "  background:white; padding:8px; font-size:13px; color:#374151; }"
                "QTextEdit:focus { border:1px solid #F7931E; }");
            bodyLayout->addWidget(addReplyHdr);
            bodyLayout->addWidget(replyEdit);

            auto* resolveBtn = new QPushButton("✓  Mark Resolved", body);
            resolveBtn->setFixedHeight(44);
            resolveBtn->setCursor(Qt::PointingHandCursor);
            resolveBtn->setStyleSheet(
                "QPushButton { background:#ECFDF5; color:#065F46; border:1.5px solid #6EE7B7;"
                "  border-radius:10px; font-size:14px; font-weight:bold; }"
                "QPushButton:hover { background:#D1FAE5; }");
            connect(resolveBtn, &QPushButton::clicked, this, &ComplaintDetailDialog::onMarkResolved);
            bodyLayout->addWidget(resolveBtn);
        }
    } else {
        replyEdit = nullptr;
    }

    cardLayout->addWidget(body);
}

Complaint* ComplaintDetailDialog::findComplaint() {
    for (auto& c : AppState::instance().complaints)
        if (c.id == m_id) return &c;
    return nullptr;
}

void ComplaintDetailDialog::onMarkResolved() {
    Complaint* c = findComplaint();
    if (!c) return;
    // Save reply text if provided
    if (replyEdit && !replyEdit->toPlainText().trimmed().isEmpty())
        c->adminReply = replyEdit->toPlainText().trimmed();
    c->status = "Resolved";
    statusLabel->setStyleSheet(
        "QLabel { background:#D1FAE5; color:#065F46; border-radius:13px;"
        "  font-size:11px; font-weight:bold; padding:0 12px; }");
    statusLabel->setText("Resolved");
    accept();
}

void ComplaintDetailDialog::onSaveReply() {}
