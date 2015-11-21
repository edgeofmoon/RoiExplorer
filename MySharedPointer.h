#pragma once

#include <memory>

/*
template< class T >
class MySharedPointer
	:public std::shared_ptr<T>
{
public:
	MySharedPointer()
		:shared_ptr(){};
	MySharedPointer(T* ptr)
		:shared_ptr(ptr){};
	MySharedPointer(T* ptr)
		:shared_ptr(ptr){};
	~MySharedPointer(){};

};
*/

template <class T>
using MySharedPointer = std::shared_ptr < T > ;