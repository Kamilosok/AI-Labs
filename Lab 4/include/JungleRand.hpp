#pragma once
#include "Common.hpp"
#include "JungleAgent.hpp"

#include <math.h>

class JungleRand : public JungleAgent
{
private:
public:
    JungleRand();
    JungleRand(state_t state);
    ~JungleRand();

    void agentMove() override;
};