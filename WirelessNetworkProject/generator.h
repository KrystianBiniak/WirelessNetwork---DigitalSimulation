#ifndef GENERATOR_H
#define GENERATOR_H

class Generator
{
public:
	Generator(int kernel, int tx_id, int id);

	double Rand();
	double RandExp(double L);
	int RandSection(int min_, int max_);
	double RandZeroOne(double P);
	int GetTxId() {return tx_id_;}
	int GetGenId() { return id_; }

private:
	int kernel_;
	int tx_id_;
	int id_;
	double k_;
	double return_;
	const double kM = 2147483647.0;
	static const int kA = 16807;
	static const int kQ = 127773;
	static const int kR = 2836;
};

#endif	//	GENERATOR_H
