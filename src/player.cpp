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
    handle = BASS_StreamCreateFile(FALSE, filename.c_str(), 0, 0, 0);
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
    reinterpret_cast<Player *>(user)->DSP(buffer, length);
}

void Player::DSP(void *buffer, DWORD length)
{
    if (sofa == nullptr)
        return;
    short *s = reinterpret_cast<short *>(buffer);
    vector<double> a;
    for (DWORD i = 0; i < length / 2; i += 2)
        s[i] = s[i + 1] = (s[i] + s[i + 1]) / 2;
}
