#include "examplescontentprovider.h"
#include "FieldDesc.h"
#include <QVariant>

class ExamplesDesc : public ExamplesData
                   , public FieldDesc
{
public:
    ExamplesDesc();
    ExamplesDesc( const ExamplesDesc& src );
    ExamplesDesc( const ExamplesData& src );

    DECLARE_FIELD_DESC( ExamplesDesc );
};

ExamplesDesc::ExamplesDesc()
{
    ExamplesData::clear();
}

ExamplesDesc::ExamplesDesc( const ExamplesDesc& src )
{
    ExamplesData::copy( src );
}

ExamplesDesc::ExamplesDesc( const ExamplesData& src )
{
    ExamplesData::copy( src );
}

BEGIN_FIELD_DESC( ExamplesDesc, "examples" )
    items.append( FieldDescItem( "id"      , id            ).setPkFlag() );
    items.append( FieldDescItem( "sense_id", senseId       ) );
    items.append( FieldDescItem( "text"    , text   , 3072 ) );
END_FIELD_DESC()

ExamplesContentProvider::ExamplesContentProvider()
{ }

bool ExamplesContentProvider::insertExample( std::shared_ptr< ExamplesData >& example )
{
    ExamplesDesc desc( *example.get() );
    if( !desc.dbInsert() )
        return false;

    QSqlQuery query;
    if( !query.exec( "select last_insert_rowid()" ) )
        return false;

    if( !query.next() )
        return false;

    example->id = query.value( 0 ).toInt();
    return true;
}

bool ExamplesContentProvider::loadExamples( int senseId, QList< std::shared_ptr< ExamplesData > >& examples )
{
    ExamplesDesc desc;

    QString queryStr = "select " + desc.makeBufferingStr() + " from " + desc.getTableName();
            queryStr += " where sense_id=:sense_id";

    QSqlQuery query;
    query.prepare( queryStr );

    query.bindValue( ":sense_id", senseId );

    if( !query.exec() )
        return false;

    QSqlRecord rec = query.record();

    while( query.next() )
    {
        desc.fetch( query, rec );
        examples.append( std::make_shared< ExamplesData >( desc ) );
    }

    return true;
}
