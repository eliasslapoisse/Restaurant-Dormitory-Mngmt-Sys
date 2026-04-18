#include "loginwindow.h"
#include "datamanager.h"
#include "styles.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QDateTime>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QFrame>

LoginWindow::LoginWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("UDRMS — Login");
    setFixedSize(1100, 700);
    setStyleSheet(Styles::APP_STYLE);

    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &LoginWindow::updateClock);
    m_clockTimer->start(1000);

    buildUi();
}

void LoginWindow::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Window background
    p.fillRect(rect(), QColor("#080C14"));

    // Left panel gradient background
    QLinearGradient leftGrad(0, 0, 550, 700);
    leftGrad.setColorAt(0.0, QColor("#0D1117"));
    leftGrad.setColorAt(0.4, QColor("#130D1A"));
    leftGrad.setColorAt(1.0, QColor("#1A0D0A"));
    p.fillRect(0, 0, 550, 700, leftGrad);

    // Decorative glowing circles (left panel)
    p.setOpacity(0.12);
    p.setBrush(QColor("#FF6B35"));
    p.setPen(Qt::NoPen);
    p.drawEllipse(-80, -80, 350, 350);
    p.setOpacity(0.08);
    p.setBrush(QColor("#F7931E"));
    p.drawEllipse(200, 400, 400, 400);
    p.setOpacity(0.05);
    p.setBrush(QColor("#FFB830"));
    p.drawEllipse(100, 200, 250, 250);
    p.setOpacity(1.0);

    // Grid pattern overlay (left)
    p.setOpacity(0.04);
    p.setPen(QPen(QColor("#FF6B35"), 0.5));
    for (int x = 0; x < 550; x += 40) p.drawLine(x, 0, x, 700);
    for (int y = 0; y < 700; y += 40) p.drawLine(0, y, 550, y);
    p.setOpacity(1.0);

    // Separator line with glow
    QLinearGradient sepGrad(550, 0, 551, 700);
    sepGrad.setColorAt(0.0, Qt::transparent);
    sepGrad.setColorAt(0.3, QColor("#FF6B35"));
    sepGrad.setColorAt(0.7, QColor("#F7931E"));
    sepGrad.setColorAt(1.0, Qt::transparent);
    p.setPen(Qt::NoPen);
    p.setBrush(sepGrad);
    p.drawRect(549, 0, 2, 700);
}

void LoginWindow::buildUi() {
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Left panel widget
    auto* left = new QWidget(this);
    left->setFixedWidth(550);
    left->setAttribute(Qt::WA_TranslucentBackground);
    buildLeftPanel(left);
    root->addWidget(left);

    // Right panel
    auto* right = new QWidget(this);
    right->setFixedWidth(550);
    right->setStyleSheet("background-color: #0D1117;");
    buildRightPanel(right);
    root->addWidget(right);
}

