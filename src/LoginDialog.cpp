#include "LoginDialog.h"
#include "appstate.h"
#include "stylehelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QScreen>
#include <QEvent>
#include <QFocusEvent>
#include <QShowEvent>
#include <QDateTime>
#include <cmath>
#include <cstdlib>
#include <ctime>


static int rn(int n) { return n > 0 ? rand() % n : 0; }

AnimBackground::AnimBackground(QWidget* parent) : QWidget(parent) {
    srand((unsigned)time(nullptr));
    setAttribute(Qt::WA_OpaquePaintEvent);

    m_orbs = {
        {0.12f, 0.18f, 310, 0.27f, 0.00f},
        {0.70f, 0.12f, 250, 0.41f, 1.30f},
        {0.46f, 0.56f, 330, 0.34f, 2.50f},
        {0.88f, 0.74f, 210, 0.54f, 0.80f},
        {0.07f, 0.82f, 230, 0.31f, 1.90f},
        {0.58f, 0.29f, 170, 0.47f, 3.10f},
        {0.33f, 0.93f, 190, 0.39f, 0.50f},
        {0.22f, 0.50f, 150, 0.60f, 2.00f},
    };

    m_timer = new QTimer(this);
    m_timer->setInterval(30);
    connect(m_timer, &QTimer::timeout, this, &AnimBackground::tick);
    m_timer->start();
}

void AnimBackground::initSparks() {
    m_sparks.clear();
    for (int i = 0; i < 90; i++) {
        m_sparks.push_back({
            (float)rn(1000) / 1000.f,
            (float)rn(1000) / 1000.f,
            0.00022f + rn(120) * 0.000011f,
            0.9f + rn(18) * 0.25f,
            0.20f + rn(80) * 0.01f,
            0.0f
        });
    }
}

void AnimBackground::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
    if (m_sparks.isEmpty()) initSparks();
}

void AnimBackground::tick() {
    m_t += 0.006f;
    for (auto& s : m_sparks) {
        s.y -= s.spd;
        if (s.y < -0.03f) {
            s.y = 1.03f;
            // Respawn within the spark's allowed x range
            int range = (int)((1.0f - s.minX) * 1000);
            s.x = s.minX + (range > 0 ? (float)rn(range) / 1000.f : 0.f);
        }
    }
    update();
}

void AnimBackground::paintEvent(QPaintEvent*) {
    if (m_sparks.isEmpty()) initSparks();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    const float W = (float)width(), H = (float)height();


    QLinearGradient bg(0, 0, W * 0.6, H);
    bg.setColorAt(0.00, QColor("#0C0100"));
    bg.setColorAt(0.30, QColor("#300800"));
    bg.setColorAt(0.65, QColor("#5A1600"));
    bg.setColorAt(1.00, QColor("#7A2800"));
    p.fillRect(rect(), bg);

    // Radial warm highlight top-center
    QRadialGradient topGlow(W * 0.5f, 0, W * 0.7f);
    topGlow.setColorAt(0.0, QColor(200, 75, 8, 20));
    topGlow.setColorAt(1.0, QColor(0, 0, 0, 0));
    p.fillRect(rect(), topGlow);


    for (auto& orb : m_orbs) {
        float cx = orb.xf * W + std::sin(m_t * orb.spd + orb.ph) * 70.f;
        float cy = orb.yf * H + std::cos(m_t * orb.spd * 0.62f + orb.ph) * 50.f;

        QRadialGradient rg(cx, cy, orb.r);
        rg.setColorAt(0.00, QColor(200, 90, 20, 35));
        rg.setColorAt(0.45, QColor(170, 55,  5, 15));
        rg.setColorAt(1.00, QColor(120, 30,  0,  0));

        p.save();
        p.setCompositionMode(QPainter::CompositionMode_Plus);
        p.setBrush(rg);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(cx, cy), orb.r, orb.r);
        p.restore();
    }


    p.setPen(QPen(QColor(255, 255, 255, 20), 1.5));
    const int GRID = 32;
    for (int gx = GRID / 2; gx < (int)W; gx += GRID)
        for (int gy = GRID / 2; gy < (int)H; gy += GRID)
            p.drawPoint(QPointF(gx, gy));


    p.setPen(QPen(QColor(255, 255, 255, 11), 1));
    p.setBrush(Qt::NoBrush);
    QPointF br(W * 0.93f, H * 0.90f);
    for (int r : {280, 210, 148, 92, 46})
        p.drawEllipse(br, (qreal)r, (qreal)r);


    QPointF tl(-W * 0.05f, H * 0.07f);
    for (int r : {185, 130, 78, 38})
        p.drawEllipse(tl, (qreal)r, (qreal)r);


    p.setPen(QPen(QColor(255, 255, 255, 5), 1));
    for (float i = -(float)H; i < W + H; i += 24.f)
        p.drawLine(QPointF(i, 0), QPointF(i + H, H));


    p.setPen(Qt::NoPen);
    for (auto& s : m_sparks) {
        float py = s.y * H;
        float fadeY = 1.f - std::abs(s.y - 0.5f) * 1.8f;
        if (fadeY <= 0.f) continue;
        int alpha = (int)(s.alpha * fadeY * 210.f);
        if (alpha < 6) continue;
        p.setBrush(QColor(255, 210, 120, alpha));
        p.drawEllipse(QPointF(s.x * W, py), s.sz * 0.55f, s.sz * 0.55f);
    }

}


