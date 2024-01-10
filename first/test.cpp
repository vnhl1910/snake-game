#include <iostream>
#include <cstdlib>          //rand
#include <windows.h>        //COORD, Console
#include <conio.h>          //kbhit
#include <ctime>            //srand
#include <vector>
using namespace std;

#define WIDTH 50		//ngang bang
#define HEIGHT 25		//sau bang

class Snake
{
    private:
        COORD pos;      //position
        int len;        //length
        int vel;        //velocity
        char dir;       //direction
        vector<COORD> body; 
    public:
        Snake(COORD pos, int vel)
        {
            this->pos = pos;
            this->vel = vel;
            len = 1;
            dir = 'n';
            body.push_back(pos);
        }
        void change_dir(char dir)
        { 
            this->dir = dir;
        }
        void move()
        {
            switch (dir)
            {
            case 'u':       //up
                pos.Y-=vel;
                break;
            case 'd':       //down
                pos.Y+=vel;
                break;
            case 'l':       //left
                pos.X-=vel;
                break;
            case 'r':       //right
                pos.X+=vel;
                break;
            }
            body.push_back(pos);
            if (body.size() > len)
            	body.erase(body.begin());
        }
        COORD get_pos()
        {
            return pos;
        }
        vector<COORD> get_body()
        {
        	return body;
		}
        bool eaten(COORD food_pos)
        {
            if ((food_pos.X == pos.X) && (food_pos.Y == pos.Y))
                return true;
            else
                return false;
        }
        void grow()
        {
            len++;
        }
        bool collided()
        {
            if (pos.X < 1 || pos.X > WIDTH-3 || pos.Y < 1 || pos.Y > HEIGHT-2)
                return true;
            for (int i=0;i<len-1;i++)
            	if (pos.X==body[i].X && pos.Y==body[i].Y)
            		return true;
            return false;
        }
};

class Food
{
    private:
        COORD pos;
    public:
        void gen_food()
        {
            pos.X = rand() % (WIDTH-2);     		//need 0-47 (giong j trong board)
            pos.Y = (rand() % (HEIGHT-2)) + 1;		//need 1-23 (giong i trong board)
        }
        COORD get_pos()
        {
            return pos;
        }
};

Snake snake({WIDTH/2, HEIGHT/2},1);
Food food;

void board()
{
    COORD snake_pos = snake.get_pos();
    COORD food_pos = food.get_pos();
    vector<COORD> snake_body = snake.get_body();
    for (int i=0;i<HEIGHT;i++)
    {
        cout << "\t\t#";                            //# dau dong
        for (int j=0;j<WIDTH-2;j++)                 //-2 do dau va cuoi nhap rieng
            if (i==0 || i==HEIGHT-1) cout << "#";
            else if (i==snake_pos.Y && j==snake_pos.X) cout << "0";
            else if (i==food_pos.Y && j==food_pos.X) cout << "*";
            else
            {
            	bool isBodyPart = false;
            	for (int k=0;k<snake_body.size()-1;k++)
            		if (i==snake_body[k].Y && j==snake_body[k].X)
            		{
            			cout << "o";
            			isBodyPart = true;
            			break;
					}
				if (!isBodyPart) 
					cout << " ";
			}
        cout << "#\n";                              //# cuoi dong
    }
}

int main()
{
    srand(time(NULL));
    food.gen_food();		//dam bao moi lan chay, food dau tien o cac vi tri khac nhau
    bool game_over = false;
    while(!game_over)
    {
        board();
        if (kbhit)      //keyboard hit
        {
            switch (getch())
            {
            case 'w':
                snake.change_dir('u');
                break;
            case 'a':
                snake.change_dir('l');
                break;
            case 's':
                snake.change_dir('d');
                break;
            case 'd':
                snake.change_dir('r');
                break;
            }
        }
        if (snake.collided()) 
            game_over = true;
        if (snake.eaten(food.get_pos()))
        {
            food.gen_food();
            snake.grow();
        }
        snake.move();
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),{0,0});
    }
    return 0;
}