void LoginWindow::buildLeftPanel(QWidget* parent) {
    auto* lay = new QVBoxLayout(parent);
    lay->setContentsMargins(50, 50, 50, 50);
    lay->setSpacing(0);

    // Clock top-right
    m_clockLabel = new QLabel(this);
    m_clockLabel->setStyleSheet("color: #8896A8; font-size: 13px; background: transparent;");
    updateClock();
    lay->addWidget(m_clockLabel, 0, Qt::AlignRight);

    lay->addStretch(2);

    // Logo / icon (ASCII art / symbol)
    auto* logoLbl = new QLabel("🏛", parent);
    logoLbl->setStyleSheet("font-size: 72px; background: transparent;");
    logoLbl->setAlignment(Qt::AlignCenter);
    lay->addWidget(logoLbl, 0, Qt::AlignCenter);
    lay->addSpacing(20);

    // University name
    auto* uniLabel = new QLabel("Sidi Abdollah\nNational School of\nArtificial Intelligence", parent);
    uniLabel->setStyleSheet(
        "font-size: 26px; font-weight: 900; color: #E8EDF5; background: transparent;"
        "line-height: 1.3; letter-spacing: 0.5px;");
    uniLabel->setAlignment(Qt::AlignCenter);
    uniLabel->setWordWrap(true);
    lay->addWidget(uniLabel);
    lay->addSpacing(10);

    // Subtitle
    auto* subtitle = new QLabel("University Dormitory & Restaurant\nManagement System", parent);
    subtitle->setStyleSheet("font-size: 14px; color: #FF6B35; font-weight: 600; background: transparent;");
    subtitle->setAlignment(Qt::AlignCenter);
    lay->addWidget(subtitle);
    lay->addSpacing(40);

    // Feature pills
    struct FeaturePill { QString icon; QString text; };
    QList<FeaturePill> features = {
        {"🏠", "6 Dormitory Complexes"},
        {"👨‍🎓", "Student Lifecycle Management"},
        {"🍽️", "Restaurant & Menu Planner"},
        {"🔧", "Maintenance Request System"},
        {"📦", "Inventory Control Center"}
    };

    for (const auto& f : features) {
        auto* row = new QHBoxLayout();
        auto* icon = new QLabel(f.icon, parent);
        icon->setStyleSheet("font-size: 16px; background: transparent;");
        auto* txt = new QLabel(f.text, parent);
        txt->setStyleSheet("font-size: 13px; color: #8896A8; background: transparent;");
        row->addStretch();
        row->addWidget(icon);
        row->addSpacing(8);
        row->addWidget(txt);
        row->addStretch();
        lay->addLayout(row);
        lay->addSpacing(6);
    }

    lay->addStretch(3);

    // Bottom note
    auto* ver = new QLabel("ENSIA 2025–2026  •  v1.0.0", parent);
    ver->setStyleSheet("font-size: 11px; color: #4A5568; background: transparent;");
    ver->setAlignment(Qt::AlignCenter);
    lay->addWidget(ver);
}

