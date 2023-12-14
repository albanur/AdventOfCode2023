#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>
#include<cstdint>
#include<map>

using namespace std;

enum Tile
{
    Empty = '.',
    RoundedRock = 'O',
    CubeRock = '#'
};

class Board
{
private:
    vector<vector<Tile>> board;

public:
    Board(){}

    bool AddLine(const char* line)
    {
        int length = strlen(line);
        if(length <= 1) { return false; }

        vector<Tile> row;
        for(int i = 0; i < length; i++)
        {
            if(line[i] == '.') { row.push_back(Empty); }
            else if(line[i] == '#') { row.push_back(CubeRock); }
            else if(line[i] == 'O') { row.push_back(RoundedRock); }
            else { break; }
        }

        board.push_back(row);

        return true;
    }

    void MoveUp()
    {
        for(int r = 1; r < board.size(); r++)
        {
            vector<Tile>& row = board[r];

            for(int c = 0; c < row.size(); c++)
            {
                if(row[c] != RoundedRock) { continue; }
                int i = r - 1;
                while(i >= 0 && board[i][c] == Empty)
                {
                    board[i][c] = RoundedRock;
                    board[i+1][c] = Empty;
                    i--;
                }
            }
        }
    }

    void MoveDown()
    {
        for(int r = board.size() - 2; r >= 0; r--)
        {
            for(int c = 0; c < board[0].size(); c++)
            {
                if(board[r][c] != RoundedRock) { continue; }
                int i = r + 1;
                while(i < board.size() && board[i][c] == Empty)
                {
                    board[i][c] = RoundedRock;
                    board[i-1][c] = Empty;
                    i++;
                }
            }
        }
    }

    void MoveLeft()
    {
        for(int c = 1; c < board[0].size(); c++)
        {
            for(int r = 0; r < board.size(); r++)
            {
                if(board[r][c] != RoundedRock) { continue; }
                int i = c - 1;
                while(i >= 0 && board[r][i] == Empty)
                {
                    board[r][i] = RoundedRock;
                    board[r][i+1] = Empty;
                    i--;
                }
            }
        }
    }

    void MoveRight()
    {
        for(int c = board[0].size() - 2; c >= 0; c--)
        {
            for(int r = 0; r < board.size(); r++)
            {
                if(board[r][c] != RoundedRock) { continue; }
                int i = c + 1;
                while(i < board[0].size() && board[r][i] == Empty)
                {
                    board[r][i] = RoundedRock;
                    board[r][i-1] = Empty;
                    i++;
                }
            }
        }
    }

    void Print()
    {
        for(int r = 0; r < board.size(); r++)
        {
            for(int c = 0; c < board[r].size(); c++)
            {
                printf("%c", (char)board[r][c]);
            }
            printf("\n");
        }
        printf("\n");
    }

    int64_t CalculateLoad()
    {
        int64_t load = 0LL;

        int rows = (int)board.size();

        for(int r = 0; r < board.size(); r++)
        {
            vector<Tile>& row = board[r];

            for(int c = 0; c < board.size(); c++)
            {
                if(row[c] != RoundedRock) { continue; }
                
                load += rows - r;
            }
        }

        return load;
    }

    bool Equals(Board& other)
    {
        for(int r = 0; r < board.size(); r++)
        {
            for(int c = 0; c < board[r].size(); c++)
            {
                if(board[r][c] != other.board[r][c]) { return false; }
            }
        }
        return true;
    }
};

class LoadHistory
{
private:
    map<int64_t, vector<int>> history;

public:
    LoadHistory(){}

    bool Contains(int64_t load)
    {
        return history.find(load) != history.end();
    }

    vector<int>& GetHistory(int64_t load)
    {
        return history[load];
    }

    void AddEntry(int64_t load, int index)
    {
        if(!Contains(load))
        {
            history[load] = vector<int>();
        }
        history[load].push_back(index);
    }
};

int main()
{
    int64_t load = 0LL;
    char buffer[1024];
    Board board;
    int64_t l;

    vector<Board> boardHistory;
    LoadHistory loadHistory;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        board.AddLine(buffer);
    }

    boardHistory.push_back(Board(board));
    l = board.CalculateLoad();
    loadHistory.AddEntry(l, 0);

    board.MoveUp();
    load = board.CalculateLoad();
    printf("%lld\n", load);

    board.MoveLeft();
    board.MoveDown();
    board.MoveRight();
    boardHistory.push_back(Board(board));
    l = board.CalculateLoad();
    loadHistory.AddEntry(l, 1);

    int64_t first, duplicated;
    bool founded = false;

    for(int i = 2; !founded; i++)
    {
        board.MoveUp();
        board.MoveLeft();
        board.MoveDown();
        board.MoveRight();
        
        l = board.CalculateLoad();

        if (loadHistory.Contains(l))
        {
            vector<int>& indexes = loadHistory.GetHistory(l);
            for(auto id : indexes)
            {
                if(board.Equals(boardHistory[id]))
                {
                    first = id;
                    duplicated = i;
                    founded = true;
                    break;
                }
            }
        }

        boardHistory.push_back(Board(board));
        loadHistory.AddEntry(l, i);
    }

    int64_t cycle = duplicated - first;
    int64_t total = 1000000000LL;
    total -= first;
    int64_t next = total % cycle;

    while(next > 0)
    {
        board.MoveUp();
        board.MoveLeft();
        board.MoveDown();
        board.MoveRight();
        next--;
    }

    load = board.CalculateLoad();
    printf("%lld\n", load);

    return 0;
}