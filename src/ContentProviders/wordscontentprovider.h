
#ifndef WORDSCONTENTPROVIDER_H
#define WORDSCONTENTPROVIDER_H

#include "wordsdata.h"
#include <QList>
#include <memory>

enum enHtmlDest{
    HtmlDest_View,
    HtmlDest_Out,
};

class WordsContentProvider
{
public:
    WordsContentProvider();

    bool loadWords( QList< std::shared_ptr< WordsData > >& words );
    bool loadAudio( int wordId, QByteArray& audio );
    bool insertWord( std::shared_ptr< WordsData >& word );
    bool updateIsLearned( std::shared_ptr< WordsData >& word );

    QString generateHtml( const std::shared_ptr< WordsData > word, enHtmlDest dest );


    QString generateHtml( const QList< std::shared_ptr< WordsData > >& words, enHtmlDest dest );
};

#endif // WORDSCONTENTPROVIDER_H
