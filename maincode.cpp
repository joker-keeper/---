#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<algorithm>
#include<vector>

using namespace std;

int endGame[100000];//存放数独终局，一维数组形式

class sudokuboard
{
    int board[9][9];
    int row[9][10];
    int col[9][10];
    int box[9][10];
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

    void Perm(int* array, int begin, int end, int op=1) {
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
                Perm(array, begin+1, end,op);
                swap(array[i], array[begin]);
            }
        }
    }

    void genRowIndex() {
        //帮忙改改这里，就是怎么得到那72种变化形式，这里实在迷了，要么优化这里要么优化那个Perm()函数
        int row[8] = { 1,2,3,4,5,6,7,8 };
        Perm(row,0,2,0);
        for (int i = 0;i < 2;i++) {
            std::copy(this->row_index[i].begin(),this->row_index[i].end(),row);
            row_index.erase(row_index.begin());
            Perm(row,2,5,0);
        }
        for (int i = 0;i < 12;i++) {
            std::copy(this->row_index[i].begin(), this->row_index[i].end(), row);
            row_index.erase(row_index.begin());
            Perm(row, 5, 8,0);
        }
    }

    void produce_sudo(int N) {
        /*
         * 生成数独终局的模块。考虑到每一种数独终局在交换2~3、4~6、7~9行后可以得到新的终局(一共72种)。
         * 我们对传入参数N做判断，看一共需要几轮全排列(第一行全排列后按照规律平移可以得到一种数独终局)
         * 这里默认全排列是从后往前，例如{1,2,3,4,5,6,7,8,9}需要两轮全排列，即{...,8,9}和{...,9,8}
         */
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
        int count = 0,row_count=1;
        Perm(first_line,9-n,9);
        //得到行表数组的72种排列，这一段不知道咋改了，很笨。主要是Perm函数实现的不好，orz。
        genRowIndex();     
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
                    endGame[k + 81 * count] = first_line[k];
                }
                for (int i = 1;i < 9;i++) {
                    for (int j = 0;j < 9;j++) {
                        endGame[i*9+j+81*count]=this->board[row[i-1]][j];
                    }
                }
                count++;
                if (count == N) {
                    // 将数独终局写入文件中，这里是将生成的所有数独终局以一维数组的形式全部写入一个文件中，如果后面感觉不如一个
                    // 数独终局写入一个文件，这里也可以改一改
                    fstream file;
                    file.open("endGame.txt", ios::out);
                    for (int i = 0;i < N * 81;i++) {
                        file << endGame[i];
                    }
                    file.close();
                    return;
                }
                std::copy(row_index[row_count].begin(),row_index[row_count].end(),row);
                row_count++;
            }
            row_count = 0;
            round--;
        }
    }

    void genSudoGame1(const char* endGamePath, int n) {
        ifstream file;
        file.open("endGame.txt", ios::in);
        char* game = new char[n * 81+1];
        file >> game;
        fstream outfile;
        outfile.open("game.txt", ios::out);
        while (n) {
            /*
            难度1就是每行固定挖2个，一共挖去18个生成数独游戏。
            */
            //todo:这里随机数还是有问题，是伪随机，每个棋盘挖空的点都一样。
            srand((unsigned)time(NULL));
            int x, y;
            for (int i = 0;i < 9;i++) {
                //生成下标[0,9)之间的随机数
                x = rand() % 9;
                do {
                    y = rand() % 9;
                } while (x == y);
                //这里由于使用的是int类型作为数独棋盘，因此将挖空部位填充为0。
                game[(n-1) * 81 + i * 9 + x] = game[(n-1) * 81 + i * 9 + y] = '_';
            }
            n--;
        }
        for (int i = 0;i < 20 * 81;i++) {
            if (i % 9 == 0)
                cout << endl;
            if (i % 81 == 0)
                cout << endl;
            cout << game[i] << " ";
        }
        outfile << game;
        outfile.close();
    }
    void genSudoGame2(const char* endGamePath, int n) {
        ifstream file;
        file.open("endGame.txt", ios::in);
        char* game = new char[n * 81 + 1];
        file >> game;
        fstream outfile;
        outfile.open("game.txt", ios::out);
        while (n) {
            /*
            难度2就是每行固定挖3个，一共挖去27个生成数独游戏。
            */
            //todo:这里随机数还是有问题，是伪随机，每个棋盘挖空的点都一样。
            srand((unsigned)time(NULL));
            int x,y,z;
            for (int i = 0;i < 9;i++) {
                //生成下标[0,9)之间的随机数
                x = rand() % 9;
                do {
                    y = rand() % 9;
                } while (x == y);
                do {
                    z = rand() % 9;
                } while (x == z || y == z);
                //这里由于使用的是int类型作为数独棋盘，因此将挖空部位填充为0。
                game[(n - 1) * 81 + i * 9 + x] = game[(n - 1) * 81 + i * 9 + y] = game[(n - 1) * 81 + i * 9 + z] = '_';
            }
            n--;
        }
        for (int i = 0;i < 20 * 81;i++) {
            if (i % 9 == 0)
                cout << endl;
            if (i % 81 == 0)
                cout << endl;
            cout << game[i] << " ";
        }
        outfile << game;
        outfile.close();
    }
    void genSudoGame3(const char* endGamePath, int n) {
        ifstream file;
        file.open("endGame.txt", ios::in);
        char* game = new char[n * 81 + 1];
        file >> game;
        fstream outfile;
        outfile.open("game.txt", ios::out);
        while (n) {
            /*
            难度3就是每行固定挖4个，一共挖去36个生成数独游戏。
            */
            //todo:这里随机数还是有问题，是伪随机，每个棋盘挖空的点都一样。
            srand((unsigned)time(NULL));
            int x, y, z, m;
            for (int i = 0;i < 9;i++) {
                //生成下标[0,9)之间的随机数
                x = rand() % 9;
                do {
                    y = rand() % 9;
                } while (x == y);
                do {
                    z = rand() % 9;
                } while (x == z || y == z);
                do {
                    m = rand() % 9;
                } while (x == m || y == m || z == m);
                //这里由于使用的是int类型作为数独棋盘，因此将挖空部位填充为0。
                game[(n - 1) * 81 + i * 9 + x] = game[(n - 1) * 81 + i * 9 + y] = game[(n - 1) * 81 + i * 9 + z] = game[(n - 1) * 81 + i * 9 + m] = '_';
            }
            n--;
        }
        for (int i = 0;i < 20 * 81;i++) {
            if (i % 9 == 0)
                cout << endl;
            if (i % 81 == 0)
                cout << endl;
            cout << game[i] << " ";
        }
        outfile << game;
        outfile.close();
    }

    void genSudoGame(const char* endGamePath,int n,int level) {
        /*
         * 生成数独游戏,这里游戏难度的主要依据是挖空总数目和每一行上挖空的数量(这里只考虑了行，实现简便一些，但没考虑列和宫，因此可能出现一些极端情况)
         * endGamePath:存放数独终局的文件路径
         * n:需要生成数独游戏的数量
         * level:数独游戏的难度:1~3
         */
        switch (level){
            case 1:
                genSudoGame1(endGamePath,n);
                break;
            case 2:
                genSudoGame2(endGamePath, n);
                break;
            case 3:
                genSudoGame3(endGamePath, n);
                break;
            default:
                cout << "args level error!" << endl;
        }   
    }

    void input()
    {
        cout << "Enter the sudoku board" << endl;
        for (int i = 0;i < 9;i++)
        {
            for (int j = 0;j < 9;j++)
            {
                cin >> board[i][j];
                if (board[i][j] != 0)
                {
                    row[i][board[i][j]] = 1;
                    col[j][board[i][j]] = 1;
                    box[(i / 3) * 3 + (j / 3)][board[i][j]] = 1;
                }
            }
        }
    }
    void print()
    {
        cout << "The sudoku board is" << endl;
        for (int i = 0;i < 9;i++)
        {
            for (int j = 0;j < 9;j++)
            {
                cout << board[i][j] << " ";
            }
            cout << endl;
        }
    }

    void print_endGame() {
        /*
        打印数独终局
        */
        /*for (int i = 0;i < 20 * 81;i++) {
            if (i % 9 == 0)
                cout << endl;
            if (i % 81 == 0)
                cout << endl;
            cout << endGame[i] << " ";
        }*/
        ifstream file;
        file.open("endGame.txt", ios::in);
        char buf[100000] = { 0 };
        file >> buf;
        for (int i = 0;i < 20 * 81;i++) {
            if (i % 9 == 0)
                cout << endl;
            if (i % 81 == 0)
                cout << endl;
            cout << buf[i] << " ";
        }
    }

    void print_game() {
        /*
        打印数独游戏棋盘
        */
        ifstream file;
        file.open("game.txt", ios::in);
        char buf[100000] = { 0 };
        file >> buf;
        for (int i = 0;i < 20 * 81;i++) {
            if (i % 9 == 0)
                cout << endl;
            if (i % 81 == 0)
                cout << endl;
            cout << buf[i] << " ";
        }
    }

    bool solve(int i, int j)
    {
        if (i == 9)
        {
            return true;
        }
        if (j == 9)
        {
            return solve(i + 1, 0);
        }
        if (board[i][j] != 0)
        {
            return solve(i, j + 1);
        }
        for (int k = 1;k <= 9;k++)
        {
            if (row[i][k] == 0 && col[j][k] == 0 && box[(i / 3) * 3 + (j / 3)][k] == 0)
            {
                board[i][j] = k;
                row[i][k] = 1;
                col[j][k] = 1;
                box[(i / 3) * 3 + (j / 3)][k] = 1;
                if (solve(i, j + 1))
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
};

int main() {
    sudokuboard sudo;
    int arr[9] = { 1,2,3,4,5,6,7,8,9 };
   /* sudo.genRowIndex();
    sudo.produce_sudo(20);*/
    sudo.print_endGame();
    sudo.genSudoGame("endGame.txt", 20, 3);
    //sudo.print_game();
    //int result[9];
    //for (int i = 0;i < sudo.rst.size();i++) {
    //    vector<int>temp = sudo.rst[i];
    //    /*for (int j = 0;j < 9;j++) {
    //        cout << temp[j] << " ";
    //    }cout << endl;*/
    //    std::copy(temp.begin(),temp.end(),result);
    //    //memcpy(result, &temp[0], temp.size() * sizeof(temp[0]));
    //    for (int j = 0;j < 9;j++) {
    //        cout << temp[j] << " ";
    //    }cout << endl;
    //}
}
