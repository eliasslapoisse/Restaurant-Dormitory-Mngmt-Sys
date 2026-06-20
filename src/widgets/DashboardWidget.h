#pragma once
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QPainter>

class CircularProgress : public QWidget {
    Q_OBJECT
    int    m_value     = 0;
    QColor m_track     = QColor("#E9ECEF");
    QColor m_arc1      = QColor("#FF6B35");
    QColor m_arc2      = QColor("#FDB827");
    QColor m_textColor = QColor("#F7931E");
public:
    explicit CircularProgress(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(90, 90);
        setAttribute(Qt::WA_TranslucentBackground);
    }
    void setValue(int v) { m_value = qBound(0, v, 100); update(); }
    int  value() const   { return m_value; }
    void setColors(QColor track, QColor arc1, QColor arc2, QColor text) {
        m_track = track; m_arc1 = arc1; m_arc2 = arc2; m_textColor = text; update();
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        int s = qMin(width(), height()) - 10;
        QRectF rc((width()-s)/2.0, (height()-s)/2.0, s, s);
        p.setPen(QPen(m_track, 9, Qt::SolidLine, Qt::RoundCap));
        p.drawEllipse(rc);
        if (m_value > 0) {
            QConicalGradient cg(rc.center(), 90);
            cg.setColorAt(0.0, m_arc2);
            cg.setColorAt(1.0, m_arc1);
            p.setPen(QPen(QBrush(cg), 9, Qt::SolidLine, Qt::RoundCap));
            p.drawArc(rc, 90*16, -(m_value * 360 / 100) * 16);
        }
        p.setPen(m_textColor);
        QFont f = p.font(); f.setBold(true); f.setPointSize(11); p.setFont(f);
        p.drawText(rc, Qt::AlignCenter, QString("%1%").arg(m_value));
    }
};

class DashboardWidget : public QWidget {
    Q_OBJECT
public:
    explicit DashboardWidget(QWidget* parent = nullptr);
    void refresh();

private:
    // Header
    QLabel* dormCountLabel;
    QLabel* studentCountLabel;
    QLabel* dateLabel;

    // Stats cards
    CircularProgress* occupancyCircle;
    QLabel*           occupancyPctLabel;
    QLabel*           occupancyDetailLabel;
    QLabel*           occupancyLegendLabel;
    QProgressBar*     occProgressBar;
    QLabel*           totalStudentsLabel;
    QLabel*           assignedCountLabel;
    QLabel*           poolCountLabel;
    QLabel*           issuesCountLabel;
    QLabel*           solvedIssuesLabel;
    QLabel*           pendingIssuesLabel;

    // Tiles
    QLabel* dormsTileLabel;
    QLabel* bedsTileLabel;
    QLabel* availTileLabel;

    // Bottom panels
    QWidget*     occupancyListContainer;
    QListWidget* activityList;

    void buildUi();
    QWidget* buildOccupancyList();
    QWidget* buildActivityFeed();
};
