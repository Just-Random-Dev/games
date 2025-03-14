#include "main.h"

Paddle* p_FirstPaddle = nullptr;
Paddle* p_SecondPaddle = nullptr;

std::unique_ptr<Ball> p_Ball = nullptr;

void Render()
{
	HANDLE m_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(m_handle, { 0, 0 });

    for (int y = 0; y < p_Height; y++) {
        for (int x = 0; x < p_Width; x++) {
            if (x == 0 || x == p_Width - 1 || y == 0 || y == p_Height - 1)
            {
                printf("#"); //Border
            }
			else {
				bool m_empty = true;

				if (x == 4)
				{
					for (int i = 0; i < p_FirstPaddle->k_Pixels.size(); i++)
					{
						if (x == (int)p_FirstPaddle->k_Pixels[i].x && y == (int)p_FirstPaddle->k_Pixels[i].y)
						{
							printf("|");
							m_empty = false;
							break;
						}
					}
				}
				else if (x == p_Width - 5)
				{
					for (int i = 0; i < p_SecondPaddle->k_Pixels.size(); i++)
					{
						if (x == (int)p_SecondPaddle->k_Pixels[i].x && y == (int)p_SecondPaddle->k_Pixels[i].y)
						{
							printf("|");
							m_empty = false;
							break;
						}
					}
				}

				if (m_empty)
				{
					if (x == (int)p_Ball->k_pos.x && y == (int)p_Ball->k_pos.y)
					{
						printf("O");
						m_empty = false;
					}
				}

				if (m_empty)
				{
					printf(" ");
				}
			}
        }
        printf("\n");
    }

	printf("Player1`s Score: %d										Player2`s Score: %d", p_FirstPaddle->k_Score, p_SecondPaddle->k_Score);
}

