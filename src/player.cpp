#include "player.h"
#include <stdexcept>
#include "utils.h"
using namespace std;

bool Player::inited = false;

Player::Player() : sofa(nullptr), handle(0), x(1), y(0), z(0)
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
    handle = BASS_StreamCreateFile(FALSE, filename.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
    if (handle == 0)
        throw runtime_error("file open failed");
    BASS_ChannelSetDSP(handle, DSP, this, 0);
}

void Player::open_hrtf(const std::string &filename)
{
    if (sofa != nullptr)
        delete sofa;
    sofa = nullptr;
    sofa = new SOFA(filename, SAMPLE_RATE);
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
    BASS_ChannelStop(handle);
}

void Player::set_source(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Player::init()
{
    if (!inited) {
        if (BASS_Init(-1, SAMPLE_RATE, 0, nullptr, nullptr) == FALSE)
            throw runtime_error("bass init failed");
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
    if (prev.empty())
        prev.resize(hrtf.size() / 2 - 1);
    vector<double> a(prev), b1, b2;
    for (DWORD i = 0; i < length; i += 2)
        a.emplace_back((buffer[i] + buffer[i + 1]) / 2);
    for (size_t i = 0; i < prev.size(); ++i)
        prev[i] = a[a.size() - prev.size() + i];
    for (size_t i = 0; i < hrtf.size() / 2; ++i)
        b1.emplace_back(hrtf[i]);
    for (size_t i = hrtf.size() / 2; i < hrtf.size(); ++i)
        b2.emplace_back(hrtf[i]);
    b1 = FFT_conv(a, b1);
    b2 = FFT_conv(a, b2);
    for (DWORD i = 0; i < length; i += 2) {
        buffer[i] = float(b1[i / 2 + prev.size()]);
        buffer[i + 1] = float(b2[i / 2 + prev.size()]);
    }
}
