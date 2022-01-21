#include "d2d1.h"
#include "Game.h"
#include <chrono>

using namespace D2D1;

extern ID2D1Factory * DFactory;
extern ID2D1HwndRenderTarget * DRenderTarget;
extern ID2D1SolidColorBrush * DBrush;
extern RECT RectWnd;



maze::maze(int nN_)
{
	nN = nN_;
	aWall = new bool[2 * nN * nN]();
}

maze::~maze()
{
	if (aWall != NULL)delete[] aWall;
}

game::game(int nN_)
{
	nN = nN_;
	pMaze = new maze(nN);
	pMazeFact = new maze(nN / PathWidth);
	aBoard = new int[nN * nN]();
	aSnakePos = new int[nN * nN]();
	aSnakePos[0] = rand() % (nN * nN);
	nLSnake = 1;
	aBoard[aSnakePos[0]] = 2;
	GenMatt();

	BoardWidth = 400.f;
	CellWidth = BoardWidth / float(nN);
	BoardWidth += 2. * CellWidth;

	CamPosX = float(aSnakePos[0] % nN) + 0.5;
	CamPosY = float(aSnakePos[0] / nN) + 0.5;
}

game::~game()
{
	if (pMaze != NULL)delete pMaze;
	if (pMazeFact != NULL) delete pMazeFact;
	if (aBoard != NULL) delete[] aBoard;
	if (aSnakePos != NULL) delete[] aSnakePos;
}

void game::DirecVec(float & VecX, float & VecY, int Direc)
{
	if (Direc == -1){
		VecX = -1.;VecY = 0.;return;}
	if (Direc == +1){
		VecX = +1.;VecY = 0.;return;}
	if (Direc == -nN){
		VecX = 0.;VecY = -1.;return;}
	if (Direc == +nN){
		VecX = 0.;VecY = +1.;return;}
}

