#include "studentswidget.h"
#include "datamanager.h"
#include "styles.h"
#include "constants.h"
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>
#include <QFrame>

// ── EnrollDialog ───────────────────────────────────────────────────────────────
EnrollDialog::EnrollDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Enroll New Student");
    setFixedSize(480, 460);
    setStyleSheet(Styles::APP_STYLE +
        "QDialog { background: #0D1117; border: 1px solid #FF6B35; border-radius: 14px; }");

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(28, 24, 28, 24);
    lay->setSpacing(16);

    auto* titleLbl = new QLabel("➕  Enroll New Student", this);
    titleLbl->setStyleSheet("font-size: 18px; font-weight: 800; color: #E8EDF5;");
    lay->addWidget(titleLbl);

    auto* form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight);

    auto mkField = [this](const QString& ph) {
        auto* f = new QLineEdit(this);
        f->setPlaceholderText(ph);
        f->setFixedHeight(40);
        return f;
    };

    m_firstName  = mkField("e.g. Mohammed"); form->addRow("First Name:", m_firstName);
    m_familyName = mkField("e.g. Benali");   form->addRow("Family Name:", m_familyName);
    m_email      = mkField("e.g. m.benali@ensia.edu.dz"); form->addRow("Email:", m_email);
    m_phone      = mkField("e.g. 0551234567"); form->addRow("Phone:", m_phone);

    m_gender = new QComboBox(this);
    m_gender->addItems({"Male", "Female"});
    m_gender->setFixedHeight(40);
    form->addRow("Gender:", m_gender);

    m_year = new QSpinBox(this);
    m_year->setRange(1, 7);
    m_year->setValue(1);
    m_year->setFixedHeight(40);
    m_year->setSuffix(" Year");
    form->addRow("Academic Year:", m_year);
    lay->addLayout(form);

    m_errorLbl = new QLabel("", this);
    m_errorLbl->setStyleSheet(
        "background: rgba(231,76,60,0.12); color: #E74C3C; border: 1px solid rgba(231,76,60,0.3);"
        " border-radius: 8px; padding: 8px 12px;");
    m_errorLbl->setVisible(false);
    m_errorLbl->setWordWrap(true);
    lay->addWidget(m_errorLbl);

    auto* btnRow = new QHBoxLayout();
    auto* cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setProperty("class", "secondary");
    cancelBtn->setFixedHeight(42);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    auto* okBtn = new QPushButton("Enroll Student ✓", this);
    okBtn->setFixedHeight(42);
    connect(okBtn, &QPushButton::clicked, this, &EnrollDialog::onAccept);
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(okBtn);
    lay->addLayout(btnRow);
}

void EnrollDialog::onAccept() {
    Student temp(m_firstName->text().toStdString(), m_familyName->text().toStdString(),
                 m_email->text().toStdString(), m_phone->text().toStdString(),
                 m_gender->currentText().toStdString(), m_year->value());
    if (!temp.isInitialized()) {
        m_errorLbl->setText("❌ Validation failed. Please check:\n• Name: letters only, 1-30 chars\n• Email: valid format with domain\n• Phone: 10 digits starting with 05/06/07");
        m_errorLbl->setVisible(true);
        Student::setIdCount(Student::getIdCount() - 0); // revert not needed, constructor doesn't increment on fail
        return;
    }
    accept();
}

QString EnrollDialog::firstName()    const { return m_firstName->text().trimmed(); }
QString EnrollDialog::familyName()   const { return m_familyName->text().trimmed(); }
QString EnrollDialog::email()        const { return m_email->text().trimmed(); }
QString EnrollDialog::phone()        const { return m_phone->text().trimmed(); }
QString EnrollDialog::gender()       const { return m_gender->currentText(); }
int     EnrollDialog::academicYear() const { return m_year->value(); }

