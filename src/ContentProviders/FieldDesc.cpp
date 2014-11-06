#include "FieldDesc.h"
#include <QDebug>
#include <QList>
#include <QVariant>
#include <QJsonDocument>
#include <QSqlError>
//#include "SqlConnectionsManager.h"
//#include "SqlQuery.h"

FieldDescItem::FieldDescItem( const QString& col, int& buf )
{
    Column = col;
    Type = Int;
    Buffer.IntValue = &buf;
    StrLength = 0;
    IsPk = false;
}

FieldDescItem::FieldDescItem( const QString& col, QString& buf,int length )
{
    Column = col;
    Type = String;
    Buffer.StringValue = &buf;
    StrLength = length;
    IsPk = false;
}

FieldDescItem::FieldDescItem( const QString& col, QJsonObject& buf, int length )
{
    Column = col;
    Type = JsonObj;
    Buffer.JsonObjValue = &buf;
    StrLength = length;
    IsPk = false;
}

FieldDescItem::FieldDescItem( const QString& col, QByteArray& buf )
{
    Column = col;
    Type = ByteArray;
    Buffer.ByteArrayValue = &buf;
    StrLength = 0;
    IsPk = false;
}

FieldDescItem::FieldDescItem( const QString& col, bool& buf )
{
    Column = col;
    Type = Bool;
    Buffer.BoolValue = &buf;
    StrLength = 0;
    IsPk = false;
}

FieldDescItem& FieldDescItem::setPkFlag()
{
    IsPk = true;
    return *this;
}

void FieldDescItem::bindValue( QSqlQuery& query )
{
    if( isNull() )
    {
        query.bindValue( ":" + Column, QVariant() );
        return;
    }

    if( Int == Type )
        query.bindValue( ":" + Column, *Buffer.IntValue );
    else if( String == Type )
        query.bindValue( ":" + Column, *Buffer.StringValue );
    else if( JsonObj == Type )
        query.bindValue( ":" + Column, QString::fromUtf8( QJsonDocument( *Buffer.JsonObjValue ).toJson() ) );
    else if( ByteArray == Type )
        query.bindValue( ":" + Column, *Buffer.ByteArrayValue );
    else if( Bool == Type )
        query.bindValue( ":" + Column, *Buffer.BoolValue );
    else
        Q_ASSERT( false );
}

void FieldDescItem::fetch( QSqlQuery& query, const QSqlRecord& rec )
{
    if( Int == Type ) {
        *Buffer.IntValue = query.value( rec.indexOf( Column ) ).toInt();
    }
    else if( String == Type ) {
        *Buffer.StringValue = query.value( rec.indexOf( Column ) ).toString();
    }
    else if( JsonObj == Type ) {
        QString jsonStr = query.value( rec.indexOf( Column ) ).toString();

        QJsonParseError error;
        *Buffer.JsonObjValue = QJsonDocument::fromJson( jsonStr.toUtf8(), &error ).object();
    }
    else if( ByteArray == Type ) {
        *Buffer.ByteArrayValue = query.value( rec.indexOf( Column ) ).toByteArray();
    }
    else if( Bool == Type ) {
        *Buffer.BoolValue = query.value( rec.indexOf( Column ) ).toBool();
    }
    else {
        Q_ASSERT( false );
    }
}

void* FieldDescItem::buffValue() const
{
    if( Int == Type ) {
        return Buffer.IntValue;
    }
    else if( String == Type ) {
        return Buffer.StringValue;
    }
    else if( JsonObj == Type ) {
        return Buffer.JsonObjValue;
    }
    else if( ByteArray == Type ) {
        return Buffer.ByteArrayValue;
    }
    else if( Bool == Type ) {
        return Buffer.BoolValue;
    }
    else {
        Q_ASSERT( false );
    }

    return nullptr;
}

bool FieldDescItem::isNull() const
{
    if( Int == Type && *Buffer.IntValue == 0 )
        return true;

    return false;
}

//QString FieldDescItem::dbType()
//{
//    if( Int == Type )
//        return "INTEGER";
//    else if( String == Type || JsonObj == Type )
//        return QString( "VARCHAR(%1)" ).arg( StrLength );
//    else if( ByteArray == Type )
//        return "BLOB";
//
//    Q_ASSERT( false );
//    return "";
//}

