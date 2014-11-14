#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>

class QSettings;

class SettingsManager : public QObject
{
public:
    SettingsManager( QObject* parent = 0 );

    QString workingDir();
    void setWorkingDir( const QString& val );

    int repeatsCount();
    void setRepeatsCount( int val );

    bool needGenerateLearned();
    void setNeedGenerateLearned( bool val );

    bool needShowLearned();
    void setNeedShowLearned( bool val );

private:
    QSettings* settings;
};

#endif // SETTINGSMANAGER_H
