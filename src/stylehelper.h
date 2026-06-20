#pragma once
#include <QString>

namespace Style {

// ─── Colours ────────────────────────────────────────────────────────────────
inline const QString PRIMARY_ORANGE = "#FF6B35";
inline const QString PRIMARY_YELLOW = "#FDB827";
inline const QString PRIMARY_MID    = "#F7931E";
inline const QString BG_LIGHT       = "#F8F9FA";
inline const QString BORDER_COLOR   = "#E9ECEF";
inline const QString TEXT_PRIMARY   = "#212529";
inline const QString TEXT_SECONDARY = "#6C757D";

// ─── Gradient string (for use inside gradient() calls) ──────────────────────
inline QString gradientCSS(const QString& x1 = "0", const QString& y1 = "0",
                            const QString& x2 = "1", const QString& y2 = "0")
{
    return QString("qlineargradient(x1:%1,y1:%2,x2:%3,y2:%4,"
                   "stop:0 #FF6B35, stop:0.5 #F7931E, stop:1 #FDB827)")
           .arg(x1, y1, x2, y2);
}

// ─── Widget stylesheets ──────────────────────────────────────────────────────
inline QString cardSS() {
    return "background:white; border-radius:12px; border:1px solid #E9ECEF;";
}

inline QString primaryBtnSS() {
    return QString("QPushButton {"
                   "  background:%1;"
                   "  color:white; border:none; border-radius:8px;"
                   "  padding:10px 20px; font-weight:bold; font-size:13px;"
                   "}"
                   "QPushButton:hover {"
                   "  background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                   "    stop:0 #E65C2A,stop:1 #E8A522);"
                   "}"
                   "QPushButton:disabled { background:#C0C0C0; color:#808080; }")
           .arg(gradientCSS());
}

inline QString dangerBtnSS() {
    return "QPushButton { background:#DC3545; color:white; border:none;"
           "  border-radius:8px; padding:10px 20px; font-weight:bold; }"
           "QPushButton:hover { background:#C82333; }";
}

inline QString secondaryBtnSS() {
    return "QPushButton { background:white; color:#F7931E; border:2px solid #F7931E;"
           "  border-radius:8px; padding:8px 16px; }"
           "QPushButton:hover { background:#FFF3E0; }";
}

inline QString smallBtnSS() {
    return "QPushButton { background:white; color:#495057; border:1px solid #DEE2E6;"
           "  border-radius:6px; padding:4px 10px; font-size:12px; }"
           "QPushButton:hover { background:#F8F9FA; border-color:#F7931E; color:#F7931E; }";
}

inline QString inputSS() {
    return "QLineEdit, QTextEdit, QSpinBox {"
           "  border:1px solid #DEE2E6; border-radius:6px;"
           "  padding:8px 12px; background:white; }"
           "QLineEdit:focus, QTextEdit:focus {"
           "  border:2px solid #F7931E; }";
}

// Dedicated combo style: forces Fusion arrow to appear with border-radius
inline QString comboSS() {
    return "QComboBox {"
           "  border:1px solid #DEE2E6; border-radius:6px;"
           "  padding:6px 8px 6px 12px; background:white;"
           "  color:#495057; font-size:13px; }"
           "QComboBox:hover { border-color:#adb5bd; }"
           "QComboBox:on, QComboBox:focus { border:2px solid #F7931E; }"
           "QComboBox::drop-down { border:none; }"
           "QComboBox QAbstractItemView {"
           "  border:1px solid #E9ECEF; background:white; outline:none;"
           "  selection-background-color:#FFF3E0; selection-color:#212529; }";
}

inline QString tableSS() {
    return "QTableWidget { border:1px solid #E9ECEF; border-radius:14px;"
           "  gridline-color:#F0F0F0; background:white;"
           "  alternate-background-color:#FAFAFA; }"
           "QTableWidget::item { padding:8px 12px; border:none; }"
           "QTableWidget::item:selected { background:#FFF3E0; color:#212529; }"
           "QHeaderView::section { background:#F8F9FA; color:#6C757D; font-weight:bold;"
           "  font-size:11px; padding:10px 12px; border:none;"
           "  border-bottom:2px solid #E9ECEF; }";
}

inline QString badgeSS(const QString& bg, const QString& fg) {
    return QString("QLabel { background:%1; color:%2; border-radius:4px;"
                   "  padding:2px 8px; font-size:11px; font-weight:bold; }")
           .arg(bg, fg);
}

inline QString badgeGreenSS()  { return badgeSS("#D4EDDA", "#155724"); }
inline QString badgeYellowSS() { return badgeSS("#FFF3CD", "#856404"); }
inline QString badgeRedSS()    { return badgeSS("#F8D7DA", "#721C24"); }
inline QString badgeBlueSS()   { return badgeSS("#D1ECF1", "#0C5460"); }

} // namespace Style
