#pragma once

#include <vector>

// manage second degree equation 
class Equation2
{
protected:
	// three coefficients
	float	mA;
	float	mB;
	float	mC;

	// compute delta for given Y
	float delta(float Y);

public:
	// init equation
	Equation2(float a, float b, float c) : mA(a), mB(b), mC(c)
	{

	}

	// change coeffs
	void Set(float a, float b, float c)
	{
		mA = a;
		mB = b;
		mC = c;
	}

	// solve equation for given Y and return vector of solutions
	std::vector<float>	Solve(float forY = 0.0f);
};