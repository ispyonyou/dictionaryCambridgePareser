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
#include "mp3wrap.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>

class MainWidgetPrivate
{
public:
    ~MainWidgetPrivate()
    {
        Q_FOREACH( int id, cachedAudio.keys() )
            delete cachedAudio[ id ];

        delete ui;
    }

public:
    CambridgeDictionaryParser* cambridgeDictParser;
    WordsModel* wordsModel;
    QMediaPlayer* mediaPlayer;
    QMap< int, QByteArray* > cachedAudio;
    Ui::MainWindow *ui;
};

Widget::Widget(QWidget *parent)
    : QMainWindow( parent )
    , d( *new MainWidgetPrivate() )
{
    d.ui = new Ui::MainWindow;
    d.cambridgeDictParser = new CambridgeDictionaryParser( this );
    d.mediaPlayer = new QMediaPlayer( this );

    d.ui->setupUi(this);

    connect( d.ui->getItBtn, SIGNAL(clicked()), this, SLOT(getItClicked()) );
    connect( d.ui->settingsBtn, SIGNAL(clicked()), this, SLOT(settingsClicked()) );
    connect( d.ui->addWordBtn, SIGNAL(clicked()), this, SLOT(addWordClicked()) );
    connect( d.ui->playBtn, SIGNAL(clicked()), this, SLOT(playClicked()) );

    if( !createConnection() ){
        QMessageBox::critical( 0, qApp->tr( "Cannot open database" ),
            qApp->tr( "Unable to establish a database connection.\n"
                      "Click Cancel to exit." ), QMessageBox::Cancel );
        qApp->quit();
    }

    d.wordsModel = new WordsModel();

    QList< std::shared_ptr< WordsData > > words;

    WordsContentProvider wordsProvider;
    wordsProvider.loadWords( words );

    Q_FOREACH( std::shared_ptr< WordsData > word, words ) {
        d.wordsModel->appendWord( word );
    }

    d.ui->tableView->setModel( d.wordsModel );

    connect( d.ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(wordsTableSelectionChanged()) );

    d.ui->webView->setUrl( QUrl( QStringLiteral( "about:blank" ) ) );
}

Widget::~Widget()
{
    delete &d;
}

int Widget::currentRow()
{
    QModelIndexList selIndexes = d.ui->tableView->selectionModel()->selection().indexes();
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

    Mp3Wrap wrap;
    wrap.setOutFile( "album_" + curTimeStr + ".mp3" );

    WordsContentProvider wordsProvider;

    QFile nullSoundFile( ":/res/nullSound.mp3" );
    if( !nullSoundFile.open( QFile::ReadOnly ) ) {
        qDebug() << "failed to open res/nullSound.mp3";
        return;
    }

    QByteArray nullSound = nullSoundFile.readAll();

    wrap.addSource( nullSound );

    for( int i = 0; i < d.wordsModel->rowCount( QModelIndex() ); i++ ) {
        std::shared_ptr< WordsData > wordData = d.wordsModel->getWordData( i );

        QByteArray audio;
        wordsProvider.loadAudio( wordData->id, audio );

        for( long i = 0; i < 3; i++ )
        {
            wrap.addSource( audio );
            wrap.addSource( nullSound );
        }
    }

    wrap.doWrap();

    QList< std::shared_ptr< WordsData > > words;
    for( int i = 0; i < d.wordsModel->rowCount( QModelIndex() ); i++ ) {
        words.append( d.wordsModel->getWordData( i ) );
    }

    QString html = wordsProvider.generateHtml( words );

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
    QString word = d.ui->addWordEdit->text();

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

    d.wordsModel->appendWord( wordData );
}

void Widget::playClicked()
{
    int row = currentRow();
    if( -1 == row )
        return;

    WordsContentProvider wordsProvider;
    int selWordId = d.wordsModel->getWordData( row )->id;

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

    std::shared_ptr< WordsData > word = d.wordsModel->getWordData( row );

    WordsContentProvider wordsProvider;
    QString html = wordsProvider.generateHtml( word );

    d.ui->webView->setHtml( html );
}
