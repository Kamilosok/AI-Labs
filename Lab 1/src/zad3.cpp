#include <fstream>
#include <string>
#include <cstring>
#include <random>
#include <chrono>
#include <cstdlib>

std::random_device rd;
auto seed = rd() ^ (std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::mt19937 gen(seed);
std::uniform_int_distribution<int> dist(0, 100);

/*
Hands Cheat sheet

Royal Flush
Straight Flush
Four of a Kind
Full House
Flush
Straight
Three of a Kind
Two Pair
One Pair
High Card
*/

enum class Rank
{
    TWO = 2,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE // Doesn't have to count as 1
};

enum class Suit
{
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
};

enum class HandRank
{
    HighCard = 0,
    OnePair,
    TwoPair,
    ThreeofaKind,
    Straight,
    Flush,
    FullHouse,
    FourofaKind,
    StraightFlush,
    RoyalFlush
};

struct Card
{
    Rank rank;
    Suit suit;
    friend bool operator==(const Card &first, const Card &second) { return first.rank == second.rank && first.suit == second.suit; }
    Card() : rank(Rank::TWO), suit(Suit::CLUBS) {};
    Card(Rank r, Suit s) : rank(r), suit(s) {}
};

// Generating Face Player's hand
void genFigHand(Card figHand[5])
{
    for (int i = 0; i < 5; i++)
    {
        Card newCard = Card(Rank(dist(gen) % 4 + 11), Suit(dist(gen) % 4));

        for (int j = 0; j < i; j++)
        {
            if (newCard == figHand[j])
            {
                newCard = Card(Rank(dist(gen) % 4 + 11), Suit(dist(gen) % 4));
                j = -1;
            }
        }

        figHand[i] = newCard;
    }
}

// Generating Pip Player's hand
void genBlotHand(Card blotHand[5], unsigned int discardLvl)
{
    using enum Rank;
    using enum Suit;
    for (int i = 0; i < 5; i++)
    {
        Card newCard = Card(Rank(dist(gen) % (9 - discardLvl) + 2 + discardLvl), Suit(dist(gen) % 4));
        for (int j = 0; j < i; j++)
        {
            if (newCard == blotHand[j])
            {
                newCard = Card(Rank(dist(gen) % (9 - discardLvl) + 2 + discardLvl), Suit(dist(gen) % 4));
                j = -1;
            }
        }

        blotHand[i] = newCard;
    }
}

// Face Player's possible hands: Four of a kind, Full house, Three of a kind, Two pair, One pair
HandRank assertFigHand(const Card figHand[5])
{
    using enum HandRank;
    // Basically impossible
    Rank fstRank = Rank(2);
    Rank sndRank = Rank(2);
    Rank trdRank = Rank(2);
    Rank fthRank = Rank(2);

    unsigned char numFst = 1, numSnd = 0, numTrd = 0, numFth = 0;

    fstRank = figHand[0].rank;

    for (int i = 1; i < 5; i++)
    {
        if (figHand[i].rank != fstRank)
        {
            if (figHand[i].rank != sndRank)
            {
                if (sndRank == Rank(2))
                {
                    sndRank = figHand[i].rank;
                    numSnd += 1;
                }
                else if (figHand[i].rank != trdRank)
                {
                    if (trdRank == Rank(2))
                    {
                        trdRank = figHand[i].rank;
                        numTrd += 1;
                    }
                    else if (figHand[i].rank != fthRank)
                    {
                        if (fthRank == Rank(2))
                        {
                            fthRank = figHand[i].rank;
                            numFth += 1;
                        }
                        else
                            return HighCard;
                    }
                    else
                        numFth += 1;
                }
                else
                    numTrd += 1;
            }
            else
                numSnd += 1;
        }
        else
            numFst += 1;
    }

    if (numFst == 4 || numSnd == 4)
        return FourofaKind;

    if ((numFst == 3 && numSnd == 2) || (numFst == 2 && numSnd == 3))
        return FullHouse;

    if (numFst == 3 || numSnd == 3 || numTrd == 3)
        return ThreeofaKind;

    if (fthRank == Rank(2))
        return TwoPair;
    else
        return OnePair;
}

// Pip Player's possible hands: Straight flush, Four of a kind, Full house, Flush, Straight, Three of a kind, Two pair, One pair, High card
HandRank assertBlotHand(const Card blotHand[5])
{
    using enum HandRank;
    // Basically impossible
    Rank fstRank = Rank(14);
    Rank sndRank = Rank(14);
    Rank trdRank = Rank(14);
    Rank fthRank = Rank(14);
    Suit fstSuit;

    bool flush = true, straight = true, hCard = false;
    unsigned char numFst = 1, numSnd = 0, numTrd = 0, numFth = 0;

    fstRank = blotHand[0].rank;
    fstSuit = blotHand[0].suit;

    for (int i = 1; i < 5; i++)
    {
        if (blotHand[i].suit != fstSuit)
            flush = false;

        if (blotHand[i].rank != fstRank)
        {
            if (blotHand[i].rank != sndRank)
            {
                if (sndRank == Rank(14))
                {
                    sndRank = blotHand[i].rank;
                    numSnd += 1;
                }
                else if (blotHand[i].rank != trdRank)
                {
                    if (trdRank == Rank(14))
                    {
                        trdRank = blotHand[i].rank;
                        numTrd += 1;
                    }
                    else if (blotHand[i].rank != fthRank)
                    {
                        if (fthRank == Rank(14))
                        {
                            fthRank = blotHand[i].rank;
                            numFth += 1;
                        }
                        else
                            hCard = true;
                    }
                    else
                        numFth += 1;
                }
                else
                    numTrd += 1;
            }
            else
                numSnd += 1;
        }
        else
            numFst += 1;
    }

    if (fthRank != Rank(14))
    {
        int ranks[5];

        for (int i = 0; i < 5; i++)
        {
            ranks[i] = static_cast<int>(blotHand[i].rank);
        }
        std::sort(ranks, ranks + 5);

        for (int i = 1; i < 5; i++)
        {
            if (ranks[i] != ranks[i - 1] + 1)
                straight = false;
        }
    }
    else
        straight = false;

    if (straight && flush)
        return StraightFlush;

    if (numFst == 4 || numSnd == 4)
        return FourofaKind;

    if ((numFst == 3 && numSnd == 2) || (numFst == 2 && numSnd == 3))
        return FullHouse;

    if (flush)
        return Flush;

    if (straight)
        return Straight;

    if (numFst == 3 || numSnd == 3 || numTrd == 3)
        return ThreeofaKind;

    if (fthRank == Rank(14))
        return TwoPair;
    else if (!hCard)
        return OnePair;
    else
        return HighCard;
}

// If players have the same hand Face Player Always wins
//  True - Pip player wins
bool assertWin(const Card figHand[5], const Card blotHand[5])
{
    using enum HandRank;
    HandRank figRank = assertFigHand(figHand);
    HandRank blotRank = assertBlotHand(blotHand);

    if (static_cast<int>(figRank) >= static_cast<int>(blotRank))
        return false;
    else
        return true;
}

// For debugging purposes
void printHand(const Card (&hand)[5])
{
    using enum Rank;
    using enum Suit;

    for (int i = 0; i < 5; i++)
    {
        std::string currRank = static_cast<int>(hand[i].rank) <= 10 ? std::to_string(static_cast<int>(hand[i].rank)).c_str() : hand[i].rank == JACK ? "Jack"
                                                                                                                           : hand[i].rank == QUEEN  ? "Queen"
                                                                                                                           : hand[i].rank == KING   ? "King"
                                                                                                                                                    : "Ace";
        std::string currSuit = hand[i].suit == HEARTS ? "Hearts" : hand[i].suit == DIAMONDS ? "Diamonds"
                                                               : hand[i].suit == CLUBS      ? "Clubs"
                                                                                            : "Spades";
        printf("%s of %s\n", currRank.c_str(), currSuit.c_str());
    }
}

// For debugging purposes
void printHandRank(HandRank rank)
{
    using enum HandRank;

    std::string rankStr = rank == HighCard        ? "High Card"
                          : rank == OnePair       ? "One Pair"
                          : rank == TwoPair       ? "Two Pair"
                          : rank == ThreeofaKind  ? "Three of a Kind"
                          : rank == Straight      ? "Straight"
                          : rank == Flush         ? "Flush"
                          : rank == FullHouse     ? "Full House"
                          : rank == FourofaKind   ? "Four of a Kind"
                          : rank == StraightFlush ? "Straight Flush"
                                                  : "Royal Flush";

    printf("Hand Rank: %s\n", rankStr.c_str());
}

int main(int argc, char *argv[])
{
    unsigned char discardLvl = 0;
    using enum Rank;
    using enum Suit;

    for (int i = 1; i < argc; i++)
    {
        discardLvl = atoi(argv[i]);
    }

    Card figHand[5];
    Card blotHand[5];

    unsigned int figWins = 0, blotWins = 0;

    for (int i = 0; i < 1000000; i++)
    {
        genFigHand(figHand);
        genBlotHand(blotHand, discardLvl);

        bool blotWin = assertWin(figHand, blotHand);

        if (blotWin)
            blotWins += 1;
        else
            figWins += 1;
    }

    printf("Chance of winning with %d discards of the lowest cards: %.3f%\n", discardLvl, 100 * ((float)blotWins / (float)(blotWins + figWins)));

    return 0;
}