#include<iostream>
using namespace std;
class sudokuboard
{
    int board[9][9];
    int row[9][10];
    int col[9][10];
    int box[9][10];
    public:
    sudokuboard()
    {
        for(int i=0;i<9;i++)
        {
            for(int j=0;j<9;j++)
            {
                board[i][j]=0;
            }
        }
        for(int i=0;i<9;i++)
        {
            for(int j=0;j<10;j++)
            {
                row[i][j]=0;
                col[i][j]=0;
                box[i][j]=0;
            }
        }
    }
    void input()
    {
        cout<<"Enter the sudoku board"<<endl;
        for(int i=0;i<9;i++)
        {
            for(int j=0;j<9;j++)
            {
                cin>>board[i][j];
                if(board[i][j]!=0)
                {
                    row[i][board[i][j]]=1;
                    col[j][board[i][j]]=1;
                    box[(i/3)*3+(j/3)][board[i][j]]=1;
                }
            }
        }
    }
    void print()
    {
        cout<<"The sudoku board is"<<endl;
        for(int i=0;i<9;i++)
        {
            for(int j=0;j<9;j++)
            {
                cout<<board[i][j]<<" ";
            }
            cout<<endl;
        }
    }
    bool solve(int i,int j)
    {
        if(i==9)
        {
            return true;
        }
        if(j==9)
        {
            return solve(i+1,0);
        }
        if(board[i][j]!=0)
        {
            return solve(i,j+1);
        }
        for(int k=1;k<=9;k++)
        {
            if(row[i][k]==0 && col[j][k]==0 && box[(i/3)*3+(j/3)][k]==0)
            {
                board[i][j]=k;
                row[i][k]=1;
                col[j][k]=1;
                box[(i/3)*3+(j/3)][k]=1;
                if(solve(i,j+1))
                {
                    return true;
                }
                board[i][j]=0;
                row[i][k]=0;
                col[j][k]=0;
                box[(i/3)*3+(j/3)][k]=0;
            }
        }
        return false;
    }
};
