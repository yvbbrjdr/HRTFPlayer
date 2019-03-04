#include "sofa.h"
#include <stdexcept>
using namespace std;

SOFA::SOFA(const string &filename, float sample_rate)
{
    int err;
    hrtf = mysofa_open(filename.c_str(), sample_rate, &filter_len, &err);
    if (hrtf == nullptr)
        throw runtime_error("sofa file open failed: " + to_string(err));
}

SOFA::~SOFA()
{
    mysofa_close(hrtf);
}

vector<float> SOFA::get_hrtf(float x, float y, float z)
{
    vector<float> ret(size_t(filter_len * 2));
    float _;
    mysofa_getfilter_float(hrtf, x, y, z, ret.data(), ret.data() + filter_len, &_, &_);
    return ret;
}
