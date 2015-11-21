#include "MyOrderedList_header.h"

template<class Type, class Compare>
MyOrderedList<Type, Compare>::MyOrderedList()
{
}


template<class Type, class Compare>
MyOrderedList<Type, Compare>::~MyOrderedList()
{
}

/*
template<class Type, class Compare>
typename std::list<Type>::iterator MyOrderedList<Type, Compare>::Insert(const Type& obj){
	std::map<Type, std::list<Type>::iterator, Compare>::iterator itr
		= mIndex.upperBound(obj);
	std::list<Type>::iterator inserted = this->insert(iterator, obj);
	mIndex[obj] = inserted;
	return inserted;
}
*/