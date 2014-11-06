#include "dbutils.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QApplication>

bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );
    db.setDatabaseName( "engwordsdb" );
    if( !db.open() )
        return false;

    return initDatabase();
}

bool initDatabase()
{
    QSqlQuery query;
    query.exec( "select 1 from sqlite_master where type='table' and name='words'" );

    if( query.next() )
        return true;

    if( !query.exec( "create table words (id            INTEGER PRIMARY KEY NOT NULL UNIQUE, "
                                         "word          VARCHAR2(256) UNIQUE,"
                                         "transcription VARCHAR2(256),"
                                         "audio         BLOB, "
                                         "is_learned    INT)" ) )
    {
        return false;
    }

    if( !query.exec( "create table senses (id            INTEGER PRIMARY KEY NOT NULL UNIQUE,"
                                          "word_id       INTEGER NOT NULL,"
                                          "defenition    VARCHAR2(2048),"
                                          "translation   VARCHAR2(2048))" ) )
    {
        return false;
    }

    if( !query.exec( "create table examples (id            INTEGER PRIMARY KEY NOT NULL UNIQUE,"
                                            "sense_id      INTEGER NOT NULL,"
                                            "text          VARCHAR2(3072))" ) )
    {
        return false;
    }

    return true;
}
