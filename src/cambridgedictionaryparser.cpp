#include "cambridgedictionaryparser.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>

#define CAMBRIDGE_DICT_BASE_URL "http://dictionary.cambridge.org/ru/%D1%81%D0%BB%D0%BE%D0%B2%D0%B0%D1%80%D1%8C/%D0%B0%D0%BD%D0%B3%D0%BB%D0%BE-%D1%80%D1%83%D1%81%D1%81%D0%BA%D0%B8%D0%B9/"

std::shared_ptr< WordsData > CambridgeDictWordInfo::toWordsData()
{
    std::shared_ptr< WordsData > data( new WordsData() );
    data->word = word;
    data->transcription = transcription;
    data->audio = audio;
    data->isLearned = false;

    return data;
}

class CambridgeDictionaryParserPrivate
{
public:
    bool loadAudio( const QUrl& url, QByteArray& audioData );

public:
    QNetworkAccessManager* networkMng;
};

CambridgeDictionaryParser::CambridgeDictionaryParser( QObject* parent )
    : QObject( parent )
    , d( *new CambridgeDictionaryParserPrivate() )
{
    d.networkMng = new QNetworkAccessManager( this );
}

CambridgeDictionaryParser::~CambridgeDictionaryParser()
{
    delete &d;
}

bool CambridgeDictionaryParser::loadWordInfo( const QString& word, CambridgeDictWordInfo& wordInfo )
{
    QUrl url = CAMBRIDGE_DICT_BASE_URL + word;

    QWebPage page;
    page.mainFrame()->load( url );

    QEventLoop loop;
    QObject::connect( page.mainFrame(), SIGNAL(loadFinished(bool)), &loop, SLOT(quit()) );
    loop.exec();

    QWebElement soundElement = page.mainFrame()->findFirstElement( "div[class=\"sound audio_play_button pron-us\"]" );

    Q_ASSERT( soundElement.hasAttribute( "data-src-mp3" ) );
    if( !soundElement.hasAttribute( "data-src-mp3" ) )
        return false;

    wordInfo.word = word;

    QUrl audioUrl = soundElement.attribute( "data-src-mp3" );
    if( !d.loadAudio( audioUrl, wordInfo.audio ) )
        return false;

    QWebElement transcryptionElement = page.mainFrame()->findFirstElement( "span[class=\"ipa\"]" );
    wordInfo.transcription = transcryptionElement.toPlainText();

    QWebElementCollection senseBlocks = page.mainFrame()->findAllElements( "div[class=\"sense-block\"]" );

    Q_FOREACH( QWebElement senseBlock, senseBlocks ) {
        CambridgeDictSenseInfo senseInfo;

        QWebElement def = senseBlock.findFirst( "span[class=\"def\"]" );
        senseInfo.defenition = def.toPlainText();

        QWebElement trans = senseBlock.findFirst( "div[class=\"trans\"]" );
        senseInfo.translation = trans.toPlainText();

        QWebElementCollection examples = senseBlock.findAll( "span[class=\"examp\"]" );
        Q_FOREACH( QWebElement example, examples ) {
            senseInfo.examples.append( example.toPlainText() );
        }

        wordInfo.senses.append( senseInfo );
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool CambridgeDictionaryParserPrivate::loadAudio( const QUrl& url, QByteArray& audioData )
{
    QNetworkReply* reply = networkMng->get( QNetworkRequest( url ) );

    QEventLoop loop;
    QObject::connect( reply, SIGNAL(finished()), &loop, SLOT(quit()) );
    loop.exec();

    reply->deleteLater();

    Q_ASSERT( reply->error() == QNetworkReply::NoError );
    if( QNetworkReply::NoError != reply->error() )
        return false;

    audioData = reply->readAll();
    return true;
}

