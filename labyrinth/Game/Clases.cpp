#include "Clases.h"
//===============================================CLASES BARRIER and PLAYER=====================================================

//============================================PLAYER===========================================================================
HGE* c_player::hge = 0;
 
c_player::c_player(hgeVector position, hgeVector velocity) : Position(position), Velocity(velocity)
{
    hge = hgeCreate(HGE_VERSION); //Get interface to hge
 
    Texture = hge->Texture_Load("images/1.png");
    Sprite  = new hgeAnimation(Texture,4,4,0,0,34,29);// создание анимации с четырься кадрами и четыри кадра в секунду
    Sprite->SetHotSpot(32,14.5);//хот-спот точк для анимации, которая является центром для масштабирования
    Sprite->Play();
 
    Speed       = 0.15;
}
c_player::~c_player()
{
    hge->Texture_Free(Texture);
    delete Sprite;
 
    hge->Release();
}
bool c_player::Update(float delta)
{
    Velocity.x *= PLAYER_FRICTION;
    Velocity.y *= PLAYER_FRICTION;
 
    Position.x += Velocity.x;
    Position.y += Velocity.y;
	if(hge->Input_GetKeyState(HGEK_A) || hge->Input_GetKeyState(HGEK_LEFT)     && Position.x > 32) Velocity.x -= (Speed * M_PI) /2;
    if(hge->Input_GetKeyState(HGEK_D) || hge->Input_GetKeyState(HGEK_RIGHT)  && Position.x < 800) Velocity.x += (Speed * M_PI) /2;
    if(hge->Input_GetKeyState(HGEK_W) || hge->Input_GetKeyState(HGEK_UP)   && Position.y > 0) Velocity.y -= (Speed * M_PI) /2;
    if(hge->Input_GetKeyState(HGEK_S) || hge->Input_GetKeyState(HGEK_DOWN)   && Position.y < 600) Velocity.y += (Speed * M_PI) /2;
	if(Position.x > 800) { Position.x -= 1; Velocity.x = -Velocity.x; };
	if(Position.x < 0)   { Position.x += 1; Velocity.x = -Velocity.x; };
	if(Position.y < 0)    { Position.y += 1; Velocity.y = -Velocity.y; };
	if(Position.y > 600) { Position.y -= 1; Velocity.y = -Velocity.y; };
	Sprite->Update(delta);
 
    Sprite->GetBoundingBox(Position.x, Position.y, &BoundingBox);
    return false;
}
void c_player::Render()
{
    Sprite->Render(Position.x, Position.y);
}

//------------------------------------------------BARIER==========================================================
HGE* barrier::hge = 0;
 
barrier::barrier(hgeVector position, HTEXTURE &Texture) : Position(position)
{
    hge = hgeCreate(HGE_VERSION);
	Sprite  = new hgeSprite(Texture,0,0,30,30);
    Sprite->SetHotSpot(15,15);
    Sprite->GetBoundingBox(Position.x, Position.y, &BoundingBox);
}

barrier::~barrier()
{
    delete Sprite;
    hge->Release();
}
void barrier::Render()
{
    Sprite->Render(Position.x, Position.y);
}

d_barrier::d_barrier(hgeVector Position,HTEXTURE &Texture,float speed_x,float speed_y,hgeVector EndPosition):barrier(Position,Texture)
{
	dx=speed_x;
	dy=speed_y;
	start_x=Position.x;
	start_y=Position.y;
	end_x= EndPosition.x;
	end_y= EndPosition.y;
}
	
void d_barrier::Update(float dt)
{
	if ((GetPosition().x<end_x+1)&&(GetPosition().x>end_x-1)) //если находимсь рядом с конечной точкой
	{	
		float temp=end_x;
		end_x=start_x;
		start_x=temp;
		temp=end_y;
		end_y=start_y;
		start_y=temp;
		dx=-dx;
		dy=-dy;
	}
	else if ((GetPosition().y<end_y+1)&&(GetPosition().y>end_y-1)) //если находимсь рядом с конечной точкой
	{	
		float temp=end_y;
		end_y=start_y;
		start_y=temp;
		temp=end_x;
		end_x=start_x;
		start_x=temp;
		dy=-dy;
		dx=-dx;
	}
	SetPosition(hgeVector(GetPosition().x+dt*dx,GetPosition().y+dt*dy));
}
	