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
	Zmod operator+=(const Zmod& z) { return (*this) = int64 + z.int64; }
	Zmod operator-=(const Zmod& z) { return (*this) = int64 - z.int64; }
	Zmod operator*=(const Zmod& z) { return (*this) = int64 * z.int64; }

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

std::tuple<int64_t, int64_t, int64_t> extended_gcd(int64_t a, int64_t b) {
	if (b == 0) return {a, 1, 0};
	auto [g, x, y] = extended_gcd(b, a%b);
	return {g, y, x - y*(a/b)};
}

#endif
