#include "JungleMCTS.hpp"

// Counted for up, then changed to fit the agent
double JungleMCTS::evaluate1(const state_t &state)
{
    // 1) Material (rat=1 ... elephant=8), zeroed if in trap:
    double M_up = 0, M_dn = 0;
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        pos_t pu = state.up[a], pd = state.down[a];
        double value = animalValues[a];
        if (pu != deadPos && getTile(pu.x, pu.y) != '#')
            M_up += value;
        if (pd != deadPos && getTile(pd.x, pd.y) != '#')
            M_dn += value;
    }
    double hM = M_up - M_dn;

    // 2) Distance of major pieces to the opponent’s den (closer is better):
    double hD = 0;
    for (auto a : {Lion, Tiger, Elephant})
    {
        pos_t pu = state.up[static_cast<uint8_t>(a)];
        if (pu != deadPos)
        {
            int32_t d = abs(pu.x - downDen.x) + abs(pu.y - downDen.y);
            hD += double(static_cast<uint8_t>(a) + 1) * (1.0 / (1 + d));
        }
        pos_t pd = state.down[static_cast<uint8_t>(a)];
        if (pd != deadPos)
        {
            int32_t d = abs(pd.x - upDen.x) + abs(pd.y - upDen.y);
            hD -= double(static_cast<uint8_t>(a) + 1) * (1.0 / (1 + d));
        }
    }

    // 3) Mobility: number of legal moves
    state_t modState = state;
    modState.upMove = true;
    std::vector<Move> upMoves;
    genMoves(modState, upMoves);
    int32_t moves_up = upMoves.size();

    std::vector<Move> dnMoves;
    modState.upMove = false;
    genMoves(modState, dnMoves);
    int32_t moves_down = dnMoves.size();
    double hR = double(moves_up - moves_down) / 16.0; // normalize roughly

    // 4) Water control: rats in water
    pos_t ratUp = state.animalAt(Rat, true);
    pos_t ratDown = state.animalAt(Rat, false);
    double hW = 0.0;
    if (ratUp != deadPos && getTile(ratUp.x, ratUp.y) == '~')
        hW += 1.0;
    if (ratDown != deadPos && getTile(ratDown.x, ratDown.y) == '~')
        hW -= 1.0;

    // 5) Threats: pieces that can be captured next move
    int upThreats = 0, dnThreats = 0;
    // threats against up:
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        pos_t p = state.up[a];
        if (p == deadPos)
            continue;

        modState.upMove = false;
        for (Move m : dnMoves)
        {
            state_t after;
            moveState(modState, m, after);
            if (after.up[a] == deadPos)
            {
                ++upThreats;
                break;
            }
        }
    }
    // threats against down:
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        pos_t p = state.down[a];
        if (p == deadPos)
            continue;
        modState.upMove = true;
        for (Move m : upMoves)
        {
            state_t after;
            moveState(modState, m, after);
            if (after.down[a] == deadPos)
            {
                ++dnThreats;
                break;
            }
        }
    }
    double hT = -0.5 * (upThreats - dnThreats);

    // 6) Trap control: number of own pieces in opponent’s traps (good) minus enemy pieces in own traps (bad)
    int upInDownTraps = 0, dnInUpTraps = 0;
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        pos_t pu = state.up[a], pd = state.down[a];
        if (pu != deadPos && getTile(pu.x, pu.y) != '#')
            upInDownTraps += 1;
        if (pd != deadPos && getTile(pd.x, pd.y) != '#')
            dnInUpTraps += 1;
    }
    double hTp = 0.3 * (upInDownTraps - dnInUpTraps);

    // 7) Win/loss terminal bonus:
    double winUp = checkWin(state, true) ? 1.0 : 0.0;
    double winDown = checkWin(state, false) ? 1.0 : 0.0;

    // Weights
    constexpr double wM = 10.0;
    constexpr double wD = 8.0;
    constexpr double wR = 1.0;
    constexpr double wW = 2.0;
    constexpr double wT = 8.0;
    constexpr double wTp = 4.0;
    constexpr double wWn = 1000.0;

    double score =
        wM * hM +
        wD * hD +
        wR * hR +
        wW * hW +
        wT * hT +
        wTp * hTp +
        wWn * (winUp - winDown);

    // Normalize to [-1,1] (approximate):
    constexpr double maxRaw = wM * 20 + wD * 16 + wR * 5 + wW * 1 + wT * 8 + wTp * 4 + wWn * 1;
    double norm = std::clamp(score / maxRaw, -1.0, 1.0);

    return playsUp ? norm : -norm;
}