FloatInput::FloatInput(const QString& labelText, bool isPassword, QWidget* parent)
    : QFrame(parent), m_isPass(isPassword)
{
    setFixedHeight(68);
    setStyleSheet("QFrame { background:transparent; border:none; }");

    // Floating label
    m_lbl = new QLabel(labelText, this);
    m_lbl->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_lbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.48); font-size:14px; background:transparent; }");

    // Line edit
    m_edit = new QLineEdit(this);
    m_edit->setFocusPolicy(Qt::StrongFocus);
    m_edit->setStyleSheet(
        "QLineEdit { background:transparent; border:none; color:rgba(255,255,255,0.95);"
        "  font-size:15px; padding:0; }"
        "QLineEdit:focus { border:none; outline:none; }");
    // Palette must be set explicitly — stylesheet color alone doesn't drive cursor color
    QPalette pal = m_edit->palette();
    pal.setColor(QPalette::Text, Qt::white);
    pal.setColor(QPalette::Base, Qt::transparent);
    m_edit->setPalette(pal);
    if (isPassword) m_edit->setEchoMode(QLineEdit::Password);

    // Bottom accent line
    m_under = new QFrame(this);
    m_under->setFixedHeight(2);
    m_under->setStyleSheet("QFrame { background:rgba(255,255,255,0.18); border:none; }");

    // Eye button for password
    if (isPassword) {
        m_eye = new QPushButton("Show", this);
        m_eye->setFixedSize(38, 20);
        m_eye->setCursor(Qt::PointingHandCursor);
        m_eye->setStyleSheet(
            "QPushButton { background:transparent; border:none; font-size:10px;"
            "  font-weight:bold; letter-spacing:0.5px; color:rgba(255,255,255,0.45); }"
            "QPushButton:hover { color:#F7931E; }");
        connect(m_eye, &QPushButton::clicked, this, &FloatInput::toggleEye);
    }

    // Label position animation
    m_posAnim = new QPropertyAnimation(m_lbl, "pos", this);
    m_posAnim->setDuration(190);
    m_posAnim->setEasingCurve(QEasingCurve::OutCubic);

    m_edit->installEventFilter(this);
}

void FloatInput::resizeEvent(QResizeEvent* e) {
    QFrame::resizeEvent(e);
    const int W = width();
    const int eyeGap = m_eye ? 44 : 0;
    const int labelW = W - eyeGap;

    m_under->setGeometry(0, height() - 2, W, 2);
    m_edit->setGeometry(0, 36, W - eyeGap - (eyeGap ? 4 : 0), 26);

    if (!m_up) m_lbl->setGeometry(0, 36, labelW, 24);
    else       m_lbl->setGeometry(0,  6, labelW, 18);

    if (m_eye) m_eye->setGeometry(W - 40, 43, 38, 20);
}

bool FloatInput::eventFilter(QObject* obj, QEvent* ev) {
    if (obj == m_edit) {
        if (ev->type() == QEvent::FocusIn)  { onFocusIn();  return false; }
        if (ev->type() == QEvent::FocusOut) { onFocusOut(); return false; }
    }
    return QFrame::eventFilter(obj, ev);
}

