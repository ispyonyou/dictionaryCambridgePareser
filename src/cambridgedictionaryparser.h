#ifndef CAMBRIDGEDICTIONARYPARSER_H
#define CAMBRIDGEDICTIONARYPARSER_H

#include <QObject>
#include "ContentProviders/wordsdata.h"
#include "ContentProviders/sencesdata.h"

struct CambridgeDictSenseInfo
{
    QString defenition;
    QString translation;
    QList< QString > examples;

    std::shared_ptr< SensesData > toSensesData() const;
};

struct CambridgeDictWordInfo
{
    QString word;
    QString transcription;
    QString html;
    QByteArray audio;
    QList< CambridgeDictSenseInfo > senses;

    std::shared_ptr< WordsData > toWordsData() const;
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
