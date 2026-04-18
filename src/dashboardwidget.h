#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include "qboxlayout.h"
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>

class StatCard : public QWidget {
    Q_OBJECT
public:
    StatCard(const QString& title, const QString& value, const QString& sub,
             const QString& icon, const QString& color, QWidget* parent = nullptr);
    void setValue(const QString& v);
    void setSub(const QString& s);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    QString m_title, m_value, m_sub, m_icon, m_color;
    QLabel* m_valueLbl;
    QLabel* m_subLbl;
};

class DashboardWidget : public QWidget {
    Q_OBJECT
public:
    explicit DashboardWidget(QWidget* parent = nullptr);
    void refresh();

private:
    void buildUi();
    void buildStatCards(QWidget* parent, QHBoxLayout* lay);
    void buildDormGrid(QWidget* parent, QLayout* lay);
    void buildActivityFeed(QWidget* parent, QVBoxLayout* lay);
    void buildQuickActions(QWidget* parent, QVBoxLayout* lay);

    StatCard* m_cardTotal;
    StatCard* m_cardAssigned;
    StatCard* m_cardPool;
    StatCard* m_cardOccupancy;
    StatCard* m_cardMaint;
    StatCard* m_cardStock;

    QList<QProgressBar*> m_dormBars;
    QList<QLabel*>       m_dormLabels;
    QListWidget*         m_activityList;
};

#endif // DASHBOARDWIDGET_H