void FloatInput::onFocusIn() {
    if (!m_up) floatUp();
    setUnderlineFocused(true);
}

void FloatInput::onFocusOut() {
    if (m_edit->text().isEmpty()) floatDown();
    setUnderlineFocused(false);
}

void FloatInput::floatUp() {
    m_up = true;
    m_posAnim->stop();
    m_posAnim->setStartValue(m_lbl->pos());
    m_posAnim->setEndValue(QPoint(0, 6));
    m_posAnim->start();
    m_lbl->setStyleSheet(
        "QLabel { color:#F7931E; font-size:11px; font-weight:bold;"
        "  background:transparent; letter-spacing:0.6px; }");
}

void FloatInput::floatDown() {
    m_up = false;
    m_posAnim->stop();
    m_posAnim->setStartValue(m_lbl->pos());
    m_posAnim->setEndValue(QPoint(0, 36));
    m_posAnim->start();
    m_lbl->setStyleSheet("QLabel { color:rgba(255,255,255,0.40); font-size:14px; background:transparent; }");
}

void FloatInput::setUnderlineFocused(bool focused) {
    if (focused)
        m_under->setStyleSheet(
            "QFrame { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "  stop:0 #FF6B35, stop:1 #FDB827); border:none; }");
    else
        m_under->setStyleSheet("QFrame { background:rgba(255,255,255,0.18); border:none; }");
}

void FloatInput::clear() {
    m_edit->clear();
    // Don't float down if the field already has focus — no FocusIn will fire
    // to bring it back up, causing the label to overlap the typed text.
    if (m_up && !m_edit->hasFocus()) floatDown();
}

void FloatInput::toggleEye() {
    bool hidden = m_edit->echoMode() == QLineEdit::Password;
    m_edit->setEchoMode(hidden ? QLineEdit::Normal : QLineEdit::Password);
    m_eye->setText(hidden ? "Hide" : "Show");
}


LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("UDRMS");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground, true);
    if (auto* screen = QGuiApplication::primaryScreen())
        setGeometry(screen->geometry());
    else
        resize(1280, 800);
    buildUi();
}

void LoginDialog::showEvent(QShowEvent* e) {
    QDialog::showEvent(e);
    // Delay so layout is finalized before positioning the tab indicator
    QTimer::singleShot(0, this, [this] { setActiveTab(0); });
}

