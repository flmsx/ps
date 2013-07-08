// testpopstar.cpp : Defines the entry point for the console application.
/********************************************************

模块名：testpopstar
作者：	liaozhenlan
日期：	2013年1月2日

功能：  popstar穷举算法
	程序运行结果为从初始局面到终局的走法。

注释：	有问题可到百度贴吧讨论
	http://tieba.baidu.com/p/2050726958

********************************************************/

#include "stdafx.h"
#include <windows.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
using namespace std;

//实际棋盘大小为BOARD_SIZE-2
//BOARD_SIZE相当加了棋盘边缘，方便计算
#define BOARD_SIZE 12

//最长步数
#define MAX_STEP ((BOARD_SIZE-2) * (BOARD_SIZE-2) /2)

//棋子颜色
enum star_color{RED = 1, YELLOW, BLUE, GREEN, PURPLE, END_COLOR=PURPLE};

//剪枝
int g_nBranch = 100; 

//不用POINT是为了节省空间
typedef struct tagMY_POINT
{
	BYTE x;
	BYTE y;
}MY_POINT;

static POINT g_NearPos[4] = {
	{-1, 0},	//左
	{0, -1},	//上
	{0, 1},		//下
	{1, 0},		//右
};

typedef struct tagSTAR_BOARD
{
	//以棋子颜色填充。0表示棋子
	BYTE board[BOARD_SIZE][BOARD_SIZE];

	//相连的棋子称为链，即可消除的棋子
	//每个链有不同的编号
	BYTE boardChain[BOARD_SIZE][BOARD_SIZE];
	//当前最大编号
	int nMaxChainNum;

	//记录已走的棋步
	MY_POINT step[MAX_STEP];
	//已走的步数
	int nCurStepNum;

	//当前实际得分
	int nValue;
	//评估分
	int nEvalValue;

}STAR_BOARD;

class BigThan
{
public:
	bool operator()(STAR_BOARD &s1, STAR_BOARD &s2)
	{
		return s1.nEvalValue > s2.nEvalValue;
	}
};

//测试用。记录复杂度
int  g_total=0;

//输出棋盘状况
void PrintBoard(const STAR_BOARD &board);
//输出走法
void PrintResult(const STAR_BOARD &board, const STAR_BOARD &result, int n);

void MakeBoardChain(STAR_BOARD &board);

//随机初始化棋盘
void RandBoard(STAR_BOARD &board)
{
	srand(GetTickCount());
	int i,j;
	for (i = 1; i < BOARD_SIZE-1;i++)
	{
		for (j = 1; j < BOARD_SIZE-1; j++)
		{
			board.board[i][j] = (rand()%END_COLOR) + 1;
		}
	}
	MakeBoardChain(board);
}


//递归生成棋子(i,j)对应的链
//nlen为链的长度
void MakeChain( STAR_BOARD &board, int i, int j, int &nLen)
{
	nLen++;
	for (int k = 0; k < 4; k++)
	{
		const POINT &pt = g_NearPos[k];
		if(board.board[i][j] == board.board[i+pt.x][j+pt.y]
			&& 0 == board.boardChain[i+pt.x][j+pt.y])
		{
			board.boardChain[i+pt.x][j+pt.y] = board.boardChain[i][j];
			MakeChain(board, i+pt.x, j+pt.y, nLen);
		}
	}
}

//当消除整列时，该列右边的棋子需要左移
void LeftMoveColumn(STAR_BOARD &board, int nCol)
{
	int i,j;
	for (i = 1; i < BOARD_SIZE-1; i++)
	{
		for (j = nCol; j < BOARD_SIZE-1; j++)
		{
			board.board[i][j] = board.board[i][j+1];
		}
	}
}

//生成整个棋盘的链，并计算评估分
void MakeBoardChain(STAR_BOARD &board)
{
	int i,j;
	int aloneStars = 0;	//零散的棋子个数
	board.nMaxChainNum = 1;
	board.nEvalValue = board.nValue;
	ZeroMemory(board.boardChain, sizeof(board.boardChain));
	for (i = 1; i < BOARD_SIZE-1;i++)
	{
		for (j = 1; j < BOARD_SIZE-1; j++)
		{
			if (board.board[i][j] && 0 == board.boardChain[i][j])
			{
				if (board.board[i+1][j] == board.board[i][j]
					|| board.board[i][j+1] == board.board[i][j])
				{
					//有新的链
					board.boardChain[i][j] = board.nMaxChainNum++;
					int nLen = 0;	//链的长度
					MakeChain(board, i, j, nLen);
					board.nEvalValue += 5 * nLen * nLen;
				}
				else
				{
					aloneStars++;
				}
			}
		}
	}
	if (aloneStars < 10)
	{
		board.nEvalValue += 2000-20*aloneStars*aloneStars;
	}
}

