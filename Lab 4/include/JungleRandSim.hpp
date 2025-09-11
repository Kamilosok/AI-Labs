#pragma once
#include "Common.hpp"
#include "JungleAgent.hpp"

class JungleRandSim : public JungleAgent
{
private:
    const uint32_t maxSimMoves = 40000;
    const uint32_t simsPerMove = 40;
    std::vector<state_t> nextStates;
    std::vector<Move> nextStatesMoves;

    inline void genNextStates();

public:
    JungleRandSim();
    JungleRandSim(state_t state);
    ~JungleRandSim();

    void agentMove() override;
};