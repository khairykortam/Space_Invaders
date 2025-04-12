#pragma once 
#include<raylib.h>

class Block {
    public:
     Block(Vector2 position);
     void Draw() const;
     Rectangle getRect();
     private:
     Vector2 position;
};
