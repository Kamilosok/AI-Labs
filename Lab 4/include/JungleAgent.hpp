#pragma once
#include "Common.hpp"
#include "JungleBase.hpp"

#include <random>
#include <chrono>

// Base class for all agents
class JungleAgent : public JungleBase
{
protected:
    bool playsUp = true;
    state_t currState;

public:
    JungleAgent();
    JungleAgent(state_t state);
    ~JungleAgent();

    const state_t &getState();
    void setState(const state_t &state);
    const bool &getPlaysUp();
    void setPlaysUp(const bool &p);
    virtual void agentMove() = 0;
    // For Q-learning
    virtual bool loadWeights([[__maybe_unused__]] const std::string &filename)
    {
        return false;
    }
    virtual bool saveWeights([[__maybe_unused__]] const std::string &filename) const
    {
        return false;
    }
};