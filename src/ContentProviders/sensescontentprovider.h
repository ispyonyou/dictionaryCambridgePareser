
#ifndef SENSESCONTENTPROVIDER_H
#define SENSESCONTENTPROVIDER_H

#include "sencesdata.h"

class SensesContentProvider
{
public:
    SensesContentProvider();

    bool insertSense( std::shared_ptr< SensesData >& sense );
};


#endif // SENSESCONTENTPROVIDER_H
