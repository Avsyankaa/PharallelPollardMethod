#pragma once
#include <eccrypto.h>

#include <random>

#include <nbtheory.h>

class TestGenerator {
public:
	TestGenerator(const CryptoPP::Integer& p);

	CryptoPP::ECP curve;
	CryptoPP::ECP::Point Q;
	CryptoPP::ECP::Point P; 
	CryptoPP::Integer p;
	CryptoPP::Integer order;

private:
	CryptoPP::Integer getSquareRoot(const CryptoPP::Integer& y);
	void GenerateCurve();
	void GenerateBasePoint();
	void CalcBasePointOrder();
};
