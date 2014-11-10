#ifndef MP3WRAP_H
#define MP3WRAP_H

#include <QObject>
#include <QString>

class Mp3WrapPrivate;

class Mp3Wrap : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString outFile READ outFile WRITE setOutFile )

public:
    Mp3Wrap( QObject* parent = 0 );
    ~Mp3Wrap();

public:
    QString outFile() const;
    void setOutFile( const QString& val );

    void addSource( const QByteArray& sourceData );

    void doWrap();

private:
    Mp3WrapPrivate& d;
};

#endif // MP3WRAP_H