double JungleMCTS::rollout(const state_t &startState)
{
    state_t st = startState;
    uint8_t noCaptureCount = 0;
    uint8_t moveCount = 0;
    while (checkWin(st, playsUp) == false && checkWin(st, !playsUp) == false)
    {
        std::vector<Move> moves;
        genMoves(st, moves);

        // Stalemate
        if (moves.empty())
            return 0.0;

        Move m = moves[moveDist(gen) % moves.size()];

        if (checkCapture(st, m))
            noCaptureCount = 0;
        else
            noCaptureCount += 1;

        moveCount += 1;

        if (noCaptureCount >= MAX_NO_CAPTURE)
            return evaluate1(st);

        if (moveCount >= MAX_NO_CAPTURE)
        {
            return evaluate1(st);
        }

        moveState(st, m, st);
    }

    if (checkWin(st, playsUp))
        return 1.0;
    else
        return -1.0;
}

JungleMCTS::MCTSNode *JungleMCTS::expand(MCTSNode *node)
{
    Move mov = node->untriedMoves.back();
    node->untriedMoves.pop_back();

    state_t nextState;
    moveState(node->state, mov, nextState);
    MCTSNode *childNode = new MCTSNode(nextState, node, mov);

    node->children.push_back(childNode);

    return childNode;
}

JungleMCTS::MCTSNode *JungleMCTS::bestChild(MCTSNode *node, double c)
{
    if (node->children.empty())
        return nullptr;

    MCTSNode *bestChild = nullptr;
    double bestUCT = -1e9;

    for (auto child : node->children)
    {
        // Exploitation + exploration
        double UCT = (child->totalValue / child->visitCount) + c * sqrt(log(node->visitCount) / child->visitCount);

        if (UCT > bestUCT)
        {
            bestUCT = UCT;
            bestChild = child;
        }
    }

    return bestChild;
}

JungleMCTS::MCTSNode *JungleMCTS::treePolicy(MCTSNode *node)
{
    while (!checkWin(node->state, playsUp) && !checkWin(node->state, !playsUp))
    {
        if (!node->untriedMoves.empty())
            return expand(node);
        else
        {
            node = bestChild(node, explorationConstant);
        }
    }

    return node;
}

void JungleMCTS::backpropagate(MCTSNode *node, double value)
{
    while (node != nullptr)
    {
        node->visitCount += 1;
        // Credit the player who just moved
        double reward = (node->state.upMove == playsUp)
                            ? -value
                            : value;
        node->totalValue += reward;
        node = node->parent;
    }
}

void JungleMCTS::deleteSubtree(MCTSNode *node)
{
    if (!node)
        return;
    for (auto child : node->children)
        deleteSubtree(child);

    delete node;
}

JungleMCTS::JungleMCTS() : JungleAgent()
{
    ;
}

JungleMCTS::JungleMCTS(state_t state) : JungleAgent(state)
{
    ;
}

JungleMCTS::~JungleMCTS()
{
    if (ogRoot)
    {
        deleteSubtree(ogRoot);
        ogRoot = nullptr;
    }
}

void JungleMCTS::agentMove()
{
    // Dummy move with no parent
    if (!root)
    {
        root = new MCTSNode(currState, nullptr, Move(Rat, {0, 0}));
        ogRoot = root;
    }

    bool found = false;

    // Reusing already built tree
    for (auto child : root->children)
    {
        if (child->state == currState)
        {
            root = child;
            root->parent = nullptr;
            found = true;
        }
    }

    if (!found)
    {
        deleteSubtree(root);
        root = new MCTSNode(currState, nullptr, Move(Rat, {0, 0}));
    }

    root->visitCount = 1;
    root->totalValue = 0;
    // Classic MCTS logic
    for (uint32_t iter = 0; iter < simsPerMove; iter++)
    {
        MCTSNode *leaf = treePolicy(root);
        double reward = rollout(leaf->state);
        backpropagate(leaf, reward);
    }

    // Pick most valuable move
    MCTSNode *best = bestChild(root, 0.0);
    Move bestMove = best->moveFromParent;

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

    currState = best->state;

    // Dueler communication
    printf("IDO %u %u %u %u\n", prevPos.x, prevPos.y, destPos.x, destPos.y);
    fflush(stdout);

    // Cleaning unwanted children
    for (auto child : root->children)
    {
        if (child != best)
        {
            deleteSubtree(child);
        }
    }

    root = best;
}