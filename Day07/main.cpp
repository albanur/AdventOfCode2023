#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<vector>
#include<algorithm>

using namespace std;

struct CardSet
{
    char cards[5];
    int64_t bet;

    CardSet(const char* line)
    {
        for(int i = 0; i < 5; i++)
        {
            cards[i] = line[i];
        }
        bet = atoll(line +6);
    }
};

int GetCardValue(char c)
{
    switch(c)
    {
        case 'A': return 14;
        case 'K': return 13;
        case 'Q': return 12;
        case 'J': return 11;
        case 'T': return 10;
        case '9': return 9;
        case '8': return 8;
        case '7': return 7;
        case '6': return 6;
        case '5': return 5;
        case '4': return 4;
        case '3': return 3;
        case '2': return 2;
        default: return 0;
    }
}

int GetCardValue2(char c)
{
    switch(c)
    {
        case 'A': return 14;
        case 'K': return 13;
        case 'Q': return 12;
        case 'J': return 1;
        case 'T': return 10;
        case '9': return 9;
        case '8': return 8;
        case '7': return 7;
        case '6': return 6;
        case '5': return 5;
        case '4': return 4;
        case '3': return 3;
        case '2': return 2;
        default: return 0;
    }
}

enum SetType
{
    FiveOfAKind = 7,
    FourOfAKind = 6,
    FullHouse = 5,
    ThreeOfAKind = 4,
    TwoPair = 3,
    OnePair = 2,
    HighRank = 1
};

SetType GetSetType(CardSet set)
{
    int cards[15];
    for(int i = 0; i < 15; i++)
    {
        cards[i] = 0;
    }

    for(int i = 0; i < 5; i++)
    {
        cards[GetCardValue(set.cards[i])]++;
    }

    sort(cards, cards+15);

    if(cards[14] == 5) return FiveOfAKind;
    if(cards[14] == 4) return FourOfAKind;
    if(cards[14] == 3 && cards[13] == 2) return FullHouse;
    if(cards[14] == 3) return ThreeOfAKind;
    if(cards[14] == 2 && cards[13] == 2) return TwoPair;
    if(cards[14] == 2) return OnePair;
    return HighRank;
}

SetType GetSetType2(CardSet set)
{
    int cards[15];
    int jokers = 0;
    for(int i = 0; i < 15; i++)
    {
        cards[i] = 0;
    }

    for(int i = 0; i < 5; i++)
    {
        int v = GetCardValue2(set.cards[i]);
        if(v == 1)
        {
            jokers++;
        }
        else
        {
            cards[v]++;
        }
    }

    sort(cards, cards+15);

    if(cards[14] + jokers == 5) return FiveOfAKind;
    if(cards[14] + jokers == 4) return FourOfAKind;
    if(cards[14] + jokers == 3 && cards[13] == 2) return FullHouse;
    if(cards[14] + jokers == 3) return ThreeOfAKind;
    if(cards[14] + jokers == 2 && cards[13] == 2) return TwoPair;
    if(cards[14] + jokers == 2) return OnePair;
    return HighRank;
}

bool operator < (const CardSet& s1, const CardSet& s2)
{
    SetType t1 = GetSetType2(s1);
    SetType t2 = GetSetType2(s2);
    if(t1 != t2) return t1 < t2;

    for(int i = 0; i < 5; i++)
    {
        int v1 = GetCardValue2(s1.cards[i]);
        int v2 = GetCardValue2(s2.cards[i]);
        if(v1 != v2) return v1 < v2;
    }
    return false;
}

int main()
{
    int64_t sum = 0;
    char buffer[256];
    vector<CardSet> sets;

    while(fgets(buffer, 256, stdin) != NULL)
    {
        if(strlen(buffer) > 0)
        {
            sets.push_back(CardSet(buffer));
        }
    }

    sort(sets.begin(), sets.end());

    for(int64_t i = 0; i < sets.size(); i++)
    {
        sum += (i+1) * sets[i].bet;
    }

    printf("%lld\n", sum);

    return 0;
}