#include "Abstract/Utils/Color.hpp"

#include <iostream>
#include <ostream>

sf::Color parseColorString(const std::string& colorString)
{
	int a,r,g,b;
	std::sscanf(colorString.c_str(), "#%02x%02x%02x%02x", &a, &r, &g, &b);

	return {static_cast<std::uint8_t>(r),
		static_cast<std::uint8_t>(g),
		static_cast<std::uint8_t>(b),
	static_cast<std::uint8_t>(a)};
}