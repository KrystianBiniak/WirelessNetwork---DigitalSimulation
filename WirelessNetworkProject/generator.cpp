#include "generator.h"
#include <fstream>
#include <math.h>

Generator::Generator(int kernel, int tx_id, int id) : kernel_(kernel), tx_id_(tx_id), id_(id) {}

double Generator::Rand()
{
	int h = kernel_ / kQ;
	kernel_ = kA * (kernel_ - kQ * h) - kR * h;
	if (kernel_ < 0)
		kernel_ = kernel_ + static_cast<int>(kM);
	return kernel_ / kM;
}

int Generator::RandSection(int min_, int max_)
{
	return Rand() * (max_ - min_) + min_;
}

double Generator::RandExp(double L)
{
	k_ = Rand();
	return -(1.0 / L) * log(k_);
}

double Generator::RandZeroOne(double P)
{
	k_ = Rand();
	if (k_ < P) return 1;
	else return 0;
}