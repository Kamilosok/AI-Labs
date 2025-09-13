#include "JungleRand.hpp"

JungleRand::JungleRand() : JungleAgent()
{
    ;
}

JungleRand::JungleRand(state_t state) : JungleAgent(state)
{
    ;
}

JungleRand::~JungleRand()
{
}

void JungleRand::agentMove()
{
    std::vector<Move> possMoves;
    genMoves(currState, possMoves);

    // Just randomly pick a move
    Move bestMove = possMoves[moveDist(gen) % possMoves.size()];

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