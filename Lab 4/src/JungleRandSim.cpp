#include "JungleRandSim.hpp"

JungleRandSim::JungleRandSim() : JungleAgent()
{
    ;
}

JungleRandSim::JungleRandSim(state_t state) : JungleAgent(state)
{
    ;
}

JungleRandSim::~JungleRandSim()
{
    nextStates.clear();
    nextStatesMoves.clear();
}

// Generate next states to ultimately choose the best one with Monte-Carlo method
void JungleRandSim::genNextStates()
{
    // currState.upMove = playsUp;
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        Animal an = static_cast<Animal>(a);

        for (auto move : moveList)
        {
            state_t nextState;
            bool valid = moveState(currState, Move(an, move), nextState);

            if (!valid)
                continue;

            // printState(nextState);
            nextStates.push_back(nextState);
            nextStatesMoves.push_back(Move(an, move));
        }
    }
}

// The agent chooses the next move as described in Lab 4 Exercise 3
void JungleRandSim::agentMove()
{
    genNextStates();

    uint32_t bestStateId = 0;
    uint32_t movesPerTry = maxSimMoves / (simsPerMove * nextStates.size());
    double bestScore = 0;

    for (uint32_t stateId = 0; stateId < nextStates.size(); stateId++)
    {
        double currScore = 0;
        for (uint32_t i = 0; i < simsPerMove; i++)
        {
            state_t nextState = nextStates[stateId];
            for (uint32_t mov = 0; mov < movesPerTry; mov++)
            {
                std::vector<Move> possMoves;
                genMoves(nextState, possMoves);

                uint8_t moveChosen = moveDist(gen) % possMoves.size();
                Animal animalToMove = possMoves[moveChosen].first;
                pos_t moveToMove = possMoves[moveChosen].second;

                moveState(nextState, Move(animalToMove, moveToMove), nextState);

                if (checkWin(nextState, playsUp))
                {
                    currScore += 1;
                    break;
                }
                else if (checkWin(nextState, !playsUp))
                {
                    currScore -= 1;
                    break;
                }
            }
        }
        double avgScore = currScore / simsPerMove;

        if (avgScore > bestScore)
        {
            bestScore = avgScore;
            bestStateId = stateId;
        }
    }

    pos_t prevPos = currState.animalAt(nextStatesMoves[bestStateId].first, playsUp);
    pos_t destPos = prevPos;
    pos_t dest = nextStatesMoves[bestStateId].second;
    destPos.x += dest.x;
    destPos.y += dest.y;

    // Fix for jumping
    if (nextStatesMoves[bestStateId].first == Tiger || nextStatesMoves[bestStateId].first == Lion)
    {
        char destTile = getTile(destPos.x, destPos.y);
        while (destTile == '~')
        {
            destPos.x += dest.x;
            destPos.y += dest.y;

            destTile = getTile(destPos.x, destPos.y);
        }
    }

    currState = nextStates[bestStateId];

    // Dueler communication
    printf("IDO %u %u %u %u\n", prevPos.x, prevPos.y, destPos.x, destPos.y);
    fflush(stdout);

    nextStates.clear();
    nextStatesMoves.clear();
}