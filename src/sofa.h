#ifndef SOFA_H
#define SOFA_H

#include <string>
#include <vector>
#include <mysofa.h>

class SOFA {
public:
    SOFA(const std::string &filename, float sample_rate);
    ~SOFA();
    std::vector<float> get_hrtf(float x, float y, float z);
private:
    MYSOFA_EASY *hrtf;
    int filter_len;
};

#endif // SOFA_H
