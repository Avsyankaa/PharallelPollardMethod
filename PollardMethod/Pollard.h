#pragma once
#include "Curve.h"

#include <functional>

class PolardSolver {
public:
	PolardSolver(const CryptoPP::ECP& curve,
		const CryptoPP::ECP::Point& P,
		const CryptoPP::ECP::Point& Q,
		unsigned N,
		CryptoPP::Integer ord) :
		P(P),
		Q(Q),
		N(N),
		Ri(Q),
		Si(Q),
		curve(curve),
		ord(ord)
	{
		Qdbl = curve.Double(Q);
	};

	// ���������� ������ (����������� ����� + ����� ������������ �������)
	// P - �����, ������� ������� ���� ����������
	CryptoPP::ECP::Point P;
	// Q - ����������� ������� ���������
	CryptoPP::ECP::Point Q;
	CryptoPP::ECP::Point Qdbl;
	// N - ����� �����
	unsigned N;
	// ������� ����� Q
	CryptoPP::Integer ord;

	CryptoPP::ECP curve;

	// ��������� �������
	CryptoPP::Integer answer;
	

	void Solve();
	void PharallelSolve(unsigned w);

	// ������� ��������� ��������� �� ������: 
	// ����: �������� �������� i, ����� ����� N
	// �����: ������ 
	unsigned DivisionFunc(const CryptoPP::Integer& i, unsigned N);
	// ������� ������������: 
	// ����: ����� Ri
	// �����: ����� Ri+1 =  f(Ri)
	CryptoPP::ECP::Point PointIterFunc(const CryptoPP::ECP::Point& Ri, const CryptoPP::ECP& curve);
	// ������� ������������ ������������ rA: 
	// ����: ����� ai: Ri = ai * P + bi * Q
	// �����: ����� ai+1: Ri+1 = ai+1 * P + bi+1 * Q
	CryptoPP::Integer AIterFunc(const CryptoPP::Integer &ai, const CryptoPP::Integer& i);
	// ������� ������������ ������������ rB: 
	// ����: ����� bi: Ri = ai * P + bi * Q
	// �����: ����� bi+1: Ri+1 = ai+1 * P + bi+1 * Q
	CryptoPP::Integer BIterFunc(const CryptoPP::Integer& bi, const CryptoPP::Integer& i);
private:
	// ������ �� ������� �������� ��� ����������������� � ������������� ������
	CryptoPP::ECP::Point Ri;
	CryptoPP::Integer rAi;
	CryptoPP::Integer rBi;

	CryptoPP::ECP::Point Si;
	CryptoPP::Integer sAi;
	CryptoPP::Integer sBi;

	void CalcSolution(const CryptoPP::Integer& sAi,
		const CryptoPP::Integer& sBi,
		const CryptoPP::Integer& rAi,
		const CryptoPP::Integer& rBi);


	// ������ �� ������� �������� ��� ������������� ������
	struct IterSet {
		IterSet() = default;
		IterSet(const CryptoPP::ECP::Point& iterPoint,
			const CryptoPP::Integer& ai,
			const CryptoPP::Integer& bi) :
			ai(ai),
			bi(bi),
			iterPoint(iterPoint) {};
		CryptoPP::ECP::Point iterPoint;
		CryptoPP::Integer ai;
		CryptoPP::Integer bi;
	};

	std::vector<IterSet> RiSet;
	std::vector<IterSet> SiSet;
};