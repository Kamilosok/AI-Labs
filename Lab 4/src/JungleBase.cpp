#include "JungleBase.hpp"

char JungleBase::getTile(char x, char y)
{
    if (y == 0 || y == 8)
    {
        // Dens
        if (x == 3)
            return '*';

        // Snares
        if (x == 2 || x == 4)
            return '#';
    }

    // Snares
    if (y == 1 || y == 7)
        if (x == 3)
            return '#';

    // Pond
    if (y >= 3 && y <= 5)
        if (x >= 1 && x <= 5 && x != 3)
            return '~';

    return '.';
}

void JungleBase::printState(const state_t &state)
{
    char board[7][9];
    std::fill(&board[0][0], &board[0][0] + sizeof(board), '.');

    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        Animal an = static_cast<Animal>(a);

        bool upAn = true;
        pos_t anPos = state.animalAt(an, upAn);

        char anChar;

        if (anPos != deadPos)
        {
            anChar = getAnimalChar(an, upAn);

            board[(uint8_t)anPos.x][(uint8_t)anPos.y] = anChar;
        }

        upAn = false;
        anPos = state.animalAt(an, upAn);
        if (anPos != deadPos)
        {
            anChar = getAnimalChar(an, upAn);

            board[(uint8_t)anPos.x][(uint8_t)anPos.y] = anChar;
        }
    }

    for (uint8_t y = 0; y < 9; y++)
    {
        for (uint8_t x = 0; x < 7; x++)
        {
            board[x][y] = board[x][y] != '.' ? board[x][y] : getTile(x, y);
        }
    }

    for (uint8_t y = 0; y < 9; y++)
    {
        for (uint8_t x = 0; x < 7; x++)
        {
            fprintf(stderr, "%c", board[x][y]);
        }
        fprintf(stderr, "\n");
    }
}

// Check win for self, providing which one the agent is
bool JungleBase::checkWin(const state_t &state, bool playsUp)
{
    bool enemyAlive = false;
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        pos_t anPos = state.animalAt(static_cast<Animal>(a), playsUp);

        if (playsUp)
        {
            if (anPos == downDen)
                return true;
        }
        else
        {
            if (anPos == upDen)
                return true;
        }

        if (state.animalAt(static_cast<Animal>(a), !playsUp) != deadPos)
        {
            enemyAlive = true;
        }
    }

    return false || !enemyAlive;
}

// Check whether a VALID move captures an enemy animal
bool JungleBase::checkCapture(const state_t &state, const Move &move)
{
    pos_t destPos = state.animalAt(move.first, state.upMove);
    destPos.x += move.second.x;
    destPos.y += move.second.y;

    for (uint8_t a = static_cast<uint8_t>(Elephant); a >= static_cast<uint8_t>(Rat); a--)
    {
        pos_t anPos = state.animalAt(static_cast<Animal>(a), !state.upMove);
        if (destPos == anPos)
        {
            return true;
        }
    }

    return false;
}

bool JungleBase::validateMove(const state_t &state, Animal animal, pos_t dest)
{
    if (abs(dest.x) + abs(dest.y) != 1)
        return false;

    pos_t currPos = state.animalAt(animal, state.upMove);
    pos_t destPos = pos_t{.x = static_cast<char>(currPos.x + dest.x), .y = static_cast<char>(currPos.y + dest.y)};
    bool upMoves = state.upMove;

    // Trying to move a dead animal
    if (currPos == deadPos)
        return false;

    // Trying to move an animal out of bounds
    if (destPos.x < 0 || destPos.x >= 7 || destPos.y < 0 || destPos.y >= 9)
        return false;

    // Trying to move an animal that's not a rat/tiger/lion onto water (tiger and lion jump)
    if (getTile(destPos.x, destPos.y) == '~' && animal != Rat && animal != Tiger && animal != Lion)
        return false;

    // Trying to move an animal into own den
    if (upMoves && destPos.x == 3 && destPos.y == 0)
        return false;
    if (!upMoves && destPos.x == 3 && destPos.y == 8)
        return false;

    // Trying to move an animal onto own animal
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        pos_t anPos = state.animalAt(static_cast<Animal>(a), upMoves);

        if (destPos == anPos)
        {
            // fprintf(stderr, "OWN ANIMAL\n");
            return false;
        }
    }

    // Trying to move an animal onto a stronger animal
    uint8_t anRank = static_cast<uint8_t>(animal);
    for (uint8_t a = static_cast<uint8_t>(Elephant); a >= static_cast<uint8_t>(Rat); a--)
    {
        pos_t anPos = state.animalAt(static_cast<Animal>(a), !upMoves);
        if (destPos == anPos)
        {
            // Can't attack from the pond into land
            if (getTile(currPos.x, currPos.y) == '~' && getTile(destPos.x, destPos.y) == '.')
                return false;

            // The elephant can't attack the rat
            if (anRank == static_cast<uint8_t>(Elephant) && a == static_cast<uint8_t>(Rat))
            {
                return false;
            }

            // Anything can be captured in the snare
            if (getTile(destPos.x, destPos.y) == '#')
                return true;

            // If trying to move onto a stronger animal, allow only the rat to attack the elephant

            if (anRank < a)
            {

                if (anRank != static_cast<uint8_t>(Rat) || a != static_cast<uint8_t>(Elephant))
                {
                    // fprintf(stderr, "STRONGER ANIMAL\n");
                    return false;
                }
            }
        }

        if (a == 0)
            break;
    }

    return true;
}

