#include "settingsdialog.h"
#include "ui_settings.h"
#include <QFileDialog>
#include <QSettings>

#define SETTINGS_WORKING_DIR   "workingDirectory"
#define SETTINGS_REPEATS_COUNT "repeatsCount"

SettingsDialog::SettingsDialog( QWidget* parent )
    : QDialog( parent )
    , ui( new Ui::Settings )

{
    ui->setupUi(this);
    settings = new QSettings( QSettings::IniFormat, QSettings::UserScope, QCoreApplication::instance()->organizationName(), QCoreApplication::instance()->applicationName(), this );

    ui->workingDirEdit->setText( settings->value( SETTINGS_WORKING_DIR ).toString() );
    ui->repeatsCountSpin->setValue( settings->value( SETTINGS_REPEATS_COUNT, 3 ).toInt() );

    connect( ui->chooseWorkingDirBtn, SIGNAL(clicked()), this, SLOT(chooseWorkingDir()));
    connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(storeSettings()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::chooseWorkingDir()
{
    QString dir = QFileDialog::getExistingDirectory( this, tr("Choose working directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->workingDirEdit->setText( dir );
}

void SettingsDialog::storeSettings()
{
    settings->setValue( SETTINGS_WORKING_DIR  , ui->workingDirEdit->text() );
    settings->setValue( SETTINGS_REPEATS_COUNT, ui->repeatsCountSpin->value() );
}