// ── AssignDialog ───────────────────────────────────────────────────────────────
AssignDialog::AssignDialog(int studentId, QWidget* parent) : QDialog(parent) {
    setWindowTitle("Assign Student to Room");
    setFixedSize(400, 320);
    setStyleSheet(Styles::APP_STYLE);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(24, 24, 24, 24);
    lay->setSpacing(14);

    auto* titleLbl = new QLabel(QString("🏠  Assign Student #%1 to Room").arg(studentId), this);
    titleLbl->setStyleSheet("font-size: 16px; font-weight: 800; color: #E8EDF5;");
    lay->addWidget(titleLbl);

    auto* form = new QFormLayout();
    form->setSpacing(10);

    m_dorm = new QComboBox(this);
    for (int i = 1; i <= 6; i++) m_dorm->addItem(QString("Dormitory %1").arg(i), i);
    m_dorm->setFixedHeight(40);
    form->addRow("Dormitory:", m_dorm);

    m_block = new QComboBox(this);
    m_block->addItems({"A", "B", "C", "D", "E"});
    m_block->setFixedHeight(40);
    form->addRow("Block:", m_block);

    m_floor = new QSpinBox(this);
    m_floor->setRange(0, LAST_FLOOR);
    m_floor->setFixedHeight(40);
    m_floor->setPrefix("Floor ");
    form->addRow("Floor:", m_floor);

    m_roomNum = new QSpinBox(this);
    m_roomNum->setRange(1, LAST_NUMBER);
    m_roomNum->setFixedHeight(40);
    m_roomNum->setPrefix("Room ");
    form->addRow("Room Number:", m_roomNum);

    lay->addLayout(form);
    lay->addStretch();

    auto* btnRow = new QHBoxLayout();
    auto* cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setProperty("class", "secondary");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    auto* okBtn = new QPushButton("Assign ✓", this);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(okBtn);
    lay->addLayout(btnRow);
}

int     AssignDialog::dormNumber() const { return m_dorm->currentData().toInt(); }
QString AssignDialog::blockName()  const { return m_block->currentText(); }
int     AssignDialog::floor()      const { return m_floor->value(); }
int     AssignDialog::roomNumber() const { return m_roomNum->value(); }

// ── StudentsWidget ─────────────────────────────────────────────────────────────
StudentsWidget::StudentsWidget(QWidget* parent) : QWidget(parent) {
    buildUi();
}

