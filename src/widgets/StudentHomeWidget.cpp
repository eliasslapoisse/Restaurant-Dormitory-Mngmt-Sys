#include "StudentHomeWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../restaurant.h"
#include "../dialogs/ComplaintDetailDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QAbstractItemView>
#include <QDateTime>
#include <QDate>
#include <QPalette>

static QGraphicsDropShadowEffect* mkShadow(QWidget* w, int blur = 24, int alpha = 30) {
    auto* s = new QGraphicsDropShadowEffect(w);
    s->setBlurRadius(blur);
    s->setOffset(0, 4);
    s->setColor(QColor(0, 0, 0, alpha));
    return s;
}

static QFrame* mkCard(QWidget* parent, int radius = 16) {
    auto* f = new QFrame(parent);
    f->setStyleSheet(
        QString("QFrame { background:white; border-radius:%1px; border:none; }").arg(radius));
    f->setGraphicsEffect(mkShadow(f));
    return f;
}

static QFrame* mkSep(QWidget* parent) {
    auto* l = new QFrame(parent);
    l->setFrameShape(QFrame::HLine);
    l->setFixedHeight(1);
    l->setStyleSheet("QFrame { background:#F3F4F6; border:none; }");
    return l;
}

static QLabel* mkSectionLabel(const QString& text, QWidget* parent) {
    auto* l = new QLabel(text, parent);
    l->setStyleSheet(
        "QLabel { font-size:10px; font-weight:bold; color:#9CA3AF;"
        "  letter-spacing:1.5px; background:transparent; }");
    return l;
}