void LoginDialog::buildUi() {

    auto* bg = new AnimBackground(this);
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->addWidget(bg);

    // Center the fused card on the background
    // Close button — absolutely positioned, doesn't affect card centering
    auto* exitBtn = new QPushButton("✕", bg);
    exitBtn->setFixedSize(38, 38);
    exitBtn->setCursor(Qt::PointingHandCursor);
    exitBtn->setStyleSheet(
        "QPushButton { background:rgba(255,255,255,0.09); color:rgba(255,255,255,0.50);"
        "  border:none; border-radius:19px; font-size:14px; font-weight:bold; }"
        "QPushButton:hover { background:rgba(220,55,30,0.55); color:white; }");
    connect(exitBtn, &QPushButton::clicked, this, &QDialog::reject);
    exitBtn->move(width() - 58, 18);
    exitBtn->raise();

    auto* bgLayout = new QVBoxLayout(bg);
    bgLayout->setContentsMargins(0, 0, 0, 0);
    bgLayout->setSpacing(0);
    bgLayout->addStretch(1);

    // FUSED CARD — branding left | form right, one unified glass pane
    auto* card = new QFrame(bg);
    card->setObjectName("loginCard");
    card->setFixedWidth(1180);
    card->setStyleSheet(
        "QFrame#loginCard { background:rgba(10,3,0,0.55);"
        "  border-radius:26px; border:1.5px solid rgba(255,175,80,0.20); }");


    auto* cardRow = new QHBoxLayout(card);
    cardRow->setContentsMargins(0, 0, 0, 0);
    cardRow->setSpacing(0);


    auto* leftW = new QWidget(card);
    leftW->setFixedWidth(450);
    leftW->setStyleSheet("background:transparent;");
    auto* ll = new QVBoxLayout(leftW);
    ll->setContentsMargins(36, 48, 36, 40);
    ll->setSpacing(0);

    auto* badge = new GlowBadge(leftW);

    auto* line1 = new QLabel("University Dormitory &", leftW);
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet(
        "QLabel { color:white; font-size:18px; font-weight:300;"
        "  background:transparent; }");

    auto* line2 = new QLabel("Restaurant Management System", leftW);
    line2->setAlignment(Qt::AlignCenter);
    line2->setStyleSheet(
        "QLabel { color:rgba(255,255,255,0.65); font-size:14px; font-weight:300;"
        "  background:transparent; }");

    auto* subtag = new QLabel("STUDENT HOUSING ADMINISTRATION", leftW);
    subtag->setAlignment(Qt::AlignCenter);
    subtag->setStyleSheet(
        "QLabel { color:rgba(255,255,255,0.30); font-size:8px; font-weight:bold;"
        "  letter-spacing:2px; background:transparent; }");

    auto* accentBar = new QWidget(leftW);
    accentBar->setFixedSize(60, 1);
    accentBar->setStyleSheet(
        "QWidget { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "  stop:0 rgba(255,107,53,0), stop:0.4 #FF6B35,"
        "  stop:0.6 #FDB827, stop:1 rgba(253,184,39,0)); }");

    struct Metric { const char* num; const char* label; };
    static const Metric metrics[] = {
        { "6",    "DORMS"    },
        { "288",  "ROOMS"    },
        { "500+", "STUDENTS" },
        { "6",    "REST."    },
    };
    auto* metricsRow = new QHBoxLayout;
    metricsRow->setContentsMargins(0, 0, 0, 0);
    metricsRow->setSpacing(0);
    for (auto& m : metrics) {
        auto* mc = new MetricCircle(m.num, m.label, leftW);
        metricsRow->addWidget(mc, 1, Qt::AlignHCenter);
    }

    ll->addStretch(2);
    ll->addWidget(badge, 0, Qt::AlignHCenter);
    ll->addSpacing(22);
    ll->addWidget(line1);
    ll->addSpacing(4);
    ll->addWidget(line2);
    ll->addSpacing(12);
    ll->addWidget(subtag);
    ll->addSpacing(16);
    ll->addWidget(accentBar, 0, Qt::AlignHCenter);
    ll->addSpacing(32);
    ll->addLayout(metricsRow);
    ll->addStretch(2);

    // Clock pinned to the bottom of the left panel
    m_clockLbl = new QLabel(leftW);
    m_clockLbl->setAlignment(Qt::AlignCenter);
    m_clockLbl->setStyleSheet(
        "QLabel { color:rgba(255,255,255,0.35); font-size:10px; background:transparent; }");
    m_clockLbl->setText(QDateTime::currentDateTime().toString("hh:mm  ·  ddd d MMM yyyy"));
    ll->addWidget(m_clockLbl, 0, Qt::AlignHCenter);

    m_clockTimer = new QTimer(this);
    m_clockTimer->setInterval(1000);
    connect(m_clockTimer, &QTimer::timeout, this, [this] {
        m_clockLbl->setText(
            QDateTime::currentDateTime().toString("hh:mm  ·  ddd d MMM yyyy"));
    });
    m_clockTimer->start();

    // Vertical divider
    auto* vsep = new QWidget(card);
    vsep->setFixedWidth(1);
    vsep->setStyleSheet("QWidget { background:rgba(255,255,255,0.10); }");


    auto* rightW = new QWidget(card);
    rightW->setStyleSheet("background:transparent;");
    auto* cl = new QVBoxLayout(rightW);
    cl->setContentsMargins(56, 0, 56, 48);
    cl->setSpacing(0);

    cl->addSpacing(42);

    // Heading row
    auto* headRow = new QHBoxLayout;
    headRow->setSpacing(14);

    auto* headIcon = new QLabel("🔐", rightW);
    headIcon->setAlignment(Qt::AlignCenter);
    headIcon->setStyleSheet("QLabel { font-size:30px; background:transparent; }");

    headRow->addStretch(1);
    headRow->addWidget(headIcon);

    auto* headText = new QVBoxLayout;
    headText->setSpacing(3);
    auto* headTitle = new QLabel("Welcome back", rightW);
    headTitle->setStyleSheet(
        "QLabel { color:rgba(255,230,200,0.92); font-size:22px; font-weight:600;"
        "  background:transparent; }");
    auto* headSub = new QLabel("Sign in to your account", rightW);
    headSub->setStyleSheet(
        "QLabel { color:rgba(255,255,255,0.40); font-size:12px; background:transparent; }");
    headText->addWidget(headTitle);
    headText->addWidget(headSub);
    headRow->addLayout(headText);
    headRow->addStretch(1);
    cl->addLayout(headRow);
    cl->addSpacing(32);

    // Tab bar
    auto* tabBar = new QWidget(rightW);
    tabBar->setFixedHeight(46);
    tabBar->setStyleSheet("QWidget { background:transparent; }");

    m_tabAdmin = new QPushButton("🔒  Admin",   tabBar);
    m_tabStu   = new QPushButton("🎓  Student", tabBar);
    for (auto* b : { m_tabAdmin, m_tabStu }) {
        b->setFixedHeight(40);
        b->setCursor(Qt::PointingHandCursor);
        b->setFlat(true);
        b->setAutoDefault(false);
        b->setDefault(false);
    }
    m_tabAdmin->setStyleSheet(
        "QPushButton { color:white; font-size:14px; font-weight:bold;"
        "  background:transparent; border:none; padding-bottom:4px; }");
    m_tabStu->setStyleSheet(
        "QPushButton { color:rgba(255,255,255,0.38); font-size:14px; font-weight:normal;"
        "  background:transparent; border:none; padding-bottom:4px; }"
        "QPushButton:hover { color:rgba(255,255,255,0.65); }");

    m_tabLine = new QFrame(tabBar);
    m_tabLine->setFixedHeight(3);
    m_tabLine->setStyleSheet(
        QString("QFrame { background:%1; border-radius:1px; border:none; }")
        .arg(Style::gradientCSS()));
    m_tabLine->raise();

    m_tabLineAnim = new QPropertyAnimation(m_tabLine, "geometry", this);
    m_tabLineAnim->setDuration(220);
    m_tabLineAnim->setEasingCurve(QEasingCurve::OutCubic);

    auto* tbl = new QHBoxLayout(tabBar);
    tbl->setContentsMargins(0, 0, 0, 0);
    tbl->setSpacing(0);
    tbl->addWidget(m_tabAdmin, 1);
    tbl->addWidget(m_tabStu,   1);

    cl->addWidget(tabBar);

    auto* tabSepLine = new QFrame(rightW);
    tabSepLine->setFixedHeight(1);
    tabSepLine->setStyleSheet("QFrame { background:rgba(255,255,255,0.10); border:none; }");
    cl->addWidget(tabSepLine);
    cl->addSpacing(30);

    // Form stack
    m_stack = new QStackedWidget(rightW);
    m_stack->setStyleSheet("QStackedWidget { background:transparent; border:none; }");

    auto makeSignInBtn = [](const QString& text, QWidget* parent) {
        auto* btn = new QPushButton(text, parent);
        btn->setFixedHeight(54);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "  stop:0 #9C3E14, stop:0.5 #A85C14, stop:1 #906010);"
            "  color:rgba(255,255,255,0.90); border:none; border-radius:14px;"
            "  font-size:15px; font-weight:bold; letter-spacing:0.3px; }"
            "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "  stop:0 #B04A18, stop:1 #A06C14); }"
            "QPushButton:pressed { padding-top:2px; }");
        return btn;
    };


    auto* adminW = new QWidget;
    adminW->setStyleSheet("background:transparent;");
    auto* al = new QVBoxLayout(adminW);
    al->setContentsMargins(0, 0, 0, 0);
    al->setSpacing(0);

    m_adminPass = new FloatInput("Admin Password", true, adminW);
    m_adminErr  = new QLabel("", adminW);
    m_adminErr->setStyleSheet(
        "QLabel { color:#FF8080; font-size:12px; background:rgba(239,68,68,0.18);"
        "  border:1px solid rgba(239,68,68,0.35); border-radius:8px; padding:8px 12px; }");
    m_adminErr->setWordWrap(true);
    m_adminErr->hide();

    al->addStretch(1);
    al->addWidget(m_adminPass);
    al->addSpacing(8);
    al->addWidget(m_adminErr);
    al->addStretch(1);

    connect(m_adminPass->edit(), &QLineEdit::returnPressed, this, &LoginDialog::onAdminLogin);


    auto* stuW = new QWidget;
    stuW->setStyleSheet("background:transparent;");
    auto* sfl = new QVBoxLayout(stuW);
    sfl->setContentsMargins(0, 0, 0, 0);
    sfl->setSpacing(0);

    m_stuEmail = new FloatInput("Email Address",          false, stuW);
    m_stuPass  = new FloatInput("Password  (Student ID)", true,  stuW);
    m_stuErr   = new QLabel("", stuW);
    m_stuErr->setStyleSheet(
        "QLabel { color:#FF8080; font-size:12px; background:rgba(239,68,68,0.18);"
        "  border:1px solid rgba(239,68,68,0.35); border-radius:8px; padding:8px 12px; }");
    m_stuErr->setWordWrap(true);
    m_stuErr->hide();

    sfl->addWidget(m_stuEmail);
    sfl->addSpacing(14);
    sfl->addWidget(m_stuPass);
    sfl->addSpacing(8);
    sfl->addWidget(m_stuErr);
    sfl->addStretch(1);

    connect(m_stuEmail->edit(), &QLineEdit::returnPressed, this, &LoginDialog::onStudentLogin);
    connect(m_stuPass->edit(),  &QLineEdit::returnPressed, this, &LoginDialog::onStudentLogin);

    m_stack->addWidget(adminW);
    m_stack->addWidget(stuW);
    cl->addWidget(m_stack);

    connect(m_tabAdmin, &QPushButton::clicked, this, &LoginDialog::switchToAdmin);
    connect(m_tabStu,   &QPushButton::clicked, this, &LoginDialog::switchToStudent);


    cl->addSpacing(20);
    auto* signInBtn = makeSignInBtn("Sign In  →", rightW);
    connect(signInBtn, &QPushButton::clicked, this, [this] {
        if (m_stack->currentIndex() == 0) onAdminLogin();
        else onStudentLogin();
    });
    cl->addWidget(signInBtn);
    cl->addSpacing(12);

    m_hintLbl = new QLabel("Default password: admin123", rightW);
    m_hintLbl->setAlignment(Qt::AlignCenter);
    m_hintLbl->setStyleSheet(
        "QLabel { color:rgba(255,255,255,0.28); font-size:11px; background:transparent; }");
    cl->addWidget(m_hintLbl);
    cl->addStretch();

    // Footer
    cl->addSpacing(24);
    auto* footSep = new QFrame(rightW);
    footSep->setFixedHeight(1);
    footSep->setStyleSheet("QFrame { background:rgba(255,255,255,0.08); border:none; }");
    cl->addWidget(footSep);
    cl->addSpacing(18);

    auto* footRow = new QHBoxLayout;
    auto* footDot = new QLabel("●", rightW);
    footDot->setStyleSheet(
        QString("QLabel { color:%1; font-size:8px; background:transparent; }")
        .arg(Style::PRIMARY_ORANGE));
    auto* footTxt = new QLabel("UDRMS  ·  University Housing Administration", rightW);
    footTxt->setStyleSheet(
        "QLabel { color:rgba(255,255,255,0.25); font-size:11px; background:transparent; }");
    footRow->addStretch();
    footRow->addWidget(footDot);
    footRow->addSpacing(6);
    footRow->addWidget(footTxt);
    footRow->addStretch();
    cl->addLayout(footRow);

    // Assemble card
    cardRow->addWidget(leftW);
    cardRow->addWidget(vsep);
    cardRow->addWidget(rightW, 1);

    bgLayout->addWidget(card, 0, Qt::AlignHCenter);
    bgLayout->addStretch(1);
}