///////////////////////////////////////////////////////////////////////////////
QList< FieldDescItem > FieldDesc::enumerateItems( const QList< void* >& includeFields
                                     , const QList< void* >& excluseFields )

{
    QList< FieldDescItem > items;
    enumerateItems( items );

    for( int i = items.size() - 1; i >= 0; i-- )
    {
        if( !includeFields.empty() &&
             includeFields.end() == std::find( includeFields.begin(), includeFields.end(), items[ i ].buffValue() ) )
        {
            items.removeAt( i );
            continue;
        }

        if( !excluseFields.empty() &&
             excluseFields.end() != std::find( excluseFields.begin(), excluseFields.end(), items[ i ].buffValue() ) )
        {
            items.removeAt( i );
            continue;
        }
    }

    return items;
}

bool FieldDesc::dbInsert()
{
    return dbInsert( QSqlQuery() );
}

bool FieldDesc::dbInsert( QSqlQuery& query )
{
    QList< FieldDescItem > items;
    enumerateItems( items );

    QString queryStr = "INSERT INTO " + getTableName() + "(";

    for( int i = 0; i < items.size(); i++ )
    {
        queryStr += items[ i ].Column;
        if( i < items.size() - 1 )
            queryStr += ",";
    }

    queryStr += ")VALUES(";

    for( int i = 0; i < items.size(); i++ )
    {
        queryStr += ":" + items[ i ].Column;
        if( i < items.size() - 1 )
            queryStr += ",";
    }

    queryStr += ");";

    query.prepare( queryStr );

    for( int i = 0; i < items.size(); i++ )
        items[ i ].bindValue( query );

    if( !query.exec() )
    {
        qDebug() << query.lastError();
        return false;
    }

    return true;
}

void FieldDesc::dbUpdate()
{
    QList< FieldDescItem > items;
    enumerateItems( items );

    QList< FieldDescItem* > pkeys;
    QList< FieldDescItem* > notPkeys;

    for( int i = 0; i < items.size(); i++ )
    {
        if( items[ i ].IsPk )
            pkeys.append( &items[ i ] );
        else
            notPkeys.append( &items[ i ] );
    }

    QString queryStr = "UPDATE " + getTableName() + " SET ";

    for( int i = 0; i < notPkeys.size(); i++ )
    {
        queryStr += notPkeys[ i ]->Column + " = :" + notPkeys[ i ]->Column;
        if( i < notPkeys.size() - 1 )
            queryStr += ",";
    }

    queryStr += " WHERE ";

    for( int i = 0; i < pkeys.size(); i++ )
    {
        queryStr += pkeys[ i ]->Column + " = :" + pkeys[ i ]->Column;
        if( i < pkeys.size() - 1 )
            queryStr += " AND ";
    }

    queryStr += ";";

    QSqlQuery query;
    query.prepare( queryStr );

    for( int i = 0; i < items.size(); i++ )
        items[ i ].bindValue( query );

    query.exec();
}

void FieldDesc::dbDropTable()
{
    QString queryStr = "DROP TABLE " + getTableName() + ";";

    QSqlQuery query;
    query.prepare( queryStr );

    query.exec();
}

bool FieldDesc::dbDropTableSafe()
{
    QString queryStr = "DROP TABLE " + getTableName() + ";";

    QSqlQuery query;
    query.prepare( queryStr );

    return query.exec();
}