void StudentsWidget::buildUi() {
    auto* rootLay = new QVBoxLayout(this);
    rootLay->setContentsMargins(24, 20, 24, 20);
    rootLay->setSpacing(16);

    // ── Header ────────────────────────────────────────────────────────────────
    auto* headerRow = new QHBoxLayout();
    auto* titleLbl = new QLabel("🎓  Student Management", this);
    titleLbl->setStyleSheet("font-size: 22px; font-weight: 900; color: #E8EDF5;");
    headerRow->addWidget(titleLbl);
    headerRow->addStretch();

    m_enrollBtn = new QPushButton("➕  Enroll Student", this);
    m_enrollBtn->setFixedHeight(40);
    connect(m_enrollBtn, &QPushButton::clicked, this, &StudentsWidget::onEnroll);
    headerRow->addWidget(m_enrollBtn);
    rootLay->addLayout(headerRow);

    // ── Toolbar ────────────────────────────────────────────────────────────────
    auto* toolRow = new QHBoxLayout();
    toolRow->setSpacing(12);

    m_searchBar = new QLineEdit(this);
    m_searchBar->setPlaceholderText("🔍  Search by name, ID, or email…");
    m_searchBar->setFixedHeight(40);
    m_searchBar->setMinimumWidth(280);
    connect(m_searchBar, &QLineEdit::textChanged, this, &StudentsWidget::onSearch);
    toolRow->addWidget(m_searchBar, 1);

    auto* yearLabel = new QLabel("Year:", this);
    yearLabel->setStyleSheet("color: #8896A8; background: transparent;");
    toolRow->addWidget(yearLabel);

    m_filterYear = new QComboBox(this);
    m_filterYear->addItem("All Years");
    for (int y = 1; y <= 7; y++) m_filterYear->addItem(QString("Year %1").arg(y), y);
    m_filterYear->setFixedHeight(40);
    connect(m_filterYear, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StudentsWidget::onFilterChanged);
    toolRow->addWidget(m_filterYear);

    // Action buttons
    auto mkBtn = [&](const QString& t, const char* cls) {
        auto* b = new QPushButton(t, this);
        b->setProperty("class", cls);
        b->setFixedHeight(40);
        b->setEnabled(false);
        return b;
    };
    m_assignDormBtn = mkBtn("📦 Auto-Assign Dorm", "success");
    m_assignRoomBtn = mkBtn("🏠 Assign to Room",   "info");
    m_unassignBtn   = mkBtn("↩ Unassign",           "warning");
    m_expelBtn      = mkBtn("🗑 Expel",              "danger");
    m_locateBtn     = mkBtn("📍 Locate",             "secondary");

    connect(m_assignDormBtn, &QPushButton::clicked, this, &StudentsWidget::onAssignToDorm);
    connect(m_assignRoomBtn, &QPushButton::clicked, this, &StudentsWidget::onAssignToRoom);
    connect(m_unassignBtn,   &QPushButton::clicked, this, &StudentsWidget::onUnassign);
    connect(m_expelBtn,      &QPushButton::clicked, this, &StudentsWidget::onExpel);
    connect(m_locateBtn,     &QPushButton::clicked, this, &StudentsWidget::onLocate);

    toolRow->addWidget(m_assignDormBtn);
    toolRow->addWidget(m_assignRoomBtn);
    toolRow->addWidget(m_unassignBtn);
    toolRow->addWidget(m_locateBtn);
    toolRow->addWidget(m_expelBtn);
    rootLay->addLayout(toolRow);

    // ── Tabs: Pool / Assigned ─────────────────────────────────────────────────
    m_tabs = new QTabWidget(this);

    // Pool tab
    auto* poolPage = new QWidget();
    auto* poolLay  = new QVBoxLayout(poolPage);
    poolLay->setContentsMargins(0, 10, 0, 0);
    poolLay->setSpacing(8);

    m_poolCountLbl = new QLabel("", poolPage);
    m_poolCountLbl->setStyleSheet("font-size: 12px; color: #8896A8; background: transparent;");
    poolLay->addWidget(m_poolCountLbl);

    m_poolTable = new QTableWidget(0, 7, poolPage);
    m_poolTable->setHorizontalHeaderLabels({"ID", "First Name", "Family Name", "Email", "Phone", "Gender", "Year"});
    m_poolTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_poolTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_poolTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_poolTable->setAlternatingRowColors(true);
    m_poolTable->verticalHeader()->setVisible(false);
    m_poolTable->setShowGrid(false);
    connect(m_poolTable, &QTableWidget::itemSelectionChanged,
            this, &StudentsWidget::onTableSelectionChanged);
    connect(m_poolTable, &QTableWidget::cellDoubleClicked,
            this, [this](int row, int) {
        auto* idItem = m_poolTable->item(row, 0);
        if (idItem) showStudentDetails(idItem->text().toInt());
    });
    poolLay->addWidget(m_poolTable);
    m_tabs->addTab(poolPage, "⏳  Student Pool");

    // Assigned tab
    auto* assignedPage = new QWidget();
    auto* assignedLay  = new QVBoxLayout(assignedPage);
    assignedLay->setContentsMargins(0, 10, 0, 0);
    assignedLay->setSpacing(8);

    m_assignedCountLbl = new QLabel("", assignedPage);
    m_assignedCountLbl->setStyleSheet("font-size: 12px; color: #8896A8; background: transparent;");
    assignedLay->addWidget(m_assignedCountLbl);

    m_assignedTable = new QTableWidget(0, 9, assignedPage);
    m_assignedTable->setHorizontalHeaderLabels({"ID", "First Name", "Family Name", "Email", "Phone", "Gender", "Year", "Dorm", "Location"});
    m_assignedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_assignedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_assignedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_assignedTable->setAlternatingRowColors(true);
    m_assignedTable->verticalHeader()->setVisible(false);
    m_assignedTable->setShowGrid(false);
    connect(m_assignedTable, &QTableWidget::itemSelectionChanged,
            this, &StudentsWidget::onTableSelectionChanged);
    connect(m_assignedTable, &QTableWidget::cellDoubleClicked,
            this, [this](int row, int) {
        auto* idItem = m_assignedTable->item(row, 0);
        if (idItem) showStudentDetails(idItem->text().toInt());
    });
    assignedLay->addWidget(m_assignedTable);
    m_tabs->addTab(assignedPage, "🏠  Assigned Students");

    rootLay->addWidget(m_tabs, 1);

    refresh();
}

