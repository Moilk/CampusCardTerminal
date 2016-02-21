#ifndef AUTHENTICATEDIALOG_H
#define AUTHENTICATEDIALOG_H

#include <QDialog>
#include "ui_authenticatedialog.h"

class authenticateDialog : public QDialog,public Ui::authenticateDialog
{
    Q_OBJECT
public:
    authenticateDialog(QWidget *parent = 0);
};

#endif // AUTHENTICATEDIALOG_H
