#include "settingsdialog.h"
#include "settingsmanager.h"
#include "ui_settings.h"
#include <QFileDialog>

#define SETTINGS_WORKING_DIR   "workingDirectory"
#define SETTINGS_REPEATS_COUNT "repeatsCount"
#define SETTINGS_GEN_LEARNED   "generateInfoForLearned"

SettingsDialog::SettingsDialog( QWidget* parent )
    : QDialog( parent )
    , ui( new Ui::Settings )
{
    ui->setupUi(this);
    settings = new SettingsManager( this );

    ui->workingDirEdit->setText( settings->workingDir() );
    ui->chooseOutHtmlFileEdit->setText( settings->outHtmlFile() );
    ui->repeatsCountSpin->setValue( settings->repeatsCount() );
    ui->genLearnedChBox->setChecked( settings->needGenerateLearned() );
    ui->showLearnedChB->setChecked( settings->needShowLearned() );

    connect( ui->chooseWorkingDirBtn, SIGNAL(clicked()), this, SLOT(chooseWorkingDir()));
    connect( ui->chooseOutHtmlFileBtn, SIGNAL(clicked()), this, SLOT(chooseOutHtmlFile()));
    connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(storeSettings()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::chooseWorkingDir()
{
    QString dir = QFileDialog::getExistingDirectory( this, tr("Choose working directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    if( dir.isEmpty() )
        return;

    ui->workingDirEdit->setText( dir );
}

void SettingsDialog::chooseOutHtmlFile()
{
    QString filePath = QFileDialog::getSaveFileName( this, tr("Choose out html file") );
    if( filePath.isEmpty() )
        return;

    ui->chooseOutHtmlFileEdit->setText( filePath );
}

void SettingsDialog::storeSettings()
{
    settings->setWorkingDir( ui->workingDirEdit->text() );
    settings->setOutHtmlFile( ui->chooseOutHtmlFileEdit->text() );
    settings->setRepeatsCount( ui->repeatsCountSpin->value() );
    settings->setNeedGenerateLearned( ui->genLearnedChBox->isChecked() );
    settings->setNeedShowLearned( ui->showLearnedChB->isChecked() );
}
