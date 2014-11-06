#ifndef CAMBRIDGEDICTIONARYPARSER_H
#define CAMBRIDGEDICTIONARYPARSER_H

#include <QObject>

struct CambridgeDictSenseInfo
{
    QString defenition;
    QString translation;
    QList< QString > examples;
};

struct CambridgeDictWordInfo
{
    QString word;
    QString transcription;
    QString html;
    QByteArray audio;

    QList< CambridgeDictSenseInfo > senses;
};

class CambridgeDictionaryParserPrivate;

class CambridgeDictionaryParser : public QObject
{
public:
    CambridgeDictionaryParser( QObject* parent = nullptr );
    ~CambridgeDictionaryParser();

    bool loadWordInfo( const QString& word, CambridgeDictWordInfo& wordInfo );

private:
    CambridgeDictionaryParserPrivate& d;
};

#endif // CAMBRIDGEDICTIONARYPARSER_H
