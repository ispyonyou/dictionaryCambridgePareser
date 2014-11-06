#ifndef COPYCLEARBASE_H
#define COPYCLEARBASE_H

template < class T >
class CopyClearBase
{
public:
    CopyClearBase()
    {
      memset( static_cast<T*>(this), 0, sizeof(T) );
    }

    void clear()
    {
        T t;
        *static_cast<T*>(this) = t;
    }

    void copy( const T & s ) { *static_cast<T*>(this) = s; }
};

#endif // COPYCLEARBASE_H