void LoginWindow::buildRightPanel(QWidget* parent) {
    auto* lay = new QVBoxLayout(parent);
    lay->setContentsMargins(60, 60, 60, 60);
    lay->setSpacing(0);

    // Header
    auto* headerLbl = new QLabel("Welcome Back", parent);
    headerLbl->setStyleSheet("font-size: 28px; font-weight: 900; color: #E8EDF5;");
    lay->addWidget(headerLbl);

    auto* subLbl = new QLabel("Select your role and sign in to continue", parent);
    subLbl->setStyleSheet("font-size: 14px; color: #8896A8; margin-top: 6px; margin-bottom: 30px;");
    lay->addWidget(subLbl);

    // Tab buttons
    auto* tabRow = new QHBoxLayout();
    tabRow->setSpacing(0);

    m_tabAdmin = new QPushButton("⚙️  Administrator", parent);
    m_tabAdmin->setCheckable(true);
    m_tabAdmin->setChecked(true);
    m_tabAdmin->setFixedHeight(44);
    m_tabAdmin->setStyleSheet(
        "QPushButton { background: #FF6B35; color: white; border-radius: 8px 0 0 8px; border: none; font-size: 13px; font-weight: 700; }"
        "QPushButton:!checked { background: #161B27; color: #8896A8; border: 1px solid #21293A; border-right: none; }"
        "QPushButton:!checked:hover { background: #1C2333; color: #E8EDF5; }");
    connect(m_tabAdmin, &QPushButton::clicked, this, &LoginWindow::switchToAdmin);

    m_tabStudent = new QPushButton("🎓  Student", parent);
    m_tabStudent->setCheckable(true);
    m_tabStudent->setFixedHeight(44);
    m_tabStudent->setStyleSheet(
        "QPushButton { background: #FF6B35; color: white; border-radius: 0 8px 8px 0; border: none; font-size: 13px; font-weight: 700; }"
        "QPushButton:!checked { background: #161B27; color: #8896A8; border: 1px solid #21293A; border-left: none; }"
        "QPushButton:!checked:hover { background: #1C2333; color: #E8EDF5; }");
    connect(m_tabStudent, &QPushButton::clicked, this, &LoginWindow::switchToStudent);

    tabRow->addWidget(m_tabAdmin);
    tabRow->addWidget(m_tabStudent);
    lay->addLayout(tabRow);
    lay->addSpacing(30);

    // Stacked forms
    m_formStack = new QStackedWidget(parent);
    m_formStack->setStyleSheet("background: transparent;");

    // ── Admin form ────────────────────────────────────────────────────────────
    auto* adminPage = new QWidget();
    auto* adminLay  = new QVBoxLayout(adminPage);
    adminLay->setContentsMargins(0, 0, 0, 0);
    adminLay->setSpacing(14);

    auto mkLabel = [&](const QString& t, QWidget* p) {
        auto* l = new QLabel(t, p);
        l->setStyleSheet("font-size: 12px; font-weight: 700; color: #8896A8; letter-spacing: 1px;");
        return l;
    };

    adminLay->addWidget(mkLabel("USERNAME", adminPage));
    m_adminUser = new QLineEdit(adminPage);
    m_adminUser->setPlaceholderText("Enter admin username");
    m_adminUser->setFixedHeight(46);
    m_adminUser->setText("admin");
    adminLay->addWidget(m_adminUser);
    adminLay->addSpacing(4);

    adminLay->addWidget(mkLabel("PASSWORD", adminPage));
    m_adminPass = new QLineEdit(adminPage);
    m_adminPass->setPlaceholderText("Enter password");
    m_adminPass->setEchoMode(QLineEdit::Password);
    m_adminPass->setFixedHeight(46);
    m_adminPass->setText("admin123");
    connect(m_adminPass, &QLineEdit::returnPressed, this, &LoginWindow::onAdminLogin);
    adminLay->addWidget(m_adminPass);
    adminLay->addSpacing(6);

    // Hint
    auto* adminHint = new QLabel("Default: admin / admin123", adminPage);
    adminHint->setStyleSheet("font-size: 11px; color: #4A5568;");
    adminLay->addWidget(adminHint);
    adminLay->addSpacing(20);

    m_adminLoginBtn = new QPushButton("Sign In as Administrator →", adminPage);
    m_adminLoginBtn->setFixedHeight(48);
    m_adminLoginBtn->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #FF6B35,stop:1 #F7931E);"
        " color: white; border: none; border-radius: 10px; font-size: 14px; font-weight: 800; }"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #FF7D4E,stop:1 #F9A53A); }"
        "QPushButton:pressed { background: #E85520; }");
    connect(m_adminLoginBtn, &QPushButton::clicked, this, &LoginWindow::onAdminLogin);
    adminLay->addWidget(m_adminLoginBtn);
    adminLay->addStretch();
    m_formStack->addWidget(adminPage);

    // ── Student form ─────────────────────────────────────────────────────────
    auto* stuPage = new QWidget();
    auto* stuLay  = new QVBoxLayout(stuPage);
    stuLay->setContentsMargins(0, 0, 0, 0);
    stuLay->setSpacing(14);

    stuLay->addWidget(mkLabel("STUDENT ID", stuPage));
    m_stuId = new QLineEdit(stuPage);
    m_stuId->setPlaceholderText("Enter your Student ID");
    m_stuId->setFixedHeight(46);
    stuLay->addWidget(m_stuId);
    stuLay->addSpacing(4);

    stuLay->addWidget(mkLabel("PASSWORD (Phone Number)", stuPage));
    m_stuPass = new QLineEdit(stuPage);
    m_stuPass->setPlaceholderText("Your phone number (default password)");
    m_stuPass->setEchoMode(QLineEdit::Password);
    m_stuPass->setFixedHeight(46);
    connect(m_stuPass, &QLineEdit::returnPressed, this, &LoginWindow::onStudentLogin);
    stuLay->addWidget(m_stuPass);
    stuLay->addSpacing(6);

    auto* stuHint = new QLabel("Default password is your registered phone number", stuPage);
    stuHint->setStyleSheet("font-size: 11px; color: #4A5568;");
    stuLay->addWidget(stuHint);
    stuLay->addSpacing(20);

    m_stuLoginBtn = new QPushButton("Sign In as Student →", stuPage);
    m_stuLoginBtn->setFixedHeight(48);
    m_stuLoginBtn->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #3498DB,stop:1 #2980B9);"
        " color: white; border: none; border-radius: 10px; font-size: 14px; font-weight: 800; }"
        "QPushButton:hover { background: #3DA8F0; }"
        "QPushButton:pressed { background: #2471A3; }");
    connect(m_stuLoginBtn, &QPushButton::clicked, this, &LoginWindow::onStudentLogin);
    stuLay->addWidget(m_stuLoginBtn);
    stuLay->addStretch();
    m_formStack->addWidget(stuPage);

    lay->addWidget(m_formStack);

    // Error label
    m_errorLabel = new QLabel("", parent);
    m_errorLabel->setStyleSheet(
        "background-color: rgba(231,76,60,0.12); color: #E74C3C; border: 1px solid rgba(231,76,60,0.3);"
        " border-radius: 8px; padding: 10px 14px; font-size: 13px;");
    m_errorLabel->setVisible(false);
    m_errorLabel->setWordWrap(true);
    lay->addWidget(m_errorLabel);

    lay->addStretch();

    // Footer
    auto* sep = new QFrame(parent);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("background: #21293A; max-height: 1px; border: none;");
    lay->addWidget(sep);
    lay->addSpacing(14);

    auto* footerLbl = new QLabel("ENSIA UDRMS  •  Secure Access Portal  •  2025-2026", parent);
    footerLbl->setStyleSheet("font-size: 11px; color: #4A5568;");
    footerLbl->setAlignment(Qt::AlignCenter);
    lay->addWidget(footerLbl);
}

