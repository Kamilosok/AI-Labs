#include "JungleDenRand.hpp"

uint32_t JungleDenRand::countDenDist(const state_t &state)
{
    uint32_t currDistSum = 0;

    // Get current dists from enemy dens
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        if (playsUp)
        {
            pos_t pu = state.up[static_cast<uint8_t>(a)];
            if (pu != deadPos)
            {
                currDistSum += abs(pu.x - downDen.x) + abs(pu.y - downDen.y);
            }
        }
        else
        {
            pos_t pd = state.down[static_cast<uint8_t>(a)];
            if (pd != deadPos)
            {
                currDistSum += abs(pd.x - upDen.x) + abs(pd.y - upDen.y);
            }
        }
    }

    return currDistSum;
}

Move JungleDenRand::pickMove(const state_t &state)
{
    uint32_t currDistSum = countDenDist(state);

    std::vector<Move> movesToDen;

    std::vector<Move> possMoves;
    genMoves(currState, possMoves);

    for (auto possMove : possMoves)
    {
        state_t newState;
        moveState(state, possMove, newState);
        uint32_t newDistSum = countDenDist(newState);

        if (newDistSum < currDistSum)
            movesToDen.push_back(possMove);
    }

    if (movesToDen.empty())
        return possMoves[moveDist(gen) % possMoves.size()];

    // Coinflip to decide move type
    if (moveDist(gen) % 2)
        return possMoves[moveDist(gen) % possMoves.size()];
    else
        return movesToDen[moveDist(gen) % movesToDen.size()];
}

JungleDenRand::JungleDenRand() : JungleAgent()
{
    ;
}

JungleDenRand::JungleDenRand(state_t state) : JungleAgent(state)
{
    ;
}

JungleDenRand::~JungleDenRand()
{
}

void JungleDenRand::agentMove()
{
    Move bestMove = pickMove(currState);

    pos_t prevPos = currState.animalAt(bestMove.first, playsUp);
    pos_t destPos = prevPos;
    pos_t dest = bestMove.second;
    destPos.x += dest.x;
    destPos.y += dest.y;

    // Fix for jumping
    if (bestMove.first == Tiger || bestMove.first == Lion)
    {
        char destTile = getTile(destPos.x, destPos.y);
        while (destTile == '~')
        {
            destPos.x += dest.x;
            destPos.y += dest.y;

            destTile = getTile(destPos.x, destPos.y);
        }
    }

    moveState(currState, bestMove, currState);

    // Dueler communication
    printf("IDO %u %u %u %u\n", prevPos.x, prevPos.y, destPos.x, destPos.y);
    fflush(stdout);
}