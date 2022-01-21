#pragma once
class maze
{
public:
	int nN;
	bool * aWall = NULL;//0号格子右侧是0号墙、下侧是1号墙，1号格子右侧是2号墙、下侧是3号墙，……

	maze(int);
	~maze();
	int WallId(int, int);
	int PosNext(int, int, bool *);
	void ExtendPath(int, int);
	void Gen();
	int GetDirec(int, int);
};

class game
{
public:
	int nN;
	maze * pMaze;
	maze * pMazeFact;
	int * aBoard;
	int * aSnakePos;
	int nLSnake;
	int PathWidth = 2;
	float CamPosX;
	float CamPosY;
	float BoardWidth;
	float CellWidth;

	game(int);
	~game();
	void Paint();
	bool SnakeMove(int);
	bool GenMatt();
	void GenMaze();
	void DirecVec(float &, float &, int);
	void CamFollowSnake();
};