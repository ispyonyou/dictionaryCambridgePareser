#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class About;
}

class AboutDialog : public QDialog
{
public:
    AboutDialog( QWidget* parent = 0 );
    ~AboutDialog();

private:
    Ui::About* ui;
};

#endif // ABOUTDIALOG_H
