#include "widget.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QBuffer>
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
#include "wordsmodel.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>

extern "C" {
  #include "third-party/mp3wrap/mp3wrap.h"
}

class MainWidgetPrivate
{
public:
    ~MainWidgetPrivate()
    {
        Q_FOREACH( int id, cachedAudio.keys() )
            delete cachedAudio[ id ];
    }

public:
    CambridgeDictionaryParser* cambridgeDictParser;
    QMediaPlayer* mediaPlayer;
    QMap< int, QByteArray* > cachedAudio;
};

Widget::Widget(QWidget *parent)
    : QMainWindow( parent )
    , ui( new Ui::MainWindow )
    , d( *new MainWidgetPrivate() )
{
    d.cambridgeDictParser = new CambridgeDictionaryParser( this );
    d.mediaPlayer = new QMediaPlayer( this );

    ui->setupUi(this);

    connect( ui->getItBtn, SIGNAL(clicked()), this, SLOT(getItClicked()) );
    connect( ui->settingsBtn, SIGNAL(clicked()), this, SLOT(settingsClicked()) );
    connect( ui->addWordBtn, SIGNAL(clicked()), this, SLOT(addWordClicked()) );
    connect( ui->playBtn, SIGNAL(clicked()), this, SLOT(playClicked()) );

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

    connect( ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(wordsTableSelectionChanged()) );

    ui->webView->setUrl( QUrl( QStringLiteral( "about:blank" ) ) );
}

Widget::~Widget()
{
    delete ui;
    delete &d;
}

int Widget::currentRow()
{
    QModelIndexList selIndexes = ui->tableView->selectionModel()->selection().indexes();
    QSet< int > selRows;
    Q_FOREACH( const QModelIndex& index, selIndexes )
    {
        selRows.insert( index.row() );
    }

    if( selRows.empty() )
        return -1;

    return *selRows.begin();
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

void Widget::playClicked()
{
    QModelIndexList selIndexes = ui->tableView->selectionModel()->selection().indexes();
    QSet< int > selRows;
    Q_FOREACH( const QModelIndex& index, selIndexes )
    {
        selRows.insert( index.row() );
    }

    if( selRows.empty() )
        return;

    WordsContentProvider wordsProvider;

    int row = *selRows.begin();
    int selWordId = wordsModel->getWordData( row )->id;

    if( d.cachedAudio.end() == d.cachedAudio.find( selWordId ) )
    {
        QByteArray* audio = new QByteArray();
        wordsProvider.loadAudio( selWordId, *audio );
        d.cachedAudio[ selWordId ] = audio;
    }

    QBuffer* buffer = new QBuffer( d.cachedAudio[ selWordId ], this );
    buffer->open( QIODevice::ReadOnly );

    d.mediaPlayer->setMedia( QMediaContent(), buffer );

    d.mediaPlayer->setVolume( 100 );
    d.mediaPlayer->play();
}

void Widget::wordsTableSelectionChanged()
{
    int row = currentRow();
    if( -1 == row )
        return;

    std::shared_ptr< WordsData > word = wordsModel->getWordData( row );

    WordsContentProvider wordsProvider;
    QString html = wordsProvider.generateHtml( word );

    ui->webView->setHtml( html );
}