void StudentsWidget::populatePool() {
    auto* DM = DataManager::instance();
    const auto& pool = DM->uni().getStudentPool();
    m_poolTable->setRowCount(0);

    QString search = m_searchBar->text().toLower();
    int yearFilter = m_filterYear->currentIndex() > 0 ? m_filterYear->currentData().toInt() : -1;

    int row = 0;
    for (const auto& s : pool) {
        if (yearFilter > 0 && s.getAcademicYear() != yearFilter) continue;
        QString fullName = QString::fromStdString(s.getFirstName() + " " + s.getFamilyName()).toLower();
        QString email    = QString::fromStdString(s.getEmail()).toLower();
        if (!search.isEmpty() && !fullName.contains(search) && !email.contains(search) &&
            !QString::number(s.getId()).contains(search)) continue;

        m_poolTable->insertRow(row);
        auto mk = [&](const QString& t) -> QTableWidgetItem* {
            auto* i = new QTableWidgetItem(t);
            i->setTextAlignment(Qt::AlignCenter);
            return i;
        };
        m_poolTable->setItem(row, 0, mk(QString::number(s.getId())));
        m_poolTable->setItem(row, 1, mk(QString::fromStdString(s.getFirstName())));
        m_poolTable->setItem(row, 2, mk(QString::fromStdString(s.getFamilyName())));
        m_poolTable->setItem(row, 3, mk(QString::fromStdString(s.getEmail())));
        m_poolTable->setItem(row, 4, mk(QString::fromStdString(s.getPhoneNumber())));
        m_poolTable->setItem(row, 5, mk(QString::fromStdString(s.getGender())));
        m_poolTable->setItem(row, 6, mk(QString("Year %1").arg(s.getAcademicYear())));
        m_poolTable->setRowHeight(row, 44);
        row++;
    }
    m_poolCountLbl->setText(QString("Showing %1 of %2 students in pool").arg(row).arg(pool.size()));
}

void StudentsWidget::populateAssigned() {
    auto* DM = DataManager::instance();
    m_assignedTable->setRowCount(0);

    QString search = m_searchBar->text().toLower();
    int yearFilter = m_filterYear->currentIndex() > 0 ? m_filterYear->currentData().toInt() : -1;

    int row = 0;
    for (const auto& dorm : DM->uni().getDormitories()) {
        auto students = dorm.getAllAssignedStudents();
        for (const auto& s : students) {
            if (yearFilter > 0 && s.getAcademicYear() != yearFilter) continue;
            QString fullName = QString::fromStdString(s.getFirstName() + " " + s.getFamilyName()).toLower();
            if (!search.isEmpty() && !fullName.contains(search) &&
                !QString::fromStdString(s.getEmail()).toLower().contains(search) &&
                !QString::number(s.getId()).contains(search)) continue;

            m_assignedTable->insertRow(row);
            auto mk = [&](const QString& t) -> QTableWidgetItem* {
                auto* i = new QTableWidgetItem(t);
                i->setTextAlignment(Qt::AlignCenter);
                return i;
            };
            QString loc = QString::fromStdString(dorm.findStudentLocation(s.getId()));
            m_assignedTable->setItem(row, 0, mk(QString::number(s.getId())));
            m_assignedTable->setItem(row, 1, mk(QString::fromStdString(s.getFirstName())));
            m_assignedTable->setItem(row, 2, mk(QString::fromStdString(s.getFamilyName())));
            m_assignedTable->setItem(row, 3, mk(QString::fromStdString(s.getEmail())));
            m_assignedTable->setItem(row, 4, mk(QString::fromStdString(s.getPhoneNumber())));
            m_assignedTable->setItem(row, 5, mk(QString::fromStdString(s.getGender())));
            m_assignedTable->setItem(row, 6, mk(QString("Year %1").arg(s.getAcademicYear())));
            m_assignedTable->setItem(row, 7, mk(QString("Dorm %1").arg(dorm.getNumber())));
            m_assignedTable->setItem(row, 8, mk(loc));
            m_assignedTable->setRowHeight(row, 44);
            row++;
        }
    }
    m_assignedCountLbl->setText(QString("Showing %1 assigned students").arg(row));
}

void StudentsWidget::refresh() {
    populatePool();
    populateAssigned();
}

