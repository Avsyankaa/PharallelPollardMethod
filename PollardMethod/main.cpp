#include <iostream>
#include <thread>
#include "Pollard.h"

#include <eccrypto.h>

#include <chrono>

#include "Curve.h"

int main() {
	
	CryptoPP::Integer p("50000017");
	CryptoPP::Integer a("500001");
	CryptoPP::Integer b("4000011");
	CryptoPP::Integer ord("49987521");

	CryptoPP::ECP curve = CryptoPP::ECP::ECP(p, a, b);

	CryptoPP::ECP::Point Q(0, 13457149);

	CryptoPP::Integer deg;
	std::cout << "Enter degree of q: " << std::endl;
	std::cin >> deg;
	
	CryptoPP::ECP::Point P = curve.Multiply(deg, Q);

	PolardSolver ps(curve, P, Q, 3, ord);

	auto begin = std::chrono::steady_clock::now();
	ps.Solve();
	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	std::cout << "The one thread time: " << elapsed_ms.count() << " ms\n";
	std::cout << "P = Q * " << ps.answer << "\n";
}