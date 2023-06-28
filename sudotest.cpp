#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<algorithm>
#include<vector>
#include<string>
#include<cstring>
using namespace std;

int endGame[81]{};//存放数独终局，一维数组形式

class sudokuboard
{
    int board[9][9]{};
    int row[9][10]{};
    int col[9][10]{};
    int box[9][10]{};
public:
    vector<vector<int>>rst; //存放数独第一行全排列的结果
    vector<vector<int>>row_index;//行下标的72种组合方式，由于并不多，就直接放进来了。。。
    sudokuboard()
    {
        for (int i = 0;i < 9;i++)
        {
            for (int j = 0;j < 9;j++)
            {
                board[i][j] = 0;
            }
        }
        for (int i = 0;i < 9;i++)
        {
            for (int j = 0;j < 10;j++)
            {
                row[i][j] = 0;
                col[i][j] = 0;
                box[i][j] = 0;
            }
        }
    }

    void _Perm(int* array, int begin, int end, int op=1) {
        /*
         * 递归法实现全排列，旨在将array的[begin,end)进行全排列。
         * 这个实现的复杂度有点高O(n!)，同时可能需要的排列个数并不等于(size-N)!。可以改进一下
         */
        if (begin == end) {
            if (op == 1) {
                vector<int>temp(array, array + 9);
                rst.push_back(temp);
            }
            else {
                vector<int>temp(array, array + 9);
                row_index.push_back(temp);
            }
        }
        else {
            for (int i = begin;i < end;i++) {
                swap(array[i], array[begin]);
                _Perm(array, begin+1, end,op);
                swap(array[i], array[begin]);
            }
        }
    }

    void _genRowIndex() {
        //帮忙改改这里，就是怎么得到那72种变化形式，这里实在迷了，要么优化这里要么优化那个Perm()函数
        int row[8] = { 1,2,3,4,5,6,7,8 };
        _Perm(row,0,2,0);
        for (int i = 0;i < 2;i++) {
            std::copy(this->row_index[i].begin(),this->row_index[i].end(),row);
            row_index.erase(row_index.begin());
            _Perm(row,2,5,0);
        }
        for (int i = 0;i < 12;i++) {
            std::copy(this->row_index[i].begin(), this->row_index[i].end(), row);
            row_index.erase(row_index.begin());
            _Perm(row, 5, 8,0);
        }
    }

    void _init()
    {
        memset(row, 0, sizeof(row));
        memset(col, 0, sizeof(col));
        memset(box, 0, sizeof(box));
        memset(board, 0, sizeof(board));
    }

    bool _solve(int i, int j, int n=0)
    {
        static int solvewant = 0;
        if(n){solvewant = n;}
        if (i == 9)
        {
            solvewant-=1;
            return solvewant == 0;
        }
        if (j == 9)
        {
            return _solve(i + 1, 0);
        }
        if (board[i][j] != 0)
        {
            return _solve(i, j + 1);
        }
        for (int k = 1;k <= 9;k++)
        {
            if (row[i][k] == 0 && col[j][k] == 0 && box[(i / 3) * 3 + (j / 3)][k] == 0)
            {
                board[i][j] = k;
                row[i][k] = 1;
                col[j][k] = 1;
                box[(i / 3) * 3 + (j / 3)][k] = 1;
                if (_solve(i, j + 1))
                {
                    return true;
                }
                board[i][j] = 0;
                row[i][k] = 0;
                col[j][k] = 0;
                box[(i / 3) * 3 + (j / 3)][k] = 0;
            }
        }
        return false;
    }

