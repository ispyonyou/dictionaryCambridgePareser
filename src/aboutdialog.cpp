#include "aboutdialog.h"
#include "ui_about.h"

AboutDialog::AboutDialog( QWidget* parent )
    : QDialog( parent )
    , ui( new Ui::About )
{
    ui->setupUi( this );
    ui->cambridgeLinkLabel->setOpenExternalLinks(true);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
