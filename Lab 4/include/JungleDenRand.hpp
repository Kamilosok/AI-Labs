#pragma once
#include "Common.hpp"
#include "JungleAgent.hpp"

#include <math.h>

class JungleDenRand : public JungleAgent
{
private:
    uint32_t countDenDist(const state_t &state);
    Move pickMove(const state_t &state);

public:
    JungleDenRand();
    JungleDenRand(state_t state);
    ~JungleDenRand();

    void agentMove() override;
};