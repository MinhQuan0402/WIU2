#pragma once

#include <vector>
#include <assert.h>

template<typename T>
class UnorderedVector : public std::vector<T>
{
public:
	inline void remove(T& element) {
		T* el = &element;
		assert(el >= &std::vector<T>::front());

		T* bck = &std::vector<T>::back();

		assert(el <= bck);

		if (el == bck) return;
		else *el = std::move(*bck);

		std::vector<T>::pop_back();
	}
};