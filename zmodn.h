#ifndef ZMODN_H
#define ZMODN_H

#include <cstdint>
#include <iostream>
#include <optional>
#include <tuple>

std::tuple<int64_t, int64_t, int64_t> extended_gcd(int64_t, int64_t);

template<int64_t N> requires(N > 1)
class Zmod {
public:
	Zmod(int64_t z) : int64{(z%N + N) % N} {}
	int64_t toint64() const { return int64; }

	Zmod operator+(const Zmod& z) const { return int64 + z.int64; }
	Zmod operator-(const Zmod& z) const { return int64 - z.int64; }
	Zmod operator*(const Zmod& z) const { return int64 * z.int64; }
	Zmod operator+=(const Zmod& z) { return int64 += z.int64; }
	Zmod operator-=(const Zmod& z) { return int64 -= z.int64; }
	Zmod operator*=(const Zmod& z) { return int64 *= z.int64; }

	bool operator==(const Zmod& z) const { return int64 == z.int64; }
	bool operator!=(const Zmod& z) const { return int64 != z.int64; }

	std::optional<Zmod> inverse() const {
		auto [g, a, _] = extended_gcd(int64, N);
		return g == 1 ? Zmod(a) : std::optional<Zmod>{};
	}

	std::optional<Zmod> operator/(const Zmod& d) const {
		auto i = d.inverse();
		return i ? (*this) * i.value() : i;
	}

	std::optional<Zmod> operator/=(const Zmod& d) {
		auto q = *this / d;
		return q ? (*this = q.value()) : q;
	}

	friend std::ostream& operator<<(std::ostream& os, const Zmod<N>& z) {
		return os << "(" << z.int64 << " mod " << N << ")";
	}
private:
	int64_t int64;
};

void swapdiv(int64_t& oldx, int64_t& x, int64_t q) {
	int64_t tmp = x;
	x = oldx - q*tmp;
	oldx = tmp;
}

std::tuple<int64_t, int64_t, int64_t> extended_gcd(int64_t a, int64_t b) {
	int64_t oldr = a;
	int64_t r = b;
	int64_t olds = 1;
	int64_t s = 0;
	int64_t oldt = 0;
	int64_t t = 1;
	while (r != 0) {
		auto q = oldr / r;
		swapdiv(oldr, r, q);
		swapdiv(olds, s, q);
		swapdiv(oldt, t, q);
	}
	return {oldr, olds, oldt};
}

#endif
