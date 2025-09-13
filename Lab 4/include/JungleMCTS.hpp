#pragma once
#include "Common.hpp"
#include "JungleAgent.hpp"

#include <math.h>

// An MCTS-driven agent
class JungleMCTS : public JungleAgent
{
private:
    // A custom node for MCTS
    struct MCTSNode
    {
        state_t state;
        MCTSNode *parent;
        Move moveFromParent;
        std::vector<MCTSNode *> children;
        std::vector<Move> untriedMoves;
        double totalValue = 0; // Q
        int visitCount = 0;    // N

        MCTSNode(const state_t &s, MCTSNode *p, const Move &m)
            : state(s), parent(p), moveFromParent(m) { JungleBase::genMoves(s, untriedMoves); }
    };

    MCTSNode *ogRoot;
    MCTSNode *root;

    const uint32_t simsPerMove = 2000;
    const double explorationConstant = 1.41;

    double evaluate1(const state_t &state);
    double rollout(const state_t &startState);
    MCTSNode *expand(MCTSNode *node);
    MCTSNode *bestChild(MCTSNode *node, double c);
    MCTSNode *treePolicy(MCTSNode *node);
    void backpropagate(MCTSNode *node, double value);
    void deleteSubtree(MCTSNode *root);

public:
    JungleMCTS();
    JungleMCTS(state_t state);
    ~JungleMCTS();

    void agentMove() override;
};