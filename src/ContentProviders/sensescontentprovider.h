
#ifndef SENSESCONTENTPROVIDER_H
#define SENSESCONTENTPROVIDER_H

#include "sencesdata.h"

class SensesContentProvider
{
public:
    SensesContentProvider();

    bool loadSenses( int wordId, QList< std::shared_ptr< SensesData > >& senses );
    bool insertSense( std::shared_ptr< SensesData >& sense );
};


#endif // SENSESCONTENTPROVIDER_H
