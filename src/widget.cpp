#include "widget.h"
#include "ui_widget.h"
#include "settingsdialog.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QBuffer>
#include <QAudioRecorder>
#include <QAudioProbe>
#include <QAudioDecoder>
#include <QFile>
#include <QProcess>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QNetworkProxyFactory>
#include <QSqlRecord>
#include <QSqlError>
#include "CambridgeDictionaryParser.h"
#include "dbutils.h"
#include "ContentProviders/wordscontentprovider.h"
#include "ContentProviders/sensescontentprovider.h"
#include "ContentProviders/examplescontentprovider.h"

extern "C" {
  #include "third-party/mp3wrap/mp3wrap.h"
}

#include "wordsmodel.h"

class MainWidgetPrivate
{
public:
    CambridgeDictionaryParser* cambridgeDictParser;
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , d( *new MainWidgetPrivate() )
    , ui(new Ui::Widget)
{
    d.cambridgeDictParser = new CambridgeDictionaryParser( this );

    ui->setupUi(this);

    connect( ui->getItBtn, SIGNAL(clicked()), this, SLOT(getItClicked()));
    connect( ui->settingsBtn, SIGNAL(clicked()), this, SLOT(settingsClicked()));
    connect( ui->addWordBtn, SIGNAL(clicked()), this, SLOT(addWordClicked()));

    if( !createConnection() ){
        QMessageBox::critical( 0, qApp->tr( "Cannot open database" ),
            qApp->tr( "Unable to establish a database connection.\n"
                      "Click Cancel to exit." ), QMessageBox::Cancel );
        qApp->quit();
    }

    wordsModel = new WordsModel();

    QList< std::shared_ptr< WordsData > > words;

    WordsContentProvider wordsProvider;
    wordsProvider.loadWords( words );

    Q_FOREACH( std::shared_ptr< WordsData > word, words ) {
        wordsModel->appendWord( word );
    }

    ui->tableView->setModel( wordsModel );
}

Widget::~Widget()
{
    delete ui;
    delete &d;
}

void Widget::getItClicked()
{
    QDate curDate = QDate::currentDate();
    QTime curTime = QTime::currentTime();

    QString curTimeStr = QString( "%1_%2_%3_%4_%5_%6" ).arg( curDate.year() )
                                                       .arg( curDate.month() )
                                                       .arg( curDate.day() )
                                                       .arg( curTime.hour() )
                                                       .arg( curTime.minute() )
                                                       .arg( curTime.second() );

    QStringList mp3wrapArgs;
    mp3wrapArgs.append( "album_" + curTimeStr + ".mp3" );

    WordsContentProvider wordsProvider;

    for( int i = 0; i < wordsModel->rowCount( QModelIndex() ); i++ )
    {
        std::shared_ptr< WordsData > wordData = wordsModel->getWordData( i );

        QByteArray audio;
        wordsProvider.loadAudio( wordData->id, audio );

        QString fileName = wordData->word + ".mp3" ;
        QFile outFile(  fileName );
        outFile.open( QFile::WriteOnly );
        outFile.write( audio );

        for( long i = 0; i < 3; i++ )
        {
            mp3wrapArgs.append( fileName );
            mp3wrapArgs.append( "nullSound.mp3" );
        }
    }

    char** args = new char*[ mp3wrapArgs.size() + 1 ];
    args[0] = new char[ 1 ];
    args[0][0] = '\0';

    for( long i = 0; i < mp3wrapArgs.size(); i++ )
    {
        args[i+1] = new char[ mp3wrapArgs[ i ].length() + 1 ];
        strcpy_s( args[i+1], mp3wrapArgs[ i ].length() + 1, mp3wrapArgs[ i ].toLatin1() );
        args[i+1][mp3wrapArgs[ i ].length()] = '\0';
    }

    mp3wrapp( mp3wrapArgs.size() + 1, args );

    QString html;

    html += "<!DOCTYPE html>"
            "<html lang=\"en\">"
            "<head>"
            "  <meta charset=\"utf-8\" /> "
            "  <style>"
            "    .wordArea {"
            "    }"
            "    .word {"""
            "      font: 22pt sans-serif; "
            "    }"
            "    .transcrypt{"
            "      font: 18pt monospace; "
            "    }"
            "    .senses{"
            "      position: relative; "
            "      left: 20px;"
            "      font: 14pt sans-serif; "
            "    }"
            "  </style> "
            "</head>"
            "<body>";

    SensesContentProvider sensesProvider;
    ExamplesContentProvider examplesProvider;

    for( int i = 0; i < wordsModel->rowCount( QModelIndex() ); i++ )
    {
        std::shared_ptr< WordsData > wordData = wordsModel->getWordData( i );

        QString header =  "<div class=\"wordArea\">"
                            "<div>"
                              "<span class=\"word\"> %1 </span><span class=\"transcrypt\">/%2/</span>"
                            "</div>"
                          "</div>";

        html += header.arg( wordData->word )
                      .arg( wordData->transcription );

        QList< std::shared_ptr< SensesData > > senses;
        sensesProvider.loadSenses( wordData->id, senses );

        html += "<div class=\"senses\">";

        Q_FOREACH( std::shared_ptr< SensesData > sense, senses )
        {
            QString senseHtml = "<li>%1<br/>%2</li>";

            html += senseHtml.arg( sense->defenition )
                             .arg( sense->translation );
        }

        html += "</div>";
    }

    html += "</body>"
            "</html>";

    QFile htmlFile( "html_" + curTimeStr + ".html" );
    htmlFile.open( QFile::WriteOnly );

    htmlFile.write( html.toUtf8() );
}

void Widget::settingsClicked()
{
    SettingsDialog* settingsDlg = new SettingsDialog();
    settingsDlg->exec();

    settingsDlg->deleteLater();
}

void Widget::addWordClicked()
{
    QString word = ui->addWordEdit->text();

    CambridgeDictWordInfo wordInfo;
    if( !d.cambridgeDictParser->loadWordInfo( word, wordInfo ) )
        return;

    std::shared_ptr< WordsData > wordData = wordInfo.toWordsData();

    WordsContentProvider wordsProvider;
    wordsProvider.insertWord( wordData );

    SensesContentProvider sensesProvider;
    ExamplesContentProvider examplesProvider;

    Q_FOREACH( const CambridgeDictSenseInfo& senseInfo, wordInfo.senses )
    {
        std::shared_ptr< SensesData > senseData = senseInfo.toSensesData();
        senseData->wordId = wordData->id;
        sensesProvider.insertSense( senseData );

        Q_FOREACH( const QString& example, senseInfo.examples )
        {
            std::shared_ptr< ExamplesData > exampleData( new ExamplesData );
            exampleData->senseId = senseData->id;
            exampleData->text    = example;
            examplesProvider.insertExample( exampleData );
        }
    }

    wordsModel->appendWord( wordData );
}
