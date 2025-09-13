#include "Common.hpp"
#include "JungleBase.hpp"
#include "JungleRandSim.hpp"
#include "JungleMCTS.hpp"
#include "JungleRand.hpp"
#include "JungleDenRand.hpp"
#include "JungleQLearn.hpp"

#include <ios>
#include <memory>
#include <unistd.h>

AgentType agentType = agentRandSim;
bool train = false;

std::random_device rd;
auto seed = rd() ^ (std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::mt19937 gen(seed);
// Potential max amount of moves, with a large margin for error
std::uniform_int_distribution<uint8_t> moveDist(0, (1 << 7));

// Polymorphically create an agent
std::unique_ptr<JungleAgent> createJGAgent(AgentType aType)
{
    switch (aType)
    {
    case agentRandSim:
        return std::make_unique<JungleRandSim>();
    case agentMCTS:
        return std::make_unique<JungleMCTS>();
    case agentRand:
        return std::make_unique<JungleRand>();
    case agentDenRand:
        return std::make_unique<JungleDenRand>();
    case agentQLearn:
        return std::make_unique<JungleQLearn>();
    default:
        return std::make_unique<JungleRand>();
        break;
    }
}

int main()
{
// Only one agent should be defined, otherwise the program will choose in this order
#if defined(RS)
    agentType = agentRandSim;
#elif defined(MCTS)
    agentType = agentMCTS;
#elif defined(RAND)
    agentType = agentRand;
#elif defined(DENRAND)
    agentType = agentDenRand;
#elif defined(QLEARN)
    agentType = agentQLearn;
#endif

#if defined(TRAIN)
    train = true;
#endif

    // For dueler communication
    char duelerMsg[10];
    double move_timeout, remaining_time;
    bool knowWhoStarts = false;
    auto agent = createJGAgent(agentType);
    state_t startState;
    agent->setState(startState);
    agent->setPlaysUp(false);

    if (!train)
    {
        if (auto qAgent = dynamic_cast<JungleQLearn *>(agent.get()))
        {
            qAgent->loadWeights("weights.txt");
            qAgent->noTrainMode();
        }

        fprintf(stderr, "Loaded weights from file!\n");
    }

    printf("RDY\n");
    fflush(stdout);

    scanf("\n%s", duelerMsg);

    // Dueler logic
    while (true)
    {
        if (strcmp(duelerMsg, "ONEMORE") == 0)
        {
            knowWhoStarts = false;
            agent->setState(startState);

            printf("RDY\n");
            fflush(stdout);
        }
        else if (strcmp(duelerMsg, "UGO") == 0)
        {
            agent->setPlaysUp(false);
            knowWhoStarts = true;
            scanf("%lf %lf", &move_timeout, &remaining_time);
            agent->agentMove();
        }
        else if (strcmp(duelerMsg, "HEDID") == 0)
        {

            if (!knowWhoStarts)
            {
                knowWhoStarts = true;
                agent->setPlaysUp(true);
            }

            scanf("%lf %lf", &move_timeout, &remaining_time);
            char fromX, fromY, toX, toY;
            scanf("%hhd %hhd %hhd %hhd", &fromX, &fromY, &toX, &toY);

            pos_t prevPos = {.x = fromX, .y = fromY};

            int dx = toX - fromX;
            int dy = toY - fromY;
            // Normalize for jumps
            pos_t dest = {
                .x = (char)(dx / (dx != 0 ? abs(dx) : 1)),
                .y = (char)(dy / (dy != 0 ? abs(dy) : 1))};

            state_t prevState = agent->getState();
            prevState.upMove = !agent->getPlaysUp();

            for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
            {
                Animal an = static_cast<Animal>(a);
                pos_t anPos = prevState.animalAt(an, prevState.upMove);

                if (anPos == prevPos)
                {
                    state_t nextState;
                    JungleBase::moveState(prevState, Move(an, dest), nextState);

                    agent->setState(nextState);
                    break;
                }
            }

            agent->agentMove();
        }

        scanf("%s", duelerMsg);

        if (strcmp(duelerMsg, "BYE") == 0)
        {
            break;
        }
    }
    // If Q-learning, write weights to some file
    if (train)
    {
        if (auto qAgent = dynamic_cast<JungleQLearn *>(agent.get()))
        {
            qAgent->saveWeights("weights.txt");
            fprintf(stderr, "Written weights to file!\n");
        }
    }

    return 0;
}