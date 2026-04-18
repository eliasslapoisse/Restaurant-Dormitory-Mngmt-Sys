#ifndef STYLES_H
#define STYLES_H

#include <QString>

namespace Styles {

// ── Palette ───────────────────────────────────────────────────────────────────
static const char* BG_DEEP        = "#080C14";
static const char* BG_BASE        = "#0D1117";
static const char* BG_CARD        = "#161B27";
static const char* BG_ELEVATED    = "#1C2333";
static const char* BG_SIDEBAR     = "#0A0D16";
static const char* BORDER         = "#21293A";
static const char* BORDER_ACTIVE  = "#FF6B35";

static const char* ORANGE         = "#FF6B35";
static const char* ORANGE_DARK    = "#E85520";
static const char* AMBER          = "#F7931E";
static const char* GOLD           = "#FFB830";

static const char* TEXT_PRIMARY   = "#E8EDF5";
static const char* TEXT_SECONDARY = "#8896A8";
static const char* TEXT_MUTED     = "#4A5568";

static const char* SUCCESS        = "#2ECC71";
static const char* WARNING        = "#F1C40F";
static const char* DANGER         = "#E74C3C";
static const char* INFO           = "#3498DB";

// ── Full App Stylesheet ────────────────────────────────────────────────────────
static const QString APP_STYLE = QString(R"(

/* ── Base ───────────────────────────────────────────────── */
QWidget {
    background-color: #0D1117;
    color: #E8EDF5;
    font-family: "Segoe UI", "SF Pro Display", Arial, sans-serif;
    font-size: 13px;
    selection-background-color: #FF6B35;
    selection-color: #FFFFFF;
}

QMainWindow {
    background-color: #080C14;
}

/* ── Scrollbars ─────────────────────────────────────────── */
QScrollBar:vertical {
    background: #0D1117;
    width: 8px;
    margin: 0;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #2D3748;
    border-radius: 4px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover { background: #FF6B35; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal {
    background: #0D1117;
    height: 8px;
    border-radius: 4px;
}
QScrollBar::handle:horizontal {
    background: #2D3748;
    border-radius: 4px;
    min-width: 30px;
}
QScrollBar::handle:horizontal:hover { background: #FF6B35; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

/* ── Input Fields ───────────────────────────────────────── */
QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDoubleSpinBox {
    background-color: #161B27;
    border: 1px solid #21293A;
    border-radius: 8px;
    padding: 8px 12px;
    color: #E8EDF5;
    font-size: 13px;
}
QLineEdit:focus, QTextEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
    border: 1px solid #FF6B35;
    background-color: #1C2333;
}
QLineEdit:hover, QTextEdit:hover, QSpinBox:hover { border-color: #374151; }
QLineEdit::placeholder { color: #4A5568; }

/* ── ComboBox ───────────────────────────────────────────── */
QComboBox {
    background-color: #161B27;
    border: 1px solid #21293A;
    border-radius: 8px;
    padding: 8px 12px;
    color: #E8EDF5;
    font-size: 13px;
    min-width: 120px;
}
QComboBox:focus, QComboBox:on { border-color: #FF6B35; }
QComboBox::drop-down {
    border: none;
    width: 24px;
}
QComboBox::down-arrow {
    image: none;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-top: 6px solid #8896A8;
    margin-right: 8px;
}
QComboBox QAbstractItemView {
    background-color: #1C2333;
    border: 1px solid #FF6B35;
    border-radius: 8px;
    padding: 4px;
    selection-background-color: #FF6B35;
    selection-color: #FFFFFF;
    outline: none;
}
QComboBox QAbstractItemView::item { padding: 6px 12px; border-radius: 4px; }

/* ── Buttons ────────────────────────────────────────────── */
QPushButton {
    background-color: #FF6B35;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 9px 20px;
    font-size: 13px;
    font-weight: 600;
    letter-spacing: 0.3px;
}
QPushButton:hover {
    background-color: #FF7D4E;
}
QPushButton:pressed {
    background-color: #E85520;
}
QPushButton:disabled {
    background-color: #2D3748;
    color: #4A5568;
}

QPushButton[class="secondary"] {
    background-color: #1C2333;
    border: 1px solid #21293A;
    color: #E8EDF5;
}
QPushButton[class="secondary"]:hover {
    background-color: #21293A;
    border-color: #FF6B35;
    color: #FF6B35;
}
QPushButton[class="danger"] {
    background-color: #E74C3C;
}
QPushButton[class="danger"]:hover { background-color: #C0392B; }

QPushButton[class="success"] {
    background-color: #2ECC71;
    color: #0D1117;
}
QPushButton[class="success"]:hover { background-color: #27AE60; }

QPushButton[class="warning"] {
    background-color: #F1C40F;
    color: #0D1117;
}
QPushButton[class="warning"]:hover { background-color: #D4AC0D; }

QPushButton[class="info"] {
    background-color: #3498DB;
}
QPushButton[class="info"]:hover { background-color: #2980B9; }

/* ── Tables ─────────────────────────────────────────────── */
QTableWidget {
    background-color: #0D1117;
    alternate-background-color: #111827;
    border: 1px solid #21293A;
    border-radius: 10px;
    gridline-color: #1A2030;
    color: #E8EDF5;
    font-size: 13px;
}
QTableWidget::item {
    padding: 10px 14px;
    border-bottom: 1px solid #1A2030;
}
QTableWidget::item:selected {
    background-color: rgba(255, 107, 53, 0.18);
    color: #FF6B35;
    border-left: 2px solid #FF6B35;
}
QTableWidget::item:hover {
    background-color: rgba(255, 107, 53, 0.08);
}
QHeaderView::section {
    background-color: #161B27;
    color: #8896A8;
    padding: 10px 14px;
    border: none;
    border-bottom: 2px solid #FF6B35;
    font-size: 11px;
    font-weight: 700;
    text-transform: uppercase;
    letter-spacing: 1px;
}
QHeaderView::section:first { border-top-left-radius: 8px; }
QHeaderView::section:last  { border-top-right-radius: 8px; }

/* ── Tab Widget ─────────────────────────────────────────── */
QTabWidget::pane {
    background-color: #0D1117;
    border: 1px solid #21293A;
    border-radius: 0 10px 10px 10px;
}
QTabBar::tab {
    background-color: #161B27;
    color: #8896A8;
    border: 1px solid #21293A;
    border-bottom: none;
    border-radius: 8px 8px 0 0;
    padding: 9px 20px;
    font-size: 13px;
    font-weight: 600;
    margin-right: 2px;
}
QTabBar::tab:selected {
    background-color: #FF6B35;
    color: #FFFFFF;
    border-color: #FF6B35;
}
QTabBar::tab:hover:!selected {
    background-color: #1C2333;
    color: #E8EDF5;
    border-color: #374151;
}

/* ── Group Box ──────────────────────────────────────────── */
QGroupBox {
    background-color: #161B27;
    border: 1px solid #21293A;
    border-radius: 10px;
    margin-top: 16px;
    padding: 16px;
    font-weight: 700;
    color: #E8EDF5;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 0 8px;
    color: #FF6B35;
    font-size: 13px;
    font-weight: 700;
    left: 14px;
    top: -8px;
}

/* ── Labels ─────────────────────────────────────────────── */
QLabel { background: transparent; }
QLabel[class="title"] { font-size: 22px; font-weight: 800; color: #E8EDF5; }
QLabel[class="subtitle"] { font-size: 14px; color: #8896A8; }
QLabel[class="stat-value"] { font-size: 32px; font-weight: 800; color: #FF6B35; }
QLabel[class="stat-label"] { font-size: 11px; color: #8896A8; text-transform: uppercase; letter-spacing: 1px; }
QLabel[class="badge-success"] { background-color: rgba(46,204,113,0.15); color: #2ECC71; border-radius: 10px; padding: 3px 10px; font-size: 11px; font-weight: 700; }
QLabel[class="badge-warning"] { background-color: rgba(241,196,15,0.15); color: #F1C40F; border-radius: 10px; padding: 3px 10px; font-size: 11px; font-weight: 700; }
QLabel[class="badge-danger"]  { background-color: rgba(231,76,60,0.15); color: #E74C3C; border-radius: 10px; padding: 3px 10px; font-size: 11px; font-weight: 700; }
QLabel[class="badge-info"]    { background-color: rgba(52,152,219,0.15); color: #3498DB; border-radius: 10px; padding: 3px 10px; font-size: 11px; font-weight: 700; }
QLabel[class="badge-orange"]  { background-color: rgba(255,107,53,0.15); color: #FF6B35; border-radius: 10px; padding: 3px 10px; font-size: 11px; font-weight: 700; }

/* ── Progress Bar ───────────────────────────────────────── */
QProgressBar {
    background-color: #1C2333;
    border: none;
    border-radius: 6px;
    height: 10px;
    text-align: center;
    color: transparent;
}
QProgressBar::chunk {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #FF6B35, stop:1 #F7931E);
    border-radius: 6px;
}

/* ── Frames / Separators ────────────────────────────────── */
QFrame[frameShape="4"] { /* HLine */
    background-color: #21293A;
    max-height: 1px;
    border: none;
}
QFrame[frameShape="5"] { /* VLine */
    background-color: #21293A;
    max-width: 1px;
    border: none;
}

/* ── Splitter ───────────────────────────────────────────── */
QSplitter::handle { background: #21293A; }
QSplitter::handle:horizontal { width: 1px; }
QSplitter::handle:vertical { height: 1px; }

/* ── Message Box ────────────────────────────────────────── */
QMessageBox {
    background-color: #161B27;
    border: 1px solid #21293A;
    border-radius: 12px;
}
QMessageBox QLabel { color: #E8EDF5; font-size: 14px; }

/* ── Dialog ─────────────────────────────────────────────── */
QDialog {
    background-color: #0D1117;
    border: 1px solid #21293A;
    border-radius: 12px;
}

/* ── List Widget ────────────────────────────────────────── */
QListWidget {
    background-color: #0D1117;
    border: 1px solid #21293A;
    border-radius: 10px;
    color: #E8EDF5;
    padding: 4px;
}
QListWidget::item {
    padding: 10px 14px;
    border-radius: 6px;
    margin: 2px 0;
}
QListWidget::item:selected {
    background-color: rgba(255,107,53,0.18);
    color: #FF6B35;
}
QListWidget::item:hover { background-color: rgba(255,107,53,0.08); }

/* ── Check Box / Radio ──────────────────────────────────── */
QCheckBox { spacing: 8px; }
QCheckBox::indicator {
    width: 18px; height: 18px;
    border: 2px solid #374151;
    border-radius: 4px;
    background: #161B27;
}
QCheckBox::indicator:checked {
    background-color: #FF6B35;
    border-color: #FF6B35;
}
QRadioButton { spacing: 8px; }
QRadioButton::indicator {
    width: 16px; height: 16px;
    border: 2px solid #374151;
    border-radius: 8px;
    background: #161B27;
}
QRadioButton::indicator:checked {
    background-color: #FF6B35;
    border-color: #FF6B35;
}

/* ── Tooltip ────────────────────────────────────────────── */
QToolTip {
    background-color: #1C2333;
    color: #E8EDF5;
    border: 1px solid #FF6B35;
    border-radius: 6px;
    padding: 6px 10px;
    font-size: 12px;
}

/* ── Status Bar ─────────────────────────────────────────── */
QStatusBar {
    background-color: #080C14;
    color: #8896A8;
    border-top: 1px solid #21293A;
}

)");

} // namespace Styles
#endif // STYLES_H