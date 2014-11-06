#ifndef EXAMPLESCONTENTPROVIDER_H
#define EXAMPLESCONTENTPROVIDER_H

#include "examplesdata.h"

class ExamplesContentProvider
{
public:
    ExamplesContentProvider();

    bool insertExample( std::shared_ptr< ExamplesData >& example );
};

#endif // EXAMPLESCONTENTPROVIDER_H
