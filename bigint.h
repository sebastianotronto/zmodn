#ifndef BIGUNSIGNED_H
#define BIGUNSIGNED_H

#include <cstdint>
#include <iostream>
#include <random>
#include <string_view>

constexpr uint64_t abs64(int64_t);
constexpr uint64_t pow10(uint64_t);

// Big integer class for numbers of at most N decimal digits.
// The number E is used to tune the size of each digit, mostly for
// testing purposes.

template<uint64_t N = 50, uint64_t E = 9>
requires (E < 10)
class BigInt {
public:
	// The member variables sign and digits are declared public so that
	// BigInt becomes a structural type and can be used in templates.

	static constexpr uint64_t M = pow10(E);
	static constexpr uint64_t D = (N / E) + 1;

	bool sign;
	uint64_t digits[D];

	constexpr BigInt() : sign{true} {
		std::fill(digits, digits+D, 0);
	}

	constexpr BigInt(int64_t n) : sign{n >= 0} {
		std::fill(digits, digits+D, 0);
		digits[0] = abs64(n);
		carryover();
	}

	constexpr BigInt(const std::string_view s) : sign{true} {
		std::fill(digits, digits+D, 0);
		if (s.size() == 0)
			return;
		for (int i = s.size()-1, j = 0; i >= 0; i--, j++) {
			if (s[i] == '\'')
				continue;
			if (i == 0 && s[i] == '-') {
				sign = false;
				break;
			}
			digits[j/E] += (pow10(j % E))
			    * static_cast<uint64_t>(s[i] - '0');
		}
	}

	constexpr auto operator<=>(const BigInt& other) const {
		if (sign != other.sign)
			return sign <=> other.sign;

		for (int i = D-1; i >= 0; i--)
			if (digits[i] != other.digits[i])
				return sign ?
				    digits[i] <=> other.digits[i] :
				    other.digits[i] <=> digits[i];

		return 0 <=> 0;
	}

	constexpr bool operator==(const BigInt& other) const = default;

	constexpr BigInt abs() const {
		BigInt ret = *this;
		ret.sign = true;
		return ret;
	}

	constexpr BigInt operator-() const {
		if (*this == 0)
			return 0;
		BigInt ret = *this;
		ret.sign = !ret.sign;
		return ret;
	}

	constexpr BigInt operator+(const BigInt& z) const {
		if (sign && z.sign)
			return positive_sum(*this, z);
		else if (sign && !z.sign)
			return positive_diff(*this, -z);
		else if (!sign && z.sign)
			return positive_diff(z, -*this);
		else
			return -positive_sum(-*this, -z);
	}

	constexpr BigInt operator-(const BigInt& z) const {
		return *this + (-z);
	}

	constexpr BigInt operator*(const BigInt& z) const {
		BigInt ret;
		ret.sign = !(sign ^ z.sign);
		for (int i = 0; i < D; i++)
			for (int j = 0; i+j < D; j++)
				ret.digits[i+j] += digits[i] * z.digits[j];
		ret.carryover();
		return ret;
	}

	constexpr BigInt operator/(const BigInt& z) const {
		auto [q, r] = euclidean_division(*this, z);
		return q;
	}

	constexpr BigInt operator%(const BigInt& z) const {
		auto [q, r] = euclidean_division(*this, z);
		return r;
	}

	constexpr BigInt operator+=(const BigInt& z) { return *this = *this + z; }
	constexpr BigInt operator++() { return *this += 1; }
	constexpr BigInt operator-=(const BigInt& z) { return *this = *this - z; }
	constexpr BigInt operator--() { return *this -= 1; }
	constexpr BigInt operator*=(const BigInt& z) { return *this = *this * z; }
	constexpr BigInt operator/=(const BigInt& z) { return *this = *this / z; }
	constexpr BigInt operator%=(const BigInt& z) { return *this = *this % z; }

