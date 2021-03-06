#include "sensescontentprovider.h"
#include "FieldDesc.h"
#include <QVariant>

class SensesDesc : public SensesData
                 , public FieldDesc
{
public:
    SensesDesc();
    SensesDesc( const SensesDesc& src );
    SensesDesc( const SensesData& src );

    DECLARE_FIELD_DESC( SensesDesc );
};

SensesDesc::SensesDesc()
{
    SensesData::clear();
}

SensesDesc::SensesDesc( const SensesDesc& src )
{
    SensesData::copy( src );
}

SensesDesc::SensesDesc( const SensesData& src )
{
    SensesData::copy( src );
}

BEGIN_FIELD_DESC( SensesDesc, "senses" )
    items.append( FieldDescItem( "id"         , id         ).setPkFlag() );
    items.append( FieldDescItem( "word_id"    , wordId     ) );
    items.append( FieldDescItem( "defenition" , defenition , 2048 ) );
    items.append( FieldDescItem( "translation", translation, 2048 ) );
END_FIELD_DESC()

SensesContentProvider::SensesContentProvider()
{ }

bool SensesContentProvider::loadSenses( int wordId, QList< std::shared_ptr< SensesData > >& senses )
{
    SensesDesc desc;

    QString queryStr = "select " + desc.makeBufferingStr() + " from " + desc.getTableName();
            queryStr += " where word_id=:word_id";

    QSqlQuery query;
    query.prepare( queryStr );

    query.bindValue( ":word_id", wordId );

    if( !query.exec() )
        return false;

    QSqlRecord rec = query.record();

    while( query.next() )
    {
        desc.fetch( query, rec );
        senses.append( std::make_shared< SensesData >( desc ) );
    }

    return true;
}

bool SensesContentProvider::insertSense( std::shared_ptr< SensesData >& sense )
{
    SensesDesc desc( *sense.get() );
    if( !desc.dbInsert() )
        return false;

    QSqlQuery query;
    if( !query.exec( "select last_insert_rowid()" ) )
        return false;

    if( !query.next() )
        return false;

    sense->id = query.value( 0 ).toInt();
    return true;
}
