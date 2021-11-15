#include "utility.hpp"


std::ostream& operator<<(std::ostream& out, const Coords& coords)
{
	out << '(' << coords.x << ';' << coords.y << ')';
	return out;
}