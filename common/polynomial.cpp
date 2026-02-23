#include "polynomial.hpp"

namespace common {

// =======================
// CubicPolynomial
// =======================

CubicPolynomial::CubicPolynomial(double a, double b, double c, double d)
    : a_(a), b_(b), c_(c), d_(d) {}

double CubicPolynomial::Evaluate(double x) const {
  // Horner's method
  return ((a_ * x + b_) * x + c_) * x + d_;
}

double CubicPolynomial::EvaluateDerivative(double x) const {
  return (3.0 * a_ * x + 2.0 * b_) * x + c_;
}

double CubicPolynomial::EvaluateSecondDerivative(double x) const {
  return 6.0 * a_ * x + 2.0 * b_;
}

double CubicPolynomial::Curvature(double x) const {
  const double fp = EvaluateDerivative(x);
  const double fpp = EvaluateSecondDerivative(x);
  const double denom = std::pow(1.0 + fp * fp, 1.5);
  return denom > 1e-12 ? std::abs(fpp) / denom : 0.0;
}

std::array<double, 4> CubicPolynomial::GetCoefficients() const {
  return {a_, b_, c_, d_};
}

OpenDriveParamPoly3 CubicPolynomial::ToOpenDriveParamPoly3(double x_start,
                                                           double x_end) const {

  OpenDriveParamPoly3 poly;

  const double L = x_end - x_start;
  if (std::abs(L) < 1e-12) {
    return poly;
  }

  // u(p) = x = x_start + L * p
  poly.aU = x_start;
  poly.bU = L;
  poly.cU = 0.0;
  poly.dU = 0.0;

  // x(p) = x_start + L*p
  // y(p) = f(x(p))
  //
  // Expand:
  // y = a(x0 + Lp)^3 + b(x0 + Lp)^2 + c(x0 + Lp) + d

  const double x0 = x_start;
  const double L2 = L * L;
  const double L3 = L2 * L;

  poly.aV = a_ * x0 * x0 * x0 + b_ * x0 * x0 + c_ * x0 + d_;

  poly.bV = 3.0 * a_ * x0 * x0 * L + 2.0 * b_ * x0 * L + c_ * L;

  poly.cV = 3.0 * a_ * x0 * L2 + b_ * L2;

  poly.dV = a_ * L3;

  return poly;
}

// =======================
// Arc length
// =======================

double CubicPolynomial::CalculateArcLengthSimpson(double x1, double x2,
                                                  int n) const {

  if (x2 <= x1 || n <= 0)
    return 0.0;
  if (n % 2 != 0)
    ++n;

  const double h = (x2 - x1) / n;
  double sum = 0.0;

  auto integrand = [&](double x) {
    const double fp = EvaluateDerivative(x);
    return std::sqrt(1.0 + fp * fp);
  };

  sum += integrand(x1) + integrand(x2);

  for (int i = 1; i < n; ++i) {
    sum += (i % 2 ? 4.0 : 2.0) * integrand(x1 + i * h);
  }

  return sum * h / 3.0;
}

double CubicPolynomial::CalculateArcLengthPolyline(double x1, double x2,
                                                   int samples) const {

  if (x2 <= x1 || samples < 2)
    return 0.0;

  double length = 0.0;
  const double dx = (x2 - x1) / (samples - 1);

  double px = x1;
  double py = Evaluate(px);

  for (int i = 1; i < samples; ++i) {
    double x = x1 + i * dx;
    double y = Evaluate(x);
    length += std::hypot(x - px, y - py);
    px = x;
    py = y;
  }
  return length;
}

// =======================
// Cubic Hermite (closed-form)
// =======================

CubicPolynomial CreateCubicHermite(double x1, double y1, double k1, double x2,
                                   double y2, double k2) {

  const double h = x2 - x1;
  if (std::abs(h) < 1e-12) {
    return CubicPolynomial(0.0, 0.0, k1, y1);
  }

  const double h2 = h * h;
  const double h3 = h2 * h;

  const double a = (2.0 * (y1 - y2) + (k1 + k2) * h) / h3;
  const double b = (3.0 * (y2 - y1) - (2.0 * k1 + k2) * h) / h2;
  const double c = k1;
  const double d = y1;

  // Convert from (x - x1) form to x form
  const double A = a;
  const double B = b - 3 * a * x1;
  const double C = c - 2 * b * x1 + 3 * a * x1 * x1;
  const double D = d - c * x1 + b * x1 * x1 - a * x1 * x1 * x1;

  return CubicPolynomial(A, B, C, D);
}

} // namespace common
