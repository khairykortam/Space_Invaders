#pragma once
#include"spaceship.hpp"
#include"obstacle.hpp"
#include"alien.hpp"
#include "mysteryship.hpp"


class Game {
    public: 
    Game();
    ~Game();
    void Draw();
    void Update();
    void HandleInput();
    bool run;
    int lives;
    int score;
    int highScore;
    Music music;
    int level; // Add level variable

    private:
    void DeleteInactiveLasers();
    std::vector<Obstacle> CreateObstacles();
    std::vector<Alien> CreateAliens();
    void MoveAliens();
    void MoveDownAliens(int distance);
    void AlienShootLaser();
    void CheckForCollisions();
    void GameOver();
    void Reset();
    void InitGame();
    void ResetForNextLevel(); // Add ResetForNextLevel method
    void checkFORHighScore();
    void saveHighScoreToFile(int highscore);
    int loadHighScoreFromFile();
    Spaceship spaceship;
    std::vector<Obstacle> obstacles;
    std::vector<Alien> aliens;
    int alienDirection;
    std::vector<Laser> alienLasers;
    constexpr static float alineLaserShootInterval =0.35;
    float timeLastAlienFired;
    MysteryShip mysteryShip;
    float mysteryShipSpawnInterval;
    float timeLastSpawn;
 Sound explosionSound;
    

};