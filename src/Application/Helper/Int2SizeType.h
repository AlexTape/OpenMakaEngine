#pragma once
#include <stdexcept>

class Int2SizeType
{
public:
	Int2SizeType(int& _v) : v(_v)
	{
	}

	operator int() const
	{
		return v;
	}

	operator size_t() const
	{
		_ASSERT(v >= 0);
		if (v < 0)
			throw std::runtime_error("int out of range for size_t");
		return static_cast<size_t>(v);
	}

	bool operator >=(const size_t& rhs) const
	{
		return static_cast<size_t>(*this) >= rhs;
	}

	bool operator >(const size_t& rhs) const
	{
		return static_cast<size_t>(*this) > rhs;
	}

	bool operator <=(const size_t& rhs) const
	{
		return static_cast<size_t>(*this) <= rhs;
	}

	bool operator <(const size_t& rhs) const
	{
		return static_cast<size_t>(*this) < rhs;
	}

	bool operator ==(const size_t& rhs) const
	{
		return static_cast<size_t>(*this) == rhs;
	}

protected:
	int& v;
};
