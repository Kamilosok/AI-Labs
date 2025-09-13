#include "JungleQLearn.hpp"

// Default values
JungleQLearn::JungleQLearn() : JungleAgent(), w{},
                               learnRate(0.001),
                               discFact(0.99),
                               explr(1.0),
                               explrMin(0.01),
                               explrDecay(0.999),
                               uniReal(0.0, 1.0),
                               episodesDone(0)
{
    for (int i = 0; i < FEATURE_COUNT; ++i)
    {
        w[i] = 0.0;
    }
}

JungleQLearn::JungleQLearn(state_t state) : JungleAgent(state), w{},
                                            learnRate(0.001),
                                            discFact(0.99),
                                            explr(1.0),
                                            explrMin(0.01),
                                            explrDecay(0.999),
                                            uniReal(0.0, 1.0),
                                            episodesDone(0)
{
    for (int i = 0; i < FEATURE_COUNT; ++i)
    {
        w[i] = 0.0;
    }
}

JungleQLearn::~JungleQLearn()
{
    ;
}

void JungleQLearn::printWeights()
{
    fprintf(stderr, "WEIGHTS:\n");
    for (int i = 0; i < FEATURE_COUNT; i++)
    {
        fprintf(stderr, "%lf, ", w[i]);
    }
    fprintf(stderr, "\n");
}

// Extract vector values of state
std::array<double, JungleQLearn::FEATURE_COUNT> JungleQLearn::extractFeatures(const state_t &state, const Move &move)
{
    std::array<double, FEATURE_COUNT> features{};
    features.fill(0.0);

    state_t nextState;
    // Only correct moves go here
    JungleAgent::moveState(state, move, nextState);

    pos_t oppDen = downDen;
    pos_t myDen = upDen;
    if (playsUp)
    {
        oppDen = rotatePos(oppDen);
        myDen = rotatePos(myDen);
    }
    pos_t dest = move.second;

    constexpr int D_max = 10;

    //  1) Material features 0...15
    //  0...7   = own features
    //  8...15  = opponent's
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        pos_t myPos = state.animalAt(static_cast<Animal>(a), playsUp);
        pos_t oppPos = state.animalAt(static_cast<Animal>(a), !playsUp);

        if (myPos != deadPos)
            features[a] = animalValues[a];
        else
            features[a] = 0;

        if (oppPos != deadPos)
            features[a + 8] = animalValues[a];
        else
            features[a + 8] = 0;
    }

    //  2) Den distance features 16...31
    //  16...23 my features
    //  24...31 opponent's
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        pos_t myPos = state.animalAt(static_cast<Animal>(a), playsUp);
        pos_t oppPos = state.animalAt(static_cast<Animal>(a), !playsUp);

        if (myPos != deadPos)
        {
            uint8_t d = std::abs(myPos.x - oppDen.x) + std::abs(myPos.y - oppDen.y);
            features[16 + a] = static_cast<double>(std::max(0, D_max - d)) / D_max;
        }
        else
            features[16 + a] = 0.0;

        if (oppPos != deadPos)
        {
            uint8_t d = std::abs(oppPos.x - myDen.x) + std::abs(oppPos.y - myDen.y);
            features[24 + a] = static_cast<double>(std::max(0, D_max - d)) / D_max;
        }
        else
            features[24 + a] = 0.0;
    }

    bool doesCapture = JungleAgent::checkCapture(state, move);
    if (doesCapture)
    {
        for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
        {
            pos_t posOpp = state.animalAt(static_cast<Animal>(a), !playsUp);
            if (posOpp == dest)
            {
                features[32] = animalValues[a];
                break;
            }
        }
    }
    else
    {
        features[32] = 0.0;
    }

    if (getTile(dest.x, dest.y) == '#')
    {
        if (dest.y < 4)
        {
            features[33] = 1.0;
        }
        else
        {
            features[33] = 0.0;
        }
    }

    // 3.3) “Would mover be immediately captured?” → feature 34 = 1.0 if yes, else 0.0
    /* NOT IMPLEMENTED
    if (getCaptured(nextState, mover))
    {
        features[34] = 1.0;
    }
    else
    {
        features[34] = 0.0;
    }
    */
    features[34] = 0.0;

    return features;
}

double JungleQLearn::computeQ(const std::array<double, FEATURE_COUNT> &features) const
{
    double q = 0.0;
    for (int i = 0; i < FEATURE_COUNT; ++i)
    {
        q += w[i] * features[i];
    }
    return q;
}

