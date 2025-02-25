#pragma once

#include <vector>
#include <algorithm>

template<typename T>
class UnorderedVector : public std::vector<T>
{
public:
	inline void remove(T& element) {
		T* el = &element;
		auto it = std::find(this->begin(), this->end(), *el);

		T* bck = &std::vector<T>::back();

		if (el == bck) return;
		else
		{
			T tmp(std::move(*el));
			std::replace(this->begin(), this->end(), *el, *bck);
			*bck = tmp;
		}

		this->pop_back();
	}
};