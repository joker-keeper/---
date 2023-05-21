#include<iostream>
#include<algorithm>
#include<vector>

using namespace std;

int endGame[100000];//��������վ֣�һά������ʽ

class sudokuboard
{
    int board[9][9];
    int row[9][10];
    int col[9][10];
    int box[9][10];
public:
    vector<vector<int>>rst; //���������һ��ȫ���еĽ��
    vector<vector<int>>row_index;//���±��72����Ϸ�ʽ�����ڲ����࣬��ֱ�ӷŽ����ˡ�����
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
         * �ݹ鷨ʵ��ȫ���У�ּ�ڽ�array��[begin,end)����ȫ���С�
         * ���ʵ�ֵĸ��Ӷ��е��O(n!)��ͬʱ������Ҫ�����и�����������(size-N)!�����ԸĽ�һ��
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
        //��æ�ĸ����������ô�õ���72�ֱ仯��ʽ������ʵ�����ˣ�Ҫô�Ż�����Ҫô�Ż��Ǹ�Perm()����
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
         * ���������վֵ�ģ�顣���ǵ�ÿһ�������վ��ڽ���2~3��4~6��7~9�к���Եõ��µ��վ�(һ��72��)��
         * ���ǶԴ������N���жϣ���һ����Ҫ����ȫ����(��һ��ȫ���к��չ���ƽ�ƿ��Եõ�һ�������վ�)
         * ����Ĭ��ȫ�����ǴӺ���ǰ������{1,2,3,4,5,6,7,8,9}��Ҫ����ȫ���У���{...,8,9}��{...,9,8}
         */
        int round = N % 72 == 0 ? N / 72 : N / 72 + 1 ;
        //��Ҫ������ĺ�λ����ȫ����
        int n=1,fac=1;
        for (int i = 1;i <= 9;i++) {
            fac *= i;
            if (fac >= round) {
                n = i;
                break;
            }
        }
        int first_line[9] = { 6,8,7,1,2,4,3,9,5 };//������һ�е�һ�����
        int change[8] = { 3,6,1,4,7,2,5,8 };//����һ�����ɣ�������һ�зֱ������ƶ�change����Եõ�һ�������վ�
        int row[8] = { 1,2,3,4,5,6,7,8 };//����ʹ���б�������ں����������վֵ�2~3��4~6��7~9��ȫ����
        int count = 0,row_count=1;
        Perm(first_line,9-n,9);
        //�õ��б������72�����У���һ�β�֪��զ���ˣ��ܱ�����Ҫ��Perm����ʵ�ֵĲ��ã�orz��
        genRowIndex();     
        while (round != 0) {
            //���������վ�
            std::copy(rst[round-1].begin(),rst[round-1].end(),first_line);
            for (int j = 0;j < 9;j++)
                this->board[0][j] = first_line[j];
            for (int i = 1;i < 9;i++) {
                for (int j = 0;j < 9;j++) {
                    this->board[i][j] = first_line[(change[i-1]+j)%9];
                }
            }
            //��������վ�
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
                if (count == N)
                    return;
                std::copy(row_index[row_count].begin(),row_index[row_count].end(),row);
                row_count++;
            }
            row_count = 0;
            round--;
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
        for (int i = 0;i < 20 * 81;i++) {
            if (i % 9 == 0)
                cout << endl;
            if (i % 81 == 0)
                cout << endl;
            cout << endGame[i] << " ";
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
    sudo.genRowIndex();
    sudo.produce_sudo(20);
    sudo.print_endGame();
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