#include <gl\freeglut.h>
#include <iostream>
#include <random>
#include <time.h>
#include <vector>
#include <math.h>
#include <list>

#define PI 3.1415927

POINT Save[2];
using namespace std;

void drawScene(void);
void reshape(int, int);
void SetupRC();

void MoveUpperTimer(int);
void CreateUpperTimer(int);
void MoveDrag(int);
void Mouse(int, int, int, int);

void Motion(int, int);

POINT Window_Pos = { 100,100 };
POINT Window_Size = { 800,600 };

int Main_Window{};

bool ObjectDrag = false;

uniform_int_distribution<> uid(0, 360);
default_random_engine dre;

struct Point2D
{
	float x, y;
};

struct Grid
{
	Point2D Pos;
	bool input;
	bool left;
	bool right;
};

Point2D MouseStart{};
Point2D MouseMove{};

struct tRectangle
{
	Point2D emphasis;
	Point2D arrive;
	float Radius;
	float Angle[4];
	float fColor;
	int Number;
	bool Drag;
	float t;
};

struct Triangle
{
	Point2D emphasis;
	Point2D arrive;
	float Radius;
	float Angle[6];
	float fColor;
	bool Star;
	int Scale;
	float t;
	float fRandomColor[3];
};
// ============================================== 하단부 ==============================================
class Lower
{
public:
	Grid cGrid[3][12];
	const float size = 50.f;
	int countY = 0;
	int countX = 0;
public:
	void init()
	{
		float x = 0.f;
		float y = 150.f;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 6; ++j)
			{
				cGrid[i][j].Pos.x = x;
				cGrid[i][j].Pos.y = y;
				cGrid[i][j].input = false;
				cGrid[i][j].left = false;
				cGrid[i][j].right = false;
				x += size;
			}
			x = 0.f;
			y -= size;
		}
		x = 500.f;
		y = 150.f;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 6; j < 12; ++j)
			{
				cGrid[i][j].Pos.x = x;
				cGrid[i][j].Pos.y = y;
				cGrid[i][j].input = false;
				cGrid[i][j].left = false;
				cGrid[i][j].right = false;
				x += size;
			}
			x = 500.0;
			y -= size;
		}
	}
	void ShowGrid()
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 12; ++j)
			{
				glPushMatrix();
				glColor3f(0.5f, 0.5f, 0.5f);
				glBegin(GL_LINE_LOOP);
				glVertex2f(cGrid[i][j].Pos.x, cGrid[i][j].Pos.y);
				glVertex2f(cGrid[i][j].Pos.x, cGrid[i][j].Pos.y - size);
				glVertex2f(cGrid[i][j].Pos.x + size, cGrid[i][j].Pos.y - size);
				glVertex2f(cGrid[i][j].Pos.x + size, cGrid[i][j].Pos.y);
				glEnd();
				glPopMatrix();

				glPushMatrix();
				glColor3f(1.f, 1.f, 1.f);
				glBegin(GL_POLYGON);
				if (cGrid[i][j].left)
				{
					glVertex2f(cGrid[i][j].Pos.x, cGrid[i][j].Pos.y);
					glVertex2f(cGrid[i][j].Pos.x + size, cGrid[i][j].Pos.y - size);
					glVertex2f(cGrid[i][j].Pos.x, cGrid[i][j].Pos.y - size);
				}
				if (cGrid[i][j].right)
				{
					glVertex2f(cGrid[i][j].Pos.x, cGrid[i][j].Pos.y);
					glVertex2f(cGrid[i][j].Pos.x + size, cGrid[i][j].Pos.y);
					glVertex2f(cGrid[i][j].Pos.x + size, cGrid[i][j].Pos.y - size);
				}
				glEnd();
				glPopMatrix();
			}
		}
	}
	//void insertGrid(int x,int y,int val) // val 0 이면 left , val 1 이면 right
	//{
	//	if (cGrid[y][x].input == false)
	//		cGrid[y][x].input = true;
	//	if(0 == left)
	//	cGrid[y][x]
	//}
};
Lower cLower{};