void LoginWindow::switchToAdmin() {
    m_formStack->setCurrentIndex(0);
    m_tabAdmin->setChecked(true);
    m_tabStudent->setChecked(false);
    m_errorLabel->setVisible(false);
}

void LoginWindow::switchToStudent() {
    m_formStack->setCurrentIndex(1);
    m_tabAdmin->setChecked(false);
    m_tabStudent->setChecked(true);
    m_errorLabel->setVisible(false);
}

void LoginWindow::onAdminLogin() {
    if (m_adminUser->text().trimmed() == "admin" && m_adminPass->text() == "admin123") {
        DataManager::instance()->logActivity("system", "Administrator logged in");
        emit adminLoggedIn();
    } else {
        showError("❌  Invalid credentials. Use admin / admin123");
    }
}

void LoginWindow::onStudentLogin() {
    bool ok;
    int id = m_stuId->text().trimmed().toInt(&ok);
    if (!ok || id <= 0) { showError("❌  Please enter a valid Student ID."); return; }
    if (DataManager::instance()->validateStudentLogin(id, m_stuPass->text())) {
        DataManager::instance()->logActivity("system", QString("Student #%1 logged in").arg(id));
        emit studentLoggedIn(id);
    } else {
        showError("❌  Invalid Student ID or password.\nDefault password is your phone number.");
    }
}

void LoginWindow::togglePasswordVisibility() {}

void LoginWindow::showError(const QString& msg) {
    m_errorLabel->setText(msg);
    m_errorLabel->setVisible(true);
}

void LoginWindow::updateClock() {
    if (m_clockLabel)
        m_clockLabel->setText(QDateTime::currentDateTime().toString("dddd, MMMM d yyyy  |  hh:mm:ss"));
}