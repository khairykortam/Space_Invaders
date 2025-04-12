#pragma once
#include<raylib.h>
#include "block.hpp"
#include<vector>

class Obstacle{
    public:
        Obstacle(Vector2 position);
        void Draw() const;
        Vector2 position;
        std::vector<Block> blocks;
        static std::vector<std::vector<int>> grid;
    private:
        
};