int main(int argc, char* argv[])
{
	srand(static_cast<unsigned int>(time(NULL)));
	SetWindowText(GetConsoleWindow(), L"Pong");

	CONSOLE_CURSOR_INFO m_cursorInfo;

	try {
		std::ios_base::sync_with_stdio(false);

		p_Ball = std::make_unique<Ball>();
		p_FirstPaddle = new Paddle(0);
		p_SecondPaddle = new Paddle(1);

		GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &m_cursorInfo);
		m_cursorInfo.bVisible = FALSE;

		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &m_cursorInfo);

		while (true)
		{
			p_FirstPaddle->Update();
			p_SecondPaddle->Update();
			
			if (!p_Ball->Update())
			{
				p_Ball = std::make_unique<Ball>();
				p_FirstPaddle->Reset();
				p_SecondPaddle->Reset();

				if (p_FirstPaddle->k_Score == 10 || p_SecondPaddle->k_Score == 10)
				{
					std::thread([]() {
						Beep(800, 300);
						Beep(600, 300);
						Beep(400, 300);
						Beep(200, 500);
					}).detach();

					char m_winner[50];
					if (p_FirstPaddle->k_Score == 10) 
						strcpy(m_winner, "Player1 has won!");

					if (p_SecondPaddle->k_Score == 10) 
						strcpy(m_winner, "Player2 has won!");
					
					MessageBoxA(nullptr, m_winner, "Game Over", MB_OK | MB_ICONINFORMATION);
					break;
				}
			}

			Render();
		}

		delete p_FirstPaddle;
		delete p_SecondPaddle;

		p_FirstPaddle = nullptr;
		p_SecondPaddle = nullptr;
	}
	catch (std::exception& ex) {
		printf("Exception thrown: %s", ex.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void Paddle::Update()
{
	if (this->k_PaddleId == 0)
	{
		if (GetKeyState('W') & 0x8000)
		{
			this->k_LastAction = PADDLE_UP;
		}

		if (GetKeyState('S') & 0x8000)
		{
			this->k_LastAction = PADDLE_DOWN;
		}
	}
	else if (this->k_PaddleId == 1)
	{
		if (GetKeyState(VK_UP) & 0x8000)
		{
			this->k_LastAction = PADDLE_UP;
		}

		if (GetKeyState(VK_DOWN) & 0x8000)
		{
			this->k_LastAction = PADDLE_DOWN;
		}
	}

	if (this->k_LastAction == PADDLE_UP && (int)this->k_Pixels[0].y != 1)
	{
		for (int i = 0; i < PaddleLenght; i++)
		{
			k_Pixels[i].y -= 1;
		}
	}

	if (this->k_LastAction == PADDLE_DOWN && (int)this->k_Pixels[PaddleLenght - 1].y != p_Height - 2)
	{
		for (int i = 0; i < PaddleLenght; i++)
		{
			k_Pixels[i].y += 1;
		}
	}
}

void Paddle::Reset()
{
	this->k_LastAction = PADDLE_DEFAULT;
	this->k_Pixels.clear();

	for (int i = 0; i < PaddleLenght; i++)
	{
		if (this->k_PaddleId == 0)
		{
			this->k_Pixels.push_back({ 4, ((p_Height / 2) - PaddleLenght / 2) + i });
		}
		else
		{
			this->k_Pixels.push_back({ p_Width - 5, ((p_Height / 2) - PaddleLenght / 2) + i });
		}
	}
}

bool Ball::Update()
{
	if (((p_FirstPaddle->k_LastAction != PADDLE_DEFAULT) || (p_SecondPaddle->k_LastAction != PADDLE_DEFAULT)) && !k_moving)
	{
		this->k_angle = p_directions[rand() % 4];
		k_moving = true;
	}

	if (k_moving)
	{
		if ((int)this->k_pos.x == 1 || (int)this->k_pos.x == 0)
		{
			p_SecondPaddle->k_Score += 1;
			Beep(1000, 200);
			return false;
		}
		else if ((int)this->k_pos.x == p_Width - 2 || (int)this->k_pos.x == p_Width - 1)
		{
			p_FirstPaddle->k_Score += 1;
			Beep(1000, 200);
			return false;
		}

		if ((int)this->k_pos.y == 1 || (int)this->k_pos.y == 0 || (int)this->k_pos.y == p_Height - 1 || (int)this->k_pos.y == p_Height - 2)
		{
			if (k_angle == 225)
			{
				k_angle = 135;
			}
			else if (k_angle == 135)
			{
				k_angle = 225;
			}
			else if (k_angle == 45)
			{
				k_angle = 315;
			}
			else if (k_angle == 315)
			{
				k_angle = 45;
			}
		}

		if ((int)this->k_pos.x == 5 || (int)this->k_pos.x == 4)
		{
			for (int i = 0; i < PaddleLenght; i++)
			{
				if ((int)this->k_pos.y == (int)p_FirstPaddle->k_Pixels[i].y)
				{
					std::thread([]() {
						Beep(1000, 200);
						Beep(1200, 200);
					}).detach();

					if (k_angle == 225)
					{
						k_angle = 315;
					}
					else if (k_angle == 135)
					{
						k_angle = 45;
					}
					if (this->k_speed <= 0.31) this->k_speed += 0.1;
					break;
				}
			}
		}

		if ((int)this->k_pos.x == p_Width - 6 || (int)this->k_pos.x == p_Width - 5)
		{
			for (int i = 0; i < PaddleLenght; i++)
			{
				if ((int)this->k_pos.y == (int)p_SecondPaddle->k_Pixels[i].y)
				{
					std::thread([]() {
						Beep(1000, 200);
						Beep(1200, 200);
					}).detach();

					if (k_angle == 315)
					{
						k_angle = 225;
					}
					else if (k_angle == 45)
					{
						k_angle = 135;
					}
					if (this->k_speed <= 0.31) this->k_speed += 0.1;
					break;
				}
			}
		}

		Move();
	}

	return true;
}

void Ball::Move()
{
	switch (k_angle)
	{
	case 45:
		this->k_pos.x += 1.0 + k_speed;
		this->k_pos.y -= 1.0 + k_speed;
		break;

	case 135:
		this->k_pos.x -= 1.0 + k_speed;
		this->k_pos.y -= 1.0 + k_speed;
		break;

	case 225:
		this->k_pos.x -= 1.0 + k_speed;
		this->k_pos.y += 1.0 + k_speed;
		break;

	case 315:
		this->k_pos.x += 1.0 + k_speed;
		this->k_pos.y += 1.0 + k_speed;
		break;

	default:
		break;
	}

	return;
}