void game::Paint()
{
	auto TimeStart = std::chrono::high_resolution_clock::now();
	float LineWidth = 4.f;
	float CenterX = (RectWnd.left + RectWnd.right) / 2;
	float CenterY = (RectWnd.top + RectWnd.bottom) / 2;
	
	float SnakeCoreWidth = LineWidth;

	int PosX = floor(CamPosX);
	int PosY = floor(CamPosY);
	CenterX += ((float)PosX - CamPosX) * CellWidth;
	CenterY += ((float)PosY - CamPosY) * CellWidth;
	PosX -= (nN / 2 + 1);
	PosY -= (nN / 2 + 1);
	int PosXStart = PosX;
	int PosYStart = PosY;

	DRenderTarget->BeginDraw();
	DRenderTarget->Clear(ColorF(0.f, 0.f, 0.f, 1.f));

	PosX = PosXStart;
	for (int i_X = 0; i_X < nN + 2; i_X++)
	{
		while (PosX < 0)
			PosX += nN;
		while (PosX > nN - 1)
			PosX -= nN;
		float DrawCellLeft = CenterX - BoardWidth / 2. + CellWidth * i_X;
		float DrawCellRight = DrawCellLeft + CellWidth;
		float DrawCellCenterX = (DrawCellLeft + DrawCellRight) / 2.;

		PosY = PosYStart;
		for (int i_Y = 0; i_Y < nN + 2; i_Y++)
		{
			while (PosY < 0)
				PosY += nN;
			while (PosY > nN - 1)
				PosY -= nN;
			float DrawCellTop = CenterY - BoardWidth / 2. + CellWidth * i_Y;
			float DrawCellBottom = DrawCellTop + CellWidth;
			float DrawCellCenterY = (DrawCellTop + DrawCellBottom) / 2.;
			{
				DBrush->SetColor(ColorF(0.1f, 0.05f, 0.5f, 1.f));

				int IdSnake = aBoard[PosY * nN + PosX] - 2;
				if (IdSnake >= 0)
				{
					DRenderTarget->FillRectangle(RectF(DrawCellLeft, DrawCellTop, DrawCellRight, DrawCellBottom), DBrush);

					DBrush->SetColor(ColorF(0.52f, 0.5f, 1.0f));

					if (nLSnake == 1)
						DRenderTarget->FillRectangle(RectF(DrawCellCenterX - SnakeCoreWidth / 2., DrawCellCenterY - SnakeCoreWidth / 2., DrawCellCenterX + SnakeCoreWidth / 2., DrawCellCenterY + SnakeCoreWidth / 2.), DBrush);
					if (nLSnake > 1)
					{
						if (IdSnake > 0)
						{
							float VecX = 0., VecY = 0.;
							DirecVec(VecX, VecY, pMaze->GetDirec(aSnakePos[IdSnake], aSnakePos[IdSnake - 1]));
							DRenderTarget->DrawLine(Point2F(DrawCellCenterX, DrawCellCenterY),
								Point2F(DrawCellCenterX + VecX * CellWidth / 2., DrawCellCenterY + VecY * CellWidth / 2.), DBrush, SnakeCoreWidth);
						}
						if (IdSnake < nLSnake - 1)
						{
							float VecX = 0., VecY = 0.;
							DirecVec(VecX, VecY, pMaze->GetDirec(aSnakePos[IdSnake], aSnakePos[IdSnake + 1]));
							DRenderTarget->DrawLine(Point2F(DrawCellCenterX, DrawCellCenterY),
								Point2F(DrawCellCenterX + VecX * CellWidth / 2., DrawCellCenterY + VecY * CellWidth / 2.), DBrush, SnakeCoreWidth);
						}
					}
				}

				DBrush->SetColor(ColorF(0.06f, 0.03f, 0.3f));

				if (aBoard[PosY * nN + PosX] == 1)
					DRenderTarget->FillRectangle(RectF(DrawCellLeft, DrawCellTop, DrawCellRight, DrawCellBottom), DBrush);
			}
			PosY++;
		}
		PosX++;
	}

	PosX = PosXStart;
	for (int i_X = 0; i_X < nN + 2; i_X++)
	{
		while (PosX < 0)
			PosX += nN;
		while (PosX > nN - 1)
			PosX -= nN;
		float DrawCellLeft = CenterX - BoardWidth / 2. + CellWidth * i_X;
		float DrawCellRight = DrawCellLeft + CellWidth;
		float DrawCellCenterX = (DrawCellLeft + DrawCellRight) / 2.;

		PosY = PosYStart;
		for (int i_Y = 0; i_Y < nN + 2; i_Y++)
		{
			while (PosY < 0)
				PosY += nN;
			while (PosY > nN - 1)
				PosY -= nN;
			float DrawCellTop = CenterY - BoardWidth / 2. + CellWidth * i_Y;
			float DrawCellBottom = DrawCellTop + CellWidth;
			float DrawCellCenterY = (DrawCellTop + DrawCellBottom) / 2.;
			{
				DBrush->SetColor(ColorF(1.f, 1.f, 1.f, 1.f));

				if (pMaze->aWall[2 * (PosY * nN + PosX) + 1])
				{
					DRenderTarget->DrawLine(Point2F(DrawCellLeft, DrawCellBottom), Point2F(DrawCellRight, DrawCellBottom), DBrush, LineWidth);
				}
				if (pMaze->aWall[2 * (PosY * nN + PosX)])
				{
					DRenderTarget->DrawLine(Point2F(DrawCellRight, DrawCellTop), Point2F(DrawCellRight, DrawCellBottom), DBrush, LineWidth);
				}
				DRenderTarget->FillRectangle(RectF(DrawCellRight - LineWidth / 2., DrawCellBottom - LineWidth / 2., DrawCellRight + LineWidth / 2., DrawCellBottom + LineWidth / 2.), DBrush);
			}
			PosY++;
		}
		PosX++;
	}

	DBrush->SetColor(ColorF(0.f, 0.f, 0.f));
	DRenderTarget->FillRectangle(RectF(RectWnd.left, RectWnd.top, RectWnd.right, (RectWnd.top + RectWnd.bottom) / 2. - 201.f), DBrush);
	DRenderTarget->FillRectangle(RectF(RectWnd.left, (RectWnd.top + RectWnd.bottom) / 2. + 201.f, RectWnd.right, RectWnd.bottom), DBrush);
	DRenderTarget->FillRectangle(RectF(RectWnd.left, RectWnd.top, (RectWnd.left + RectWnd.right) / 2. - 201.f, RectWnd.bottom), DBrush);
	DRenderTarget->FillRectangle(RectF((RectWnd.left + RectWnd.right) / 2. + 201.f, RectWnd.top, RectWnd.right, RectWnd.bottom), DBrush);

	DRenderTarget->EndDraw();
	auto TimeNow = std::chrono::high_resolution_clock::now();
	double Time = std::chrono::duration_cast<std::chrono::microseconds>(TimeNow - TimeStart).count();
	Time;
}

