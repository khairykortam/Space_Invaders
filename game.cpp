#include "game.hpp"
#include<iostream>
#include <fstream>

Game::Game()
{
    music = LoadMusicStream("Sounds/music.ogg");
    explosionSound = LoadSound("Sounds/explosion.ogg");
    PlayMusicStream(music);
    obstacles = CreateObstacles();
    aliens = CreateAliens();
    alienDirection =1;
    timeLastAlienFired = 0.0;
    timeLastSpawn = 0.0;
    lives =3;
    run = true;
    mysteryShipSpawnInterval  = GetRandomValue(10,20);
    mysteryShip.Spawn();
    level = 1; // Initialize level
    score = 0; // Initialize score
    lives = 3; // Initialize lives
    InitGame();
}


Game::~Game(){
 Alien::UnloadImages();
 UnloadMusicStream(music);
 UnloadSound(explosionSound);
}

void Game::Update(){
    if(run){

double currentTIme = GetTime();
if(currentTIme - timeLastSpawn > mysteryShipSpawnInterval){
    mysteryShip.Spawn();
    timeLastSpawn = GetTime();
    mysteryShipSpawnInterval = GetRandomValue(10,20);
}

    for(auto& laser: spaceship.lasers){
        laser.Update();
    }

    MoveAliens();

    AlienShootLaser();

    for(auto& laser : alienLasers){
        laser.Update();
    }
    
    DeleteInactiveLasers();

    mysteryShip.Update();

    CheckForCollisions();

    if(aliens.empty()) { // Check if all aliens are cleared
        score += 1000; // Add bonus points
        checkFORHighScore(); // Update high score if necessary
        level++; // Increment level
        ResetForNextLevel(); // Reset only necessary elements for the next level
        InitGame(); // Reinitialize the game
    }

    } else{
        if(IsKeyDown(KEY_ENTER)){
           Reset();
           InitGame();
        }
    }
}

void Game::Draw(){
    spaceship.Draw();

    for(auto& laser: spaceship.lasers){
        laser.Draw();
    }

    for(auto& obstacle: obstacles){
        obstacle.Draw();
    }

    for(auto& alien: aliens){
        alien.Draw();
    }

    for(auto& laser: alienLasers){
        laser.Draw();
    }
    
    mysteryShip.Draw();
    DrawText(TextFormat("LEVEL: %d", level), 350, 15, 20, YELLOW); // Display current level
}

void Game::HandleInput(){
    if(run){
    if(IsKeyDown(KEY_LEFT)){
        spaceship.MoveLeft();
    } else if(IsKeyDown(KEY_RIGHT)){
        spaceship.MoveRight();
    } else if(IsKeyDown(KEY_SPACE)){
        spaceship.FireLaser();
    }
}
}

void Game::DeleteInactiveLasers()
{
    for(auto it = spaceship.lasers.begin(); it != spaceship.lasers.end();){
        if(!it -> active){
            it = spaceship.lasers.erase(it);
        } else {
            ++ it ;
        }
    }

    for(auto it = alienLasers.begin(); it != alienLasers.end();){
        if(!it -> active){
            it = alienLasers.erase(it);
        } else {
            ++ it ;
        }
    }

}
std::vector<Obstacle> Game::CreateObstacles()
{
    int obstacleWidth = Obstacle::grid[0].size() * 3;
    float gap = (GetScreenWidth() - (obstacleWidth * 4)) / 5;

    for(int i=0;i<4;i++){
        float offsetX = (i+1)*gap + i * obstacleWidth;
        obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight() - 200)}));
        
    }
     return obstacles;
}

std::vector<Alien> Game::CreateAliens()
{
    std::vector<Alien> aliens;
    for(int row=0;row<5;row++){
      for(int col =0;col<11;col++){

     int alienType;
     if(row == 0){
        alienType =3;
     }else if (row ==1||row==2){
        alienType =2;
     } else {
        alienType = 1;
     }

        float x = 75+ col * 55;
        float y = 110+ row * 55;
        aliens.push_back(Alien(alienType,{x,y}));
      }
    }
    return aliens;
}

