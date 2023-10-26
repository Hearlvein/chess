#pragma once

#include <iostream>
#include <vector>

#include "Types.hpp"


std::ostream& operator<<(std::ostream& out, const Coords& coords);


template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
	if (auto it = vec.cbegin(); it != vec.cend())
	{
		out << *it;
		++it;
		for (; it != vec.cend(); ++it)
			out << ' ' << *it;
	}

	return out;
}