    void _readInSudoku(fstream& f)
{
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            int num;
            f>>num;
            this->board[i][j]=num;
            this->row[i][num]=1;
            this->col[j][num]=1;
            this->box[(i/3)*3+(j/3)][num]=1;
        }
    }

}

    bool _isOneResult()
    {
        int tempboard[9][9];
        memcpy(tempboard, this->board, sizeof(this->board));
        bool result=_solve(0, 0, 2);
        memcpy(this->board, tempboard, sizeof(tempboard));
        return !result;
    }

    void produce_sudo(int N,string filepath="endGame.txt") {
        /*
         * 生成数独终局的模块。考虑到每一种数独终局在交换2~3、4~6、7~9行后可以得到新的终局(一共72种)。
         * 我们对传入参数N做判断，看一共需要几轮全排列(第一行全排列后按照规律平移可以得到一种数独终局)
         * 这里默认全排列是从后往前，例如{1,2,3,4,5,6,7,8,9}需要两轮全排列，即{...,8,9}和{...,9,8}
         * N:生成的终局数目
         * filepath:生成的终局存放的文件路径
         */
        fstream file;
        file.open(filepath, ios::out);
        int round = N % 72 == 0 ? N / 72 : N / 72 + 1 ;
        //需要对数组的后几位进行全排列
        int n=1,fac=1;
        for (int i = 1;i <= 9;i++) {
            fac *= i;
            if (fac >= round) {
                n = i;
                break;
            }
        }
        int first_line[9] = { 6,8,7,1,2,4,3,9,5 };//数独第一行的一种情况
        int change[8] = { 3,6,1,4,7,2,5,8 };//这是一个规律，数独第一行分别向左移动change后可以得到一个数独终局
        int row[8] = { 1,2,3,4,5,6,7,8 };//这里使用行表数组便于后续对数独终局的2~3、4~6、7~9行全排列
        int row_count=1;
        _Perm(first_line,9-n,9);
        //得到行表数组的72种排列，这一段不知道咋改了，很笨。主要是Perm函数实现的不好，orz。
        _genRowIndex();
        int count=0;     
        while (round != 0) {
            //生成数独终局
            std::copy(rst[round-1].begin(),rst[round-1].end(),first_line);
            for (int j = 0;j < 9;j++)
                this->board[0][j] = first_line[j];
            for (int i = 1;i < 9;i++) {
                for (int j = 0;j < 9;j++) {
                    this->board[i][j] = first_line[(change[i-1]+j)%9];
                }
            }
            //存放数独终局
            while (row_count<72) {
                for (int k = 0;k < 9;k++) {
                    endGame[k] = first_line[k];
                }
                for (int i = 1;i < 9;i++) {
                    for (int j = 0;j < 9;j++) {
                        endGame[i*9+j]=this->board[row[i-1]][j];
                    }
                }
                count++;
                if(count<=N){
                for(int i=0;i<9;i++){
                for(int j=0;j<9;j++){
                    file<<endGame[i*9+j]<<' ';
                }
                file<<endl;
                }
                file<<endl;
                }
                std::copy(row_index[row_count].begin(),row_index[row_count].end(),row);
                row_count++;
            }
            row_count = 0;
            round--;
        }
        file.close();
    }

    void genGameWithLevel(int gamenum,int gamelevel,bool oneresult=false,string outpath="game.txt",string inpath="endGame.txt")
    {
        /*
        用等级生成数独游戏的函数，等级123代表每行挖走的格子数，等级1最简单，等级3最难分别挖走234格
        gamenum:生成的游戏数目
        gamelevel:游戏等级
        oneresult:是否要求只有一个解
        outpath:生成的游戏存放的文件路径
        inpath:生成游戏所依赖的终局文件路径
        */
        if(gamelevel<1||gamelevel>3)
        {
            cout<<"gamelevel must be 1,2 or 3"<<endl;
            return;
        }
        gamelevel+=1;
        int rands[4];
        srand((unsigned)time(NULL));
        fstream readin;
        fstream writeout;
        readin.open(inpath,ios::in);
        writeout.open(outpath,ios::out);
        for(int n=0;n<gamenum;n++)
        {
            
            bool flag2=false;
            int tempboard[9][9],temprow[9][10],tempcol[9][10],tempbox[9][10];
            if(oneresult)
            {
                flag2=true;
                _init();
            }
            _readInSudoku(readin);
            if(oneresult)
            {
            memcpy(tempboard,this->board,sizeof(this->board));
            memcpy(temprow,this->row,sizeof(this->row));
            memcpy(tempcol,this->col,sizeof(this->col));
            memcpy(tempbox,this->box,sizeof(this->box));
            }
            do{
                //按等级挖空
            for(int i=0;i<9;i++)
            {
                for(int l=0;l<gamelevel;l++)
                {
                    bool flag=false;
                    while(!flag)
                    {
                    flag=true;
                    rands[l]=rand()%9;
                    for(int l2=l-1;l2>=0;l2--)
                    {
                        if(rands[l]==rands[l2])
                        {
                            flag=false;
                            break;
                        }
                    }
                    }
                    this->board[i][rands[l]]=0;
                    this->row[i][rands[l]]=0;
                    this->col[rands[l]][i]=0;
                    this->box[i/3*3+rands[l]/3][i%3*3+rands[l]%3]=0;
                }
            }
            
            //判断是否有唯一解
            if(oneresult)
            {
                bool is_one_result=_isOneResult();
                if(is_one_result)
                {
                    flag2=false;
                    break;
                }
                else
                {
                    memcpy(this->board,tempboard,sizeof(this->board));
                    memcpy(this->row,temprow,sizeof(this->row));
                    memcpy(this->col,tempcol,sizeof(this->col));
                    memcpy(this->box,tempbox,sizeof(this->box));
                }
            }

            }
            while(flag2);


            //写入
            for(int i=0;i<9;i++)
            {
                for(int j=0;j<9;j++)
                {
                    writeout<<this->board[i][j]<<' ';
                }
                if(n!=gamenum-1||i!=8)
                writeout<<endl;
            }
            if(n!=gamenum-1)
            writeout<<endl;
        }
        readin.close();
        writeout.close();
    }

    void solveSUdokus(string inpath="game.txt",string outpath="sudoku.txt")
    {
        /*
        解数独的函数
        inpath:数独游戏文件路径
        outpath:数独解文件路径
        */
        fstream readin;
        fstream writeout;
        readin.open(inpath,ios::in);
        writeout.open(outpath,ios::out);
        while(!readin.eof()){
            _init();
            _readInSudoku(readin);
            if(readin.eof())
            {
                break;
            }
            _solve(0,0,1);
            for(int i=0;i<9;i++)
            {
                for(int j=0;j<9;j++)
                {
                    writeout<<this->board[i][j]<<' ';
                }
                writeout<<endl;
            }
            writeout<<endl;
        }
        readin.close();
        writeout.close();
    }

    void genGameWithHollowsNum(int gamenum,int hollowsnum,bool oneresult=false,string outpath="game.txt",string inpath="endGame.txt")
    {
        /*
        用挖空数目生成数独游戏的函数
        gamenum:生成的游戏数目
        hollowsnum:挖空数目，1-64
        oneresult:是否要求只有一个解
        outpath:生成的游戏存放的文件路径
        inpath:生成游戏所依赖的终局文件路径
        */
        if(hollowsnum<1||hollowsnum>64)
        {
            cout<<"hollowsnum must be 1-64"<<endl;
            return;
        }
        int rands[64];
        srand((unsigned)time(NULL));
        fstream readin;
        fstream writeout;
        readin.open(inpath,ios::in);
        writeout.open(outpath,ios::out);
        for(int n=0;n<gamenum;n++)
        {
            bool flag2=false;
            int tempboard[9][9],temprow[9][10],tempcol[9][10],tempbox[9][10];
            if(oneresult)
            {
                flag2=true;
                _init();
            }
            _readInSudoku(readin);
            if(oneresult)
            {
            memcpy(tempboard,this->board,sizeof(this->board));
            memcpy(temprow,this->row,sizeof(this->row));
            memcpy(tempcol,this->col,sizeof(this->col));
            memcpy(tempbox,this->box,sizeof(this->box));
            }
            do{
                //挖空之
                for(int l=0;l<hollowsnum;l++)
                {
                    bool flag=false;
                    while(!flag)
                    {
                    flag=true;
                    rands[l]=rand()%81;
                    for(int l2=l-1;l2>=0;l2--)
                    {
                        if(rands[l]==rands[l2])
                        {
                            flag=false;
                            break;
                        }
                    }
                    }
                    this->board[rands[l]/9][rands[l]%9]=0;
                    this->row[rands[l]/9][rands[l]%9]=0;
                    this->col[rands[l]%9][rands[l]/9]=0;
                    this->box[rands[l]/27*3+rands[l]%9/3][rands[l]%9%3]=0;
                }
            
            //判断是否有唯一解
            if(oneresult)
            {
                bool is_one_result=_isOneResult();
                if(is_one_result)
                {
                    flag2=false;
                    break;
                }
                else
                {
                    memcpy(this->board,tempboard,sizeof(this->board));
                    memcpy(this->row,temprow,sizeof(this->row));
                    memcpy(this->col,tempcol,sizeof(this->col));
                    memcpy(this->box,tempbox,sizeof(this->box));
                }
            }

            }
            while(flag2);
            for(int i=0;i<9;i++)
            {
                for(int j=0;j<9;j++)
                {
                    writeout<<this->board[i][j]<<' ';
                }
                if(n!=gamenum-1||i!=8)
                writeout<<endl;
            }
            if(n!=gamenum-1)
            writeout<<endl;
            
        }
        readin.close();
        writeout.close();
    }

};

int main()
{
sudokuboard s;
s.produce_sudo(3);
s.genGameWithHollowsNum(3,20,true);
s.solveSUdokus();
}
