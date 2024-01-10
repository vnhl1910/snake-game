#include<iostream>
#include<raylib.h>
#include<deque> //add and remove from both end->use for snake
#include<raymath.h> //Vector2Add...

Color green = {36,150,34,255}; //4th argument: transparency
Color darkGreen = {255,192,203,255};
Color headColor = {147,216,35,255};
Color button = {128,32,147,255};

float cellSize = 30;    //float, for rectangle struct, line 24
float cellCount = 25; //30x25=750
float border = 75; 

double lastUpdateTime = 0;
double event = 0.2;

using namespace std;

bool checkDeque(Vector2 element, deque<Vector2> deque)  //check if food and snake body are in the same cell
{
    for(unsigned int i=0; i<deque.size(); i++)
        if(Vector2Equals(deque[i],element))
            return true;
    return false;
}

//after interval, the event happened
bool eventTriggered(double interval)    //interval: khoang thoi gian
{
    double currentTime = GetTime(); //get current time
    if(currentTime - lastUpdateTime >= interval)    
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    Vector2 direction = {1,0};
    bool addBody = false;   //eat food

    void Draw()
    {
        Rectangle head = Rectangle{border+body[0].x*cellSize, border+body[0].y*cellSize,cellSize,cellSize};
        DrawRectangleRounded(head, 0.5, 6, headColor);   //0.5: roundness, 6: int segment(???)
        for(unsigned int i=1;i<body.size();i++) //deque size is unsigned
        {
            float x = body[i].x;
            float y = body[i].y;
            //add border because of frame
            Rectangle segment = Rectangle{border+x*cellSize, border+y*cellSize, cellSize, cellSize};  //segment: doan
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);   //0.5: roundness, 6: int segment(???)
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0],direction));
        if(addBody == true)
            addBody = false;
        else body.pop_back();
    }

    void Reset()
    {
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {1,0};
    }
};

class Food

{
public:
    Vector2 position;   //struct Vector2 {x,y}
    Texture2D texture;

    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("Graphics/food.png");   
        texture = LoadTextureFromImage(image);
        UnloadImage(image); //for free memory
        position = randomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawTexture(texture, border+position.x*cellSize, border+position.y*cellSize, WHITE);  //WHITE: filled color
    }

    Vector2 randomCell()
    {
        float x = GetRandomValue(0, cellCount-1);
        float y = GetRandomValue(0, cellCount-1);
        return Vector2{x,y};
    }

    Vector2 randomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = randomCell();
        while(checkDeque(position, snakeBody))
            position = randomCell();
        return position;
    }
};

class Game  //manage and easy to fix bug
{
public:
    Snake snake;
    Food food = Food(snake.body);
    bool run = true;    //check reset game
    int score = 0;
    Sound eat;
    Sound edge;

