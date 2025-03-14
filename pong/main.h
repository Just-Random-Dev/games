#include <vector>
#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <memory>
#include <thread>
#include <cmath>
#include <cstdlib>

const int PaddleLenght = 6;

const std::vector<int> p_directions = { 45, 135, 225, 315 };

const double p_Width = 119;
const double p_Height = 29;

typedef struct
{
	double x;
	double y;
} pos_t;

enum eLastPaddleAction
{
	PADDLE_UP,
	PADDLE_DOWN,
	PADDLE_DEFAULT
};

class Ball
{
public:
	Ball()
	{
		k_pos = {p_Width / 2, p_Height / 2};
	}

	bool Update();

	pos_t k_pos;

private:
	bool k_moving = false;
	double k_speed = 0.0;
	int k_angle = 0;

	void Move();
};

class Paddle
{
public: 
	Paddle(int id)
	{
		k_PaddleId = id;

		for (int i = 0; i < PaddleLenght; i++)
		{
			if (id == 0)
			{
				k_Pixels.push_back({ 4, ((p_Height / 2) - PaddleLenght /2 ) + i });
			}
			else 
			{
				k_Pixels.push_back({ p_Width - 5, ((p_Height / 2) - PaddleLenght / 2) + i });
			}
		}
	}
	~Paddle() {}

	void Update();
	void Reset();

	int k_Score = 0;
	eLastPaddleAction k_LastAction = PADDLE_DEFAULT;
	std::vector<pos_t> k_Pixels;

private:
	int k_PaddleId = 0;
};