void LoginDialog::setActiveTab(int idx) {
    m_stack->setCurrentIndex(idx);

    QString active =
        "QPushButton { color:white; font-size:14px; font-weight:bold;"
        "  background:transparent; border:none; padding-bottom:4px; }";
    QString inactive =
        "QPushButton { color:rgba(255,255,255,0.38); font-size:14px; font-weight:normal;"
        "  background:transparent; border:none; padding-bottom:4px; }"
        "QPushButton:hover { color:rgba(255,255,255,0.65); }";

    m_tabAdmin->setStyleSheet(idx == 0 ? active : inactive);
    m_tabStu->setStyleSheet(  idx == 1 ? active : inactive);

    QWidget* tabBar = m_tabAdmin->parentWidget();
    if (!tabBar) return;

    QRect aRect = m_tabAdmin->geometry();
    QRect sRect = m_tabStu->geometry();
    int barH    = tabBar->height();

    QRect target = (idx == 0)
        ? QRect(aRect.x(), barH - 3, aRect.width(), 3)
        : QRect(sRect.x(), barH - 3, sRect.width(), 3);

    if (m_tabLine->width() == 0) {
        m_tabLine->setGeometry(target);
    } else {
        m_tabLineAnim->stop();
        m_tabLineAnim->setStartValue(m_tabLine->geometry());
        m_tabLineAnim->setEndValue(target);
        m_tabLineAnim->start();
    }
}

