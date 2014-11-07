#ifndef EXAMPLESCONTENTPROVIDER_H
#define EXAMPLESCONTENTPROVIDER_H

#include "examplesdata.h"
#include <memory>

class ExamplesContentProvider
{
public:
    ExamplesContentProvider();

    bool insertExample( std::shared_ptr< ExamplesData >& example );
};

#endif // EXAMPLESCONTENTPROVIDER_H
