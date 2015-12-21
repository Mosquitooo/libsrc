
#ifndef __SINGLETON_H__
#define __SINGLETON_H__
#include <cassert>


template <typename T> 
class  Singleton
{
protected:
    static  T* m_Singleton;

public:
    Singleton( void )
    {
        assert( !m_Singleton );
        m_Singleton = static_cast<T*>(this);
    }

    ~Singleton( void )
    {  
        assert( m_Singleton );  
        m_Singleton = 0;  
    }

    static T& getSingleton( void )
    {  
        assert( m_Singleton );
        return ( *m_Singleton );  
    }

    static T* getSingletonPtr( void )
    {  
        assert( m_Singleton );
        return ( m_Singleton );  
    }

private:

    //阻止对象赋值
    Singleton& operator=(const Singleton&) 
    { 
        return *this; 
    }

    //阻止拷贝对象
    Singleton(const Singleton&) {}
};

#endif	
