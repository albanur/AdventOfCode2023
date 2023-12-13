#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>

using namespace std;

class Pattern
{
private:
    vector<vector<bool>> board;

public:
    Pattern(){}

    bool AddLine(const char* line)
    {
        int length = strlen(line);
        if(length <= 1) { return false; }

        vector<bool> row;
        for(int i = 0; i < length; i++)
        {
            if(line[i] == '.') { row.push_back(false); }
            else if(line[i] == '#') { row.push_back(true); }
            else { break; }
        }

        board.push_back(row);

        return true;
    }

    bool IsEmpty()
    {
        return board.empty();
    }

    void Clear()
    {
        board.clear();
    }

    int FindMirroredRow(int difference)
    {
        int rows = (int)board.size();

        for(int i = 1; i < rows; i++)
        {
            int diff = 0;
            for(int j = 1; (i - j) >= 0 && (i + j - 1) < rows; j++)
            {
                diff += CompareRows(i-j, i+j-1);
            }
            if(diff == difference) { return i; }
        }

        return 0;
    }

    int FindMirroredColumn(int difference)
    {
        int columns = (int)board[0].size();

        for(int i = 1; i < columns; i++)
        {
            int diff = 0;
            for(int j = 1; (i - j) >= 0 && (i + j - 1) < columns; j++)
            {
                diff += CompareColumns(i-j, i+j-1);
            }
            if(diff == difference) { return i; }
        }

        return 0;
    }

private:
    int CompareRows(int r1, int r2)
    {
        int diff = 0;
        vector<bool>& row1 = board[r1];
        vector<bool>& row2 = board[r2];

        for(int i = 0; i < (int)row1.size(); i++)
        {
            if(row1[i] != row2[i]) { diff++; }
        }

        return diff;
    }

    int CompareColumns(int c1, int c2)
    {
        int diff = 0;

        for(int i = 0; i < (int)board.size(); i++)
        {
            if(board[i][c1] != board[i][c2]) { diff++; }
        }

        return diff;
    }
};

int main()
{
    int sum0 = 0;
    int sum1 = 0;
    char buffer[1024];
    Pattern pattern;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        if(!pattern.AddLine(buffer))
        {
            if(!pattern.IsEmpty())
            {
                sum0 += 100 * pattern.FindMirroredRow(0) + pattern.FindMirroredColumn(0);
                sum1 += 100 * pattern.FindMirroredRow(1) + pattern.FindMirroredColumn(1);
            }
            
            pattern.Clear();
        }
    }

    printf("%i\n", sum0);
    printf("%i\n", sum1);

    return 0;
}