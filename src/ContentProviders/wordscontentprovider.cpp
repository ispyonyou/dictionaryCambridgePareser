#include "wordscontentprovider.h"
#include "FieldDesc.h"
#include <QVariant>

class WordsDesc : public WordsData
                , public FieldDesc
{
public:
    WordsDesc();
    WordsDesc( const WordsDesc& src );
    WordsDesc( const WordsData& src );

    DECLARE_FIELD_DESC( WordsDesc );
};

WordsDesc::WordsDesc()
{
    WordsData::clear();
}

WordsDesc::WordsDesc( const WordsDesc& src )
{
    WordsData::copy( src );
}

WordsDesc::WordsDesc( const WordsData& src )
{
    WordsData::copy( src );
}

BEGIN_FIELD_DESC( WordsDesc, "words" )
    items.append( FieldDescItem( "id"           , id                 ).setPkFlag() );
    items.append( FieldDescItem( "word"         , word         , 256 ) );
    items.append( FieldDescItem( "transcription", transcription, 256 ) );
    items.append( FieldDescItem( "audio"        , audio              ) );
    items.append( FieldDescItem( "is_learned"   , isLearned          ) );
END_FIELD_DESC()

WordsContentProvider::WordsContentProvider()
{ }

bool WordsContentProvider::loadWords( QList< std::shared_ptr< WordsData > >& words )
{
    WordsDesc desc;
    return desc.loadAll( [&](){ words.append( std::make_shared< WordsData >( desc ) ); }, QList< void* >(), QList< void* >() << (void*)&desc.audio );
}

bool WordsContentProvider::loadAudio( int wordId, QByteArray& audio )
{
    WordsDesc desc;
    desc.id = wordId;

    if( !desc.loadByPkeys() )
        return false;

    audio = desc.audio;
    return true;
}

bool WordsContentProvider::insertWord( std::shared_ptr< WordsData >& word )
{
    WordsDesc desc( *word.get() );
    if( !desc.dbInsert() )
        return false;

    QSqlQuery query;
    if( !query.exec( "select last_insert_rowid()" ) )
        return false;

    if( !query.next() )
        return false;

    word->id = query.value( 0 ).toInt();
    return true;
}
