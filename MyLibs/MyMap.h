#pragma once

#include <map>
#include "MySharedPointer.h"

template<typename keyType, typename valueType>
class MyMap :public std::map < typename keyType, typename valueType > 
{
public:
	bool HasKey(const keyType& key) const;
};


template<typename keyType, typename valueType>
bool MyMap<keyType, valueType>::HasKey(const keyType& key) const{
	std::map<keyType, valueType>::const_iterator itr = this->find(key);
	return itr != this->end();
}

template<typename keyType, typename valueType>
using MyMapSPtr = MySharedPointer < MyMap<keyType, valueType> > ;

template<typename keyType, typename valueType>
using MyMapScPtr = MySharedPointer < const MyMap<keyType, valueType> >;
