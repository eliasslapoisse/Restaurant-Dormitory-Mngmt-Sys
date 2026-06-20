#include "SeederWidget.h"
#include "../appstate.h"
#include "../stylehelper.h"
#include "../AdminMainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <cstdlib>
#include <ctime>

static const char* FIRST_NAMES[] = {
    "Ahmed","Mohamed","Yacine","Karim","Bilal","Omar","Amine","Rayan",
    "Fatima","Amina","Sara","Nour","Lina","Yasmine","Meriem","Hana"
};
static const char* LAST_NAMES[] = {
    "Boudiaf","Benali","Cherif","Meziane","Hamidi","Rahmani","Benkhelifa",
    "Ouali","Djaber","Belloula","Messaoud","Kaci","Brahim","Ferhat"
};

SeederWidget::SeederWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void SeederWidget::buildUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    auto* title = new QLabel("Data Seeder", this);
    title->setStyleSheet("QLabel { font-size:22px; font-weight:bold; color:#212529; }");
    auto* sub = new QLabel("Development tool: populate the system with test data", this);
    sub->setStyleSheet("QLabel { color:#6C757D; font-size:13px; }");
    mainLayout->addWidget(title);
    mainLayout->addWidget(sub);

    auto* card = new QFrame(this);
    card->setStyleSheet("QFrame { background:white; border-radius:12px; border:1px solid #E9ECEF; }");
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 20, 24, 20);
    cardLayout->setSpacing(16);

    // Seed students row
    auto* seedRow = new QHBoxLayout;
    auto* seedLbl = new QLabel("Seed students:", card);
    seedLbl->setStyleSheet("QLabel { color:#212529; font-size:13px; background:transparent; }");
    seedCountSpin = new QSpinBox(card);
    seedCountSpin->setRange(1, 200);
    seedCountSpin->setValue(20);
    seedCountSpin->setFixedHeight(38);
    seedCountSpin->setFixedWidth(80);
    seedCountSpin->setStyleSheet(Style::inputSS());
    auto* seedBtn = new QPushButton("Seed Students", card);
    seedBtn->setFixedHeight(38);
    seedBtn->setStyleSheet(Style::primaryBtnSS());
    seedBtn->setCursor(Qt::PointingHandCursor);
    connect(seedBtn, &QPushButton::clicked, this, &SeederWidget::onSeedStudents);
    seedRow->addWidget(seedLbl);
    seedRow->addWidget(seedCountSpin);
    seedRow->addWidget(seedBtn);
    seedRow->addStretch();
    cardLayout->addLayout(seedRow);

    // Assign random row
    auto* assignRow = new QHBoxLayout;
    auto* assignLbl = new QLabel("Assign randomly:", card);
    assignLbl->setStyleSheet("QLabel { color:#212529; font-size:13px; background:transparent; }");
    assignCountSpin = new QSpinBox(card);
    assignCountSpin->setRange(1, 500);
    assignCountSpin->setValue(10);
    assignCountSpin->setFixedHeight(38);
    assignCountSpin->setFixedWidth(80);
    assignCountSpin->setStyleSheet(Style::inputSS());
    auto* assignBtn = new QPushButton("Assign Random", card);
    assignBtn->setFixedHeight(38);
    assignBtn->setStyleSheet(Style::secondaryBtnSS());
    assignBtn->setCursor(Qt::PointingHandCursor);
    connect(assignBtn, &QPushButton::clicked, this, &SeederWidget::onAssignRandom);
    assignRow->addWidget(assignLbl);
    assignRow->addWidget(assignCountSpin);
    assignRow->addWidget(assignBtn);
    assignRow->addStretch();
    cardLayout->addLayout(assignRow);

    auto* sep = new QFrame(card);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("QFrame { color:#E9ECEF; }");
    cardLayout->addWidget(sep);

    // Clear all
    auto* clearBtn = new QPushButton("🗑  Clear All Students", card);
    clearBtn->setFixedHeight(40);
    clearBtn->setStyleSheet(Style::dangerBtnSS());
    clearBtn->setCursor(Qt::PointingHandCursor);
    connect(clearBtn, &QPushButton::clicked, this, &SeederWidget::onClearAll);
    cardLayout->addWidget(clearBtn);

    statusLabel = new QLabel("", card);
    statusLabel->setStyleSheet("QLabel { color:#28A745; font-size:13px; background:transparent; }");
    cardLayout->addWidget(statusLabel);

    mainLayout->addWidget(card);
    mainLayout->addStretch();
}

