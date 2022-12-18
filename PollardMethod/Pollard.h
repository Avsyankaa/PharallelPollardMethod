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

	// Глобальные данные (порождающая точка + точка неизвестного порядка)
	// P - точка, порядой которой надо определить
	CryptoPP::ECP::Point P;
	// Q - порождающий элемент подгруппы
	CryptoPP::ECP::Point Q;
	CryptoPP::ECP::Point Qdbl;
	// N - число групп
	unsigned N;
	// порядок точки Q
	CryptoPP::Integer ord;

	CryptoPP::ECP curve;

	// результат решения
	CryptoPP::Integer answer;
	

	void Solve();
	void PharallelSolve(unsigned w);

	// Функция разбиения множества на группы: 
	// вход: значение итерации i, число групп N
	// выход: группа 
	unsigned DivisionFunc(const CryptoPP::Integer& i, unsigned N);
	// Функция итерирования: 
	// вход: точка Ri
	// выход: точка Ri+1 =  f(Ri)
	CryptoPP::ECP::Point PointIterFunc(const CryptoPP::ECP::Point& Ri, const CryptoPP::ECP& curve);
	// Функция итерирования коэффициента rA: 
	// вход: точка ai: Ri = ai * P + bi * Q
	// выход: точка ai+1: Ri+1 = ai+1 * P + bi+1 * Q
	CryptoPP::Integer AIterFunc(const CryptoPP::Integer &ai, const CryptoPP::Integer& i);
	// Функция итерирования коэффициента rB: 
	// вход: точка bi: Ri = ai * P + bi * Q
	// выход: точка bi+1: Ri+1 = ai+1 * P + bi+1 * Q
	CryptoPP::Integer BIterFunc(const CryptoPP::Integer& bi, const CryptoPP::Integer& i);
private:
	// Данные по текущей итерации для последовательного и параллельного метода
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


	// Данные по текущей итерации для параллельного метода
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