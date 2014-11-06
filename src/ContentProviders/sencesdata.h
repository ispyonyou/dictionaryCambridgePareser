#ifndef SENCESDATA_H
#define SENCESDATA_H

#include "../CopyClearBase.h"
#include <QString>

struct SensesData : public CopyClearBase< SensesData >
{
    int id;
    int wordId;
    QString defenition;
    QString translation;
};

#endif // SENCESDATA_H
