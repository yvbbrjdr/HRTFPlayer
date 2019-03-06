#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <complex>

typedef std::complex<double> Complex;

std::vector<Complex> FFT(const std::vector<double> &a, size_t len);
std::vector<double> IFFT(const std::vector<Complex> &a, size_t len);
std::vector<double> FFT_conv(const std::vector<double> &a, const std::vector<double> &b);
std::vector<double> FFT_conv2(const std::vector<Complex> &a, const std::vector<double> &b);

#endif // UTILS_H
