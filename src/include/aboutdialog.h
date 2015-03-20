

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H
#include "ui_about.h"


// AboutForm is the class that shows the about dialog (Help Menu)
// the about dialog is designed with Qt designer (about.ui)

class CAboutForm : public QDialog, public Ui::AboutForm
{
 Q_OBJECT
 public:
 CAboutForm(QWidget* =0);

};
#endif

