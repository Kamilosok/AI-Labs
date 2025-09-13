#pragma once
#include "Common.hpp"
#include "JungleAgent.hpp"

#include <math.h>
#include <fstream>

// A Q-learning driven agent
class JungleQLearn : public JungleAgent
{
    // The base player always plays down, rotated if plays up
private:
    std::array<double, 35> w;

    double learnRate;
    double discFact;

    double explr;
    double explrMin;
    double explrDecay;

    std::uniform_real_distribution<double> uniReal;
    int episodesDone;

    static constexpr int FEATURE_COUNT = 35;

    std::array<double, FEATURE_COUNT> extractFeatures(const state_t &state, const Move &move);
    double computeQ(const std::array<double, FEATURE_COUNT> &features) const;
    double computeReward(const state_t &newState);
    bool getCaptured(const state_t &nextState, Animal mover); // NOT IMPLEMENTED
    void updateWeights(const std::array<double, FEATURE_COUNT> &fun_sa,
                       double reward,
                       const std::array<double, FEATURE_COUNT> &next_sa);

public:
    JungleQLearn();
    JungleQLearn(state_t state);
    ~JungleQLearn();
    void printWeights();
    void agentMove() override;

    bool loadWeights(const std::string &filename);
    bool saveWeights(const std::string &filename) const;

    void noTrainMode();
};