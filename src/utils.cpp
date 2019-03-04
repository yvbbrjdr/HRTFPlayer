#include "utils.h"
#include <complex>
#include <cmath>
using namespace std;

typedef complex<double> Complex;

static vector<Complex> FFT(const vector<double> &a, size_t len);
static vector<double> IFFT(const vector<Complex> &a, size_t len);

vector<double> FFT_conv(const vector<double> &a, const vector<double> &b)
{
    size_t ifft_len = max(a.size(), b.size());
    size_t fft_len = size_t(pow(2, ceil(log2(ifft_len))));
    vector<Complex> fa = FFT(a, fft_len),
                    fb = FFT(b, fft_len);
    for (size_t i = 0; i < fa.size(); ++i)
        fa[i] *= fb[i];
    return IFFT(fa, ifft_len);
}

static vector<Complex> FFT_internal(const vector<Complex> &a, const Complex &omega);

vector<Complex> FFT(const vector<double> &a, size_t len)
{
    vector<Complex> input;
    for (double d : a)
        input.emplace_back(d);
    long diff = long(len - input.size());
    for (long i = 0; i < diff; ++i)
        input.emplace_back();
    return FFT_internal(input, polar(1.0, M_PI * 2 / len));
}

vector<double> IFFT(const vector<Complex> &a, size_t len)
{
    size_t n = a.size();
    vector<Complex> output = FFT_internal(a, polar(1.0, -M_PI * 2 / n));
    vector<double> ret;
    for (size_t i = 0; i < len; ++i)
        ret.emplace_back(output[i].real() / n);
    return ret;
}

vector<Complex> FFT_internal(const vector<Complex> &a, const Complex &omega)
{
    size_t n = a.size();
    if (n == 1)
        return a;
    vector<Complex> even, odd;
    for (size_t i = 0; i < n; ++i)
        if (i % 2 == 0)
            even.push_back(a[i]);
        else
            odd.push_back(a[i]);
    vector<Complex> re = FFT_internal(even, omega * omega),
                    ro = FFT_internal(odd, omega * omega),
                    ret;
    ret.resize(n);
    Complex t(1.0);
    for (size_t i = 0; i < n / 2; ++i, t *= omega) {
        Complex tmp = t * ro[i];
        ret[i] = re[i] + tmp;
        ret[i + n / 2] = re[i] - tmp;
    }
    return ret;
}