	static BigInt random(BigInt r) {
		std::random_device rd;
		std::default_random_engine rng(rd());
		std::uniform_int_distribution<int> distribution(0, M-1);

		BigInt ret;
		for (uint64_t i = 0; i < D; i++)
			ret.digits[i] = distribution(rng);

		return ret % r;
	}

	friend std::ostream& operator<<(std::ostream& os, const BigInt<N, E>& z) {
		if (z == 0) {
			os << "0";
			return os;
		}

		if (!z.sign)
			os << "-";

		int j;
		for (j = z.D-1; z.digits[j] == 0; j--) ;
		os << z.digits[j]; // Top digit is not padded

		for (int i = j-1; i >= 0; i--) {
			std::string num = std::to_string(z.digits[i]);
			os << std::string(E - num.length(), '0') << num;
		}
		return os;
	}

private:
	constexpr void carryover() {
		for (int i = 1; i < D; i++) {
			auto c = digits[i-1] / M;
			digits[i-1] -= c * M;
			digits[i] += c;
		}
	}

	constexpr BigInt half() const {
		BigInt ret;
		uint64_t carry = 0;
		for (int i = D-1; i >= 0; i--) {
			ret.digits[i] += (digits[i] + M * carry) / 2;
			carry = digits[i] % 2;
		}
		return ret;
	}

	static constexpr BigInt powM(uint64_t e) {
		BigInt ret;
		ret.digits[e] = 1;
		return ret;
	}

	// Sum of non-negative integers
	static constexpr BigInt positive_sum(const BigInt& x, const BigInt& y) {
		BigInt ret;
		for (int i = 0; i < D; i++)
			ret.digits[i] = x.digits[i] + y.digits[i];
		ret.carryover();
		return ret;
	}

	// Difference of non-negative integers (result may be negative)
	static constexpr BigInt positive_diff(const BigInt& x, const BigInt& y) {
		if (y > x)
			return -positive_diff(y, x);

		BigInt ret;
		uint64_t carry = 0;
		for (int i = 0; i < D; i++) {
			uint64_t oldcarry = carry;
			if (x.digits[i] < y.digits[i] + oldcarry) {
				ret.digits[i] = M;
				carry = 1;
			} else {
				carry = 0;
			}
			ret.digits[i] += x.digits[i];
			ret.digits[i] -= y.digits[i] + oldcarry;
		}
		ret.carryover();
		return ret;
	}

	// Division with remainder, UB if y == 0
	static constexpr std::pair<BigInt, BigInt>
	euclidean_division(const BigInt& x, const BigInt& y) {
		auto [q, r] = positive_div(x.abs(), y.abs());
		if (x.sign && y.sign)
			return std::pair(q, r);
		else if (x.sign && !y.sign)
			return r == 0 ? std::pair(-q, 0) : std::pair(-q-1, y+r);
		else if (!x.sign && y.sign)
			return r == 0 ? std::pair(-q, r) : std::pair(-q-1, y-r);
		else
			return std::pair(q, -r);
	}

	// Division with remainder of non-negative integers, UB if y == 0
	// This method is inefficient (O(log(x/y)) BigInt multiplications)
	static constexpr std::pair<BigInt, BigInt>
	positive_div(const BigInt& x, const BigInt& y) {
		BigInt q = 0;
		BigInt r = x;

		if (y > x)
			return std::pair(q, r);

		BigInt lb = 0;
		BigInt ub = x;
		while (true) {
			BigInt q = (ub + lb).half();
			BigInt r = x - y*q;

			if (r < 0)
				ub = q;
			else if (r >= y)
				lb = q+1;
			else
				return std::pair(q, r);
		}
	}
};

constexpr uint64_t abs64(int64_t x) {
	return static_cast<uint64_t>(x > 0 ? x : -x);
}

constexpr uint64_t pow10(uint64_t e) {
	if (e == 0)
		return 1;
	else
		return 10 * pow10(e-1);
}

#endif
