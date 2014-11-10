#ifndef FIELDDESC_H
#define FIELDDESC_H

#include <QString>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QList>
#include <functional>

class FieldDescItem
{
public:
    FieldDescItem( const QString& col, int& buf );
    FieldDescItem( const QString& col, QString& buf, int length );
    FieldDescItem( const QString& col, QJsonObject& buf, int length );
    FieldDescItem( const QString& col, QByteArray& buf );
    FieldDescItem( const QString& col, bool& buf );

    FieldDescItem& setPkFlag();

    void bindValue( QSqlQuery& query );
    void fetch( QSqlQuery& query, const QSqlRecord& rec );

    void* buffValue() const;

    bool isNull() const;

//    QString dbType();

public:
    enum {
        Int,
        String,
        JsonObj,
        ByteArray,
        Bool,
    } Type;

    QString Column;
    int StrLength;
    bool IsPk;

    union {
        int* IntValue;
        QString* StringValue;
        QJsonObject* JsonObjValue;
        QByteArray* ByteArrayValue;
        bool* BoolValue;
    } Buffer;
};

class FieldDesc
{
public:
    virtual ~FieldDesc() {};

    virtual QString getTableName() = 0;
    virtual void enumerateItems( QList< FieldDescItem >& items ) = 0;

    QList< FieldDescItem > enumerateItems( const QList< void* >& includeFields = QList< void* >()
                                         , const QList< void* >& excluseFields = QList< void* >() );

    bool dbInsert();
    bool dbInsert( QSqlQuery& query );
    bool dbUpdate( const QList< void* >& includeFields = QList< void* >() );

    void dbDropTable();
    bool dbDropTableSafe();
//    bool dbCreateTable();
    void dbDeleteAll();

    QString makeBufferingStr();
    QString makeBufferingStr( const QList< FieldDescItem >& items );

    void fetch( QSqlQuery& query, const QSqlRecord& rec );
    void fetch( QSqlQuery& query, const QSqlRecord& rec, QList< FieldDescItem >& items );

public:
    bool loadAll( std::function<void ()> rowFetched, const QList< void* >& includeFields = QList< void* >()
                                                   , const QList< void* >& excluseFields = QList< void* >() );
    bool loadByPkeys();
};

#define DECLARE_FIELD_DESC( classname ) \
    virtual QString getTableName() override; \
    virtual void enumerateItems( QList< FieldDescItem >& items ) override; \
    static QList< classname > dbLoadAll(); \
    static classname dbLoadById( int id );

#define BEGIN_FIELD_DESC( classname, table ) \
    QString classname::getTableName() \
    { return table; } \
    classname classname::dbLoadById( int id ) \
    { \
        classname data; \
        data.id = id; \
        data.loadByPkeys(); \
        return data; \
    } \
    QList< classname > classname::dbLoadAll() \
    { \
        QList< classname > res; \
        classname data; \
        data.loadAll( [&](){ res.append( data ); } ); \
        return res; \
    }\
    void classname::enumerateItems( QList< FieldDescItem >& items ) \
    {

#define END_FIELD_DESC() \
    }


#endif // FIELDDESC_H
