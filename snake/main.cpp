#include "main.h"

const uint64_t width = 119;
const uint64_t height = 29;

void SpawnFruit()
{
    uint64_t x_loc = rand() % (width - 1) + 1;
    uint64_t y_loc = rand() % (height - 1) + 1;

    if (x_loc == 0 || x_loc == width - 1 || y_loc == 0 || y_loc == height - 1) 
    {
        SpawnFruit();
    } 

    FruitPos = pos_t{x_loc, y_loc};
}

class Snake
{
public:
    Snake(bool _second)
    {
        k_second = _second;
        if (!_second)
        {
            k_bones.push_back(pos_t{ 80 , height / 2 });
        }
        else
        {
            k_bones.push_back(pos_t{ width / 3, height / 2 });
        }
    }

    bool Update(Snake*& _secondSnake)
    {
        for (size_t i = k_bones.size() - 1; i > 0; --i)
        {
            k_bones[i] = k_bones[i - 1];
        }

        switch (k_lastMovementAction)
        {
        case MOVE_UP:
            k_bones[0].y -= 1;
            break;

        case MOVE_DOWN:
            k_bones[0].y += 1;
            break;

        case MOVE_RIGHT:
            k_bones[0].x += 1;
            break;

        case MOVE_LEFT:
            k_bones[0].x -= 1;
            break;

        default:
            break;
        }

        for (int i = 1; i < k_bones.size(); i++)
        {
            if (k_bones[0].x == k_bones[i].x && k_bones[0].y == k_bones[i].y)
            {
                strcpy(Winner, k_second ? "Player1" : "Player2");
                return false;
            }
        }

        for (int i = 0; i < _secondSnake->k_bones.size(); i++)
        {
            if (k_bones[0].x == _secondSnake->k_bones[i].x && k_bones[0].y == _secondSnake->k_bones[i].y)
            {
                strcpy(Winner, k_second ? "Player1" : "Player2");
                return false;
            }
        }

        if (k_bones[0].x == FruitPos.x && k_bones[0].y == FruitPos.y)
        {
            k_bones.push_back(k_bones.back());
            std::thread([]() {
                Beep(1000, 200);
                Beep(1200, 200);
            }).detach();

            SpawnFruit();
        }

        return true;
    }

    eSnakeMovement k_lastMovementAction = MOVE_DEFAULT;
    std::vector<pos_t> k_bones;

private:
    bool k_second = false;
};


void Render(Snake*& _snake1, Snake*& _snake2)
{
    HANDLE m_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD m_pos = { 0, 0 };
    SetConsoleCursorPosition(m_handle, m_pos);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1) 
            {
                printf("#"); //Border
            } 
            else if (x == FruitPos.x && y == FruitPos.y)
            {
                printf("*"); //Fruit
            }
            else 
            {
                bool m_IsSnake = false;

                for (int i = 0; i <= _snake1->k_bones.size(); i++)
                {
                    if (x == _snake1->k_bones[i].x && y == _snake1->k_bones[i].y)
                    {
                        printf("O"); //Snake
                        m_IsSnake = true;
                        break;
                    }
                }
                for (int i = 0; i <= _snake2->k_bones.size(); i++)
                {
                    if (x == _snake2->k_bones[i].x && y == _snake2->k_bones[i].y)
                    {   
                        if (!m_IsSnake)
                        {
                            printf("@"); //Snake
                            m_IsSnake = true;
                            break;
                        }
                    }
                }
                if (!m_IsSnake)
                {
                    printf(" "); //Nothink
                }
            }
        }
        printf("\n");
    }

    printf("Player1`s Score: %zu     Player2`s Score: %zu    ", _snake1->k_bones.size() - 1, _snake2->k_bones.size() - 1);
}

void UpdateInput(Snake*& _snake1, Snake*& _snake2)
{
    if (_kbhit()) {
        int m_key = _getch();
        if (m_key == 0xE0)
        {
            m_key = _getch();

            switch (m_key)
            {
            case 0x48:
                _snake1->k_lastMovementAction = MOVE_UP;
                break;

            case 0x4B:
                _snake1->k_lastMovementAction = MOVE_LEFT;
                break;

            case 0x4D:
                _snake1->k_lastMovementAction = MOVE_RIGHT;
                break;

            case 0x50:
                _snake1->k_lastMovementAction = MOVE_DOWN;
                break;
            }
        }

        else {
            switch (m_key)
            {
            case 'w':
            case 'W':
                _snake2->k_lastMovementAction = MOVE_UP;
                break;

            case 'a':
            case 'A':
                _snake2->k_lastMovementAction = MOVE_LEFT;
                break;

            case 'd':
            case 'D':
                _snake2->k_lastMovementAction = MOVE_RIGHT;
                break;

            case 's':
            case 'S':
                _snake2->k_lastMovementAction = MOVE_DOWN;
                break;
            }
        }
    }        
}

int main(int argc, char* argv[]) 
{
    srand(static_cast<unsigned int>(time(NULL)));
    printf("\033[2J\033[1;1H");

    SetWindowTextW(GetConsoleWindow(), L"Snake");

    CONSOLE_CURSOR_INFO m_cursorInfo;

    try {
        Snake* m_snake1 = new Snake(false);
        Snake* m_snake2 = new Snake(true);

        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &m_cursorInfo);
        m_cursorInfo.bVisible = FALSE;

        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &m_cursorInfo);

        SpawnFruit();

        while (true) 
        {
            UpdateInput(m_snake1, m_snake2);

            if (!m_snake1->Update(m_snake2) || !m_snake2->Update(m_snake1))
            {
                break;
            }

            std::thread(Beep, 400, 100).detach();

            Render(m_snake1, m_snake2);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }

        printf("                %s Won! Press Enter to continue.", Winner);
        std::thread([]() { 
            Beep(800, 300);
            Beep(600, 300);
            Beep(400, 300);
            Beep(200, 500);
        }).detach();    

        std::cin.ignore();

        delete m_snake1;
        delete m_snake2;
        m_snake1 = nullptr;
        m_snake2 = nullptr;
    }
    catch (const std::exception& ex) {
        printf("Exception thrown: %s", ex.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}