    Game()
    {
        InitAudioDevice();
        eat = LoadSound("Sounds/eat.mp3");
        edge = LoadSound("Sounds/wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eat);
        UnloadSound(edge);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }
    void Update()
    {  
        if(run) //after game over, stop update
        {
            snake.Update();
            eatFood();
            checkCollisionEdges();
            checkCollisionBody();
        }
    }

    void eatFood()
    {
        if(Vector2Equals(snake.body[0],food.position))
        {
            food.position = food.randomPos(snake.body);
            snake.addBody = true;
            score+=10;
            PlaySound(eat);
            if(event > 0.01) event-=0.01;
        }
    }

    void checkCollisionEdges()
    {
        if(snake.body[0].x==-1 || snake.body[0].x==cellCount || snake.body[0].y==-1 ||snake.body[0].y==cellCount)
            GameOver();
    }

    void checkCollisionBody()
    {
        deque<Vector2> headless = snake.body;   
        headless.pop_front();   //pop head to check same cell with other
        if(checkDeque(snake.body[0],headless))
            GameOver();
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.randomPos(snake.body);
        run = false;
        score = 0;
        PlaySound(edge);
        event = 0.2;
    }
};

void Play()
{
    InitWindow(cellCount*cellSize + 2*border, cellCount*cellSize + 2*border , "Snake game");   //draw game window size 750x750
    SetTargetFPS(24);   //set 60fps -> update the display 60/s
    
    Game game;

    while(!WindowShouldClose())
    {
        BeginDrawing();
        if(eventTriggered(event)) //update after 0.2s -> to make the snake slow
        {
            game.Update();
        }

        if(IsKeyPressed(KEY_W) && game.snake.direction.y!=1)
        {
            game.snake.direction = {0,-1};
            game.run = true;    //after game reset, press key to play again
        }
        if(IsKeyPressed(KEY_S) && game.snake.direction.y!=-1)
        {   
            game.snake.direction = {0,1};
            game.run = true;
        }
        if(IsKeyPressed(KEY_A) && game.snake.direction.x!=1)
        {
            game.snake.direction = {-1,0};
            game.run = true;
        }
        if(IsKeyPressed(KEY_D) && game.snake.direction.x!=-1)
        {
            game.snake.direction = {1,0};
            game.run = true;
        }

        ClearBackground(green);
        //draw outline rectangle with position (border-5) because of thickness 5 and add 10 to size to offset
        DrawRectangleLinesEx(Rectangle{border-5,border-5,cellCount*cellSize+10,cellCount*cellSize+10},5,darkGreen);
        DrawText("SNAKE GAME",border+5,20,40,darkGreen);    
        DrawText(TextFormat("%i", game.score),border-5,border+cellSize*cellCount+10,40,darkGreen);
        game.Draw();

        EndDrawing();
    }
}

void Guide()
{
    InitWindow(cellCount*cellSize + 2*border, cellCount*cellSize + 2*border , "Snake game");
    Texture2D asuna = LoadTexture("Graphics/rule.png");
    while(!WindowShouldClose())
    {
        BeginDrawing();
        DrawTexture(asuna,0,0,WHITE);
        EndDrawing();
    }
    UnloadTexture(asuna);
}

void About()
{
    InitWindow(cellCount*cellSize + 2*border, cellCount*cellSize + 2*border , "Snake game");
    Texture2D asuna = LoadTexture("Graphics/about.png");
    while(!WindowShouldClose())
    {
        BeginDrawing();
        DrawTexture(asuna,0,0,WHITE);
        EndDrawing();
    }
    UnloadTexture(asuna);
}

int main()
{
    InitWindow(cellCount*cellSize + 2*border, cellCount*cellSize + 2*border , "Snake game");
    Rectangle vanh = {(cellCount*cellSize + 2*border)/2-125,(cellCount*cellSize + 2*border)/2,cellSize*9,cellSize*3};
    Rectangle vanh1 = {(cellCount*cellSize + 2*border)/2-125,(cellCount*cellSize + 2*border)/2+150,cellSize*9,cellSize*3};
    Rectangle vanh2 = {(cellCount*cellSize + 2*border)/2-125,(cellCount*cellSize + 2*border)/2+300,cellSize*9,cellSize*3};
    while(!WindowShouldClose())
    {
        BeginDrawing();
        Texture2D background = LoadTexture("Graphics/1.png");
        ClearBackground(RAYWHITE);
        DrawTexture(background,0,0,WHITE);
        DrawRectangleRec(vanh,RED);
        DrawText("Play",vanh.x+75,vanh.y+20,50,WHITE);
        DrawRectangleLinesEx(vanh,2,{0,0,0,255});
        DrawRectangleRec(vanh1,RED);
        DrawText("Guide",vanh1.x+65,vanh1.y+20,50,WHITE);
        DrawRectangleLinesEx(vanh1,2,{0,0,0,255});
        DrawRectangleRec(vanh2,RED);
        DrawText("About",vanh2.x+65,vanh2.y+20,50,WHITE);
        DrawRectangleLinesEx(vanh2,2,{0,0,0,255});
        if(CheckCollisionPointRec(GetMousePosition(),vanh) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            CloseWindow();
            Play();     
        }
        else if(CheckCollisionPointRec(GetMousePosition(),vanh1) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            CloseWindow();
            Guide();     
        }
        else if(CheckCollisionPointRec(GetMousePosition(),vanh2) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            CloseWindow();
            About();     
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}