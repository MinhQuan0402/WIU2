#pragma once

#include <vector>
#include <assert.h>
#include <algorithm>

template<typename T>
class UnorderedVector : public std::vector<T>
{
public:
	inline void remove(T& element) {
		T* el = &element;
		auto it = std::find(this->begin(), this->end(), element);

		T* bck = &std::vector<T>::back();

		if (it == this->end()) 
			return;
		else 
			*el = std::move(*bck);

		std::vector<T>::pop_back();
	}
};