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

        T* bck = &this->back();

        if (it == this->end())
            return;
        else
        {
            T tmp(std::move(*el));
            std::replace(this->begin(), this->end(), *el, *bck);
            *bck = std::move(tmp);
        }

        this->pop_back();
	}
};