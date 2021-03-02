#include "Equation2.h"

// at^2 + bt + c = 0
// at^2 + bt + c = Y <=> at^2 + bt + c-Y = 0

float Equation2::delta(float Y)
{
	// for a given Y delta = b^2 - 4 * a * (c-Y)
	float result = mB * mB - 4.0f * mA * (mC - Y);
	return result;
}

std::vector<float>	Equation2::Solve(float forY)
{
	// empty result vector
	std::vector<float> result;

	// get delta
	float d = delta(forY);

	if (d < 0.0f) // no solution, return empty vector
	{
		return result;
	}
	
	// compute first solution ( -b - sqrt(delta))/2a and push it on result vector
	result.push_back((-mB - sqrtf(d)) / (2.0f * mA));
	// if d is > 0.0 then compute the other solution ( -b + sqrt(delta))/2a and push it on result vector
	if (d > 0.0f)
	{
		result.push_back((-mB + sqrtf(d)) / (2.0f * mA));
	}
	// return solutions
	return result;
}