int maze::WallId(int Pos, int Direc)
{
	int PosV = Pos / nN;
	int WallId = Pos;
	if (Direc < 0)
		WallId += Direc;
	if (Pos % nN == 0 && Direc == -1)
		WallId += nN;
	if (Pos / nN == 0 && Direc == -nN)
		WallId += nN * nN;
	WallId *= 2;
	if (Direc == +nN || Direc == -nN)
		WallId += 1;
	return WallId;
}

int maze::PosNext(int Pos, int Direc, bool * fThroughBorder = NULL)
{
	bool fThroughBorder_ = false;
	if (Direc == -1 && Pos % nN == 0)
	{
		Pos += nN;
		fThroughBorder_ = true;
	}
	if (Direc == +1 && Pos % nN == nN - 1)
	{
		Pos -= nN;
		fThroughBorder_ = true;
	}
	if (Direc == -nN && Pos / nN == 0)
	{
		Pos += nN * nN;
		fThroughBorder_ = true;
	}
	if (Direc == +nN && Pos / nN == nN - 1)
	{
		Pos -= nN * nN;
		fThroughBorder_ = true;
	}
	if (fThroughBorder != NULL)
		*fThroughBorder = fThroughBorder_;
	return Pos + Direc;
}

int maze::GetDirec(int Pos, int Pos1)
{
	int Diff = Pos1 - Pos;
	if (Diff == -1 || Diff == -1 + nN)
		return -1;
	if (Diff == +1 || Diff == +1 - nN)
		return +1;
	if (Diff == -nN || Diff == -nN + nN * nN)
		return -nN;
	if (Diff == +nN || Diff == +nN - nN * nN)
		return +nN;
}

void maze::ExtendPath(int Pos, int PrevDirec)
{
	static const int aDirecPool[4] = { -1, +1, -nN, +nN };
	for (int i_Direc = 0; i_Direc < 4; i_Direc++)
	{
		if (i_Direc == PrevDirec)
			continue;
		aWall[WallId(Pos, aDirecPool[i_Direc])] = true;
	}
	if (PrevDirec == 0)
		return;
	aWall[WallId(Pos, PrevDirec)] = false;
}

void maze::Gen()
{
	std::chrono::steady_clock::time_point Time0;
	double TimeStart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Time0).count();
	
	static const int aDirecPool[4] = { -1, +1, -nN, +nN };
	int Pos = rand() % (nN * nN);//(rand() % nN) * nN + (rand() % nN);
	for (int i_Direc = 0; i_Direc < 4; i_Direc++)
	{
		aWall[WallId(Pos, aDirecPool[i_Direc])] = true;
	}
	auto aExtendPool = new int[nN * nN]();//to be extended
	aExtendPool[0] = Pos;
	int nLExtendPool = 1;
	auto afPosOccu = new bool[nN * nN]();
	afPosOccu[Pos] = true;

	while (nLExtendPool > 0)
	{
		Pos = aExtendPool[rand() % nLExtendPool];
		int i_DirecS = rand() % 4;
		int PosExtend = 0;
		int i_Direc = i_DirecS;
		do
		{
			PosExtend = PosNext(Pos, aDirecPool[i_Direc]);
			if (!afPosOccu[PosExtend])
			{
				ExtendPath(PosExtend, -aDirecPool[i_Direc]);
				aExtendPool[nLExtendPool] = PosExtend;
				nLExtendPool++;
				afPosOccu[PosExtend] = true;
				break;
			}
			i_Direc = (i_Direc + 1) % 4;
		} while (i_Direc != i_DirecS);
		int PosNearExtend[5] = {};
		for (int i_Direc = 0; i_Direc < 4; i_Direc++)
			PosNearExtend[i_Direc] = PosNext(PosExtend, aDirecPool[i_Direc]);
		PosNearExtend[4] = PosExtend;
		int aIdClosed[5] = {};
		int nLIdClosed = 0;
		
		for (int i_ExtendPool = 0; i_ExtendPool < nLExtendPool; i_ExtendPool++)//这里是从前向后遍历
		{
			for (int i_Near = 0; i_Near < 5; i_Near++)
			{
				if (aExtendPool[i_ExtendPool] == PosNearExtend[i_Near])
				{
					bool fClosed = true;
					for (int i_Direc = 0; i_Direc < 4; i_Direc++)
					{
						if (afPosOccu[PosNext(aExtendPool[i_ExtendPool], aDirecPool[i_Direc])] == false)
						{
							fClosed = false;
							break;
						}
					}
					if (fClosed)
					{
						aIdClosed[nLIdClosed] = i_ExtendPool;
						nLIdClosed++;
						break;
					}
				}
			}
		}
		//因为aIdClosed一定是从小到大排列的，这里从后往前遍历就不会出问题
		for (int i_IdClosed = nLIdClosed - 1; i_IdClosed >= 0; i_IdClosed--)
		{
			aExtendPool[aIdClosed[i_IdClosed]] = aExtendPool[nLExtendPool - 1];
			nLExtendPool--;
		}
	}
	
	delete[] aExtendPool;
	delete[] afPosOccu;
}

