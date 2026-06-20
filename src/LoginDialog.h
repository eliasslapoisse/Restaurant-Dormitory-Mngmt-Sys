#pragma once
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QStackedWidget>
#include <QFrame>
#include <QTimer>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <vector>

// ── Animated orbit badge: rotating dashed rings + glow circle + icon ────────
class GlowBadge : public QWidget {
    Q_OBJECT
    QTimer* m_timer;
    float   m_angle = 0.f;
public:
    explicit GlowBadge(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(130, 130);
        setAttribute(Qt::WA_TranslucentBackground);
        m_timer = new QTimer(this);
        m_timer->setInterval(33);
        connect(m_timer, &QTimer::timeout, this, [this]{
            m_angle = std::fmod(m_angle + 0.35f, 360.f);
            update();
        });
        m_timer->start();
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QPointF C(65, 65);

        // Outer ring — clockwise dashed
        p.save();
        p.translate(C); p.rotate(m_angle); p.translate(-C);
        QPen d1(QColor(255, 175, 80, 60), 1.2, Qt::CustomDashLine);
        d1.setDashPattern({5.0, 9.0});
        p.setPen(d1); p.setBrush(Qt::NoBrush);
        p.drawEllipse(QRectF(4, 4, 122, 122));
        p.restore();

        // Inner ring — counter-clockwise, slower
        p.save();
        p.translate(C); p.rotate(-m_angle * 0.6f); p.translate(-C);
        QPen d2(QColor(255, 210, 110, 38), 1.0, Qt::CustomDashLine);
        d2.setDashPattern({2.0, 14.0});
        p.setPen(d2); p.setBrush(Qt::NoBrush);
        p.drawEllipse(QRectF(14, 14, 102, 102));
        p.restore();

        // Filled glow circle
        QRadialGradient g(C, 50);
        g.setColorAt(0.0, QColor(255, 150, 55, 210));
        g.setColorAt(0.6, QColor(215, 80, 18, 185));
        g.setColorAt(1.0, QColor(140, 40, 5, 150));
        p.setPen(QPen(QColor(255, 200, 100, 100), 1.5));
        p.setBrush(g);
        p.drawEllipse(QRectF(22, 22, 86, 86));

        // House icon — drawn manually for exact centering in the circle
        p.setPen(QPen(QColor(255, 255, 255, 210), 2.2f, Qt::SolidLine,
                      Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        // Roof triangle: apex(65,46), base corners(44,64)(86,64)
        QPainterPath roof;
        roof.moveTo(44, 64); roof.lineTo(65, 46); roof.lineTo(86, 64);
        p.drawPath(roof);
        // Body rect: (49,64) → width 32, height 18 → bottom at 82
        // Visual height span 46..82 = 36px, center at 64 ≈ circle center 65
        p.drawRect(QRectF(49, 64, 32, 18));
    }
};

// ── Glowing metric circle: mini-badge style ──────────────────────────────────
class MetricCircle : public QWidget {
    QString m_num, m_lbl;
public:
    MetricCircle(const QString& num, const QString& lbl, QWidget* parent = nullptr)
        : QWidget(parent), m_num(num), m_lbl(lbl) {
        setFixedSize(72, 96);
        setAttribute(Qt::WA_TranslucentBackground);
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QPointF C(36, 33);
        const qreal R = 29.0;

        // Dashed outer ring — same language as GlowBadge
        QPen dring(QColor(255, 150, 55, 45), 1.0, Qt::CustomDashLine);
        dring.setDashPattern({4.0, 8.0});
        p.setPen(dring); p.setBrush(Qt::NoBrush);
        p.drawEllipse(C, R + 5, R + 5);

        // Filled glow circle
        QRadialGradient g(C, R);
        g.setColorAt(0.0, QColor(255, 130, 45, 160));
        g.setColorAt(0.55, QColor(210, 70, 10, 130));
        g.setColorAt(1.0,  QColor(140, 38, 0, 100));
        p.setPen(QPen(QColor(255, 165, 75, 80), 1.2));
        p.setBrush(g);
        p.drawEllipse(C, R, R);

        // Number
        p.setPen(Qt::white);
        QFont fNum = p.font();
        fNum.setPointSize(m_num.length() >= 4 ? 11 : (m_num.length() == 3 ? 13 : 16));
        fNum.setBold(true);
        p.setFont(fNum);
        p.drawText(QRectF(6, 4, 60, 58), Qt::AlignCenter, m_num);

        // Label
        p.setPen(QColor(255, 255, 255, 85));
        QFont fLbl; fLbl.setPointSize(7); fLbl.setBold(true);
        p.setFont(fLbl);
        p.drawText(QRectF(0, 70, 72, 26), Qt::AlignCenter, m_lbl);
    }
};

// ── Animated gradient background: orbs + dot grid + sparks + rings ──────────
class AnimBackground : public QWidget {
    Q_OBJECT
public:
    explicit AnimBackground(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private slots:
    void tick();

private:
    void initSparks();

    QTimer* m_timer;
    float   m_t = 0.f;

    struct Orb   { float xf, yf, r, spd, ph; };
    struct Spark { float x, y, spd, sz, alpha, minX; };

    QVector<Orb>   m_orbs;
    QVector<Spark> m_sparks;
};

// ── Floating-label input with animated label + password eye toggle ───────────
class FloatInput : public QFrame {
    Q_OBJECT
public:
    FloatInput(const QString& labelText, bool isPassword = false,
               QWidget* parent = nullptr);

    QLineEdit* edit()       const { return m_edit; }
    QString    text()       const { return m_edit->text(); }
    void       clear();
    void       focusField()       { m_edit->setFocus(); }

protected:
    bool eventFilter(QObject*, QEvent*) override;
    void resizeEvent(QResizeEvent*)     override;

private slots:
    void onFocusIn();
    void onFocusOut();
    void toggleEye();

private:
    void floatUp();
    void floatDown();
    void setUnderlineFocused(bool focused);

    QLabel*      m_lbl;
    QLineEdit*   m_edit;
    QFrame*      m_under;
    QPushButton* m_eye = nullptr;
    bool         m_isPass;
    bool         m_up   = false;
    QPropertyAnimation* m_posAnim;
};

// ── Login dialog ─────────────────────────────────────────────────────────────
class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent*) override;

private slots:
    void onAdminLogin();
    void onStudentLogin();
    void switchToAdmin();
    void switchToStudent();

private:
    QStackedWidget* m_stack;

    FloatInput* m_adminPass;
    FloatInput* m_stuEmail;
    FloatInput* m_stuPass;
    QLabel*     m_adminErr;
    QLabel*     m_stuErr;

    QPushButton* m_tabAdmin;
    QPushButton* m_tabStu;
    QFrame*      m_tabLine;
    QPropertyAnimation* m_tabLineAnim;

    QLabel* m_hintLbl;
    QLabel* m_clockLbl;
    QTimer* m_clockTimer;

    void buildUi();
    void setActiveTab(int idx);
    void shakeWidget(QWidget* w);
    void flashError(QLabel* lbl);
};