bool JungleBase::moveState(const state_t &state, Move move, state_t &outState)
{
    Animal animal = move.first;
    pos_t dest = move.second;

    if (!validateMove(state, animal, dest))
        return false;

    if (checkWin(state, true) || checkWin(state, false))
        return true;

    // Small redundancy
    pos_t currPos = state.animalAt(animal, state.upMove);
    pos_t destPos = pos_t{.x = static_cast<char>(currPos.x + dest.x), .y = static_cast<char>(currPos.y + dest.y)};
    // Trying to jump with the tiger or lion over the rat + setting the correct destPos
    if (animal == Tiger || animal == Lion)
    {
        char destTile = getTile(destPos.x, destPos.y);
        while (destTile == '~')
        {
            // The dueler forbids jumping over own rat, so I have to forbid it too
            if (state.animalAt(Rat, !state.upMove) == destPos || state.animalAt(Rat, state.upMove) == destPos)
                return false;

            destPos.x += dest.x;
            destPos.y += dest.y;

            destTile = getTile(destPos.x, destPos.y);
        }
        for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
        {
            pos_t anPos = state.animalAt(static_cast<Animal>(a), state.upMove);

            // Landing on own animal
            if (destPos == anPos)
                return false;

            // Landing on stronger animal
            anPos = state.animalAt(static_cast<Animal>(a), !state.upMove);
            if (destPos == anPos && move.first < static_cast<Animal>(a) && getTile(destPos.x, destPos.y) != '#')
            {
                return false;
            }
        }
    }
    bool upMoves = state.upMove;

    // At this point all moves are legal
    outState = state;
    outState.upMove = !upMoves;
    if (upMoves)
    {
        outState.up[static_cast<uint8_t>(animal)] = destPos;
    }
    else
    {
        outState.down[static_cast<uint8_t>(animal)] = destPos;
    }

    // Process any captured animals
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        pos_t anPos = state.animalAt(static_cast<Animal>(a), !upMoves);

        if (destPos == anPos)
        {
            if (upMoves)
            {
                // printf("Capturing down animal!\n");
                outState.down[a] = deadPos;
            }
            else
            {
                // printf("Capturing up animal!\n");
                outState.up[a] = deadPos;
            }
        }
    }

    // Moving into enemy den
    if (!upMoves && destPos.x == 3 && destPos.y == 0)
    {
        // Maybe do more, or other checkWin function
        // printf("Down won!\n");
        // printState(state);
    }
    if (upMoves && destPos.x == 3 && destPos.y == 8)
    {
        // Maybe do more, or other checkWin function
        // printf("Up won!\n");
        // printState(state);
    }

    return true;
}

// Potentially slow, check times
void JungleBase::genMoves(const state_t &state, std::vector<Move> &possMoves)
{
    // currState.upMove = playsUp;
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        Animal an = static_cast<Animal>(a);

        for (auto move : moveList)
        {
            state_t nextState;
            bool valid = moveState(state, Move(an, move), nextState);

            if (!valid)
                continue;

            // printState(nextState);
            possMoves.push_back({an, move});
        }
    }
}

void JungleBase::genMoves(const state_t &state, std::vector<Move> &possMoves, bool rotated)
{
    // currState.upMove = playsUp;
    state_t canonState = rotated
                             ? rotateState(state)
                             : state;

    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        Animal an = static_cast<Animal>(a);

        for (auto move : moveList)
        {
            state_t nextState;
            bool valid = moveState(canonState, Move(an, move), nextState);

            if (!valid)
                continue;

            // printState(nextState);
            possMoves.push_back({an, rotated ? rotateMove(move) : move});
        }
    }
}