//===============================================가운데===============================================
class Central
{
public:
	list<tRectangle> g_RectList;
	int slice_state;
	int select;
	float speed = 0.f;
	float acceleration = 0.15f;
	Point2D MouseMotion;
public:
	void init()
	{
		CreateRect(); // 처음 Rect를 만든다.
		slice_state = 2;
	}
	void CreateRect()
	{
		select = rand() % 2;
		slice_state = 2;

		tRectangle temp;

		temp.emphasis = { 400.f,-60.f };
		temp.fColor = 1.f;
		temp.Radius = 35.f;
		temp.arrive = {};
		temp.Number = 0;
		temp.Drag = false;
		temp.t = 0.f;
		if (0 == select)
		{
			temp.Angle[0] = 45.f;
			temp.Angle[1] = temp.Angle[0] + 90.f;
			temp.Angle[2] = temp.Angle[1] + 90.f;
			temp.Angle[3] = temp.Angle[2] + 90.f;
		}
		else if (1 == select)
		{
			temp.Angle[0] = 90.f;
			temp.Angle[1] = temp.Angle[0] + 90.f;
			temp.Angle[2] = temp.Angle[1] + 90.f;
			temp.Angle[3] = temp.Angle[2] + 90.f;
		}
		g_RectList.push_back(temp);
	}
	void CreateSlice()
	{
		list<tRectangle>::iterator iter = g_RectList.begin();
		list<tRectangle>::iterator iter_End = g_RectList.end();
		if (slice_state == 0)
		{
			while (true)
			{
				Save[0] = { rand() % 6 , rand() % 3 };
				Save[1] = { rand() % 6 + 6 ,rand() % 3 };
				if ((cLower.cGrid[Save[0].y][Save[0].x].left == false || cLower.cGrid[Save[0].y][Save[0].x].right == false) &&
					(cLower.cGrid[Save[1].y][Save[1].x].left == false || cLower.cGrid[Save[1].y][Save[1].x].right == false))
					break;
				else
					continue;
			}
			tRectangle Temp1;
			Temp1.Angle[0] = (*iter).Angle[0];
			Temp1.Angle[1] = (*iter).Angle[1];
			Temp1.Angle[2] = (*iter).Angle[2];
			Temp1.Angle[3] = NULL;
			Temp1.Number = 1;
			Temp1.Drag = false;
			Temp1.emphasis = { (*iter).emphasis.x - 2.f,(*iter).emphasis.y };
			Temp1.fColor = (*iter).fColor;
			Temp1.Radius = (*iter).Radius;
			Temp1.arrive.x = cLower.cGrid[Save[0].y][Save[0].x].Pos.x + 25;
			Temp1.arrive.y = cLower.cGrid[Save[0].y][Save[0].x].Pos.y - 25;
			Temp1.t = 0.f;
			g_RectList.push_back(Temp1);

			tRectangle Temp2;
			Temp2.Angle[0] = (*iter).Angle[0];
			Temp2.Angle[1] = (*iter).Angle[2];
			Temp2.Angle[2] = (*iter).Angle[3];
			Temp2.Angle[3] = NULL;
			Temp2.Number = 2;
			Temp2.arrive.x = cLower.cGrid[Save[1].y][Save[1].x].Pos.x + 25;
			Temp2.arrive.y = cLower.cGrid[Save[1].y][Save[1].x].Pos.y - 25;
			Temp2.Drag = false;
			Temp2.emphasis = { (*iter).emphasis.x + 2.f,(*iter).emphasis.y };
			Temp2.fColor = (*iter).fColor;
			Temp2.Radius = (*iter).Radius;
			Temp2.t = 0.f;
			g_RectList.push_back(Temp2);
			g_RectList.pop_front();
			speed = 10.f;
		}
		else if (slice_state == 1)
		{
			while (true)
			{
				Save[0] = { rand() % 6 , rand() % 3 };
				Save[1] = { rand() % 6 + 6 ,rand() % 3 };
				if ((cLower.cGrid[Save[0].y][Save[0].x].left == false || cLower.cGrid[Save[0].y][Save[0].x].right == false) &&
					(cLower.cGrid[Save[1].y][Save[1].x].left == false || cLower.cGrid[Save[1].y][Save[1].x].right == false))
					break;
				else
					continue;
			}
			tRectangle Temp1;
			Temp1.Angle[0] = (*iter).Angle[1];
			Temp1.Angle[1] = (*iter).Angle[2];
			Temp1.Angle[2] = (*iter).Angle[3];
			Temp1.Angle[3] = NULL;
			Temp1.Number = 1;
			Temp1.arrive.x = cLower.cGrid[Save[0].y][Save[0].x].Pos.x + 25;
			Temp1.arrive.y = cLower.cGrid[Save[0].y][Save[0].x].Pos.y - 25;
			Temp1.t = 0.f;
			Temp1.Drag = false;
			Temp1.emphasis = { (*iter).emphasis.x - 2.f,(*iter).emphasis.y };
			Temp1.fColor = (*iter).fColor;
			Temp1.Radius = (*iter).Radius;
			g_RectList.push_back(Temp1);

			tRectangle Temp2;
			Temp2.Angle[0] = (*iter).Angle[0];
			Temp2.Angle[1] = (*iter).Angle[1];
			Temp2.Angle[2] = (*iter).Angle[3];
			Temp2.Angle[3] = NULL;
			Temp2.Number = 2;
			Temp2.arrive.x = cLower.cGrid[Save[1].y][Save[1].x].Pos.x + 25;
			Temp2.arrive.y = cLower.cGrid[Save[1].y][Save[1].x].Pos.y - 25;
			Temp2.t = 0.f;
			Temp2.Drag = false;
			Temp2.emphasis = { (*iter).emphasis.x + 2.f,(*iter).emphasis.y };
			Temp2.fColor = (*iter).fColor;
			Temp2.Radius = (*iter).Radius;
			g_RectList.push_back(Temp2);
			g_RectList.pop_front();
			speed = 10.f;
		}
	}
	void ShowRect()
	{
		list<tRectangle>::iterator iter;
		list<tRectangle>::iterator iter_End = g_RectList.end();

		for (iter = g_RectList.begin(); iter != iter_End; ++iter)
		{
			if (slice_state == 0 || slice_state == 1)
			{
				glPushMatrix();
				glColor3f(1.f, 1.f, 1.f);
				glBegin(GL_POLYGON);
				glVertex2f((*iter).emphasis.x + (*iter).Radius*cosf(PI / 180.f * (*iter).Angle[0]), (*iter).emphasis.y + (*iter).Radius*sinf(PI / 180.f * (*iter).Angle[0]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius*cosf(PI / 180.f * (*iter).Angle[1]), (*iter).emphasis.y + (*iter).Radius*sinf(PI / 180.f * (*iter).Angle[1]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius*cosf(PI / 180.f * (*iter).Angle[2]), (*iter).emphasis.y + (*iter).Radius*sinf(PI / 180.f * (*iter).Angle[2]));
				glEnd();
				glPopMatrix();
			}
			else
			{
				glPushMatrix();
				glColor3f((*iter).fColor, (*iter).fColor, (*iter).fColor);
				glBegin(GL_POLYGON);
				glVertex2f((*iter).emphasis.x + (*iter).Radius*cos(PI / 180.f * (*iter).Angle[0]), (*iter).emphasis.y + (*iter).Radius*sin(PI / 180 * (*iter).Angle[0]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius*cos(PI / 180.f * (*iter).Angle[1]), (*iter).emphasis.y + (*iter).Radius*sin(PI / 180 * (*iter).Angle[1]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius*cos(PI / 180.f * (*iter).Angle[2]), (*iter).emphasis.y + (*iter).Radius*sin(PI / 180 * (*iter).Angle[2]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius*cos(PI / 180.f * (*iter).Angle[3]), (*iter).emphasis.y + (*iter).Radius*sin(PI / 180 * (*iter).Angle[3]));
				glEnd();
				glPopMatrix();
			}
		}
	}

	void MoveRect()
	{
		list<tRectangle>::iterator iter;
		list<tRectangle>::iterator iter_End = g_RectList.end();

		glPushMatrix();
		for (iter = g_RectList.begin(); iter != iter_End;)
		{
			if (slice_state == 0 || slice_state == 1)
			{
				if ((*iter).Drag == false)
				{
					if ((*iter).Number == 1)
					{
						(*iter).Angle[0] += 1.f;
						(*iter).Angle[1] += 1.f;
						(*iter).Angle[2] += 1.f;
						if ((*iter).t < 100.f)
						{
							float t = (*iter).t / 100.f;
							(*iter).emphasis.x = (2 * t*t - 3 * t + 1) * (*iter).emphasis.x + (-4 * t*t + 4 * t)*(((*iter).emphasis.x + (*iter).arrive.x) / 2) + (2 * t*t - t)*(*iter).arrive.x;
							(*iter).emphasis.y = (2 * t*t - 3 * t + 1) * (*iter).emphasis.y + (-4 * t*t + 4 * t)*(((*iter).emphasis.y + (*iter).arrive.y) / 2) + (2 * t*t - t)*(*iter).arrive.y;
							(*iter).t += 0.01f;
							if (((*iter).arrive.x - 1 < (*iter).emphasis.x && (*iter).emphasis.x < (*iter).arrive.x + 1) && ((*iter).arrive.y - 1 < (*iter).emphasis.y && (*iter).arrive.y + 1))
							{
								if (cLower.cGrid[Save[0].y][Save[0].x].left == false)
								{
										cLower.cGrid[Save[0].y][Save[0].x].left = true;
										iter = g_RectList.erase(iter);
										iter_End = g_RectList.end();
								}
								else
								{
									cLower.cGrid[Save[0].y][Save[0].x].right = true;
										iter = g_RectList.erase(iter);
										iter_End = g_RectList.end();
								}
							}
							else
							++iter;
						}
					}
					else if ((*iter).Number == 2)
					{
						(*iter).Angle[0] -= 1.f;
						(*iter).Angle[1] -= 1.f;
						(*iter).Angle[2] -= 1.f;
						
						if ((*iter).t < 100.f)
						{
							float t = (*iter).t / 100.f;
							(*iter).emphasis.x = (2 * t*t - 3 * t + 1) * (*iter).emphasis.x + (-4 * t*t + 4 * t)*(((*iter).emphasis.x + (*iter).arrive.x) / 2) + (2 * t*t - t)*(*iter).arrive.x;
							(*iter).emphasis.y = (2 * t*t - 3 * t + 1) * (*iter).emphasis.y + (-4 * t*t + 4 * t)*(((*iter).emphasis.y + (*iter).arrive.y) / 2) + (2 * t*t - t)*(*iter).arrive.y;
							(*iter).t += 0.01f;
							if (((*iter).arrive.x - 1 < (*iter).emphasis.x && (*iter).arrive.x + 1) && ((*iter).arrive.y - 1 < (*iter).emphasis.y && (*iter).arrive.y + 1))
							{
								if (cLower.cGrid[Save[1].y][Save[1].x].left == false)
								{
									cLower.cGrid[Save[1].y][Save[1].x].left = true;
									iter = g_RectList.erase(iter);
									iter_End = g_RectList.end();
								}
								else
								{
									cLower.cGrid[Save[1].y][Save[1].x].right = true;
									iter = g_RectList.erase(iter);
									iter_End = g_RectList.end();
								}
							}
							else
								++iter;
						}
					}
					speed -= acceleration;
					//(*iter).fColor -= 0.001f;
				}
				else if ((*iter).Drag == true)
				{
					//(*iter).emphasis.x = MouseStart.x;
					//(*iter).emphasis.y = (Window_Size.y - MouseMotion.y);
					++iter;
				}
			}
			else if (slice_state == 2)
			{
				(*iter).emphasis.y += 2.f;
				(*iter).fColor -= 0.0025f;

				if ((*iter).emphasis.y + (*iter).Radius > 600)
				{
					iter = g_RectList.erase(iter);
					iter_End = g_RectList.end();
					CreateRect();
				}
				else
					++iter;
			}
		}
		if (g_RectList.empty())
		{
			CreateRect();
		}
	}
public:
	int CrossTest(Point2D* MouseLine)
	{
		float m, n;
		float A, B1[4], C[4], D[4];
		float X[4], Y[4];
		float Radius = 10.f;
		if (MouseLine[0].x != MouseLine[1].x)
		{
			// m, n계산
			m = (MouseLine[1].y - MouseLine[0].y) / (MouseLine[1].x - MouseLine[0].x); // 기울기
			n = (MouseLine[0].y*MouseLine[1].x - MouseLine[0].x*MouseLine[1].y) / (MouseLine[1].x - MouseLine[0].x); //y 절편

			A = m*m + 1;

			list<tRectangle>::iterator iter;
			list<tRectangle>::iterator iter_End = g_RectList.end();
			for (iter = g_RectList.begin(); iter != iter_End; ++iter)
			{
				for (int i = 0; i < 4; ++i)
				{
					X[i] = (*iter).emphasis.x + (*iter).Radius*cosf(PI / 180 * (*iter).Angle[i]);
					Y[i] = (*iter).emphasis.y + (*iter).Radius*sinf(PI / 180 * (*iter).Angle[i]);
					B1[i] = (m*n - m*Y[i] - X[i]);
					C[i] = ((X[i] * X[i]) + (Y[i] * Y[i]) - (Radius*Radius) + (n*n) - 2 * (n*Y[i]));
					D[i] = B1[i] * B1[i] - A*C[i];
				}
			}

			if (D[0] >= 0 && D[2] >= 0)
				return 0;
			else if (D[1] >= 0 && D[3] >= 0)
				return 1;
			else
				return 2;

		}
		else
		{
			if (((X[0] - Radius) <= MouseLine[0].x && MouseLine[0].x <= (X[0] + Radius) &&
				(X[2] - Radius) <= MouseLine[0].x && MouseLine[0].x <= (X[2] + Radius)) && 
				((X[0] - Radius) <= MouseLine[1].x && MouseLine[1].x <= (X[0] + Radius) &&
				(X[2] - Radius) <= MouseLine[1].x && MouseLine[1].x <= (X[2] + Radius)))
				return 0; // ↘, ↓
			else if (((X[1] - Radius) <= MouseLine[0].x && MouseLine[0].x <= (X[1] + Radius) &&
				(X[3] - Radius) <= MouseLine[0].x && MouseLine[0].x <= (X[3] + Radius)) &&
				((X[0] - Radius) <= MouseLine[1].x && MouseLine[1].x <= (X[0] + Radius) &&
				(X[2] - Radius) <= MouseLine[1].x && MouseLine[1].x <= (X[2] + Radius)))
				return 1; // ↙, →
			else
				return 2; // 교차점이 없을때!!!
		}
	}
	bool CheckTriangle(Point2D p1, Point2D p2, Point2D p3, int mx, int my)
	{
		//p1, p2, p3의 세 점의 y값을 기준으로 순서 정렬 작은값 -> 큰값
		Point2D temp;
		if (p1.y > p2.y)
		{
			temp = p1;
			p1 = p2;
			p2 = temp;
		}
		if (p1.y > p3.y)
		{
			temp = p1;
			p1 = p3;
			p3 = temp;
		}
		if (p2.y > p3.y)
		{
			temp = p2;
			p2 = p3;
			p3 = temp;
		}
		//---------------------------------------------------------

		// 현재 마우스좌표의 y값이 p1.y와 p3.y의 밖에 있으면 FALSE 리턴
		if (my < p1.y || my > p3.y) return FALSE;

		// 기울기로 사용할 변수 m1, m2
		int m1, m2;

		// 마우스좌표의 y값에 대응되는 삼각형의 x좌표 2개
		int x1, x2, tmp;

		// 삼각형의 3점의 중간 y값이 마우스y값보다 작으면 
		// 마우스좌표는 p1.y와 p2.y값 사이에 있으므로
		// 마우스좌표x의 값은 점p1과 p2을 연결하는 선의 기울기를 구하여 
		// 마우스좌표y에 대응되는 x값을구한다(점p1과 점p3도 마찬가지)
		// 구해진 x값2개사이에 마우스좌표x가 있으면 삼각형안 이고 아니면 삼각형 밖이다.
		// 마우스좌표y가 p2.y와 p3.y사이에 있으면 점p3과 p2, 점p3과 p1의 기울기를 구하여 위의 방법으로 계산한다.
		if (my <= p2.y)
		{
			// 점p1과 점p2에 대한 기울기(고정소수점)
			m1 = ((int)(p2.x - p1.x) << 16) / (p2.y - p1.y);

			// 점p1과 점p3에 대한 기울기
			m2 = ((int)(p3.x - p1.x) << 16) / (p3.y - p1.y);

			// 기울기를 가지고 마우스y에대응되는 x값 구함
			x1 = p1.x + (((int)(my - p1.y) * m1) >> 16);
			x2 = p1.x + (((int)(my - p1.y) * m2) >> 16);

			// x1, x2스왑                
			if (x1 > x2)
			{
				tmp = x1;
				x1 = x2;
				x2 = tmp;
			}

			// x1과 x2사이에 있는지 비교
			if (mx < x1 || mx > x2) return false;
			return true;
		}
		else
		{
			m1 = ((int)(p2.x - p3.x) << 16) / (p2.y - p3.y);

			m2 = ((int)(p1.x - p3.x) << 16) / (p1.y - p3.y);

			x1 = p3.x + ((int)((my - p3.y) * m1) >> 16);
			x2 = p3.x + ((int)((my - p3.y) * m2) >> 16);

			if (x1 > x2)
			{
				tmp = x1;
				x1 = x2;
				x2 = tmp;
			}
			if (mx < x1 || mx > x2) return false;
			return true;
		}
	}
	void MoveDrag(float x, float y, int mx, int my)
	{
		x = mx;
		y = my;
	}
};

Central cCentral{};
// ===============================================상단부===============================================
class Upper
{
private:
	list<Triangle> g_TriList;

public:
	void MakeTri()
	{
		Triangle Temp{};
		Temp.emphasis.x = 0.f;
		Temp.emphasis.y = 550.f;
		Temp.Radius = 35.f;
		Temp.fColor = 1.f;
		Temp.Star = false;
		Temp.Scale = 0;
		Temp.arrive = {};
		Temp.Angle[0] = (double)uid(dre);
		Temp.Angle[1] = Temp.Angle[0] + 105.f; // + 105
		Temp.Angle[2] = Temp.Angle[1] + 90.f; // + 90
		g_TriList.push_back(Temp);
	}
	void ShowTri()
	{
		list<Triangle>::iterator iter;
		list<Triangle>::iterator iter_End = g_TriList.end();

		for (iter = g_TriList.begin(); iter != iter_End; ++iter)
		{
			if ((*iter).Star == false)
			{
				glPushMatrix();
				glColor3f((*iter).fColor, (*iter).fColor, (*iter).fColor);
				glBegin(GL_LINE_LOOP);
				glVertex2f((*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[0]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[0]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[1]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[1]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[2]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[2]));
				glEnd();
				glPopMatrix();
			}
			else
			{
				glPushMatrix();
				glColor3f((*iter).fRandomColor[0], (*iter).fRandomColor[1], (*iter).fRandomColor[2]);
				glBegin(GL_LINE_LOOP);
				glVertex2f((*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[0]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[0]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[1]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[1]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[2]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[2]));
				glEnd();
				glBegin(GL_LINE_LOOP);
				glVertex2f((*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[3]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[3]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[4]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[4]));
				glVertex2f((*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[5]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[5]));
				glEnd();
				glPopMatrix();
			}
		}
	}
	void UpdateUpper()
	{
		list<Triangle>::iterator iter;
		list<Triangle>::iterator iter_End = g_TriList.end();

		for (iter = g_TriList.begin(); iter != iter_End;)
		{
			if ((*iter).Star == false)
			{
				// 상단부 삼각형 이동, 회전시키기
				for (int i = 0; i < 3; ++i)
				{
					(*iter).Angle[i] -= 1.f;
				}
				(*iter).fColor -= 0.001f;
				(*iter).emphasis.x += 1.f;
				// 상단부 삼각형 지우기
				if ((*iter).emphasis.x - (*iter).Radius >= 800)
				{
					iter = g_TriList.erase(iter);
					iter_End = g_TriList.end();
				}
				else
					++iter;
			}
			else
			{
				for (int i = 0; i < 6; ++i)
				{
					(*iter).Angle[i] -= 1.f;
				}
				(*iter).fRandomColor[0] = 1.f / 255.f * (rand() % 255);
				(*iter).fRandomColor[1] = 1.f / 255.f * (rand() % 255);
				(*iter).fRandomColor[2] = 1.f / 255.f * (rand() % 255);

				if ((*iter).Scale == -1)
				{
					(*iter).Radius -= 1.f;

					if ((*iter).Radius < 20)
					{
						(*iter).Scale = 1;
					}
				}
				else if ((*iter).Scale == 1)
				{
					(*iter).Radius += 1.f;

					if ((*iter).Radius > 35)
					{
						(*iter).Scale = -1;
					}
				}

				if ((*iter).t < 100)
				{
					(*iter).emphasis.x = (1 - ((*iter).t / 100))*(*iter).emphasis.x + ((*iter).t / 100) *(*iter).arrive.x;
					(*iter).emphasis.y = (1 - ((*iter).t / 100))*(*iter).emphasis.y + ((*iter).t / 100) * (*iter).arrive.y;
					++(*iter).t;
				}
				++iter;
			}
		}
	}
	bool CheckTriangle(Point2D p1, Point2D p2, Point2D p3, int mx, int my)
	{
		//p1, p2, p3의 세 점의 y값을 기준으로 순서 정렬 작은값 -> 큰값
		Point2D temp;
		if (p1.y > p2.y)
		{
			temp = p1;
			p1 = p2;
			p2 = temp;
		}
		if (p1.y > p3.y)
		{
			temp = p1;
			p1 = p3;
			p3 = temp;
		}
		if (p2.y > p3.y)
		{
			temp = p2;
			p2 = p3;
			p3 = temp;
		}
		//---------------------------------------------------------

		// 현재 마우스좌표의 y값이 p1.y와 p3.y의 밖에 있으면 FALSE 리턴
		if (my < p1.y || my > p3.y) return FALSE;

		// 기울기로 사용할 변수 m1, m2
		int m1, m2;

		// 마우스좌표의 y값에 대응되는 삼각형의 x좌표 2개
		int x1, x2, tmp;

		// 삼각형의 3점의 중간 y값이 마우스y값보다 작으면 
		// 마우스좌표는 p1.y와 p2.y값 사이에 있으므로
		// 마우스좌표x의 값은 점p1과 p2을 연결하는 선의 기울기를 구하여 
		// 마우스좌표y에 대응되는 x값을구한다(점p1과 점p3도 마찬가지)
		// 구해진 x값2개사이에 마우스좌표x가 있으면 삼각형안 이고 아니면 삼각형 밖이다.
		// 마우스좌표y가 p2.y와 p3.y사이에 있으면 점p3과 p2, 점p3과 p1의 기울기를 구하여 위의 방법으로 계산한다.
		if (my <= p2.y)
		{
			// 점p1과 점p2에 대한 기울기(고정소수점)
			m1 = ((int)(p2.x - p1.x) << 16) / (p2.y - p1.y);

			// 점p1과 점p3에 대한 기울기
			m2 = ((int)(p3.x - p1.x) << 16) / (p3.y - p1.y);

			// 기울기를 가지고 마우스y에대응되는 x값 구함
			x1 = p1.x + (((int)(my - p1.y) * m1) >> 16);
			x2 = p1.x + (((int)(my - p1.y) * m2) >> 16);

			// x1, x2스왑                
			if (x1 > x2)
			{
				tmp = x1;
				x1 = x2;
				x2 = tmp;
			}

			// x1과 x2사이에 있는지 비교
			if (mx < x1 || mx > x2) return false;
			return true;
		}
		else
		{
			m1 = ((int)(p2.x - p3.x) << 16) / (p2.y - p3.y);

			m2 = ((int)(p1.x - p3.x) << 16) / (p1.y - p3.y);

			x1 = p3.x + ((int)((my - p3.y) * m1) >> 16);
			x2 = p3.x + ((int)((my - p3.y) * m2) >> 16);

			if (x1 > x2)
			{
				tmp = x1;
				x1 = x2;
				x2 = tmp;
			}
			if (mx < x1 || mx > x2) return false;
			return true;
		}
	}
	bool collision(float mx, float my)
	{
		list<Triangle>::iterator iter;
		list<Triangle>::iterator iter_End = g_TriList.end();
		Point2D P[3];

		for (iter = g_TriList.begin(); iter != iter_End; ++iter)
		{
			if ((*iter).Star == false)
			{
				for (int i = 0; i < 3; ++i)
				{
					P[i] = { (*iter).emphasis.x + (*iter).Radius * cosf(PI / 180.f * (*iter).Angle[i]), (*iter).emphasis.y + (*iter).Radius * sinf(PI / 180.f * (*iter).Angle[i]) };
				}

				if (CheckTriangle(P[0], P[1], P[2], (float)mx, (float)my))
				{
					(*iter).Angle[0] = 90.f;
					(*iter).Angle[1] = 210.f;
					(*iter).Angle[2] = 330.f;
					(*iter).Angle[3] = 30.f;
					(*iter).Angle[4] = 150.f;
					(*iter).Angle[5] = 270.f;
					(*iter).arrive = { (float)((rand() % 715) + 35),(float)((rand() % 200) + 150) };
					(*iter).Star = true;
					(*iter).Scale = -1;
					(*iter).t = 0.f;
					return true;
				}
			}
		}
	}
	void CollisionStar()
	{
		list<Triangle>::iterator iterTemp = g_TriList.begin();
		list<Triangle>::iterator iter1;
		list<Triangle>::iterator iter2;
		list<Triangle>::iterator iter_End = g_TriList.end();

		for (iter1 = g_TriList.begin(); iter1 != iter_End; ++iter1)
		{
			for (iter2 = g_TriList.begin(); iter2 != iter_End; ++iter2)
			{
				if (iter1 != iter2)
				{
					if ((*iter1).Star == true && (*iter2).Star == true && (*iter1).t == 100 && (*iter1).t == 100)
					{
						float deltaX = (*iter1).emphasis.x - (*iter2).emphasis.x;
						float deltaY = (*iter1).emphasis.y - (*iter2).emphasis.y;

						float length = sqrtf(deltaX * deltaX + deltaY * deltaY);

						if (length < ((*iter1).Radius + (*iter2).Radius))
						{
							if ((*iter1).emphasis.x <= (*iter2).emphasis.x)
							{
								(*iter1).arrive.x = (*iter1).emphasis.x - length / 2;//(*iter1).emphasis.x -= length;
								(*iter2).arrive.x = (*iter2).emphasis.x + length / 2;
							}
							else if ((*iter1).emphasis.x > (*iter2).emphasis.x)
							{
								(*iter1).arrive.x = (*iter1).emphasis.x + length / 2;
								(*iter2).arrive.x = (*iter2).emphasis.x - length / 2;
							}
							if ((*iter1).emphasis.y <= (*iter2).emphasis.y)
							{
								(*iter1).arrive.y = (*iter1).emphasis.y - length / 2;
								(*iter2).arrive.y = (*iter2).emphasis.y + length / 2;
							}
							else if ((*iter1).emphasis.y >(*iter2).emphasis.y)
							{
								(*iter1).arrive.y = (*iter1).emphasis.y + length;
								(*iter2).arrive.y = (*iter2).emphasis.y - length;
							}
							(*iter1).t = 0;
							(*iter2).t = 0;
						}
					}
				}
			}
			if ((*iter1).t == 100 && (*iter1).Star == true)
			{
				if (!(35 < (*iter1).emphasis.x && (*iter1).emphasis.x < 750) ||
					!(150 < (*iter1).emphasis.y && (*iter1).emphasis.y < 450))
				{
					(*iter1).arrive.x = (rand() % 715) + 35;
					(*iter1).arrive.y = (rand() % 200) + 150;
					(*iter1).t = 0.f;
				}
			}
		}
	}
};

Upper cUpper{};
Point2D MousePos[2]{};

bool Left_Button = false;
bool Right_Button = false;

void main(int argc, char** argv)
{
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // 디스플레이 모드 설정
	glutInitWindowPosition(Window_Pos.x, Window_Pos.y); // 윈도우 위치 설정 - 좌측 상단 모서리 위치 지정
	glutInitWindowSize(Window_Size.x, Window_Size.y); // 윈도우 위치 설정 - 위치 변환
	Main_Window = glutCreateWindow("That star is my star");// 윈도우 생성 ↔ glutDestroyWindow(int Win); - 윈도우 파괴

	SetupRC();

	glutReshapeFunc(reshape);
	glutDisplayFunc(drawScene);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(10, MoveUpperTimer, 1);
	glutTimerFunc(2000, CreateUpperTimer, 2);
	glutTimerFunc(10, MoveDrag, 3);

	glutMainLoop(); // GLUT 이벤트 프로세싱 루프 실행
}

void drawScene(void)
{
	glClearColor(0.f, 0.f, 0.f, 1.f); // 바탕색
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체 칠하기
	glMatrixMode(GL_MODELVIEW);

	glColor3f(1.f, 1.f, 1.f);
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex2f(0.0, 500.0);
	glVertex2f(800.0, 500.0);
	glEnd();
	glPopMatrix();

	cUpper.ShowTri();
	cLower.ShowGrid();
	cCentral.ShowRect();

	if (Left_Button)
	{
		glPushMatrix();
		glBegin(GL_LINES);
		glVertex2f(MousePos[0].x, MousePos[0].y);
		glVertex2f(MousePos[1].x, MousePos[1].y);
		glEnd();
		glPopMatrix();
	}

	glutSwapBuffers(); // 드로잉 명령을 실행하고 버퍼 교체를 설정
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glOrtho(0, w, 0, h, -400, 400);
}

void SetupRC()
{
	cLower.init();
	cCentral.init();
}

void MoveUpperTimer(int val)
{
	cUpper.UpdateUpper();
	cCentral.MoveRect();
	glutPostRedisplay();
	glutTimerFunc(10, MoveUpperTimer, 1);
}

void CreateUpperTimer(int val)
{
	cUpper.MakeTri();
	glutPostRedisplay();
	glutTimerFunc(2000, CreateUpperTimer, 2);
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		MousePos[0] = { (float)x, (float)(Window_Size.y - y) };
		MousePos[1] = { (float)x, (float)(Window_Size.y - y) };
		Left_Button = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		Left_Button = false;
		if (cCentral.CrossTest(MousePos) == 0)
		{
			cCentral.slice_state = 0;
			cCentral.CreateSlice();
		}
		else if (cCentral.CrossTest(MousePos) == 1)
		{
			cCentral.slice_state = 1;
			cCentral.CreateSlice();
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		if (cCentral.slice_state != 2)
		{
			Right_Button = true;
			MouseStart = { (float)x, (float)(Window_Size.y - y) };
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		ObjectDrag = false;
		if (cCentral.slice_state != 2)
		{
			Right_Button = false;
			list<tRectangle>::iterator iter;
			list<tRectangle>::iterator iter_End = cCentral.g_RectList.end();

			for (iter = cCentral.g_RectList.begin(); iter != iter_End; ++iter)
			{
				if ((*iter).Drag)
				{
					(*iter).Drag = false;
				}
			}
		}
	}
}
void Motion(int x, int y)
{
	if (Left_Button)
	{
		MousePos[1] = { (float)x , (float)(Window_Size.y - y) };
	}
	else if (Right_Button)
	{
		MouseMove = { (float)x , (float)(Window_Size.y - y) };
		list<tRectangle>::iterator iter;
		list<tRectangle>::iterator iter_End = cCentral.g_RectList.end();

		for (iter = cCentral.g_RectList.begin(); iter != iter_End;++iter)
		{
			Point2D p[3];
			for (int i = 0; i < 3; ++i)
			{
				p[i] = { (*iter).emphasis.x + (*iter).Radius*cosf(PI / 180 * (*iter).Angle[i]) ,(*iter).emphasis.y + (*iter).Radius*sinf(PI / 180 * (*iter).Angle[i]) };
			}

			if (cCentral.CheckTriangle(p[0], p[1], p[2], x, (Window_Size.y - y)))
			{
				if (ObjectDrag == false)
				{
					(*iter).Drag = true;
					ObjectDrag = true;
				}
			}
		}
	}
}

void MoveDrag(int value)
{
	list<tRectangle>::iterator iter;
	list<tRectangle>::iterator iter_End = cCentral.g_RectList.end();
	for (iter = cCentral.g_RectList.begin(); iter != iter_End;)
	{
		if ((*iter).Drag)
		{
			(*iter).emphasis.x = MouseMove.x;
			(*iter).emphasis.y = MouseMove.y;
			if (cUpper.collision((*iter).emphasis.x, (*iter).emphasis.y))
			{
				iter = cCentral.g_RectList.erase(iter);
				iter_End = cCentral.g_RectList.end();
			}
			else
				++iter;
		}
		else
			++iter;
	}
	cUpper.CollisionStar();
	glutPostRedisplay();
	glutTimerFunc(10, MoveDrag, 3);
}