
#ifndef WORDSCONTENTPROVIDER_H
#define WORDSCONTENTPROVIDER_H

#include "../CopyClearBase.h"
#include <QString>

struct WordsData : public CopyClearBase< WordsData >
{
    int id;
    QString word;
    QString transcription;
    QByteArray audio;
    bool isLearned;
};

class WordsContentProvider
{
public:
    WordsContentProvider();

    bool loadWords( QList< std::shared_ptr< WordsData > >& words );
    bool insertWord( std::shared_ptr< WordsData >& word );
};

#endif // WORDSCONTENTPROVIDER_H