double JungleQLearn::computeReward(const state_t &newState)
{
    if (checkWin(newState, playsUp))
    {
        return +1.0;
    }

    if (checkWin(newState, !playsUp))
    {
        return -1.0;
    }
    // For wasting time
    return -0.001;
}

void JungleQLearn::updateWeights(const std::array<double, FEATURE_COUNT> &phi_sa, double reward, const std::array<double, FEATURE_COUNT> &phi_sp)
{

    double q_sa = computeQ(phi_sa);

    double q_sp = computeQ(phi_sp);

    // Temporal difference
    double delta = reward + discFact * q_sp - q_sa;

    for (int i = 0; i < FEATURE_COUNT; ++i)
    {
        w[i] += learnRate * delta * phi_sa[i];
    }
}

// For Q-learning the agent treats as if it always plays down, it worked inconsistently otherwise
void JungleQLearn::agentMove()
{
    std::vector<Move> possMoves;
    state_t canonState = playsUp
                             ? rotateState(currState)
                             : currState;
    // If plays up, then generate rotated moves
    genMoves(canonState, possMoves, playsUp);
    if (possMoves.empty())
        return;

    Move chosenMove;
    // Either explore or choose a bestQ move
    if (uniReal(gen) < explr)
    {
        chosenMove = possMoves[moveDist(gen) % possMoves.size()];
    }
    else
    {
        double bestQ = -1e9;
        for (auto &m : possMoves)
        {
            auto phi = extractFeatures(canonState, m);
            double qVal = computeQ(phi);
            if (qVal > bestQ)
            {
                bestQ = qVal;
                chosenMove = m;
            }
        }
    }
    pos_t prevPos = canonState.animalAt(chosenMove.first, playsUp);
    pos_t destPos = prevPos;
    pos_t offset = chosenMove.second;
    destPos.x += offset.x;
    destPos.y += offset.y;

    // Fix for jumping
    if (chosenMove.first == Tiger || chosenMove.first == Lion)
    {
        char tile = getTile(destPos.x, destPos.y);
        while (tile == '~')
        {
            destPos.x += offset.x;
            destPos.y += offset.y;
            tile = getTile(destPos.x, destPos.y);
        }
    }

    state_t nextState;
    moveState(canonState, chosenMove, nextState);

    // Q-learning logic
    double r = computeReward(nextState);

    auto phi_sa = extractFeatures(canonState, chosenMove);

    std::vector<Move> nextMoves;
    genMoves(nextState, nextMoves);

    double maxQ_next = 0.0;
    std::array<double, FEATURE_COUNT> bestNextPhi{};
    bestNextPhi.fill(0.0);

    if (!nextMoves.empty())
    {
        maxQ_next = -1e9;
        for (auto &m2 : nextMoves)
        {
            auto phi_sp_candidate = extractFeatures(nextState, m2);
            double q2 = computeQ(phi_sp_candidate);
            if (q2 > maxQ_next)
            {
                maxQ_next = q2;
                bestNextPhi = phi_sp_candidate;
            }
        }
    }

    if (checkWin(nextState, playsUp) || checkWin(nextState, !playsUp))
    {
        episodesDone++;
        explr = std::max(explrMin, explr * explrDecay);
    }

    updateWeights(phi_sa, r, bestNextPhi);

    // Canonizing state
    currState = playsUp ? rotateState(nextState) : nextState;

    if (playsUp)
    {
        prevPos = rotatePos(prevPos);
        destPos = rotatePos(destPos);
    }

    // Dueler communication
    printf("IDO %u %u %u %u\n", prevPos.x, prevPos.y, destPos.x, destPos.y);
    fflush(stdout);
}

// Load weights from file
bool JungleQLearn::loadWeights(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in)
        return false;
    for (int i = 0; i < FEATURE_COUNT; ++i)
    {
        if (!(in >> w[i]))
            return false;
    }
    return true;
}

// Save weights to file
bool JungleQLearn::saveWeights(const std::string &filename) const
{
    std::ofstream out(filename);
    if (!out)
        return false;
    for (int i = 0; i < FEATURE_COUNT; ++i)
    {
        out << w[i] << (i + 1 < FEATURE_COUNT ? ' ' : '\n');
    }
    return true;
}

// Don't learn at all or explore
void JungleQLearn::noTrainMode()
{
    learnRate = 0;
    explr = 0;
}
