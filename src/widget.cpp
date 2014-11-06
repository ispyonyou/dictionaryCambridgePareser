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