/*
void CutSnake(const int nN, bool * a4WallH, bool * a3WallV, int & nSnakeLeng, int * aSnakePos, int * a4Board)
{
	int i_L = 0;
	for (; i_L < nSnakeLeng - 1; i_L++)
	{
		if (aSnakePos[i_L + 1] - aSnakePos[i_L] == 1 || aSnakePos[i_L + 1] - aSnakePos[i_L] == -1)
		{
			if (a3WallV[WallId(nN, aSnakePos[i_L], aSnakePos[i_L + 1] - aSnakePos[i_L])])
				break;
		}
		else
		{
			if (a4WallH[WallId(nN, aSnakePos[i_L], aSnakePos[i_L + 1] - aSnakePos[i_L])])
				break;
		}
	}

	for (int i_L1 = i_L + 1; i_L1 < nSnakeLeng; i_L1++)
	{
		a4Board[aSnakePos[i_L1]] = 1;
	}
	nSnakeLeng = i_L + 1;
}*/

void game::GenMaze()
{
	pMazeFact->Gen();
	for (int PosX = 0; PosX < nN; PosX++)
	{
		for (int PosY = 0; PosY < nN; PosY++)
		{
			int PosFactX = PosX / PathWidth;
			int PosFactY = PosY / PathWidth;
			if (PosX % PathWidth == PathWidth - 1)
			{
				pMaze->aWall[2 * (PosY * nN + PosX)] = pMazeFact->aWall[2 * (PosFactY * nN / PathWidth + PosFactX)];
			}
			if (PosY % PathWidth == PathWidth - 1)
			{
				pMaze->aWall[2 * (PosY * nN + PosX) + 1] = pMazeFact->aWall[2 * (PosFactY * nN / PathWidth + PosFactX) + 1];
			}
		}
	}
}

bool game::SnakeMove(int Direc)
{
	int IdWall = pMaze->WallId(aSnakePos[0], Direc);
	if (pMaze->aWall[IdWall])
		return false;
	bool fThroughBorder = false;
	int SnakePosNext = pMaze->PosNext(aSnakePos[0], Direc, &fThroughBorder);
	if (aBoard[SnakePosNext] >= 2)
		return false;
	if (aBoard[SnakePosNext] == 1)
	{
		nLSnake++;
		GenMatt();
		aBoard[SnakePosNext] = 0;
	}
	else
	{
		aBoard[aSnakePos[nLSnake - 1]] = 0;
	}
	for (int i_L = nLSnake - 1; i_L > 0; i_L--)
	{
		aSnakePos[i_L] = aSnakePos[i_L - 1];
		aBoard[aSnakePos[i_L]] = i_L + 2;
	}
	aSnakePos[0] = SnakePosNext;
	aBoard[SnakePosNext] = 2;
	if (fThroughBorder)
	{
		if (Direc == -1)
			CamPosX += float(nN);
		if (Direc == +1)
			CamPosX -= float(nN);
		if (Direc == -nN)
			CamPosY += float(nN);
		if (Direc == +nN)
			CamPosY -= float(nN);
	}
	return true;
}

bool game::GenMatt()
{
	int PosInit = rand() % (nN * nN);
	int Pos = PosInit;
	do
	{
		if (aBoard[Pos] == 0)
		{
			aBoard[Pos] = 1;
			return true;
		}
		Pos = (Pos + 1) % (nN * nN);
	} while (Pos != PosInit);
	return false;
}

void game::CamFollowSnake()
{
	float CamCenterX = float(aSnakePos[0] % nN) + 0.5;
	float CamCenterY = float(aSnakePos[0] / nN) + 0.5;
	float Speed = 0.03;
	CamPosX += Speed * (CamCenterX - CamPosX);
	CamPosY += Speed * (CamCenterY - CamPosY);
}