void SeederWidget::onSeedStudents() {
    int n = seedCountSpin->value();
    std::srand((unsigned)std::time(nullptr));
    auto& uni = AppState::instance().university;

    int added = 0;
    int attempt = 0;
    while (added < n && attempt < n * 3) {
        attempt++;
        QString fn = FIRST_NAMES[std::rand() % 16];
        QString ln = LAST_NAMES[std::rand() % 14];
        QString email = QString("%1.%2.%3@univ.dz")
            .arg(fn.toLower()).arg(ln.toLower()).arg(Student::getIdCount() + 1);
        // Generate unique phone
        QString phone = QString("0%1%2%3%4%5%6%7%8%9")
            .arg(5 + std::rand() % 3) // 05, 06, or 07
            .arg(std::rand() % 10).arg(std::rand() % 10).arg(std::rand() % 10)
            .arg(std::rand() % 10).arg(std::rand() % 10).arg(std::rand() % 10)
            .arg(std::rand() % 10).arg(std::rand() % 10);

        QString gender = (std::rand() % 2 == 0) ? "M" : "F";
        int year = 1 + std::rand() % 7;

        if (uni.isEmailDuplicate(email.toStdString()) ||
            uni.isPhoneDuplicate(phone.toStdString())) continue;

        Student s(fn.toStdString(), ln.toStdString(),
                  email.toStdString(), phone.toStdString(),
                  gender.toStdString(), year);
        if (s.isInitialized() && uni.enrollStudent(s)) added++;
    }

    AppState::instance().logActivity("SEEDER", QString("%1 students seeded").arg(added));
    statusLabel->setText(QString("✓ Added %1 students to pool").arg(added));
    if (auto* admin = AdminMainWindow::instance()) admin->refreshAll();
}

void SeederWidget::onAssignRandom() {
    int n = assignCountSpin->value();
    auto& uni = AppState::instance().university;
    int assigned = 0;

    // Get pool students
    const auto& pool = uni.getStudentPool();
    int limit = qMin(n, (int)pool.size());

    for (int i = 0; i < limit; i++) {
        int sid = pool[0].getId(); // always take first since pool shrinks
        if (uni.assignStudentToDormitory(sid, (std::rand() % 6) + 1)) {
            AppState::instance().logActivity("STUDENT_ASSIGNED",
                QString("Student %1 auto-assigned").arg(sid));
            assigned++;
        }
    }

    statusLabel->setText(QString("✓ Assigned %1 students to dormitories").arg(assigned));
    if (auto* admin = AdminMainWindow::instance()) admin->refreshAll();
}

void SeederWidget::onClearAll() {
    auto reply = QMessageBox::warning(this, "Clear All",
        "Remove ALL students from the system?",
        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (reply != QMessageBox::Yes) return;

    auto& uni = AppState::instance().university;
    auto all = AppState::instance().allStudents();
    int removed = 0;
    for (const auto& s : all) {
        if (uni.expelStudent(s.getId())) removed++;
    }
    Student::setIdCount(0);
    statusLabel->setText(QString("✓ Removed %1 students").arg(removed));
    AppState::instance().logActivity("SEEDER", "All students cleared");
    if (auto* admin = AdminMainWindow::instance()) admin->refreshAll();
}
