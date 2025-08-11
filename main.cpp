#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;
using namespace sf;

const float pi = 3.14159f;
const int MAP_SIZE = 16;
const int CELL_SIZE = 30;
const int WIDTH = 800;
const int HEIGHT = 600;
const Color WALL_COLOR(180, 50, 50);
const Color FLOOR_COLOR(80, 80, 100);
const Color SKY_COLOR(110, 140, 220);


vector<vector<int>> generateMap() {
    vector<vector<int>> map(MAP_SIZE, vector<int>(MAP_SIZE, 0));
    srand(time(0));

    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            if (x == 0 || y == 0 || x == MAP_SIZE - 1 || y == MAP_SIZE - 1) {
                map[y][x] = 1;
            }
            else if (x == MAP_SIZE / 2 && y > 2 && y < MAP_SIZE - 3) {
                map[y][x] = 1;
            }
            else {
                map[y][x] = (rand() % 10) == 0 ? 1 : 0;
            }
        }
    }
    return map;
}

bool CanMoveTo(const vector<vector<int>>& map, float x, float y) {

    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            int checkX = static_cast<int>(x + i * 0.3f);
            int checkY = static_cast<int>(y + j * 0.3f);

            if (checkX < 0 || checkX >= MAP_SIZE ||
                checkY < 0 || checkY >= MAP_SIZE ||
                map[checkY][checkX] == 1) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "TG:Virus studio");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    auto map = generateMap();
    float playerX = 3.0f, playerY = 3.0f, playerAngle = 0.0f;


    const float moveSpeed = 3.0f;
    const float rotSpeed = 1.5f;

    Clock clock;
    float timeCounter = 0;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        timeCounter += dt;


        float walkEffect = 0.0f;
        bool isMoving = false;

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }


        float moveDelta = moveSpeed * dt;
        float rotDelta = rotSpeed * dt;

        if (Keyboard::isKeyPressed(Keyboard::W)) {
            float newX = playerX + sinf(playerAngle) * moveDelta;
            float newY = playerY + cosf(playerAngle) * moveDelta;
            if (CanMoveTo(map, newX, newY)) {
                playerX = newX;
                playerY = newY;
                isMoving = true;
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::S)) {
            float newX = playerX - sinf(playerAngle) * moveDelta;
            float newY = playerY - cosf(playerAngle) * moveDelta;
            if (CanMoveTo(map, newX, newY)) {
                playerX = newX;
                playerY = newY;
                isMoving = true;
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            playerAngle -= rotDelta;
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            playerAngle += rotDelta;
        }


        if (isMoving) {
            walkEffect = sinf(timeCounter * 10.0f) * 0.02f;
        }

        

        for (int x = 0; x < WIDTH; x++) {
            float rayAngle = (playerAngle - pi / 6.0f) + (x / (float)WIDTH) * (pi / 3.0f);
            float distanceToWall = 0;
            bool hitWall = false;

            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);

            while (!hitWall && distanceToWall < 20) {
                distanceToWall += 0.05f;
                int testX = (int)(playerX + eyeX * distanceToWall);
                int testY = (int)(playerY + eyeY * distanceToWall);

                if (testX < 0 || testX >= MAP_SIZE || testY < 0 || testY >= MAP_SIZE) {
                    hitWall = true;
                    distanceToWall = 20;
                }
                else if (map[testY][testX] == 1) {
                    hitWall = true;
                }
            }

            
            int ceiling = (int)((HEIGHT / 2.0 - HEIGHT / distanceToWall) + walkEffect * HEIGHT);
            int floor = HEIGHT - ceiling;

            
            float darkness = 1.0f - min(distanceToWall / 20.0f, 0.9f);
            Color wallShade(
                (Uint8)(WALL_COLOR.r * darkness),
                (Uint8)(WALL_COLOR.g * darkness),
                (Uint8)(WALL_COLOR.b * darkness)
            );

            
            Vertex wallLine[] = {
                Vertex(Vector2f(x, ceiling), wallShade),
                Vertex(Vector2f(x, floor), wallShade)
            };
            window.draw(wallLine, 2, Lines);

       
            for (int y = floor; y < HEIGHT; y++) {
                float floorDistance = HEIGHT / (2.0f * y - HEIGHT);
                float floorDarkness = 1.0f - min(floorDistance / 20.0f, 0.9f);
                Color floorShade(
                    (Uint8)(FLOOR_COLOR.r * floorDarkness),
                    (Uint8)(FLOOR_COLOR.g * floorDarkness),
                    (Uint8)(FLOOR_COLOR.b * floorDarkness)
                );
                Vertex floorPoint(Vector2f(x, y), floorShade);
                window.draw(&floorPoint, 1, Points);
            }
        }

   
        CircleShape cursor(3);
        cursor.setFillColor(Color::Red);
        cursor.setPosition(WIDTH / 2 - 3, HEIGHT / 2 - 3);
        window.draw(cursor);


        window.clear(SKY_COLOR);



        CircleShape cursor(3);
        cursor.setFillColor(Color::Red);
        cursor.setPosition(WIDTH / 2 - 3, HEIGHT / 2 - 3);
        window.draw(cursor);

        window.display();
    }

    return 0;
}
