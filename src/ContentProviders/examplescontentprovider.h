#ifndef EXAMPLESCONTENTPROVIDER_H
#define EXAMPLESCONTENTPROVIDER_H

#include "examplesdata.h"
#include <memory>

class ExamplesContentProvider
{
public:
    ExamplesContentProvider();

    bool insertExample( std::shared_ptr< ExamplesData >& example );
    bool loadExamples( int senseId, QList< std::shared_ptr< ExamplesData > >& examples );
};

#endif // EXAMPLESCONTENTPROVIDER_H
