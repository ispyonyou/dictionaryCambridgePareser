#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "settingsmanager.h"
#include "aboutdialog.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QNetworkProxyFactory>
#include <QSqlRecord>
#include <QSqlError>
#include "cambridgedictionaryparser.h"
#include "dbutils.h"
#include "ContentProviders/wordscontentprovider.h"
#include "ContentProviders/sensescontentprovider.h"
#include "ContentProviders/examplescontentprovider.h"
#include "wordsmodel.h"
#include "mp3wrap.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QToolBar>
#include <QSortFilterProxyModel>
#include <QHeaderView>

class MainWindowPrivate
{
public:
    ~MainWindowPrivate()
    {
        Q_FOREACH( int id, cachedAudio.keys() )
            delete cachedAudio[ id ];

        delete ui;
    }

    int currentRow()
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

    std::shared_ptr< WordsData > currentRowData()
    {
        int wordId = proxyModel->data( proxyModel->index( currentRow(), 0 ), WordsModel::WordIdRole ).toInt();
        return wordsModel->getWordDataById( wordId );
    }

public:
    CambridgeDictionaryParser* cambridgeDictParser;
    WordsModel* wordsModel;
    WordsSortFilterProxyModel* proxyModel;
    QMediaPlayer* mediaPlayer;
    QMap< int, QByteArray* > cachedAudio;
    Ui::MainWindow *ui;
    QAction* actionPlay;
    QAction* actionRemove;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow( parent )
    , d( *new MainWindowPrivate() )
{
    d.ui = new Ui::MainWindow;
    d.cambridgeDictParser = new CambridgeDictionaryParser( this );
    d.mediaPlayer = new QMediaPlayer( this );

    d.ui->setupUi(this);

    connect( d.ui->addWordEdit, &QLineEdit::textChanged, [=]( const QString& text ){
        d.ui->addWordBtn->setEnabled( !text.isEmpty() );
    } );
    connect( d.ui->addWordEdit, &QLineEdit::returnPressed, [=](){
        addWord( d.ui->addWordEdit->text() );
    } );
    connect( d.ui->addWordBtn, &QToolButton::clicked, [=]( bool ){
        addWord( d.ui->addWordEdit->text() );
    } );

    d.ui->addWordBtn->setEnabled( !d.ui->addWordEdit->text().isEmpty() );

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

    d.proxyModel = new WordsSortFilterProxyModel( this );
    d.proxyModel->setNeedShowLearned( SettingsManager().needShowLearned() );
    d.proxyModel->setSourceModel( d.wordsModel );

    d.ui->tableView->setModel( d.proxyModel );

    d.ui->tableView->setSortingEnabled( true );
    d.ui->tableView->horizontalHeader()->setSortIndicator( Hid_WordModel_Word, Qt::AscendingOrder );

    connect( d.ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(wordsTableSelectionChanged()) );

    connect( d.ui->addWordEdit, &QLineEdit::textChanged, [=]( const QString& text ){
        d.proxyModel->setWordFilter( text );
    } );

    d.ui->webView->setUrl( QUrl( QStringLiteral( "about:blank" ) ) );

    d.actionPlay = new QAction( QIcon(":res/icons/play.ico"), tr( "&Play" ), this );
    connect( d.actionPlay, SIGNAL(triggered()), this, SLOT(playCurrentWord()) );

    QAction* actionGenerate = new QAction( QIcon(":res/icons/generate.ico"), tr( "&Generate mp3 and html" ), this );
    connect( actionGenerate, SIGNAL(triggered()), this, SLOT(generateMp3andHtml()) );

    QAction* actionSettings = new QAction( QIcon(":res/icons/settings.ico"), tr( "&Settings" ), this );
    connect( actionSettings, SIGNAL(triggered()), this, SLOT(showSettings()) );

    d.actionRemove = new QAction( QIcon(":res/icons/remove.ico"), tr( "&Remove" ), this );
    connect( d.actionRemove, SIGNAL(triggered()), this, SLOT(removeCurrentWord()) );

    QToolBar* mainToolBar = addToolBar( "Main" );
    mainToolBar->addAction( d.actionPlay );
    mainToolBar->addAction( d.actionRemove );
    mainToolBar->addAction( actionGenerate );
    mainToolBar->addAction( actionSettings );

    d.actionPlay->setEnabled( d.currentRow() != -1 );
    d.actionRemove->setEnabled( d.currentRow() != -1 );

    connect( d.ui->action_About, &QAction::triggered, []( bool ){ AboutDialog().exec(); } );

    connect( d.ui->tableView, &QTableView::doubleClicked, [=]( const QModelIndex& /*index*/ ) {
        playCurrentWord();
//        int wordId = proxyModel->data( index, WordsModel::WordIdRole ).toInt();
//        return wordsModel->getWordDataById( wordId );
    } );
}

MainWindow::~MainWindow()
{
    delete &d;
}

void MainWindow::generateMp3andHtml()
{
    SettingsManager settings;

    QDate curDate = QDate::currentDate();
    QTime curTime = QTime::currentTime();

    QString curTimeStr = QString( "%1_%2_%3_%4_%5_%6" ).arg( curDate.year() )
                                                       .arg( curDate.month() )
                                                       .arg( curDate.day() )
                                                       .arg( curTime.hour() )
                                                       .arg( curTime.minute() )
                                                       .arg( curTime.second() );

    Mp3Wrap wrap;
    wrap.setOutFile( settings.workingDir() + QDir::separator() + "album_" + curTimeStr + ".mp3" );

    WordsContentProvider wordsProvider;

    QFile nullSoundFile( ":/res/nullSound.mp3" );
    if( !nullSoundFile.open( QFile::ReadOnly ) ) {
        qDebug() << "failed to open res/nullSound.mp3";
        return;
    }

    QByteArray nullSound = nullSoundFile.readAll();

    wrap.addSource( nullSound );

    WordsSortFilterProxyModel proxy;
    proxy.setNeedShowLearned( SettingsManager().needGenerateLearned() );
    proxy.setSourceModel( d.wordsModel );

    proxy.sort( Hid_WordModel_Word );

    QList< std::shared_ptr< WordsData > > words;
    for( int i = 0; i < proxy.rowCount( QModelIndex() ); i++ )
    {
        int wordId = proxy.data( proxy.index( i, 0 ), WordsModel::WordIdRole ).toInt();
        std::shared_ptr< WordsData > wordData =  d.wordsModel->getWordDataById( wordId );

        if( !settings.needGenerateLearned() && wordData->isLearned )
            continue;

        words.append( wordData );
    }

    Q_FOREACH( std::shared_ptr< WordsData > wordData, words )
    {
        QByteArray audio;
        wordsProvider.loadAudio( wordData->id, audio );

        for( long i = 0; i < 3; i++ )
        {
            wrap.addSource( audio );
            wrap.addSource( nullSound );
        }
    }

    wrap.doWrap();

    QString html = wordsProvider.generateHtml( words, HtmlDest_Out );

    QFile htmlFile( settings.outHtmlFile() );
    htmlFile.open( QFile::WriteOnly );

    htmlFile.write( html.toUtf8() );
}

void MainWindow::showSettings()
{
    SettingsDialog* settingsDlg = new SettingsDialog();
    settingsDlg->exec();

    settingsDlg->deleteLater();

    d.proxyModel->setNeedShowLearned( SettingsManager().needShowLearned() );
}

void MainWindow::addWord( const QString& wordSource )
{
    QString word = wordSource.toLower().trimmed();

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

    int row = d.wordsModel->appendWord( wordData );
    d.ui->tableView->selectRow( row );

    playCurrentWord();
}

void MainWindow::playCurrentWord()
{
    if( -1 == d.currentRow() )
        return;

    WordsContentProvider wordsProvider;
    int selWordId = d.currentRowData()->id;

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

void MainWindow::removeCurrentWord()
{
}

void MainWindow::wordsTableSelectionChanged()
{
    d.actionPlay->setEnabled( d.currentRow() != -1 );
    d.actionRemove->setEnabled( d.currentRow() != -1 );

    if( -1 == d.currentRow() )
        return;

    std::shared_ptr< WordsData > word = d.currentRowData();

    WordsContentProvider wordsProvider;
    QString html = wordsProvider.generateHtml( word, HtmlDest_View );

    d.ui->webView->setHtml( html );
}
