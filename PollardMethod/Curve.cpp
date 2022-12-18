#include "Curve.h"

TestGenerator::TestGenerator(const CryptoPP::Integer& p) :
	p(p) 
{
	GenerateCurve();
	GenerateBasePoint();
	CalcBasePointOrder();

	srand((unsigned)time(0));
	auto deg =  rand() % order;
	if (!deg) {
		deg = 1;
	}
	P = curve.Multiply(deg, Q);
}

void TestGenerator::GenerateCurve() {
	srand((unsigned)time(0));

	CryptoPP::Integer a((rand() + 1) % p);
	CryptoPP::Integer b((rand() + 1) % p);

	curve = CryptoPP::ECP::ECP(p, a, b);
}

CryptoPP::Integer TestGenerator::getSquareRoot(const CryptoPP::Integer& y) {
	if (CryptoPP::Jacobi(y, p) != 1)
		return -1;

	return CryptoPP::ModularSquareRoot(y, p);
}

void TestGenerator::GenerateBasePoint() {
	srand((unsigned)time(0));

	CryptoPP::Integer y = -1;
	CryptoPP::Integer x = 0;

	while (y == -1) {
		CryptoPP::Integer x = rand() % p;
		if (!x) {
			continue;
		}
		CryptoPP::Integer y_sqr = x * x * x + curve.GetA() * x + curve.GetB();
		y = getSquareRoot(y_sqr);
	}

	Q = CryptoPP::ECP::Point(x, y);
}


void TestGenerator::CalcBasePointOrder() {
	CryptoPP::ECP::Point MultQ = Q;

	for (CryptoPP::Integer i = 1; ; i++) {
		MultQ = curve.Add(Q, MultQ);
		if (MultQ.x == 0 && MultQ.y == 0) {
			order = i + 1;
			return;
		}
	}
}