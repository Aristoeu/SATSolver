#define M 8

//sudoku
int index(int i, int j)
{
    return M * i + j + 1;
}
int additionalIndex(int rc, int i, int j)
{
    return M * M + (3 * M + 1) * ((rc - 1) * M * (M - 1) / 2 + M * (i - 1) - i * (i - 1) / 2 + j - i - 1) + 1;
}
int additionalIndex(int rc, int i, int j, int k)
{
    return additionalIndex(rc, i, j) + 3 * (k - 1) + 1;
}
int additionalIndex(int rc, int i, int j, int k, int l)
{
    return additionalIndex(rc, i, j, k) + l + 1;
}
/* test the accuracy of the index functions above
    for(int rc = 1;rc<=2;rc++)
    {
        for (int i = 1; i <= M; i++)
        {
            for(int j = i+1;j<=M;j++)
            {
                printf("%d ",additionalIndex(rc,i,j));
                for(int k = 1;k<=M;k++)
                {
                    printf("%d ",additionalIndex(rc,i,j,k));
                    for(int l = 0;l<=1;l++)
                    {
                        printf("%d ",additionalIndex(rc,i,j,k,l));
                    }
                }
            }
        }
    }
*/

void SudokuToSAT(int board[M][M], FILE *out, int count)
{
    //count为给出的变元个数
    fprintf(out, "p cnf %d %d\n", 1464, 6576 + count);
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (board[i][j] == 0)
            {
                fprintf(out, "%d 0\n", -index(i, j));
            }
            else
            {
                if (board[i][j] == 1)
                {
                    fprintf(out, "%d 0\n", index(i, j));
                }
            }
        }
    }
    //no three continious ‘1’ in a row 96
    for (int i = 0; i < M; i++) //row
    {
        for (int j = 0; j + 2 < M; j++) //colomn
        {
            fprintf(out, "%d %d %d 0\n", index(i, j), index(i, j + 1), index(i, j + 2));
            fprintf(out, "%d %d %d 0\n", -index(i, j), -index(i, j + 1), -index(i, j + 2));
        }
    }
    //no three continious ‘1’ in a column 96
    for (int i = 0; i < M; i++) //column
    {
        for (int j = 0; j + 2 < M; j++) //row
        {
            fprintf(out, "%d %d %d 0\n", index(j, i), index(j + 1, i), index(j + 2, i));
            fprintf(out, "%d %d %d 0\n", -index(j, i), -index(j + 1, i), -index(j + 2, i));
        }
    }
    //count of '1' and '0' is same in a row 896
    for (int i = 0; i < M; i++) //row
    {
        for (int j1 = 0; j1 < M - 4; j1++)
        {
            for (int j2 = j1 + 1; j2 < M - 3; j2++)
            {
                for (int j3 = j2 + 1; j3 < M - 2; j3++)
                {
                    for (int j4 = j3 + 1; j4 < M - 1; j4++)
                    {
                        for (int j5 = j4 + 1; j5 < M; j5++)
                        {
                            fprintf(out, "%d %d %d %d %d 0\n", index(i, j1), index(i, j2), index(i, j3), index(i, j4), index(i, j5));
                            fprintf(out, "%d %d %d %d %d 0\n", -index(i, j1), -index(i, j2), -index(i, j3), -index(i, j4), -index(i, j5));
                        }
                    }
                }
            }
        }
    }
    //count of '1' and '0' is same in a column 896
    for (int i = 0; i < M; i++) //column
    {
        for (int j1 = 0; j1 < M - 4; j1++)
        {
            for (int j2 = j1 + 1; j2 < M - 3; j2++)
            {
                for (int j3 = j2 + 1; j3 < M - 2; j3++)
                {
                    for (int j4 = j3 + 1; j4 < M - 1; j4++)
                    {
                        for (int j5 = j4 + 1; j5 < M; j5++)
                        {
                            fprintf(out, "%d %d %d %d %d 0\n", index(j1, i), index(j2, i), index(j3, i), index(j4, i), index(j5, i));
                            fprintf(out, "%d %d %d %d %d 0\n", -index(j1, i), -index(j2, i), -index(j3, i), -index(j4, i), -index(j5, i));
                        }
                    }
                }
            }
        }
    }
    //no euqal rows or columns 4536
    for (int rc = 1; rc <= 2; rc++)
    {
        for (int i = 1; i <= M; i++)
        {
            for (int j = i + 1; j <= M; j++)
            {
                fprintf(out, "%d ", -additionalIndex(rc, i, j));
                for (int k = 1; k <= M; k++)
                {
                    fprintf(out, "%d ", -additionalIndex(rc, i, j, k));
                }
                fprintf(out, "0\n");
            }
        }
    }
    //for (int rc = 1; rc <= 2; rc++)
    //{
        int rc=1;
        for (int i = 1; i <= M; i++)
        {
            for (int j = i + 1; j <= M; j++)
            {
                int a = additionalIndex(rc, i, j);
                fprintf(out,"%d 0\n",a);
                for (int k = 1; k <= M; k++)
                {
                    int b = additionalIndex(rc, i, j, k);
                    int c = additionalIndex(rc, i, j, k, 1);
                    int d = additionalIndex(rc, i, j, k, 0);
                    fprintf(out, "%d %d 0\n", a, b);
                    fprintf(out, "%d %d 0\n", b, -c);
                    fprintf(out, "%d %d 0\n", b, -d);
                    fprintf(out, "%d %d %d 0\n", -b, c, d);
                    fprintf(out, "%d %d 0\n", index(i - 1, k - 1), -c);
                    fprintf(out, "%d %d 0\n", index(j - 1, k - 1), -c);
                    fprintf(out, "%d %d %d 0\n", -index(i - 1, k - 1), -index(j - 1, k - 1), c);
                    fprintf(out, "%d %d 0\n", -index(i - 1, k - 1), -d);
                    fprintf(out, "%d %d 0\n", -index(j - 1, k - 1), -d);
                    fprintf(out, "%d %d %d 0\n", index(i - 1, k - 1), index(j - 1, k - 1), d);
                }
            }
        }
    //}
        rc=2;
        for (int i = 1; i <= M; i++)
        {
            for (int j = i + 1; j <= M; j++)
            {
                int a = additionalIndex(rc, i, j);
                fprintf(out,"%d 0\n",a);
                for (int k = 1; k <= M; k++)
                {
                    int b = additionalIndex(rc, i, j, k);
                    int c = additionalIndex(rc, i, j, k, 1);
                    int d = additionalIndex(rc, i, j, k, 0);
                    fprintf(out, "%d %d 0\n", a, b);
                    fprintf(out, "%d %d 0\n", b, -c);
                    fprintf(out, "%d %d 0\n", b, -d);
                    fprintf(out, "%d %d %d 0\n", -b, c, d);
                    fprintf(out, "%d %d 0\n", index(k - 1, i - 1), -c);
                    fprintf(out, "%d %d 0\n", index(k - 1, j - 1), -c);
                    fprintf(out, "%d %d %d 0\n", -index(k - 1, i - 1), -index(k - 1, j - 1), c);
                    fprintf(out, "%d %d 0\n", -index(k - 1, i - 1), -d);
                    fprintf(out, "%d %d 0\n", -index(k - 1, j - 1), -d);
                    fprintf(out, "%d %d %d 0\n", index(k - 1, i - 1), index(k - 1, j - 1), d);
                }
            }
        }

}

