#pragma once

#include <array>
#include <cmath>

namespace common {

/**
 * @brief OpenDRIVE paramPoly3 representation
 *
 * u(p) = aU + bU*p + cU*p^2 + dU*p^3
 * v(p) = aV + bV*p + cV*p^2 + dV*p^3
 * p in [0, 1]
 */
struct OpenDriveParamPoly3 {
    double aU = 0.0;
    double bU = 0.0;
    double cU = 0.0;
    double dU = 0.0;

    double aV = 0.0;
    double bV = 0.0;
    double cV = 0.0;
    double dV = 0.0;
};

/**
 * @brief Cubic polynomial: f(x) = a*x^3 + b*x^2 + c*x + d
 */
class CubicPolynomial {
public:
    CubicPolynomial() = default;
    CubicPolynomial(double a, double b, double c, double d);

    // --- Basic evaluation ---
    [[nodiscard]] double Evaluate(double x) const;
    [[nodiscard]] double EvaluateDerivative(double x) const;
    [[nodiscard]] double EvaluateSecondDerivative(double x) const;

    // --- Geometry ---
    [[nodiscard]] double Curvature(double x) const;

    // --- Arc length ---
    [[nodiscard]] double CalculateArcLengthSimpson(
        double x1, double x2, int num_intervals = 200) const;

    [[nodiscard]] double CalculateArcLengthPolyline(
        double x1, double x2, int num_samples = 50) const;

    // --- Coefficients ---
    [[nodiscard]] std::array<double, 4> GetCoefficients() const;

    // OpenDRIVE
    [[nodiscard]] OpenDriveParamPoly3 ToOpenDriveParamPoly3(
        double x_start, double x_end) const;

    double a() const { return a_; }
    double b() const { return b_; }
    double c() const { return c_; }
    double d() const { return d_; }

private:
    double a_ = 0.0;
    double b_ = 0.0;
    double c_ = 0.0;
    double d_ = 0.0;
};

/**
 * @brief Cubic Hermite interpolation (closed-form, no matrix)
 */
CubicPolynomial CreateCubicHermite(double x1, double y1, double k1,
                                   double x2, double y2, double k2);

}  // namespace common
