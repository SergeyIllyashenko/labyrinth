#ifndef CLASES
#define CLASES
#pragma once
//===============================================CLASES BARRIER and PLAYER=====================================================

#include <hge.h>
#include <hgevector.h>
#include <hgeanim.h>
 
//============================================PLAYER===========================================================================
#define PLAYER_FRICTION 0.95
class c_player
{
private:
    static HGE*     hge;
    hgeVector       Position;
    hgeVector       Velocity;
    HTEXTURE        Texture;
    hgeAnimation*   Sprite;
    hgeRect         BoundingBox; //необходимо  для обнаружения столкновений
    float           Speed;
public:
    c_player(hgeVector Position, hgeVector Velocity);
    ~c_player();
 
    bool Update(float delta);
    void Render();
 
    void        SetVelocity(hgeVector velocity) { Velocity = velocity; };
    void        SetPosition(hgeVector position) 
	{
		Position = position;
	    Sprite->GetBoundingBox(Position.x, Position.y, &BoundingBox); 
	};
    hgeRect     GetBoundingBox() { return BoundingBox; };
    hgeVector   GetPosition() { return Position; };
};
 
//------------------------------------------------BARIER==========================================================
class barrier
{
private:
    static HGE*     hge;
    hgeVector       Position;
	hgeSprite*   Sprite;
    hgeRect         BoundingBox;
public:
	barrier(hgeVector Position, HTEXTURE &Texture);
    ~barrier();
    void Render();
	virtual void Update(float dt)=0;
    hgeRect     GetBoundingBox() { Sprite->GetBoundingBox(Position.x, Position.y, &BoundingBox); return BoundingBox; };
    hgeVector   GetPosition() { return Position; };
    void        SetPosition(hgeVector position) { Position = position; };
};

class c_barrier : public barrier
{
public:
	c_barrier(hgeVector Position,HTEXTURE &Texture):barrier(Position,Texture) {};
	void Update(float dt){};
	~c_barrier(){};
};
class d_barrier : public barrier
{
private:
	float dx; //скорость по х
	float dy; //скорость по у

	float start_x;  //стартовая точка по х
	float start_y;  //стартовая точка по у
	float end_x;  //конечная точка по х
	float end_y;  //конечная точка по у

public:
	d_barrier(hgeVector Position,HTEXTURE &Texture,float speed_x,float speed_y,hgeVector EndPosition);
	void Update(float dt);
	~d_barrier(){};
};

#endif