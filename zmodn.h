#ifndef ZMODN_H
#define ZMODN_H

#include <cstdint>
#include <iostream>
#include <optional>
#include <tuple>
#include <type_traits>

template<typename INT>
requires std::is_integral_v<INT>
std::tuple<INT, INT, INT> extended_gcd(INT a, INT b) {
	if (b == 0) return {a, 1, 0};
	auto [g, x, y] = extended_gcd(b, a%b);
	return {g, y, x - y*(a/b)};
}

template<auto N>
requires(N > 1) && std::is_integral_v<decltype(N)>
class Zmod {
public:
	Zmod(decltype(N) z) : value{(z%N + N) % N} {}
	decltype(N) toint() const { return value; }

	Zmod operator+(const Zmod& z) const { return value + z.value; }
	Zmod operator-(const Zmod& z) const { return value - z.value; }
	Zmod operator*(const Zmod& z) const { return value * z.value; }
	Zmod operator+=(const Zmod& z) { return (*this) = value + z.value; }
	Zmod operator-=(const Zmod& z) { return (*this) = value - z.value; }
	Zmod operator*=(const Zmod& z) { return (*this) = value * z.value; }

	bool operator==(const Zmod& z) const { return value == z.value; }
	bool operator!=(const Zmod& z) const { return value != z.value; }

	std::optional<Zmod> inverse() const {
		auto [g, a, _] = extended_gcd(value, N);
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
		return os << "(" << z.value << " mod " << N << ")";
	}
private:
	decltype(N) value;
};

#endif
