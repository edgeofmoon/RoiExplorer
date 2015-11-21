#pragma once

#include <map>
#include <list>

template<class Type, class Compare = less<Type>>
class MyOrderedList
	:public std::list<typename Type>
{
public:
	MyOrderedList();
	~MyOrderedList();

	typename std::list<typename Type>::iterator Insert(const Type& obj){
		std::map<Type, std::list<Type>::iterator, Compare>::iterator itr
			= mIndex.upper_bound(obj);
		std::list<Type>::iterator inserted;
		if (itr != mIndex.end()) inserted = this->insert(itr->second, obj);
		else inserted = this->insert(this->end(), obj);
		mIndex[obj] = inserted;
		return inserted;
	};

protected:
	std::map<Type, typename std::list<typename Type>::iterator, Compare> mIndex;
};