//生成完整数独棋盘的思路：随机找几个点（其中每加一个点判断符不符合约束条件）->求解出整个棋盘

int isSatisfied(int x, int y, int board[M][M], int a)
{ //return if putting number 'a' in board[x][y] satisifies the sudoku rule
    //note that you have made board[x][y]=a before entering this function

    //judge if there are three sequential '1's in a column
    if (x - 2 >= 0)
    {
        if (board[x - 2][y] == board[x - 1][y] && board[x - 1][y] == a)
            return 0;
    }
    if (x - 1 >= 0 && x + 1 < M)
    {
        if (board[x - 1][y] == board[x + 1][y] && board[x - 1][y] == a)
            return 0;
    }
    if (x + 2 < M)
    {
        if (board[x + 2][y] == board[x + 1][y] && board[x + 1][y] == a)
            return 0;
    }
    //judge if there are three sequential '1's in a row
    if (y - 2 >= 0)
    {
        if (board[x][y - 2] == board[x][y - 1] && board[x][y - 1] == a)
            return 0;
    }
    if (y - 1 >= 0 && y + 1 < M)
    {
        if (board[x][y - 1] == board[x][y + 1] && board[x][y - 1] == a)
            return 0;
    }
    if (y + 2 < M)
    {
        if (board[x][y + 2] == board[x][y + 1] && board[x][y + 1] == a)
            return 0;
    }
    //ensure every row has the same number of '0' and '1'
    int cnt0 = 0, cnt1 = 0;
    for (int i = 0; i < M; i++)
    {
        if (board[x][i] == 0)
            cnt0++;
        else if (board[x][i] == 1)
            cnt1++;
    }
    if (cnt0 > 4 || cnt1 > 4)
        return 0;
    if (cnt0 + cnt1 == M && cnt0 != cnt1)
        return 0;
    //ensure every column has the same number of '0' and '1'
    cnt0 = 0;
    cnt1 = 0;
    for (int i = 0; i < M; i++)
    {
        if (board[i][y] == 0)
            cnt0++;
        else if (board[i][y] == 1)
            cnt1++;
    }
    if (cnt0 > 4 || cnt1 > 4)
        return 0;
    if (cnt0 + cnt1 == M && cnt0 != cnt1)
        return 0;
    //judge if there are two same rows
    for (int i = 0; i < M; i++)
    {
        int flag = 1;
        if (i == x)
            continue;
        for (int j = 0; j < M; j++)
        {
            if (board[x][j] == board[i][j])
                continue;
            else
            {
                flag = 0;
                break;
            }
        }
        if (flag == 1)
            return 0;
    }
    //judge if there are two same columns
    for (int i = 0; i < M; i++)
    {
        int flag = 1;
        if (i == y)
            continue;
        for (int j = 0; j < M; j++)
        {
            if (board[j][y] == board[j][i])
                continue;
            else
            {
                flag = 0;
                break;
            }
        }
        if (flag == 1)
            return 0;
    }
    return 1;
}

void generateBoard(int board[M][M], int num/*, int show[M][M]*/)
{
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            board[i][j] = -1;
            //show[i][j] = -1;
        }
    }
    time_t seed;
    srand(time(&seed));
    int i = 0;
    while (i < num)
    {
        int m = rand() % M, n = rand() % M, b = rand() % 2;
        if (board[m][n] != -1)
            continue;
        int tmp = board[m][n];
        board[m][n] = b;
        if (isSatisfied(m, n, board, b) == 1)
        {
            i++;
            //show[m][n] = b;
        }
        else
        {
            board[m][n] = tmp; //恢复
        }
    }
}
