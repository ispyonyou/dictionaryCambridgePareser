#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class Settings;
}

class SettingsManager;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog( QWidget* parent = nullptr );
    ~SettingsDialog();

public Q_SLOTS:
    void chooseWorkingDir();
    void storeSettings();

private:
    Ui::Settings* ui;
    SettingsManager* settings;
};

#endif // SETTINGSDIALOG_H
