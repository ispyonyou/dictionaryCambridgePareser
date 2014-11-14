#include "settingsmanager.h"

#include <QCoreApplication>
#include <QSettings>

#define SETTINGS_WORKING_DIR   "workingDirectory"
#define SETTINGS_REPEATS_COUNT "repeatsCount"
#define SETTINGS_GEN_LEARNED   "generateInfoForLearned"

SettingsManager::SettingsManager( QObject* parent )
    : QObject( parent )
{
    settings = new QSettings( QSettings::IniFormat, QSettings::UserScope, QCoreApplication::instance()->organizationName(), QCoreApplication::instance()->applicationName(), this );
}

QString SettingsManager::workingDir()
{
    return settings->value( SETTINGS_WORKING_DIR,  "" ).toString();
}

void SettingsManager::setWorkingDir( const QString& val )
{
    settings->setValue( SETTINGS_WORKING_DIR, val );
}

int SettingsManager::repeatsCount()
{
    return settings->value( SETTINGS_REPEATS_COUNT, 3 ).toInt();
}

void SettingsManager::setRepeatsCount( int val )
{
     settings->setValue( SETTINGS_REPEATS_COUNT, val );
}

bool SettingsManager::needGenerateLearned()
{
    return settings->value( SETTINGS_GEN_LEARNED, false ).toBool();
}

void SettingsManager::setNeedGenerateLearned( bool val )
{
    settings->setValue( SETTINGS_GEN_LEARNED, val );
}
