#include "Pollard.h"

#include <iostream>

unsigned PolardSolver::DivisionFunc(const CryptoPP::Integer& i, unsigned N) {
	return (i % N) ;
}

CryptoPP::ECP::Point PolardSolver::PointIterFunc(const CryptoPP::ECP::Point& Ri, const CryptoPP::ECP& curve) {
	unsigned ni = DivisionFunc(Ri.x, N);
	switch (ni)
	{
	case 0:
		return curve.Add(Q, Ri);
	case 1:
		return curve.Add(P, Ri);
	case 2:
		return curve.Add(Qdbl, Ri);
	default:
		break;
	}
}

CryptoPP::Integer PolardSolver::BIterFunc(const CryptoPP::Integer& bi, const CryptoPP::Integer& i) {
	unsigned ni = DivisionFunc(i, N);
	switch (ni)
	{
	case 0:
		return (bi + 1) % ord;
	case 1:
		return (bi) % ord;
	case 2:
		return (bi + 2) % ord;
	default:
		break;
	}
}

CryptoPP::Integer PolardSolver::AIterFunc(const CryptoPP::Integer& ai, const CryptoPP::Integer& i) {
	unsigned ni = DivisionFunc(i, N);
	switch (ni)
	{
	case 0:
		return ai % ord;
	case 1:
		return (ai + 1) % ord;
	case 2:
		return ai % ord;
	default:
		break;
	}
}

void PolardSolver::Solve() {
	// В качестве стартовой точки берем точку Q
	Ri = Si = Q;
	rAi = sAi = 0;
	rBi = sBi = 1;

	for (CryptoPP::Integer i = 1; ; i++) {
		// Цикл
		// Вычисляем Ri, Si = R2i, сравниваем
		rAi = AIterFunc(rAi, Ri.x);
		rBi = BIterFunc(rBi, Ri.x);
		Ri = PointIterFunc(Ri, curve);
		
		auto Ai_tmp = AIterFunc(sAi, Si.x);
		auto Bi_tmp = BIterFunc(sBi, Si.x);
		auto Si_tmp = PointIterFunc(Si, curve);
		sAi = AIterFunc(Ai_tmp, Si_tmp.x);
		sBi = BIterFunc(Bi_tmp, Si_tmp.x);
		Si = PointIterFunc(Si_tmp, curve);

		if (Ri == Si) {
			CalcSolution(sAi, sBi, rAi, rBi);
			return;
		}
	}
}

void PolardSolver::CalcSolution(const CryptoPP::Integer& sAi,
	const CryptoPP::Integer& sBi,
	const CryptoPP::Integer& rAi,
	const CryptoPP::Integer& rBi) {
	auto bDiff = (rBi - sBi) % ord;
	if (bDiff < 0) {
		bDiff += ord;
	}
	auto aDiff = (sAi - rAi) % ord;
	if (aDiff < 0) {
		aDiff += ord;
	}
	answer = ((bDiff)*curve.GetField().Divide(1, aDiff)) % curve.GetField().GetModulus();
}

void PolardSolver::PharallelSolve(unsigned w) {
	Ri = Si = Q;
	rAi = sAi = 0;
	rBi = sBi = 1;

	RiSet.resize(w + 1);
	SiSet.resize(w + 1);

	auto calcRi = [this, w]() {
		for (unsigned i = 1; i <= w; i++) {
			rAi = AIterFunc(rAi, Ri.x);
			rBi = BIterFunc(rBi, Ri.x);
			Ri = PointIterFunc(Ri, curve);
			RiSet[i] = IterSet(Ri, rAi, rBi);
		}
	};
	auto curveCopy = curve;

	auto calcSi = [this, w, curveCopy]() {
		for (unsigned i = 1; i <= w; i++) {
			auto Ai_tmp = AIterFunc(sAi, Si.x);
			auto Bi_tmp = BIterFunc(sBi, Si.x);
			auto Si_tmp = PointIterFunc(Si, curveCopy);
			sAi = AIterFunc(Ai_tmp, Si_tmp.x);
			sBi = BIterFunc(Bi_tmp, Si_tmp.x);
			Si = PointIterFunc(Si_tmp, curveCopy);
			SiSet[i] = IterSet(Si, sAi, sBi);
		}
	};
	std::atomic_bool stopCondition = false;

	auto compareRiSi = [this, &stopCondition, w](bool firstPart) {
		unsigned start;
		unsigned end;
		if (firstPart) {
			start = 1;
			end = w / 2;
		}
		else {
			start = (w / 2) + 1;
			end = w;
		}
		for (unsigned i = start; i <= end; i++) {
			if (stopCondition) {
				break;
			}
			if (RiSet[i].iterPoint == SiSet[i].iterPoint) {
				stopCondition = true;
				CalcSolution(SiSet[i].ai, SiSet[i].bi, RiSet[i].ai, RiSet[i].bi);
				break;
			}
		}
	};

	while (!stopCondition) {
		std::thread calcRiTh(calcRi);
		std::thread calcSiTh(calcSi);

		calcRiTh.join();
		calcSiTh.join();

		std::thread compRiSiFirst(compareRiSi, true);
		std::thread compRiSiSecond(compareRiSi, false);

		compRiSiFirst.join();
		compRiSiSecond.join();
	}

	return;
}