StudentHomeWidget::StudentHomeWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void StudentHomeWidget::buildUi() {
    setAutoFillBackground(true);
    { QPalette p = palette(); p.setColor(QPalette::Window, QColor("#F5F6FA")); setPalette(p); }

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // BANNER
    auto* banner = new QFrame(this);
    banner->setStyleSheet(
        QString("QFrame { background:%1; border-radius:0; }")
        .arg(Style::gradientCSS("0","0","1","0")));
    banner->setFixedHeight(150);
    banner->setGraphicsEffect(mkShadow(banner, 20, 50));

    auto* bl = new QHBoxLayout(banner);
    bl->setContentsMargins(32, 22, 32, 22);
    bl->setSpacing(20);

    // Avatar circle
    avatarLabel = new QLabel("?", banner);
    avatarLabel->setFixedSize(62, 62);
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet(
        "QLabel { background:rgba(255,255,255,0.22); border-radius:31px;"
        "  color:white; font-size:22px; font-weight:bold; }");

    // Name + badges column
    auto* nameCol = new QVBoxLayout;
    nameCol->setSpacing(6);
    nameCol->setAlignment(Qt::AlignVCenter);

    auto* welcomeLbl = new QLabel("Welcome back", banner);
    welcomeLbl->setStyleSheet(
        "QLabel { color:rgba(255,255,255,0.80); font-size:12px; background:transparent; }");

    nameLabel = new QLabel("—", banner);
    nameLabel->setStyleSheet(
        "QLabel { color:white; font-size:26px; font-weight:bold; background:transparent; }");

    auto* badgeRow = new QHBoxLayout;
    badgeRow->setSpacing(8);
    const QString bSS =
        "QLabel { color:rgba(255,255,255,0.90); font-size:11px; font-weight:600; }";
    idBadge     = new QLabel("—", banner); idBadge->setStyleSheet(bSS);
    yearBadge   = new QLabel("—", banner); yearBadge->setStyleSheet(bSS);
    genderBadge = new QLabel("—", banner); genderBadge->setStyleSheet(bSS);
    badgeRow->addWidget(idBadge);
    badgeRow->addWidget(yearBadge);
    badgeRow->addWidget(genderBadge);
    badgeRow->addStretch();

    nameCol->addWidget(welcomeLbl);
    nameCol->addWidget(nameLabel);
    nameCol->addLayout(badgeRow);

    bl->addWidget(avatarLabel, 0, Qt::AlignVCenter);
    bl->addLayout(nameCol, 1);

    // Assignment info card (right side of banner)
    auto* assignCard = new QFrame(banner);
    assignCard->setStyleSheet(
        "QFrame { background:rgba(255,255,255,0.16);"
        "  border-radius:14px; }");
    assignCard->setFixedWidth(260);

    auto* acl = new QVBoxLayout(assignCard);
    acl->setContentsMargins(18, 14, 18, 14);
    acl->setSpacing(8);

    assignBannerLabel = new QLabel("—", assignCard);
    assignBannerLabel->setWordWrap(true);
    assignBannerLabel->setStyleSheet(
        "QLabel { color:white; font-size:12px; background:transparent; }");
    acl->addWidget(assignBannerLabel);

    bl->addWidget(assignCard, 0, Qt::AlignVCenter);
    root->addWidget(banner);

    // SCROLLABLE CONTENT
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background:#F5F6FA; border:none; }");

    auto* content = new QWidget;
    content->setAutoFillBackground(true);
    { QPalette p = content->palette(); p.setColor(QPalette::Window, QColor("#F5F6FA")); content->setPalette(p); }

    auto* cl = new QVBoxLayout(content);
    cl->setContentsMargins(28, 24, 28, 28);
    cl->setSpacing(20);

    auto* colRow = new QHBoxLayout;
    colRow->setSpacing(16);
    colRow->setAlignment(Qt::AlignTop);

    // COL 1 — MY PROFILE
    auto* profileCard = mkCard(content);
    auto* pl = new QVBoxLayout(profileCard);
    pl->setContentsMargins(24, 22, 24, 24);
    pl->setSpacing(0);

    pl->addWidget(mkSectionLabel("MY PROFILE", profileCard));
    pl->addSpacing(4);

    auto* pTitle = new QLabel("Personal Information", profileCard);
    pTitle->setStyleSheet(
        "QLabel { font-size:15px; font-weight:bold; color:#111827; background:transparent; }");
    pl->addWidget(pTitle);
    pl->addSpacing(14);
    pl->addWidget(mkSep(profileCard));
    pl->addSpacing(18);

    auto addField = [&](const QString& lbl, const QString& ph, QLineEdit*& edit) {
        auto* l = new QLabel(lbl, profileCard);
        l->setStyleSheet(
            "QLabel { font-size:11px; font-weight:bold; color:#6B7280; background:transparent; }");
        edit = new QLineEdit(profileCard);
        edit->setPlaceholderText(ph);
        edit->setFixedHeight(42);
        edit->setStyleSheet(
            "QLineEdit { border-radius:10px;"
            "  padding:0 14px; font-size:13px; color:#111827; background:#F9FAFB; }"
            "QLineEdit:focus { background:#FFFBF7; }");
        pl->addWidget(l);
        pl->addSpacing(5);
        pl->addWidget(edit);
        pl->addSpacing(14);
    };
    addField("Email address", "email@example.com", emailEdit);
    addField("Phone number", "+213 ...", phoneEdit);

    saveStatusLabel = new QLabel("", profileCard);
    saveStatusLabel->setStyleSheet("QLabel { font-size:11px; background:transparent; }");
    pl->addWidget(saveStatusLabel);
    pl->addSpacing(4);

    auto* saveBtn = new QPushButton("Save Changes", profileCard);
    saveBtn->setFixedHeight(44);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        QString("QPushButton { background:%1; color:white; border:none;"
                "  border-radius:10px; font-size:13px; font-weight:bold; }"
                "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "  stop:0 #E65C2A,stop:1 #E8A522); }").arg(Style::gradientCSS()));
    connect(saveBtn, &QPushButton::clicked, this, &StudentHomeWidget::onSaveInfo);
    pl->addWidget(saveBtn);
    pl->addStretch();

    colRow->addWidget(profileCard, 3);

    // COL 2 — TODAY'S MEALS
    auto* mealsCard = mkCard(content);
    auto* ml = new QVBoxLayout(mealsCard);
    ml->setContentsMargins(24, 22, 24, 24);
    ml->setSpacing(0);

    ml->addWidget(mkSectionLabel("DINING", mealsCard));
    ml->addSpacing(4);

    auto* mTitle = new QLabel("Today's Meals", mealsCard);
    mTitle->setStyleSheet(
        "QLabel { font-size:15px; font-weight:bold; color:#111827; background:transparent; }");
    ml->addWidget(mTitle);
    ml->addSpacing(14);
    ml->addWidget(mkSep(mealsCard));
    ml->addSpacing(18);

    // Content when assigned
    mealsContent = new QWidget(mealsCard);
    mealsContent->setStyleSheet("QWidget { background:transparent; }");
    auto* mcl = new QVBoxLayout(mealsContent);
    mcl->setContentsMargins(0, 0, 0, 0);
    mcl->setSpacing(12);

    const char* mealInitials[] = { "B",   "L",    "D"       };
    const char* mealNames[]    = { "Breakfast", "Lunch", "Dinner" };

    for (int m = 0; m < 3; m++) {
        auto* row = new QFrame(mealsContent);
        row->setStyleSheet(
            "QFrame { background:#F9FAFB; border-radius:12px; }");
        auto* rl = new QHBoxLayout(row);
        rl->setContentsMargins(14, 12, 14, 12);
        rl->setSpacing(12);

        // Icon
        auto* icon = new QLabel(mealInitials[m], row);
        icon->setFixedSize(36, 36);
        icon->setAlignment(Qt::AlignCenter);
        icon->setStyleSheet(
            "QLabel { background:#1F2937; border-radius:10px;"
            "  color:white; font-size:13px; font-weight:bold; }");

        // Text col
        auto* tc = new QVBoxLayout;
        tc->setSpacing(2);
        auto* nameLbl = new QLabel(mealNames[m], row);
        nameLbl->setStyleSheet(
            "QLabel { font-size:13px; font-weight:bold; color:#111827; background:transparent; }");
        mealDishLabels[m] = new QLabel("—", row);
        mealDishLabels[m]->setStyleSheet(
            "QLabel { font-size:11px; color:#6B7280; background:transparent; }");
        mealTimeLabels[m] = new QLabel("—", row);
        mealTimeLabels[m]->setStyleSheet(
            "QLabel { font-size:10px; color:#9CA3AF; background:transparent; }");
        tc->addWidget(nameLbl);
        tc->addWidget(mealDishLabels[m]);
        tc->addWidget(mealTimeLabels[m]);

        mealStatusBadges[m] = new QLabel("—", row);
        mealStatusBadges[m]->setAlignment(Qt::AlignCenter);
        mealStatusBadges[m]->setStyleSheet(
            "QLabel { color:#9CA3AF; font-size:11px; font-weight:600; }");

        rl->addWidget(icon);
        rl->addLayout(tc, 1);
        rl->addWidget(mealStatusBadges[m]);
        mcl->addWidget(row);
    }

    ml->addWidget(mealsContent);

    mealsEmptyLabel = new QLabel("Assign to a dormitory\nto see meal information.", mealsCard);
    mealsEmptyLabel->setAlignment(Qt::AlignCenter);
    mealsEmptyLabel->setWordWrap(true);
    mealsEmptyLabel->setStyleSheet(
        "QLabel { color:#9CA3AF; font-size:13px; padding:20px; background:transparent; }");
    ml->addWidget(mealsEmptyLabel);
    ml->addStretch();

    colRow->addWidget(mealsCard, 4);

    // COL 3 — COMPLAINTS
    auto* cmpCard = mkCard(content);
    auto* cpl = new QVBoxLayout(cmpCard);
    cpl->setContentsMargins(24, 22, 24, 24);
    cpl->setSpacing(0);

    auto* cmpHdr = new QHBoxLayout;
    cmpHdr->addWidget(mkSectionLabel("COMPLAINTS", cmpCard));
    cmpHdr->addStretch();
    complaintsCountLabel = new QLabel("0", cmpCard);
    complaintsCountLabel->setStyleSheet(
        QString("QLabel { color:%1; font-size:11px; font-weight:bold; }").arg(Style::PRIMARY_ORANGE));
    cmpHdr->addWidget(complaintsCountLabel);
    cpl->addLayout(cmpHdr);
    cpl->addSpacing(4);

    auto* cTitle = new QLabel("Submit & History", cmpCard);
    cTitle->setStyleSheet(
        "QLabel { font-size:15px; font-weight:bold; color:#111827; background:transparent; }");
    cpl->addWidget(cTitle);
    cpl->addSpacing(14);
    cpl->addWidget(mkSep(cmpCard));
    cpl->addSpacing(16);

    // Submit form
    auto* catLbl = new QLabel("Category", cmpCard);
    catLbl->setStyleSheet(
        "QLabel { font-size:11px; font-weight:bold; color:#6B7280; background:transparent; }");
    cpl->addWidget(catLbl);
    cpl->addSpacing(5);

    categoryCombo = new QComboBox(cmpCard);
    categoryCombo->addItem("Select category...");
    categoryCombo->addItems({ "Maintenance", "Cleanliness", "Noise", "Food", "Other" });
    categoryCombo->setFixedHeight(40);
    categoryCombo->setStyleSheet(
        "QComboBox { border-radius:10px; padding:0 12px;"
        "  background:#F9FAFB; color:#6B7280; font-size:13px; }"
        "QComboBox:focus { background:#FFFBF7; }"
        "QComboBox::drop-down { width:20px; }"
        "QComboBox::down-arrow { width:0; height:0; }");
    categoryCombo->view()->setStyleSheet(
        "QListView { outline:none; background:white; padding:4px; }"
        "QListView::item { padding:8px 12px; color:#212529; border-radius:6px; }"
        "QListView::item:hover    { background:#FFF3E0; color:#F7931E; }"
        "QListView::item:selected { background:#FFF3E0; color:#F7931E; }");
    cpl->addWidget(categoryCombo);
    cpl->addSpacing(10);

    auto* descLbl = new QLabel("Description", cmpCard);
    descLbl->setStyleSheet(
        "QLabel { font-size:11px; font-weight:bold; color:#6B7280; background:transparent; }");
    cpl->addWidget(descLbl);
    cpl->addSpacing(5);

    descEdit = new QTextEdit(cmpCard);
    descEdit->setFixedHeight(90);
    descEdit->setPlaceholderText("Describe the issue...");
    descEdit->setStyleSheet(
        "QTextEdit { border-radius:10px; padding:8px 12px;"
        "  font-size:13px; color:#111827; background:#F9FAFB; }"
        "QTextEdit:focus { background:#FFFBF7; }");
    cpl->addWidget(descEdit);
    cpl->addSpacing(8);

    submitStatusLabel = new QLabel("", cmpCard);
    submitStatusLabel->setStyleSheet("QLabel { font-size:11px; background:transparent; }");
    cpl->addWidget(submitStatusLabel);
    cpl->addSpacing(4);

    auto* submitBtn = new QPushButton("Submit Complaint", cmpCard);
    submitBtn->setFixedHeight(42);
    submitBtn->setCursor(Qt::PointingHandCursor);
    submitBtn->setStyleSheet(
        QString("QPushButton { background:%1; color:white; border:none;"
                "  border-radius:10px; font-size:13px; font-weight:bold; }"
                "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "  stop:0 #E65C2A,stop:1 #E8A522); }").arg(Style::gradientCSS()));
    connect(submitBtn, &QPushButton::clicked, this, &StudentHomeWidget::onSubmitComplaint);
    cpl->addWidget(submitBtn);
    cpl->addSpacing(18);
    cpl->addWidget(mkSep(cmpCard));
    cpl->addSpacing(14);

    // Complaints list
    auto* listScroll = new QScrollArea(cmpCard);
    listScroll->setWidgetResizable(true);
    listScroll->setFrameShape(QFrame::NoFrame);
    listScroll->setFixedHeight(220);
    listScroll->setStyleSheet("QScrollArea { background:transparent; border:none; }");

    complaintsListContent = new QWidget;
    complaintsListContent->setStyleSheet("QWidget { background:transparent; }");
    complaintsListLayout = new QVBoxLayout(complaintsListContent);
    complaintsListLayout->setContentsMargins(0, 0, 0, 0);
    complaintsListLayout->setSpacing(8);
    complaintsListLayout->addStretch();

    listScroll->setWidget(complaintsListContent);
    cpl->addWidget(listScroll, 1);

    colRow->addWidget(cmpCard, 4);
    cl->addLayout(colRow);

    // WEEKLY MENU (full-width)
    auto* weekCard = mkCard(content);
    auto* wl = new QVBoxLayout(weekCard);
    wl->setContentsMargins(24, 20, 24, 24);
    wl->setSpacing(0);

    auto* wHdr = new QHBoxLayout;
    wHdr->addWidget(mkSectionLabel("WEEKLY MENU", weekCard));
    wHdr->addStretch();
    auto* todayDateLbl = new QLabel(
        QDate::currentDate().toString("dddd, MMMM d"), weekCard);
    todayDateLbl->setStyleSheet(
        "QLabel { font-size:11px; color:#9CA3AF; background:transparent; }");
    wHdr->addWidget(todayDateLbl);
    wl->addLayout(wHdr);
    wl->addSpacing(4);

    auto* wTitle = new QLabel("This Week's Schedule", weekCard);
    wTitle->setStyleSheet(
        "QLabel { font-size:15px; font-weight:bold; color:#111827; background:transparent; }");
    wl->addWidget(wTitle);
    wl->addSpacing(14);
    wl->addWidget(mkSep(weekCard));
    wl->addSpacing(16);

    // Day grid
    weeklyContent = new QWidget(weekCard);
    weeklyContent->setStyleSheet("QWidget { background:transparent; }");
    auto* grid = new QHBoxLayout(weeklyContent);
    grid->setSpacing(8);
    grid->setContentsMargins(0, 0, 0, 0);

    const char* dayNames[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
    const char* dayIni[]   = { "B","L","D" };
    int todayIdx = (QDate::currentDate().dayOfWeek() == 7) ? 0
                                                            : QDate::currentDate().dayOfWeek();
    for (int d = 0; d < 7; d++) {
        bool today = (d == todayIdx);
        auto* dc = new QFrame(weeklyContent);
        dc->setStyleSheet(today
            ? QString("QFrame { background:%1; border-radius:12px; border:none; }")
              .arg(Style::gradientCSS("0","0","0","1"))
            : "QFrame { background:#F9FAFB; border-radius:12px; }");

        auto* dcl = new QVBoxLayout(dc);
        dcl->setContentsMargins(8, 10, 8, 10);
        dcl->setSpacing(5);

        dayHeaders[d] = new QLabel(dayNames[d], dc);
        dayHeaders[d]->setAlignment(Qt::AlignCenter);
        dayHeaders[d]->setStyleSheet(today
            ? "QLabel { color:white; font-weight:bold; font-size:11px; background:transparent; }"
            : "QLabel { color:#374151; font-weight:bold; font-size:11px; background:transparent; }");
        dcl->addWidget(dayHeaders[d]);

        auto* dl = new QFrame(dc);
        dl->setFrameShape(QFrame::HLine);
        dl->setStyleSheet(today
            ? "QFrame { background:rgba(255,255,255,0.3); border:none; }"
            : "QFrame { background:#E5E7EB; border:none; }");
        dl->setFixedHeight(1);
        dcl->addWidget(dl);

        for (int m = 0; m < 3; m++) {
            auto* mr = new QHBoxLayout;
            mr->setSpacing(4);
            auto* dot = new QLabel(dayIni[m], dc);
            dot->setAlignment(Qt::AlignCenter);
            dot->setStyleSheet(today
                ? "QLabel { color:rgba(255,255,255,0.70);"
                  "  font-size:8px; font-weight:bold; }"
                : "QLabel { color:#6B7280;"
                  "  font-size:8px; font-weight:bold; }");
            dayMeals[d][m] = new QLabel("—", dc);
            dayMeals[d][m]->setWordWrap(true);
            dayMeals[d][m]->setStyleSheet(today
                ? "QLabel { color:rgba(255,255,255,0.90); font-size:9px; background:transparent; }"
                : "QLabel { color:#6B7280; font-size:9px; background:transparent; }");
            mr->addWidget(dot);
            mr->addWidget(dayMeals[d][m], 1);
            dcl->addLayout(mr);
        }
        grid->addWidget(dc, 1);
    }

    weeklyEmptyLabel = new QLabel(
        "Assign to a dormitory to view the weekly menu.", weekCard);
    weeklyEmptyLabel->setAlignment(Qt::AlignCenter);
    weeklyEmptyLabel->setStyleSheet(
        "QLabel { color:#9CA3AF; font-size:13px; padding:20px; background:transparent; }");

    wl->addWidget(weeklyContent);
    wl->addWidget(weeklyEmptyLabel);
    cl->addWidget(weekCard);
    cl->addStretch();

    scroll->setWidget(content);
    root->addWidget(scroll, 1);
}

void StudentHomeWidget::refresh() {
    auto& app = AppState::instance();
    const Student* s = app.findStudentById(app.currentStudentId);
    if (!s) return;

    // Banner
    QString fn = QString::fromStdString(s->getFirstName());
    QString ln = QString::fromStdString(s->getFamilyName());
    avatarLabel->setText(fn.left(1).toUpper() + ln.left(1).toUpper());
    nameLabel->setText(fn + " " + ln);
    idBadge->setText("ID " + QString::number(s->getId()));
    yearBadge->setText("Year " + QString::number(s->getAcademicYear()));
    genderBadge->setText(
        QString::fromStdString(s->getGender()) == "M" ? "Male" : "Female");

    emailEdit->setText(QString::fromStdString(s->getEmail()));
    phoneEdit->setText(QString::fromStdString(s->getPhoneNumber()));
    saveStatusLabel->clear();

    // Assignment
    Dormitory* dorm = app.findStudentDorm(app.currentStudentId);
    if (dorm) {
        QString blockName = "—";
        for (const auto& b : dorm->getBlocks())
            if (b.isStudentIn(app.currentStudentId))
                { blockName = QString::fromStdString(b.getName()); break; }

        Room* r = dorm->findRoomByStudentID(app.currentStudentId);
        int floor = r ? r->getFloor() : 0;
        int room  = r ? r->getNumber() : 0;

        assignBannerLabel->setText(
            QString("Dormitory %1  ·  Block %2\nFloor %3  ·  Room %4")
            .arg(dorm->getNumber()).arg(blockName).arg(floor).arg(room));

        // Meals
        const Restaurant& rest = dorm->getRestaurant();
        const Restaurant::dailyMenu* weekly = rest.getWeeklyMenu();
        int todayIdx = (QDate::currentDate().dayOfWeek() == 7)
                       ? 0 : QDate::currentDate().dayOfWeek();

        for (int d = 0; d < 7; d++) {
            dayMeals[d][0]->setText(
                QString::fromStdString(Restaurant::getMorningDishName(weekly[d].breakfast)));
            dayMeals[d][1]->setText(
                QString::fromStdString(Restaurant::getDishName(weekly[d].lunch)));
            dayMeals[d][2]->setText(
                QString::fromStdString(Restaurant::getDishName(weekly[d].dinner)));
        }

        mealDishLabels[0]->setText(
            QString::fromStdString(Restaurant::getMorningDishName(weekly[todayIdx].breakfast)));
        mealDishLabels[1]->setText(
            QString::fromStdString(Restaurant::getDishName(weekly[todayIdx].lunch)));
        mealDishLabels[2]->setText(
            QString::fromStdString(Restaurant::getDishName(weekly[todayIdx].dinner)));

        mealTimeLabels[0]->setText(
            QString("%1:00 – %2:00").arg(Restaurant::getBR_OPEN()).arg(Restaurant::getBR_CLOSE()));
        mealTimeLabels[1]->setText(
            QString("%1:00 – %2:00").arg(Restaurant::getLU_OPEN()).arg(Restaurant::getLU_CLOSE()));
        mealTimeLabels[2]->setText(
            QString("%1:00 – %2:00").arg(Restaurant::getDI_OPEN()).arg(Restaurant::getDI_CLOSE()));

        bool ate[] = {
            rest.didStudentEatBreakfast(app.currentStudentId),
            rest.didStudentEatLunch(app.currentStudentId),
            rest.didStudentEatDinner(app.currentStudentId)
        };
        for (int m = 0; m < 3; m++) {
            if (ate[m]) {
                mealStatusBadges[m]->setText("✓ Eaten");
                mealStatusBadges[m]->setStyleSheet(
                    "QLabel { color:#065F46; font-size:11px; font-weight:600; }");
            } else {
                mealStatusBadges[m]->setText("—");
                mealStatusBadges[m]->setStyleSheet(
                    "QLabel { color:#9CA3AF; font-size:11px; font-weight:600; }");
            }
        }

        mealsContent->show();
        mealsEmptyLabel->hide();
        weeklyContent->show();
        weeklyEmptyLabel->hide();
    } else {
        assignBannerLabel->setText("In waiting pool\nAwaiting room assignment");
        mealsContent->hide();
        mealsEmptyLabel->show();
        weeklyContent->hide();
        weeklyEmptyLabel->show();
    }

    populateComplaints();
}

void StudentHomeWidget::populateComplaints() {
    auto& app = AppState::instance();
    while (complaintsListLayout->count() > 1) {
        auto* item = complaintsListLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int count = 0;
    for (auto& c : app.complaints) {
        if (c.studentId != app.currentStudentId) continue;

        auto* card = new QFrame(complaintsListContent);
        card->setStyleSheet(
            "QFrame { background:#F9FAFB; border-radius:10px; }");
        card->setCursor(Qt::PointingHandCursor);
        auto* ccl = new QVBoxLayout(card);
        ccl->setContentsMargins(14, 10, 14, 10);
        ccl->setSpacing(4);

        auto* topRow = new QHBoxLayout;
        auto* catLbl = new QLabel(c.category, card);
        catLbl->setStyleSheet(
            QString("QLabel { font-size:12px; font-weight:bold; color:%1; background:transparent; }")
            .arg(Style::PRIMARY_MID));
        auto* dateLbl = new QLabel(c.dateTime.toString("MMM d"), card);
        dateLbl->setStyleSheet("QLabel { font-size:10px; color:#9CA3AF; background:transparent; }");
        auto* statusLbl = new QLabel(c.status, card);
        statusLbl->setStyleSheet(c.status == "Resolved"
            ? "QLabel { color:#065F46; font-size:10px; font-weight:bold; }"
            : "QLabel { color:#854D0E; font-size:10px; font-weight:bold; }");
        topRow->addWidget(catLbl);
        topRow->addSpacing(6);
        topRow->addWidget(dateLbl);
        topRow->addStretch();
        topRow->addWidget(statusLbl);
        ccl->addLayout(topRow);

        QString preview = c.description.length() > 60
            ? c.description.left(57) + "..." : c.description;
        auto* descLbl = new QLabel(preview, card);
        descLbl->setStyleSheet("QLabel { font-size:11px; color:#6B7280; background:transparent; }");
        ccl->addWidget(descLbl);

        auto* btn = new QPushButton(card);
        btn->setFlat(true);
        btn->setStyleSheet("QPushButton { background:transparent; border:none; }");
        btn->setCursor(Qt::PointingHandCursor);
        btn->setGeometry(0, 0, 2000, 80);
        btn->raise();
        int cid = c.id;
        connect(btn, &QPushButton::clicked, this, [this, cid]{
            ComplaintDetailDialog dlg(cid, false, this);
            dlg.exec();
            populateComplaints();
        });

        complaintsListLayout->insertWidget(complaintsListLayout->count() - 1, card);
        count++;
    }

    complaintsCountLabel->setText(QString::number(count));

    if (count == 0) {
        auto* el = new QLabel("No complaints yet.", complaintsListContent);
        el->setAlignment(Qt::AlignCenter);
        el->setStyleSheet(
            "QLabel { color:#9CA3AF; font-size:12px; padding:16px; background:transparent; }");
        complaintsListLayout->insertWidget(0, el);
    }
}

void StudentHomeWidget::onSaveInfo() {
    auto& app = AppState::instance();
    const Student* s = app.findStudentById(app.currentStudentId);
    if (!s) return;

    QString email = emailEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();

    Student tmp;
    if (!tmp.verifyInput(s->getFirstName(), s->getFamilyName(),
                         email.toStdString(), phone.toStdString(),
                         s->getGender(), s->getAcademicYear())) {
        saveStatusLabel->setStyleSheet(
            "QLabel { font-size:11px; color:#DC3545; background:transparent; }");
        saveStatusLabel->setText("Invalid email or phone format.");
        return;
    }
    auto& uni = app.university;
    if (s->getEmail() != email.toStdString() && uni.isEmailDuplicate(email.toStdString())) {
        saveStatusLabel->setStyleSheet(
            "QLabel { font-size:11px; color:#DC3545; background:transparent; }");
        saveStatusLabel->setText("Email already in use.");
        return;
    }
    if (s->getPhoneNumber() != phone.toStdString() && uni.isPhoneDuplicate(phone.toStdString())) {
        saveStatusLabel->setStyleSheet(
            "QLabel { font-size:11px; color:#DC3545; background:transparent; }");
        saveStatusLabel->setText("Phone already in use.");
        return;
    }
    if (app.updateStudentData(app.currentStudentId,
            s->getFirstName(), s->getFamilyName(),
            email.toStdString(), phone.toStdString(),
            s->getGender(), s->getAcademicYear())) {
        saveStatusLabel->setStyleSheet(
            "QLabel { font-size:11px; color:#28A745; background:transparent; }");
        saveStatusLabel->setText("✓ Saved successfully.");
    } else {
        saveStatusLabel->setStyleSheet(
            "QLabel { font-size:11px; color:#DC3545; background:transparent; }");
        saveStatusLabel->setText("Failed to save.");
    }
}

void StudentHomeWidget::onSubmitComplaint() {
    auto& app = AppState::instance();
    if (categoryCombo->currentIndex() == 0) {
        submitStatusLabel->setStyleSheet(
            "QLabel { font-size:11px; color:#DC3545; background:transparent; }");
        submitStatusLabel->setText("Please select a category.");
        return;
    }
    QString desc = descEdit->toPlainText().trimmed();
    if (desc.length() < 10) {
        submitStatusLabel->setStyleSheet(
            "QLabel { font-size:11px; color:#DC3545; background:transparent; }");
        submitStatusLabel->setText("Describe the issue (min 10 chars).");
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

    descEdit->clear();
    categoryCombo->setCurrentIndex(0);
    submitStatusLabel->setStyleSheet(
        "QLabel { font-size:11px; color:#28A745; background:transparent; }");
    submitStatusLabel->setText("✓ Complaint submitted.");
    populateComplaints();
}
