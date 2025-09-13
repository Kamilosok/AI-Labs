#pragma once
#include "Common.hpp"
#include "JungleAgent.hpp"

#include <math.h>

// A randomly playing agent
class JungleRand : public JungleAgent
{
private:
public:
    JungleRand();
    JungleRand(state_t state);
    ~JungleRand();

    void agentMove() override;
};