void Game::MoveAliens(){
    for(auto& alien: aliens){
      if(alien.position.x + alien.alienImages[alien.type -1].width> GetScreenWidth() -25){
        alienDirection = -1;
        MoveDownAliens(4);
      }
      if(alien.position.x < 25){
        alienDirection =1;
        MoveDownAliens(4);
      }

        alien.Update(alienDirection);
    }


}

void Game::MoveDownAliens(int distance)
{
    for(auto& alien: aliens){
        alien.position.y +=distance;
    }
}

void Game::AlienShootLaser()
{
    double currentTime = GetTime();
    if(currentTime - timeLastAlienFired >= alineLaserShootInterval && !aliens.empty()){
    int randomIndex = GetRandomValue(0,aliens.size()-1);
    Alien& alien = aliens[randomIndex];
    alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type-1].width/2,
                                alien.position.y + alien.alienImages[alien.type-1].height}, 6));
 timeLastAlienFired = GetTime();    
}
 }

 void Game::CheckForCollisions()
 {
    // spaceship lasers with aliens

    for(auto& laser: spaceship.lasers){
        auto it = aliens.begin();
        while(it!=aliens.end()){
            if(CheckCollisionRecs(it-> getRect(),laser.getRect())){

                PlaySound(explosionSound);
              if(it-> type ==1){ 
                score+=100;
              } else if(it->type ==2){
                score +=200;
              } else if(it->type ==3){
                score +=300;
              }
              checkFORHighScore();

                it = aliens.erase(it);
                laser.active = false;
            } else{
                ++it;
            }
        }

        for(auto& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it->getRect(), laser.getRect())){
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }

            if(CheckCollisionRecs(mysteryShip.getRect(), laser.getRect())){
                mysteryShip.alive = false;
                laser.active = false;
                score+= 500;
                checkFORHighScore();
                PlaySound(explosionSound);

            }
        }

        }

        // alien lasers 
        for(auto& laser: alienLasers){
            if(CheckCollisionRecs(laser.getRect(), spaceship.getRect())){
                laser.active = false;
              lives--;
              if(lives == 0){
                GameOver();
              }
            }
            for(auto& obstacle: obstacles){
                auto it = obstacle.blocks.begin();
                while(it != obstacle.blocks.end()){
                    if(CheckCollisionRecs(it->getRect(), laser.getRect())){
                        it = obstacle.blocks.erase(it);
                        laser.active = false;
                    } else {
                        ++it;
                    }
                }
            }
        }

        // alien with obstcals collision

        for(auto& alien: aliens){
            for(auto& obstacle: obstacles){
                auto it = obstacle.blocks.begin();
                while(it != obstacle.blocks.end()){
                    if(CheckCollisionRecs(it->getRect(), alien.getRect())){
                        it = obstacle.blocks.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

           if(CheckCollisionRecs(alien.getRect(), spaceship.getRect())){
                GameOver();
            }

        }

    }

void Game::GameOver()
{
    run = false;
    score = 0; // Reset the score when the game is over
}

void Game::InitGame()
{
obstacles = CreateObstacles();
aliens = CreateAliens();
alienDirection =1;
timeLastAlienFired = 0.0;
timeLastSpawn = 0.0;
run = true;
mysteryShipSpawnInterval  = GetRandomValue(10,20);
mysteryShip.Spawn();
highScore = loadHighScoreFromFile();
// Note: Score and lives are no longer reset here
}

void Game::checkFORHighScore()
{
    if(score > highScore){
        highScore = score;
        saveHighScoreToFile(highScore);
    }
}

void Game::saveHighScoreToFile(int highscore)
{
 std::ofstream highscoreFile("highscore.txt");
if(highscoreFile.is_open()){
    highscoreFile << highscore;
    highscoreFile.close();
} else {
    std::cerr << "Unable to open file";
}
}

int Game::loadHighScoreFromFile(){
    int loadHighscore = 0;
    std::ifstream higscoreFile("highscore.txt");
    if(higscoreFile.is_open()){
        higscoreFile >> loadHighscore;
        higscoreFile.close();
    } else {
        std::cerr << "Unable to open file";
    }
    return loadHighscore;
}

void Game::Reset(){
    spaceship.Reset();
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
}

void Game::ResetForNextLevel(){
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
    mysteryShip.alive = false;
    timeLastSpawn = 0.0;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
    // Note: Score and lives are preserved
}
