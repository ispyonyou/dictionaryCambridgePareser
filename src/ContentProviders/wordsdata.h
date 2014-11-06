#ifndef WORDSDATA_H
#define WORDSDATA_H

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

#endif // WORDSDATA_H
