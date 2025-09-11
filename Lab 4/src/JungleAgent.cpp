#include "JungleAgent.hpp"

JungleAgent::JungleAgent()
{
    ;
}

JungleAgent::JungleAgent(state_t state)
{
    setState(state);
}

JungleAgent::~JungleAgent()
{
    ;
}

const state_t &JungleAgent::getState()
{
    return currState;
}

void JungleAgent::setState(const state_t &state)
{
    currState = state;
}

const bool &JungleAgent::getPlaysUp()
{
    return playsUp;
}

void JungleAgent::setPlaysUp(const bool &p)
{
    playsUp = p;
}
