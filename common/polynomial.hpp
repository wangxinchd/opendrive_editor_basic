#pragma once

#include <array>
#include <vector>

namespace common {

/**
 * @brief Represents a cubic polynomial: f(x) = a*x^3 + b*x^2 + c*x + d
 */
class CubicPolynomial {
public:
    /**
     * @brief Default constructor
     */
    CubicPolynomial() = default;

    /**
     * @brief Constructor with coefficients
     * @param a x^3 coefficient
     * @param b x^2 coefficient
     * @param c x coefficient
     * @param d constant term
     */
    CubicPolynomial(double a, double b, double c, double d);

    /**
     * @brief Evaluate the polynomial at given x
     * @param x Input value
     * @return f(x)
     */
    double Evaluate(double x) const;

    /**
     * @brief Evaluate the derivative at given x
     * @param x Input value
     * @return f'(x)
     */
    double EvaluateDerivative(double x) const;

    /**
     * @brief Get polynomial coefficients as array [a, b, c, d]
     * @return Array of coefficients
     */
    std::array<double, 4> GetCoefficients() const;

    /**
     * @brief Get x^3 coefficient
     */
    double GetA() const { return a_; }

    /**
     * @brief Get x^2 coefficient
     */
    double GetB() const { return b_; }

    /**
     * @brief Get x coefficient
     */
    double GetC() const { return c_; }

    /**
     * @brief Get constant term
     */
    double GetD() const { return d_; }

    /**
     * @brief Calculate the arc length of the curve between x1 and x2
     *
     * The arc length is computed using numerical integration (Simpson's rule)
     * with adaptive subdivision for accuracy.
     *
     * @param x1 Lower bound
     * @param x2 Upper bound (must be > x1)
     * @param num_intervals Number of intervals for integration (default: 200)
     * @return Arc length between x1 and x2
     */
    double CalculateArcLength(double x1, double x2, int num_intervals = 200) const;

private:
    double a_ = 0.0;  // x^3 coefficient
    double b_ = 0.0;  // x^2 coefficient
    double c_ = 0.0;  // x coefficient
    double d_ = 0.0;  // constant term
};

/**
 * @brief Create a cubic polynomial that interpolates two points with given slopes
 *
 * Given two points (x1, y1), (x2, y2) and their slopes k1, k2,
 * this function constructs a cubic polynomial f(x) = ax^3 + bx^2 + cx + d
 * such that:
 * - f(x1) = y1
 * - f(x2) = y2
 * - f'(x1) = k1
 * - f'(x2) = k2
 *
 * @param x1 First point x-coordinate
 * @param y1 First point y-coordinate
 * @param k1 Slope at first point
 * @param x2 Second point x-coordinate
 * @param y2 Second point y-coordinate
 * @param k2 Slope at second point
 * @return Cubic polynomial that satisfies all conditions
 */
CubicPolynomial CreateCubicHermite(double x1, double y1, double k1,
                                   double x2, double y2, double k2);

}  // namespace common