void StudentsWidget::onSearch(const QString&)   { refresh(); }
void StudentsWidget::onFilterChanged()           { refresh(); }

void StudentsWidget::onTableSelectionChanged() {
    bool poolSel     = !m_poolTable->selectedItems().isEmpty();
    bool assignedSel = !m_assignedTable->selectedItems().isEmpty();

    m_assignDormBtn->setEnabled(poolSel);
    m_assignRoomBtn->setEnabled(poolSel);
    m_unassignBtn->setEnabled(assignedSel);
    m_expelBtn->setEnabled(poolSel || assignedSel);
    m_locateBtn->setEnabled(poolSel || assignedSel);
}

void StudentsWidget::onEnroll() {
    EnrollDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    auto* DM = DataManager::instance();
    if (DM->uni().isEmailDuplicate(dlg.email().toStdString())) {
        QMessageBox::warning(this, "Duplicate Email", "This email is already registered in the system.");
        return;
    }
    if (DM->uni().isPhoneDuplicate(dlg.phone().toStdString())) {
        QMessageBox::warning(this, "Duplicate Phone", "This phone number is already registered.");
        return;
    }

    Student s(dlg.firstName().toStdString(), dlg.familyName().toStdString(),
              dlg.email().toStdString(), dlg.phone().toStdString(),
              dlg.gender().toStdString(), dlg.academicYear());

    if (!s.isInitialized()) {
        QMessageBox::critical(this, "Validation Error", "Student data is invalid. Please check all fields.");
        return;
    }

    if (DM->uni().enrollStudent(s)) {
        DM->logActivity("student", QString("Enrolled: %1 %2 (ID #%3)")
            .arg(dlg.firstName()).arg(dlg.familyName()).arg(s.getId()));
        refresh();
        QMessageBox::information(this, "Success",
            QString("✅ Student enrolled successfully!\nID: %1\nDefault password: %2")
                .arg(s.getId()).arg(dlg.phone()));
    } else {
        QMessageBox::critical(this, "Error", "Failed to enroll student. Possible duplicate data.");
    }
}

void StudentsWidget::onAssignToDorm() {
    auto* table = m_poolTable;
    if (table->selectedItems().isEmpty()) return;
    int row = table->currentRow();
    int id  = table->item(row, 0)->text().toInt();

    // Pick a dorm
    bool ok;
    QStringList dormChoices;
    auto* DM = DataManager::instance();
    for (const auto& d : DM->uni().getDormitories()) {
        int avail = d.getAvailableSpace();
        dormChoices << QString("Dorm %1 (%2 spaces available)").arg(d.getNumber()).arg(avail);
    }
    QString choice = QInputDialog::getItem(this, "Select Dormitory", "Choose dormitory:", dormChoices, 0, false, &ok);
    if (!ok) return;

    int dormNum = dormChoices.indexOf(choice) + 1;
    if (DM->uni().assignStudentToDormitory(id, dormNum)) {
        DM->logActivity("student", QString("Student #%1 auto-assigned to Dorm %2").arg(id).arg(dormNum));
        refresh();
        QMessageBox::information(this, "Assigned", QString("✅ Student #%1 assigned to Dormitory %2").arg(id).arg(dormNum));
    } else {
        QMessageBox::warning(this, "Failed", "Assignment failed. Dormitory may be full or student not found.");
    }
}

void StudentsWidget::onAssignToRoom() {
    if (m_poolTable->selectedItems().isEmpty()) return;
    int row = m_poolTable->currentRow();
    int id  = m_poolTable->item(row, 0)->text().toInt();

    AssignDialog dlg(id, this);
    if (dlg.exec() != QDialog::Accepted) return;

    auto* DM = DataManager::instance();
    Dormitory* dorm = DM->uni().getDormitory(dlg.dormNumber());
    if (!dorm) { QMessageBox::warning(this, "Error", "Dormitory not found."); return; }

    // Find student in pool
    const Student* s = nullptr;
    for (const auto& st : DM->uni().getStudentPool()) {
        if (st.getId() == id) { s = &st; break; }
    }
    if (!s) { QMessageBox::warning(this, "Error", "Student not found in pool."); return; }

    if (dorm->assignStudentToRoom(dlg.blockName().toStdString(), dlg.floor(), dlg.roomNumber(), *s)) {
        DM->uni().removeFromPool(id);
        DM->logActivity("student", QString("Student #%1 → Dorm %2, Block %3, Floor %4, Room %5")
            .arg(id).arg(dlg.dormNumber()).arg(dlg.blockName()).arg(dlg.floor()).arg(dlg.roomNumber()));
        refresh();
        QMessageBox::information(this, "Assigned", "✅ Student assigned to specific room successfully.");
    } else {
        QMessageBox::warning(this, "Failed", "Room assignment failed. Check room capacity or duplicates.");
    }
}

