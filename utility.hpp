#pragma once

#include <iostream>
#include <vector>

#include "Types.hpp"


std::ostream& operator<<(std::ostream& out, const Coords& coords);


template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
	for (const auto& v : vec)
		out << v << ' ';
	return out;
}