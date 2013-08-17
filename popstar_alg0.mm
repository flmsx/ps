// testpopstar.cpp : Defines the entry point for the console application.

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imageOSX.h"
#include "shellOSX.h"
using namespace std;

typedef uint8_t BYTE;

typedef struct POINT_t {
	int x;
	int y;
} POINT;
#define BOARD_SIZE 12

#define MAX_STEP ((BOARD_SIZE-2) * (BOARD_SIZE-2) /2)

int g_nBranch = 2000; 

typedef struct MY_POINT_t
{
	BYTE x;
	BYTE y;
}MY_POINT;

static POINT g_NearPos[4] = {
	{-1, 0},
	{0, -1},
	{0, 1},	
	{1, 0},	
};

typedef struct STAR_BOARD_t
{
	BYTE board[BOARD_SIZE][BOARD_SIZE];

	BYTE boardChain[BOARD_SIZE][BOARD_SIZE];
	int nMaxChainNum;

	MY_POINT step[MAX_STEP];
	int nCurStepNum;

	int nValue;
	int nEvalValue;

}STAR_BOARD;

static bool BigThan(const STAR_BOARD &s1, const STAR_BOARD &s2)
{
	return s1.nEvalValue > s2.nEvalValue;
}

int  g_total=0;

void PrintBoard(const STAR_BOARD &board, BYTE step_x, BYTE step_y);
void PrintResult(const STAR_BOARD &board, const STAR_BOARD &result, int n);

void MakeBoardChain(STAR_BOARD &board);

void RandBoard(STAR_BOARD &board)
{
	srand(10000);
	//srand(GetTickCount());
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

void MakeBoardChain(STAR_BOARD &board)
{
	int i,j;
	int aloneStars = 0;	
	board.nMaxChainNum = 1;
	board.nEvalValue = board.nValue;
	//ZeroMemory(board.boardChain, sizeof(board.boardChain));
	memset(board.boardChain, 0, sizeof(board.boardChain));
	for (i = 1; i < BOARD_SIZE-1;i++)
	{
		for (j = 1; j < BOARD_SIZE-1; j++)
		{
			if (board.board[i][j] && 0 == board.boardChain[i][j])
			{
				if (board.board[i+1][j] == board.board[i][j]
					|| board.board[i][j+1] == board.board[i][j])
				{
					board.boardChain[i][j] = board.nMaxChainNum++;
					int nLen = 0;
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

bool AddStepBoards(const STAR_BOARD &board, vector<STAR_BOARD> &stepNewBoards)
{
	int i,j;

	if (1 == board.nMaxChainNum)	
	{
		return false;
	}
	int k;
	for(k = 1; k < board.nMaxChainNum; k++)	
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
		newBoard.nValue += 5 * chainLen * chainLen;

		for (j = BOARD_SIZE-2; j >=1; j--)
		{
			if (0 == newBoard.board[BOARD_SIZE-2][j])
			{
				//printf("LeftMoveColumn \n");
				LeftMoveColumn(newBoard, j);
			}
		}
		g_total++;
		int n;
		for(n = 0; n < stepNewBoards.size(); n++)
		{
			if (0 == memcmp(stepNewBoards[n].board, newBoard.board, sizeof(newBoard.board)))	
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
	return true;
}


bool MakeStepBoards(const vector<STAR_BOARD> &stepOldBoards, vector<STAR_BOARD> &stepNewBoards, vector<STAR_BOARD> &resultBoards)
{
	int i;
	stepNewBoards.clear();
	bool bEnd = true;
	for (i = 0; i < stepOldBoards.size() && i < g_nBranch; i++)
	{
		bool bAdd = AddStepBoards(stepOldBoards[i], stepNewBoards);
		if (!bAdd)
		{
			resultBoards.push_back(stepOldBoards[i]);
		}
		else
		{
			bEnd = false;
		}
	}
	return bEnd;
}

SHELL_MK_CMD(go)
{
    static int n = 0;
    
    STAR_BOARD board = ((STAR_BOARD*)shell->data)[0]; 
    STAR_BOARD result = ((STAR_BOARD*)shell->data)[1];
	
    if (n == result.nCurStepNum)
	{
		return;
	}
	STAR_BOARD newBoard = board;
	int chainLen=0; 
	int i,j,k;
	k = board.boardChain[result.step[n].x][result.step[n].y];
	
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

    PrintBoard(board, newBoard.step[newBoard.nCurStepNum-1].x, newBoard.step[newBoard.nCurStepNum-1].y);
    
    ((STAR_BOARD*)shell->data)[0] = newBoard;
    ++n;
}

shell_cmd_t shell_cmds_table[] = {
    SHELL_CMD(go, "g", "", "Move one step"),
};

int main(int argc, char* argv[])
{
	g_nBranch = atoi(argv[2]);
	vector<STAR_BOARD> stepTmpBoards1;
	vector<STAR_BOARD> stepTmpBoards2;
	vector<STAR_BOARD> resultBoards;
	STAR_BOARD board;
	memset(&board, 0, sizeof(board));
	image2Board(argv[1], (BYTE*)(board.board));
	MakeBoardChain(board);
	
	stepTmpBoards1.clear();
	stepTmpBoards2.clear();
	resultBoards.clear();
	stepTmpBoards1.push_back(board);
	bool bEnd = false;
	vector<STAR_BOARD> *p1 = &stepTmpBoards1;
	vector<STAR_BOARD> *p2 = &stepTmpBoards2;
	while(!bEnd)
	{
		bEnd = MakeStepBoards(*p1, *p2, resultBoards);
		//printf("newBoard size=%d , result board = %d, total=%d\n", p2->size(), resultBoards.size(),g_total);
		swap(p1, p2);
		sort(p1->begin(), p1->end(), BigThan);
	}
	sort(resultBoards.begin(), resultBoards.end(), BigThan);
	printf("result board = %ld, max score=%d \n", resultBoards.size(), resultBoards[0].nEvalValue);
	printf("g_total = %d\n", g_total);
	//PrintResult(board, resultBoards[0], 0);

    STAR_BOARD shell_data[] = {board, resultBoards[0]};
    shell_t *shell = shell_new((void*)shell_data);

    int i;
    for (i = 0; i < sizeof(shell_cmds_table)/sizeof(shell_cmd_t); i++)
        shell_register_cmd(shell_cmds_table[i], shell);
    int ret = shell_run(shell);                    
    shell_destroy(shell);
    return ret;
}


void PrintBoard(const STAR_BOARD &board, BYTE step_x, BYTE step_y)
{
	int i,j;
	for (i = 1; i < BOARD_SIZE-1;i++)
	{
		for (j = 1; j < BOARD_SIZE-1; j++)
		{
			//printf("%d  ", board.board[i][j]);
            if (step_x == i && step_y == j)
			    printStar(board.board[i][j], P_Bright|P_Reverse, '*');
			else
                printStar(board.board[i][j], P_Bright, '*');
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
	//PrintBoard(board);
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

    PrintBoard(board, newBoard.step[newBoard.nCurStepNum-1].x, newBoard.step[newBoard.nCurStepNum-1].y);
	
	PrintResult(newBoard, result, n+1);
}

