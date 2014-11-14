#include "wordscontentprovider.h"
#include "sensescontentprovider.h"
#include "examplescontentprovider.h"
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

bool WordsContentProvider::updateIsLearned( std::shared_ptr< WordsData >& word )
{
    WordsDesc desc( *word.get() );
    return desc.dbUpdate( QList< void* >() << &desc.isLearned );
}

QString WordsContentProvider::generateHtml( const std::shared_ptr< WordsData > word, enHtmlDest dest )
{
    return generateHtml( QList< std::shared_ptr< WordsData > >() << word, dest );
}

QString WordsContentProvider::generateHtml( const QList< std::shared_ptr< WordsData > >& words, enHtmlDest dest )
{
    QString html;

    html += "<!DOCTYPE html>"
            "<html lang=\"en\">"
            "<head>\n"
            "  <meta charset=\"utf-8\" />"
            "  <style>"
            "    .wordArea{"
            "    }\n"
            "    .word {"
            "      font: 22pt sans-serif;"
            "    }"
            "    .transcrypt{"
            "      font: 18pt monospace;"
            "    }"
            "    .senses{"
            "      position: relative;"
            "      left: 20px;"
            "      font: 14pt sans-serif;"
            "    }"
            "    .defenition{"
            "      color: #606060"
            "    }"
            "  </style>"
            "</head>"
            "<body>";

    html += "<div id=\"top\"/>";

    SensesContentProvider sensesProvider;
    ExamplesContentProvider examplesProvider;

    QString wordsRefsHtml = "<div class=\"word\">";
    QString wordsHtml;

    for( int i = 0; i < words.size(); i++ )
    {
        std::shared_ptr< WordsData > wordData = words[ i ];

        if( HtmlDest_Out == dest )
            wordsRefsHtml += QString( "<a href=\"#%1\">%1</a><br/>" ).arg( wordData->word );

        QString header =  "<div class=\"wordArea\">"
                            "<div>"
                              "<span class=\"word\" id=\"%1\"> %1 </span>"
                              "<span class=\"transcrypt\">/%2/</span>";
        if( HtmlDest_Out == dest )
            header += "<span><a href=\"#top\">back</a></span>";

        header += "</div></div>";

        wordsHtml += header.arg( wordData->word )
                           .arg( wordData->transcription );

        QList< std::shared_ptr< SensesData > > senses;
        sensesProvider.loadSenses( wordData->id, senses );

        wordsHtml += "<div class=\"senses\">";

        Q_FOREACH( std::shared_ptr< SensesData > sense, senses )
        {
            QString senseHtml = "<li><span class=\"defenition\">%1</span><br/>%2</li>";

            wordsHtml += senseHtml.arg( sense->defenition )
                                  .arg( sense->translation );
        }

        wordsHtml += "</div><br/>";
    }

    if( HtmlDest_Out == dest )
        html += wordsRefsHtml + "</div><br/>";

    html += wordsHtml;

    html += "</body>"
            "</html>";

    return html;
}