//从当前局面消除一个链后生成新的局面
BOOL AddStepBoards(const STAR_BOARD &board, vector<STAR_BOARD> &stepNewBoards)
{
	int i,j;

	if (1 == board.nMaxChainNum)	//没有可消除的棋子
	{
		return FALSE;
	}
	int k;
	for(k = 1; k < board.nMaxChainNum; k++)	//消除编号为k的链，生成新的局面
	{
		STAR_BOARD newBoard = board;
		int chainLen=0; ;
		for (i = 1; i < BOARD_SIZE-1; i++)
		{
			for (j = 1; j < BOARD_SIZE-1; j++)
			{
				if (board.boardChain[i][j] == k)
				{
					if (chainLen==0)
					{
						newBoard.step[newBoard.nCurStepNum].x = i;
						newBoard.step[newBoard.nCurStepNum].y = j;
						newBoard.nCurStepNum++;						
					}
					chainLen++;
					int pos;
					for (pos = i; pos > 0; pos--)
					{
						newBoard.board[pos][j] = newBoard.board[pos-1][j];
					}
					//PrintBoard(newBoard);
				}
			}
		}
		//消除的得分
		newBoard.nValue += 5 * chainLen * chainLen;

		for (j = BOARD_SIZE-2; j >=1; j--)
		{
			if (0 == newBoard.board[BOARD_SIZE-2][j])	//空列
			{
				//printf("LeftMoveColumn \n");
				LeftMoveColumn(newBoard, j);
			}
		}
		g_total++;
		int n;
		for(n = 0; n < stepNewBoards.size(); n++)
		{
			if (0 == memcmp(stepNewBoards[n].board, newBoard.board, sizeof(newBoard.board)))	//已记录相同局面
			{
				break;
			}
		}

		if (n == stepNewBoards.size())
		{	
			MakeBoardChain(newBoard);
			//PrintBoard(newBoard);
			stepNewBoards.push_back(newBoard);
		}
	}
	return TRUE;
}


BOOL MakeStepBoards(const vector<STAR_BOARD> &stepOldBoards, vector<STAR_BOARD> &stepNewBoards, vector<STAR_BOARD> &resultBoards)
{
	int i;
	stepNewBoards.clear();
	BOOL bEnd = TRUE;
	for (i = 0; i < stepOldBoards.size() && i < g_nBranch; i++)
	{
		BOOL bAdd = AddStepBoards(stepOldBoards[i], stepNewBoards);
		if (!bAdd)
		{
			resultBoards.push_back(stepOldBoards[i]);
		}
		else
		{
			bEnd = FALSE;
		}
	}
	return bEnd;
}



int main(int argc, char* argv[])
{
	vector<STAR_BOARD> stepTmpBoards1;
	vector<STAR_BOARD> stepTmpBoards2;
	vector<STAR_BOARD> resultBoards;
	STAR_BOARD board;
	int totalscore = 0;
	for (int n = 0; n < 1; n++)
	{
		ZeroMemory(&board, sizeof(board));
		RandBoard(board);
	//PrintBoard(board);

		stepTmpBoards1.clear();
		stepTmpBoards2.clear();
		resultBoards.clear();
		stepTmpBoards1.push_back(board);
		BOOL bEnd = FALSE;
		vector<STAR_BOARD> *p1 = &stepTmpBoards1;
		vector<STAR_BOARD> *p2 = &stepTmpBoards2;
		while(!bEnd)
		{
			bEnd = MakeStepBoards(*p1, *p2, resultBoards);
			//printf("newBoard size=%d , result board = %d, total=%d\n", p2->size(), resultBoards.size(),g_total);
			swap(p1, p2);
			sort(p1->begin(), p1->end(), BigThan());
		}
		sort(resultBoards.begin(), resultBoards.end(), BigThan());
		printf("result board = %d, max score=%d \n", resultBoards.size(), resultBoards[0].nEvalValue);
		//PrintBoard(resultBoards[0]);
		totalscore += resultBoards[0].nEvalValue;
	}
	printf("average score = %d\n\n", totalscore/n);
	PrintResult(board, resultBoards[0], 0);
	getchar();

	return 0;
}


void PrintBoard(const STAR_BOARD &board)
{
	int i,j;
	for (i = 1; i < BOARD_SIZE-1;i++)
	{
		for (j = 1; j < BOARD_SIZE-1; j++)
		{
			printf("%d  ", board.board[i][j]);
			
		}
		printf("\n");
	}
	for(int k = 0; k < board.nCurStepNum; k++)
	{
		printf("(%d, %d) ", board.step[k].x, board.step[k].y);
	}
	printf("EvalScore = %d, score=%d ", board.nEvalValue, board.nValue);
	printf("\n");
#if 0
	printf("board chain \n");
	for (i = 1; i < BOARD_SIZE-1;i++)
	{
		for (j = 1; j < BOARD_SIZE-1; j++)
		{
			for (j = 1; j < BOARD_SIZE-1; j++)
			{
				printf("%02d  ", board.boardChain[i][j]);
			}
			printf("\n");
		}
	}
	printf("\n");
#endif
}

void PrintResult(const STAR_BOARD &board, const STAR_BOARD &result, int n)
{
	PrintBoard(board);
	if (n == result.nCurStepNum)
	{
		return;
	}
	STAR_BOARD newBoard = board;
	int chainLen=0; 
	int i,j,k;
	k = board.boardChain[result.step[n].x][result.step[n].y];
	//for test 
	
	for (i = 1; i < BOARD_SIZE-1; i++)
	{
		for (j = 1; j < BOARD_SIZE-1; j++)
		{
			if (board.boardChain[i][j] == k)
			{
				if (chainLen==0)
				{
					newBoard.step[newBoard.nCurStepNum].x = i;
					newBoard.step[newBoard.nCurStepNum].y = j;
					newBoard.nCurStepNum++;						
				}
				chainLen++;
				int pos;
				for (pos = i; pos > 0; pos--)
				{
					newBoard.board[pos][j] = newBoard.board[pos-1][j];
				}
				//PrintBoard(newBoard);
			}
		}
	}
	newBoard.nValue += 5 * chainLen * chainLen;
	for (j = BOARD_SIZE-2; j >=1; j--)
	{
		if (0 == newBoard.board[BOARD_SIZE-2][j])
		{
			//printf("LeftMoveColumn \n");
			LeftMoveColumn(newBoard, j);
		}
	}	
	
	MakeBoardChain(newBoard);
	
	PrintResult(newBoard, result, n+1);
}