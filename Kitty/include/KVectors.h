/**
 * Kitty Engine
 * KVectors.h
 *
 * Custom implementation of vectors written specifically for the Kitty engine.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KTYPES_H
#define KENGINE_KTYPES_H

#include <cmath>

namespace Kitty
{
	template <class T>
	class Vector2
	{
	public:
		// Variables

		T x;
		T y;

		// Constructors

		//! Empty constructor
		Vector2() : x(0), y(0) {}

		//! Assign values to vector
		Vector2(T vx, T vy) : x(vx), y(vy) {}

		// Operators

		//! Assign 2D vector
		Vector2<T>& operator=(const Vector2<T>&value) { x = value.x; y = value.y; return *this; }

		//! Negate
		Vector2<T> operator-() const { return Vector2<T>(-x, -y); }

		//! 2D vector addition
		Vector2<T> operator+(const Vector2<T>&addend) const { return Vector2<T>(x + addend.x, y + addend.y); }
		//! 2D vector addition
		Vector2<T>& operator+=(const Vector2<T>&addend) { x += addend.x; y += addend.y; return *this; }
		//! Scalar addition
		Vector2<T> operator+(const T addend) const { return Vector2<T>(x + addend, y + addend); }
		//! Scalar addition
		Vector2<T>& operator+=(const T addend) { x += addend; y += addend; return *this; }

		//! 2D vector subtraction
		Vector2<T> operator-(const Vector2<T>&subtrahend) const { return Vector2<T>(x - subtrahend.x, y - subtrahend.y); }
		//! 2D vector subtraction
		Vector2<T>& operator-=(const Vector2<T>&subtrahend) { x -= subtrahend.x; y -= subtrahend.y; return *this; }
		//! Scalar subtraction
		Vector2<T> operator-(const T subtrahend) const { return Vector2<T>(x - subtrahend, y - subtrahend); }
		//! Scalar subtraction
		Vector2<T>& operator-=(const T subtrahend) { x -= subtrahend; y -= subtrahend; return *this; }

		//! 2D vector multiplication
		Vector2<T> operator*(const Vector2<T>&factor) const { return Vector2<T>(x * factor.x, y * factor.y); }
		//! 2D vector multiplication
		Vector2<T>& operator*=(const Vector2<T>&factor) { x *= factor.x; y *= factor.y; return *this; }
		//! Scalar multiplication
		Vector2<T> operator*(const T factor) const { return Vector2<T>(x * factor, y * factor); }
		//! Scalar multiplication
		Vector2<T>& operator*=(const T factor) { x *= factor; y *= factor; return *this; }

		//! 2D vector division
		Vector2<T> operator/(const Vector2<T>&divisor) const { return Vector2<T>(x / divisor.x, y / divisor.y); }
		//! 2D vector division
		Vector2<T>& operator/=(const Vector2<T>&divisor) { x /= divisor.x; y /= divisor.y; return *this; }
		//! Scalar division
		Vector2<T> operator/(const T divisor) const { return Vector2<T>(x / divisor, y / divisor); }
		//! Scalar division
		Vector2<T>& operator/=(const T divisor) { x /= divisor; y /= divisor; return *this; }

		//! 2D vector equals
		bool operator==(const Vector2<T>&comparator) { return (x == comparator.x && y == comparator.y); }
		//! 2D vector does not equal
		bool operator!=(const Vector2<T>&comparator) { return (x != comparator.x || y != comparator.y); }
		//! 2D vector less
		bool operator<(const Vector2<T>&comparator) { return (x < comparator.x && y < comparator.y); }
		//! 2D vector more
		bool operator>(const Vector2<T>&comparator) { return (x > comparator.x && y > comparator.y); }
		//! 2D vector less or equals
		bool operator<=(const Vector2<T>&comparator) { return (!(x > comparator.x) && !(y > comparator.y)); }
		//! 2D vector more or equals
		bool operator>=(const Vector2<T>&comparator) { return (!(comparator.x > x) && !(comparator.y > y)); }

		/**
		 * \brief Get vector magnitude.
		 *
		 * Gets the magnitude (length) of the vector.
		 *
		 * \return Length of the vector
		 */
		T GetMagnitude() const { return sqrt(x*x + y*y); }

		/**
		 * \brief Get squared vector magnitude.
		 *
		 * Gets the squared magnitude (length) of the vector. This is faster than getMagnitude() because we can skip
		 * taking the square root. Use this if you only want to compare vector magnitudes.
		 *
		 * \return Square length of the vector
		 */
		T GetMagnitudeSq() const { return (x*x + y*y); }

		//! 2D double vector
		typedef Vector2<double> Vector2d;

		//! 2D float vector
		typedef Vector2<float> Vector2f;

		//! 2D long vector
		typedef Vector2<long> Vector2l;

		//! 2D integer vector
		typedef Vector2<int> Vector2i;
	};
}

#endif //KENGINE_KTYPES_H
