#include "polynomial.hpp"

#include <cmath>

namespace common {

CubicPolynomial::CubicPolynomial(double a, double b, double c, double d)
    : a_(a), b_(b), c_(c), d_(d) {}

double CubicPolynomial::Evaluate(double x) const {
    return a_ * x * x * x + b_ * x * x + c_ * x + d_;
}

double CubicPolynomial::EvaluateDerivative(double x) const {
    return 3.0 * a_ * x * x + 2.0 * b_ * x + c_;
}

std::array<double, 4> CubicPolynomial::GetCoefficients() const {
    return {a_, b_, c_, d_};
}

double CubicPolynomial::CalculateArcLength(double x1, double x2, int num_intervals) const {
    if (x2 <= x1) {
        return 0.0;
    }

    if (num_intervals <= 0) {
        num_intervals = 1000;
    }

    // Use Simpson's rule for numerical integration
    // ∫√(1 + (f'(x))²) dx from x1 to x2

    const double h = (x2 - x1) / num_intervals;
    double sum = 0.0;

    // First and last points
    const double f_prime_x1 = EvaluateDerivative(x1);
    const double f_prime_x2 = EvaluateDerivative(x2);
    sum += std::sqrt(1.0 + f_prime_x1 * f_prime_x1);
    sum += std::sqrt(1.0 + f_prime_x2 * f_prime_x2);

    // Interior points
    for (int i = 1; i < num_intervals; ++i) {
        const double x = x1 + i * h;
        const double f_prime = EvaluateDerivative(x);
        const double integrand = std::sqrt(1.0 + f_prime * f_prime);

        if (i % 2 == 0) {
            // Even indices (2, 4, 6, ...)
            sum += 2.0 * integrand;
        } else {
            // Odd indices (1, 3, 5, ...)
            sum += 4.0 * integrand;
        }
    }

    return sum * h / 3.0;
}

CubicPolynomial CreateCubicHermite(double x1, double y1, double k1,
                                   double x2, double y2, double k2) {
    // Handle the special case where x1 == x2
    if (std::abs(x1 - x2) < 1e-12) {
        // Cannot interpolate with identical x values
        return CubicPolynomial(0.0, 0.0, k1, y1);  // Return constant polynomial with slope k1
    }

    // For cubic Hermite interpolation, we solve the system:
    // f(x1) = y1:   a*x1^3 + b*x1^2 + c*x1 + d = y1
    // f(x2) = y2:   a*x2^3 + b*x2^2 + c*x2 + d = y2
    // f'(x1) = k1:  3a*x1^2 + 2b*x1 + c = k1
    // f'(x2) = k2:  3a*x2^2 + 2b*x2 + c = k2

    const double x1_2 = x1 * x1;  // x1^2
    const double x1_3 = x1_2 * x1;  // x1^3
    const double x2_2 = x2 * x2;  // x2^2
    const double x2_3 = x2_2 * x2;  // x2^3

    // We can solve this system using Gaussian elimination or matrix inversion
    // Let's use a direct approach by computing the coefficients

    // Let h = x2 - x1
    const double h = x2 - x1;

    // The cubic Hermite interpolation polynomial can be written as:
    // f(x) = (1 - t)^3 * y1 + t^3 * y2 + t*(1-t)^2 * k1 * h + t^2*(1-t) * k2 * h
    // where t = (x - x1)/h

    // To get monomial coefficients, we need to expand this expression
    // Let's compute the coefficients by expanding the parametric form

    // First, let's compute powers of h
    const double h_2 = h * h;
    const double h_3 = h_2 * h;

    // The general form for Hermite cubic interpolation:
    // f(x) = a*x^3 + b*x^2 + c*x + d
    //
    // We can derive the coefficients by solving the system or using the standard formula

    // Using the standard Hermite interpolation formula:
    // For two points, the cubic polynomial is:
    // f(x) = y1*(1 + 2*(x-x1)/h) * ((x-x2)/h)^2 +
    //        y2*(1 + 2*(x-x2)/h) * ((x-x1)/h)^2 +
    //        k1*(x-x1)*((x-x2)/h)^2 +
    //        k2*(x-x2)*((x-x1)/h)^2

    // Let me simplify this. Actually, let me use a direct matrix approach.
    // Let's solve the 4x4 system directly.

    // Matrix form:
    // [x1^3, x1^2, x1, 1]   [a]   [y1]
    // [x2^3, x2^2, x2, 1]   [b]   [y2]
    // [3x1^2, 2x1, 1, 0]    [c] = [k1]
    // [3x2^2, 2x2, 1, 0]    [d]   [k2]

    // Let's implement Gaussian elimination for this 4x4 system

    // Augmented matrix [coefficients | constants]
    double mat[4][5] = {
        {x1_3, x1_2, x1, 1.0, y1},
        {x2_3, x2_2, x2, 1.0, y2},
        {3.0 * x1_2, 2.0 * x1, 1.0, 0.0, k1},
        {3.0 * x2_2, 2.0 * x2, 1.0, 0.0, k2}
    };

    // Gaussian elimination
    for (int i = 0; i < 4; ++i) {
        // Find pivot
        int max_row = i;
        for (int k = i + 1; k < 4; ++k) {
            if (std::abs(mat[k][i]) > std::abs(mat[max_row][i])) {
                max_row = k;
            }
        }

        // Swap rows
        if (max_row != i) {
            for (int k = 0; k < 5; ++k) {
                std::swap(mat[i][k], mat[max_row][k]);
            }
        }

        // Eliminate
        for (int k = i + 1; k < 4; ++k) {
            double factor = mat[k][i] / mat[i][i];
            for (int j = i; j < 5; ++j) {
                mat[k][j] -= factor * mat[i][j];
            }
        }
    }

    // Back substitution
    double coeffs[4];
    for (int i = 3; i >= 0; --i) {
        coeffs[i] = mat[i][4];
        for (int j = i + 1; j < 4; ++j) {
            coeffs[i] -= mat[i][j] * coeffs[j];
        }
        coeffs[i] /= mat[i][i];
    }

    return CubicPolynomial(coeffs[0], coeffs[1], coeffs[2], coeffs[3]);
}

}  // namespace common
