#pragma once
#include <functional>

struct DepthFunc
{

	typedef std::function<bool(double, double)> closure;

	static bool always(const double inputZ, const double z)
	{
		return true;
	}

	static bool never(const double inputZ, const double z)
	{
		return false;
	}

	static bool less(const double inputZ, const double z)
	{
		return inputZ < z;
	}

	static bool equal(const double inputZ, const double z)
	{
		return inputZ == z;
	}

	static bool lequal(const double inputZ, const double z)
	{
		return inputZ <= z;
	}

	static bool greater(const double inputZ, const double z)
	{
		return inputZ > z;
	}

	static bool notequal(const double inputZ, const double z)
	{
		return inputZ != z;
	}

	static bool gequal(const double inputZ, const double z)
	{
		return inputZ >= z;
	}
};