void LoginDialog::switchToAdmin() {
    setActiveTab(0);
    m_hintLbl->setText("Default password: admin123");
}
void LoginDialog::switchToStudent() {
    setActiveTab(1);
    m_hintLbl->setText("Password is your numeric Student ID");
}


void LoginDialog::shakeWidget(QWidget* w) {
    const QPoint orig = w->pos();
    auto* group = new QSequentialAnimationGroup(w);
    for (int dx : {-9, 9, -7, 7, -4, 4, -2, 2, 0}) {
        auto* a = new QPropertyAnimation(w, "pos");
        a->setDuration(32);
        a->setEndValue(orig + QPoint(dx, 0));
        group->addAnimation(a);
    }
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void LoginDialog::flashError(QLabel* lbl) {
    lbl->show();
    // Brief highlight pulse — already styled red, just show it
    QTimer::singleShot(80, lbl, [lbl] {
        lbl->setStyleSheet(
            "QLabel { color:#FF9090; font-size:12px; background:rgba(220,38,38,0.22);"
            "  border:1px solid rgba(252,165,165,0.40); border-radius:8px; padding:8px 12px; }");
    });
}


void LoginDialog::onAdminLogin() {
    m_adminErr->hide();
    if (m_adminPass->text() == "admin123") {
        AppState::instance().role = AppState::ADMIN;
        accept();
    } else {
        m_adminErr->setText("❌  Incorrect password. Please try again.");
        flashError(m_adminErr);
        shakeWidget(m_adminPass);
        m_adminPass->clear();
        m_adminPass->focusField();
    }
}

void LoginDialog::onStudentLogin() {
    m_stuErr->hide();
    const QString email    = m_stuEmail->text().trimmed();
    const QString password = m_stuPass->text().trimmed();

    if (email.isEmpty() || password.isEmpty()) {
        m_stuErr->setText("❌  Please enter both email and password.");
        flashError(m_stuErr);
        return;
    }

    auto& uni = AppState::instance().university;
    const Student* found = nullptr;

    for (const auto& s : uni.getStudentPool()) {
        if (QString::fromStdString(s.getEmail()) == email) { found = &s; break; }
    }

    if (!found) {
        for (const auto& dorm : uni.getDormitories()) {
            for (const auto& block : dorm.getBlocks()) {
                for (const auto& floorVec : block.getRooms()) {
                    for (const auto& room : floorVec) {
                        for (const auto& s : room.getResidents()) {
                            if (QString::fromStdString(s.getEmail()) == email) {
                                found = AppState::instance().findStudentById(s.getId());
                                if (!found) found = &s;
                                goto doneLookup;
                            }
                        }
                    }
                }
            }
        }
    }
    doneLookup:

    if (!found) {
        m_stuErr->setText("❌  Email not found. Please check your credentials.");
        flashError(m_stuErr);
        shakeWidget(m_stuEmail);
        return;
    }

    bool ok      = false;
    int enteredId = password.toInt(&ok);
    if (ok && enteredId == found->getId()) {
        AppState::instance().role             = AppState::STUDENT;
        AppState::instance().currentStudentId = found->getId();
        accept();
        return;
    }

    m_stuErr->setText("❌  Incorrect password. Use your Student ID number.");
    flashError(m_stuErr);
    shakeWidget(m_stuPass);
    m_stuPass->clear();
}