void StudentsWidget::onUnassign() {
    if (m_assignedTable->selectedItems().isEmpty()) return;
    int row = m_assignedTable->currentRow();
    int id  = m_assignedTable->item(row, 0)->text().toInt();
    QString name = m_assignedTable->item(row, 1)->text() + " " + m_assignedTable->item(row, 2)->text();

    if (QMessageBox::question(this, "Confirm Unassign",
        QString("Move %1 (ID #%2) back to the student pool?").arg(name).arg(id)) != QMessageBox::Yes) return;

    auto* DM = DataManager::instance();
    if (DM->uni().unassignStudent(id)) {
        DM->logActivity("student", QString("Student #%1 unassigned → returned to pool").arg(id));
        refresh();
    } else {
        QMessageBox::warning(this, "Error", "Could not unassign student.");
    }
}

void StudentsWidget::onExpel() {
    QTableWidget* table = m_tabs->currentIndex() == 0 ? m_poolTable : m_assignedTable;
    if (table->selectedItems().isEmpty()) return;
    int row = table->currentRow();
    int id  = table->item(row, 0)->text().toInt();
    QString name = table->item(row, 1)->text() + " " + table->item(row, 2)->text();

    if (QMessageBox::question(this, "⚠️ Confirm Expulsion",
        QString("Permanently remove %1 (ID #%2) from the system?\nThis action cannot be undone.").arg(name).arg(id),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;

    auto* DM = DataManager::instance();
    if (DM->uni().expelStudent(id)) {
        DM->logActivity("student", QString("Student #%1 (%2) expelled from system").arg(id).arg(name));
        refresh();
        QMessageBox::information(this, "Expelled", QString("Student %1 has been removed from the system.").arg(name));
    } else {
        QMessageBox::warning(this, "Error", "Could not expel student.");
    }
}

void StudentsWidget::onLocate() {
    QTableWidget* table = m_tabs->currentIndex() == 0 ? m_poolTable : m_assignedTable;
    if (table->selectedItems().isEmpty()) return;
    int id = table->item(table->currentRow(), 0)->text().toInt();

    auto* DM = DataManager::instance();
    QString location = QString::fromStdString(DM->uni().locateStudent(id));
    QMessageBox::information(this, "Student Location", QString("📍 Student #%1\n\n%2").arg(id).arg(location));
}

void StudentsWidget::showStudentDetails(int studentId) {
    auto* DM = DataManager::instance();
    QString info;

    auto check = [&](const Student& s) {
        info = QString(
            "👤 Student Details\n\n"
            "ID:            %1\n"
            "Full Name:     %2 %3\n"
            "Email:         %4\n"
            "Phone:         %5\n"
            "Gender:        %6\n"
            "Academic Year: Year %7\n\n"
            "📍 Location:   %8")
            .arg(s.getId())
            .arg(QString::fromStdString(s.getFirstName()))
            .arg(QString::fromStdString(s.getFamilyName()))
            .arg(QString::fromStdString(s.getEmail()))
            .arg(QString::fromStdString(s.getPhoneNumber()))
            .arg(QString::fromStdString(s.getGender()))
            .arg(s.getAcademicYear())
            .arg(QString::fromStdString(DM->uni().locateStudent(studentId)));
    };

    for (const auto& s : DM->uni().getStudentPool()) {
        if (s.getId() == studentId) { check(s); break; }
    }
    for (const auto& d : DM->uni().getDormitories()) {
        for (const auto& s : d.getAllAssignedStudents()) {
            if (s.getId() == studentId) { check(s); break; }
        }
    }

    if (!info.isEmpty()) QMessageBox::information(this, "Student Details", info);
}