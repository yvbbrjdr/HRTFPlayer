#include "player.h"
#include <cmath>
#include <stdexcept>
#include <bassenc.h>
#include "utils.h"
using namespace std;

bool Player::inited = false;

Player::Player() : sofa(nullptr), handle(0), x(1), y(0), z(0), prev_x(1), prev_y(0), prev_z(0)
{
    init();
}

Player::~Player()
{
    if (handle != 0)
        BASS_StreamFree(handle);
    if (sofa != nullptr)
        delete sofa;
}

void Player::open(const std::string &filename)
{
    if (handle != 0)
        BASS_StreamFree(handle);
    prev_data.clear();
    handle = BASS_StreamCreateFile(FALSE, filename.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
    if (handle == 0)
        throw runtime_error("file open failed");
    BASS_ChannelSetDSP(handle, DSP, this, 0);
}

void Player::open_hrtf(const std::string &filename)
{
    SOFA *tmp = new SOFA(filename, SAMPLE_RATE);
    swap(sofa, tmp);
    if (tmp != nullptr)
        delete tmp;
}

void Player::play()
{
    BASS_ChannelPlay(handle, FALSE);
}

void Player::pause()
{
    BASS_ChannelPause(handle);
}

void Player::stop()
{
    BASS_Encode_Stop(handle);
    BASS_ChannelStop(handle);
    prev_data.clear();
    set_position(0);
}

void Player::set_source(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

double Player::get_length()
{
    if (handle == 0)
        return 1;
    return BASS_ChannelBytes2Seconds(handle, BASS_ChannelGetLength(handle, BASS_POS_BYTE));
}

double Player::get_position()
{
    if (handle == 0)
        return 0;
    return BASS_ChannelBytes2Seconds(handle, BASS_ChannelGetPosition(handle, BASS_POS_BYTE));
}

void Player::set_position(double pos)
{
    if (handle == 0)
        return;
    BASS_ChannelSetPosition(handle, BASS_ChannelSeconds2Bytes(handle, pos), BASS_POS_BYTE);
}

void Player::set_record(const std::string &filename)
{
    BASS_Encode_Stop(handle);
    BASS_Encode_Start(handle, filename.c_str(), BASS_ENCODE_PCM, nullptr, nullptr);
}

void Player::init()
{
    if (!inited) {
        if (BASS_Init(-1, SAMPLE_RATE, 0, nullptr, nullptr) == FALSE)
            throw runtime_error("bass init failed");
        BASS_SetConfig(BASS_CONFIG_BUFFER, 200);
        inited = true;
    }
}

void CALLBACK Player::DSP(HDSP, DWORD, void *buffer, DWORD length, void *user)
{
    reinterpret_cast<Player *>(user)->DSP(reinterpret_cast<float *>(buffer), length / sizeof(float));
}

void Player::DSP(float *buffer, DWORD length)
{
    if (sofa == nullptr)
        return;
    vector<float> hrtf = sofa->get_hrtf(x, y, z);
    size_t prev_size = hrtf.size() / 2 - 1;
    if (prev_data.size() != prev_size)
        prev_data.resize(prev_size);
    vector<double> a(prev_data), b1, b2;
    for (DWORD i = 0; i < length; i += 2)
        a.emplace_back((buffer[i] + buffer[i + 1]) / 2);
    vector<Complex> fa = FFT(a, size_t(pow(2, ceil(log2(a.size())))));
    for (size_t i = 0; i < prev_data.size(); ++i)
        prev_data[i] = a[a.size() - prev_data.size() + i];
    for (size_t i = 0; i < hrtf.size() / 2; ++i)
        b1.emplace_back(hrtf[i]);
    for (size_t i = hrtf.size() / 2; i < hrtf.size(); ++i)
        b2.emplace_back(hrtf[i]);
    b1 = FFT_conv2(fa, b1);
    b2 = FFT_conv2(fa, b2);
    if (x != prev_x || y != prev_y || z != prev_z) {
        vector<float> hrtf2 = sofa->get_hrtf(prev_x, prev_y, prev_z);
        vector<double> c1, c2;
        for (size_t i = 0; i < hrtf2.size() / 2; ++i)
            c1.emplace_back(hrtf2[i]);
        for (size_t i = hrtf2.size() / 2; i < hrtf2.size(); ++i)
            c2.emplace_back(hrtf2[i]);
        c1 = FFT_conv2(fa, c1);
        c2 = FFT_conv2(fa, c2);
        for (DWORD i = 0; i < length; i += 2) {
            double t = double(i) / length;
            b1[i / 2 + prev_data.size()] = t * b1[i / 2 + prev_data.size()] + (1 - t) * c1[i / 2 + prev_data.size()];
            b2[i / 2 + prev_data.size()] = t * b2[i / 2 + prev_data.size()] + (1 - t) * c2[i / 2 + prev_data.size()];
        }
    }
    prev_x = x;
    prev_y = y;
    prev_z = z;
    double dist = sqrt(double(x * x + y * y + z * z));
    if (dist < 1.0)
        dist = 1.0;
    for (DWORD i = 0; i < length; i += 2) {
        buffer[i] = float(b1[i / 2 + prev_data.size()] / dist);
        buffer[i + 1] = float(b2[i / 2 + prev_data.size()] / dist);
    }
}
