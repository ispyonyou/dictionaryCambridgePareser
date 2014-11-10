
#ifndef WORDSCONTENTPROVIDER_H
#define WORDSCONTENTPROVIDER_H

#include "wordsdata.h"
#include <QList>
#include <memory>

class WordsContentProvider
{
public:
    WordsContentProvider();

    bool loadWords( QList< std::shared_ptr< WordsData > >& words );
    bool loadAudio( int wordId, QByteArray& audio );
    bool insertWord( std::shared_ptr< WordsData >& word );
    bool updateIsLearned( std::shared_ptr< WordsData >& word );

    QString generateHtml( const std::shared_ptr< WordsData > word );
    QString generateHtml( const QList< std::shared_ptr< WordsData > >& words );
};

#endif // WORDSCONTENTPROVIDER_H