//bool FieldDesc::dbCreateTable()
//{
//    QList< FieldDescItem > items;
//    enumerateItems( items );
//
//    QList< FieldDescItem* > pkeys;
//    QList< FieldDescItem* > notPkeys;
//
//    for( int i = 0; i < items.size(); i++ )
//    {
//        if( items[ i ].IsPk )
//            pkeys.append( &items[ i ] );
//        else
//            notPkeys.append( &items[ i ] );
//    }
//
//    QString queryStr = "CREATE TABLE " + getTableName() + "(";
//
//    for( int i = 0; i < pkeys.size(); i++ )
//    {
//        queryStr += pkeys[ i ]->Column + " " + pkeys[i]->dbType() + "PRIMARY KEY NOT NULL UNIQUE";
//        if( i < pkeys.size() - 1 )
//            queryStr += ", ";
//    }
//
//    if( !pkeys.empty() && !notPkeys.empty() )
//        queryStr += ", ";
//
//    for( int i = 0; i < notPkeys.size(); i++ )
//    {
//        queryStr += notPkeys[ i ]->Column + " " + notPkeys[i]->dbType();
//        if( i < notPkeys.size() - 1 )
//            queryStr += ", ";
//    }
//
//    queryStr += ");";
//
//    QSqlQuery query;
//    query.prepare( queryStr );
//
//    return query.exec();
//}

void FieldDesc::dbDeleteAll()
{
    QSqlQuery query;
    query.prepare( "DELETE FROM " + getTableName() + ";" );
    query.exec();
}

QString FieldDesc::makeBufferingStr()
{
    return makeBufferingStr( enumerateItems() );
}

QString FieldDesc::makeBufferingStr( const QList< FieldDescItem >& items )
{
    QString queryStr = " ";

    for( int i = 0; i < items.size(); i++ )
    {
//        if( !includeFields.empty() &&
//             includeFields.end() == std::find( includeFields.begin(), includeFields.end(), items[ i ].buffValue() ) )
//        {
//            continue;
//        }
//
//        if( !excluseFields.empty() &&
//             excluseFields.end() != std::find( excluseFields.begin(), excluseFields.end(), items[ i ].buffValue() ) )
//        {
//            continue;
//        }

        queryStr += items[ i ].Column;
        if( i < items.size() - 1 )
            queryStr += ",";
    }

//    queryStr[ queryStr.length() - 1 ] = ' ';

    return queryStr + " ";
}

void FieldDesc::fetch( QSqlQuery& query, const QSqlRecord& rec )
{
    QList< FieldDescItem > items = enumerateItems();
    fetch( query, rec, items );
}

void FieldDesc::fetch( QSqlQuery& query, const QSqlRecord& rec, QList< FieldDescItem >& items )
{
    for( int i = 0; i < items.size(); i++ )
        items[ i ].fetch( query, rec );
}

bool FieldDesc::loadAll( std::function<void ()> rowFetched, const QList< void* >& includeFields
                                                          , const QList< void* >& excluseFields )
{
    QList< FieldDescItem > items = enumerateItems( includeFields, excluseFields );

    QString queryStr = "SELECT " + makeBufferingStr( items ) + "FROM " + getTableName() + ";";

    QSqlQuery query;
    query.prepare( queryStr );

    if( !query.exec() )
        return false;

    QSqlRecord rec = query.record();

    while( query.next() )
    {
        for( int i = 0; i < items.size(); i++ )
            items[ i ].fetch( query, rec );

        rowFetched();
    }

    return true;
}

void FieldDesc::loadByPkeys()
{
    QList< FieldDescItem > items;
    enumerateItems( items );

    QList< FieldDescItem* > pkeys;

    for( int i = 0; i < items.size(); i++ )
    {
        if( items[ i ].IsPk )
            pkeys.append( &items[ i ] );
    }

    QString queryStr = "SELECT ";

    for( int i = 0; i < items.size(); i++ )
    {
        queryStr += items[ i ].Column;
        if( i < items.size() - 1 )
            queryStr += ",";
    }

    queryStr += " FROM " + getTableName() + " WHERE ";

    for( int i = 0; i < pkeys.size(); i++ )
    {
        queryStr += pkeys[ i ]->Column + " = :" + pkeys[ i ]->Column;
        if( i < pkeys.size() - 1 )
            queryStr += " AND ";
    }

    queryStr += ";";

    QSqlQuery query;
    query.prepare( queryStr );

    for( int i = 0; i < items.size(); i++ )
        items[ i ].bindValue( query );

    query.exec();

    QSqlRecord rec = query.record();

    if( query.next() )
        for( int i = 0; i < items.size(); i++ )
            items[ i ].fetch( query, rec );
}
