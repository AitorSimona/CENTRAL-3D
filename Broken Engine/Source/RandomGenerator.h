#ifndef _RNGEN_H_
#define _RNGEN_H_

#include <random>
#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API RNGen
{

public:

	RNGen();
	~RNGen();

	//Ints
	int GetIntRN(); //Shouldn't give negative numbers
	int GetIntRNinRange(int rangeMin_Value, int rangeMaxValue);

	std::vector<int>GetIntRNVecinRange(int vecSize, int rangeMin_Value, int rangeMaxValue);
	std::vector<int>GetIntRNVec(int vecSize);

	//Doubles (use this for floats)
	double GetDoubleRN();
	double GetDoubleRNinRange(double rangeMinValue, double rangeMaxValue);

	std::vector<double>GetDoubleRNVecinRange(int vecSize, double rangeMin_Value, double rangeMaxValue);
	std::vector<double>GetDoubleRNVec(int vecSize);


private:

	std::default_random_engine m_RNEngine;
	std::random_device m_RandomDevice;

	std::uniform_real_distribution<double> m_DoubleDistribution;
	std::uniform_int_distribution<int> m_IntDistribution;

};

BE_END_NAMESPACE
#endif