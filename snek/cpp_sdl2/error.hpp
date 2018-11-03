#pragma once

#include <stdexcept>

namespace Snek
{

class Error : public std::runtime_error
{
public:
	using runtime_error::runtime_error;
};

}
