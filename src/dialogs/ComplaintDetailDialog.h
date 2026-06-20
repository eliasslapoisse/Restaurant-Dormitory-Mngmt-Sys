#pragma once
#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include "appstate.h"

class ComplaintDetailDialog : public QDialog {
    Q_OBJECT
public:
    explicit ComplaintDetailDialog(int complaintId, bool adminView,
                                   QWidget* parent = nullptr);

private slots:
    void onMarkResolved();
    void onSaveReply();

private:
    int   m_id;
    bool  m_adminView;

    QLabel*    statusLabel;
    QTextEdit* descriptionEdit;
    QTextEdit* replyEdit = nullptr;

    void buildUi();
    Complaint* findComplaint();
};
