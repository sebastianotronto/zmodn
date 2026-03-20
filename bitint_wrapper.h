#ifndef BITINT_WRAPPER_H
#define BITINT_WRAPPER_H

#include <cstdio>
#include <iostream>

// Wrapper class for _BitInt(N) (port of the C23 feature, compiler
// extension for C++ available in Clang and maybe GCC). This is needed
// because operator<< is not defined on _BitInt(N), apparently.

template<uint64_t N = 128>
class BitInt {
public:
	_BitInt(N) x;

	constexpr BitInt() : x{_BitInt(N)(0)} {}
	constexpr BitInt(int n) : x{_BitInt(N)(n)} {}
	constexpr BitInt(long long n) : x{_BitInt(N)(n)} {}
	constexpr BitInt(_BitInt(N) n) : x{n} {}
	constexpr auto operator<=>(const BitInt& b) const { return x <=> b.x; }
	constexpr bool operator==(const BitInt& b) const = default;
	constexpr BitInt operator+(const BitInt& b) const { return x + b.x; }
	constexpr BitInt operator-(const BitInt& b) const { return x - b.x; }
	constexpr BitInt operator*(const BitInt& b) const { return x * b.x; }
	constexpr BitInt operator/(const BitInt& b) const { return x / b.x; }
	constexpr BitInt operator%(const BitInt& b) const { return x % b.x; }
	constexpr BitInt operator-() const { return -x; }
	constexpr BitInt operator+=(const BitInt& b) { return *this = *this + b; }
	constexpr BitInt operator-=(const BitInt& b) { return *this = *this - b; }
	constexpr BitInt operator*=(const BitInt& b) { return *this = *this * b; }
	constexpr BitInt operator/=(const BitInt& b) { return *this = *this / b; }
	constexpr BitInt operator%=(const BitInt& b) { return *this = *this % b; }

	friend std::ostream& operator<<(std::ostream& os, const BitInt<N>& b) {
		if (b > 0) {
			std::string s;
			auto bb = b;
			while (bb != 0) {
				char c = (bb.x % 10) + '0';
				s = c + s;
				bb /= 10;
			}
			return os << s;
		} else if (b < 0) {
			return os << "-" << -b;
		} else return os << "0";
	}
};

#endif
