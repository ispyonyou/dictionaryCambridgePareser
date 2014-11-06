#ifndef EXAMPLEDATA_H
#define EXAMPLEDATA_H

#include "../CopyClearBase.h"
#include <QString>

struct ExamplesData : public CopyClearBase< ExamplesData >
{
    int id;
    int senseId;
    QString text;
};

#endif // EXAMPLEDATA_H
