#include <hge.h>
#include <hgesprite.h>
#include <hgevector.h>
#include <List>
#include "Clases.h"
#include "menuitem.h"

#include <iostream>
#include <fstream>

HGE*    hge = NULL;

//Background
HTEXTURE    g_tBackground       = 0;
HTEXTURE    g_tEColors          = 0; 
HTEXTURE	g_tExit				= 0;
hgeSprite*	g_sExit				= NULL;
hgeSprite*  g_sBackground       = NULL;
hgeVector   g_vBGPosition       = hgeVector(0,0);
//Player and Barrier
c_player*   Player1             = NULL;
std::list<barrier*> Barriers;
//Sounds
HEFFECT     g_eBGMusic          = 0;
HEFFECT     g_MenueBGMusic          = 0;
HTEXTURE    g_tExplosion        = 0;
HEFFECT     g_eExplosion        = 0;
// Pointers to the HGE objects we will use
hgeGUI				*gui;
hgeFont				*fnt;
hgeSprite			*spr;
HTEXTURE			tcur;

//Levels
int		lifes=5;
int		level=1;
float	left_time;
bool	show_message=false;
char	message[200];

void Save()
{
	FILE *f;
	f = fopen("save.txt","w");
	fprintf(f,"%d\n",lifes);
	fprintf(f, "%d\n", level);
	fprintf(f,"%d\n", left_time);
	fclose(f);
}
void Load()
{
	std::ifstream load("save.txt") ; 
	load >> lifes;
	load >> level;
	load >> left_time;
	left_time=left_time/100000000;
	if (left_time<0)
	{
		left_time=left_time*(-1);
	}
}
struct explosion
{
	hgeAnimation*   Animation;
	hgeVector       Position;
};
std::list<explosion> Explosions;

void		CreateExplosion(hgeVector Position);

bool ActiveMenu=true;

int lvlload;

void init_level(int lvl); 

bool FrameFunc()
{
	float delta = hge->Timer_GetDelta();
	if (ActiveMenu)			
	{
		int id;
		static int lastid=0;

		id=gui->Update(delta);
		if(id == -1)
		{
			switch(lastid)
			{
			case 1:
				ActiveMenu=false;
				lvlload=0;
				init_level(1);
				gui->Leave();
				break;
			case 2:
				Save();
				gui->Leave();
				gui->Enter();
				break;
			case 3:
				ActiveMenu=false;
				Load();
				lvlload=1;
				init_level(level);
				gui->Leave();
				break;
			case 4:
				ActiveMenu=false;
				if ((strcmp(message,"GAME OVER\npress Esc to go to menu...")==0)||(strcmp(message,"You are WIN"))==0)
				{
					strcpy(message, " ");
				    lvlload=0;
					init_level(1);
				}
				gui->Leave();
				break;
			case 5: return true;
			}
		}
		else if(id) { lastid=id; gui->Leave(); }
	}
	else if(!show_message)
	{
		left_time-=delta;
		if ((left_time<=0)||(lifes<=0))
		{
			level--;
			show_message=true;
			if (level==0)
			{
				strcpy(message,"GAME OVER\npress Esc to go to menu...");
			}
			else
			{
				strcpy(message,"You lose this level...");
			}
		}
		if(hgeRect(720,520,770,570).Intersect(&Player1->GetBoundingBox()))
		{
				show_message=true;
				level++;
				if (level>10)
				{
					show_message=true;
					strcpy(message,"You are WIN");
				}
				else 
					strcpy(message,"Go to the next level");
				
		}
			
		//Explosions
		for(std::list<explosion>::iterator i = Explosions.begin(); i != Explosions.end(); /**/)
		{
			if((*i).Animation->GetFrame() == 4)
			{
				delete (*i).Animation;
				i = Explosions.erase(i);
			}
			else
			{
				(*i).Animation->Update(delta);
				i++;
			}
		}
		Player1->Update(delta);
		for(std::list<barrier*>::iterator i = Barriers.begin(); i != Barriers.end(); i++)
		{
			(*i)->Update(delta);
		}
		for(std::list<barrier*>::iterator i = Barriers.begin(); i != Barriers.end(); i++)
		{
			if((*i)->GetBoundingBox().Intersect(&Player1->GetBoundingBox()))
			{
				CreateExplosion(Player1->GetPosition());
				Player1->SetPosition(hgeVector(10,268));
				Player1->SetVelocity(hgeVector(5,0));
				lifes--;
				break;
			}
		}
		
		
		if (hge->Input_GetKeyState(HGEK_ESCAPE))
		{ 
			ActiveMenu=true;
			gui->Enter();
		}
	}
	else
	{
		delta=0;
		if (hge->Input_GetKeyState(HGEK_ESCAPE))
		{ 
			ActiveMenu=true;
			gui->Enter();
		}
		if ((hge->Input_GetKeyState(HGEK_ENTER))&&(show_message)&&(strcmp(message,"You are WIN")!=0)&&(strcmp(message,"GAME OVER\npress Esc to go to menu...")!=0))
		{ 
			init_level(level);
		}
	}
	return false;
}
bool RenderFunc()
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);
	g_sBackground->Render(g_vBGPosition.x,g_vBGPosition.y);
	Player1->Render();
	for(std::list<barrier*>::iterator i = Barriers.begin(); i != Barriers.end(); i++)
	{
		(*i)->Render();
	}
	//Explosions
	for(std::list<explosion>::iterator i = Explosions.begin(); i != Explosions.end(); i++)
	{
		(*i).Animation->Render((*i).Position.x, (*i).Position.y);
	}
	if (ActiveMenu)
	{
		hge->Gfx_Clear(0);
		g_sBackground->Render(g_vBGPosition.x,g_vBGPosition.y);
	
		gui->Render();
	}
	else if (show_message)
	{
		hge->Gfx_Clear(0);
		fnt->SetColor(ARGB(255,100,0,255));
		fnt->printf(400,250,HGETEXT_CENTER,"%s",message);
	}
	else
	{		
		fnt->SetColor(ARGB(255,0,255,0));
		fnt->printf(20,20,HGETEXT_LEFT,"Time Left %f\nLevel: %d\nLifes: %d",left_time,level,lifes);
		g_sExit->Render(750,550);
	}
	hge->Gfx_EndScene();
	return false;
}

void Razvertka(int line,int from,int to=20)
{
	for (int i=from;i<=to;i++)
	{ 
		c_barrier* Enemy = new c_barrier(hgeVector(135+i*30, 15+line*30), g_tEColors);
		Barriers.push_back(Enemy);
	}
}

void init_level(int lev)
{	
	Player1->SetPosition(hgeVector(10,268));
	Player1->SetVelocity(hgeVector(5,0));
	
	show_message=false;
	strcpy(message," ");
	for(std::list<barrier*>::iterator i = Barriers.begin(); i != Barriers.end(); /**/)
	{
		delete (*i);
		i = Barriers.erase(i);
	}
	for(std::list<explosion>::iterator i = Explosions.begin(); i != Explosions.end(); /**/)
	{
		i = Explosions.erase(i);
	}

	std::fstream file;
	if (lev==1)
	{
		if (lvlload==1)
		{
			Load();
			file.open("levels/1.lvl");
		}
		else
		{
			lifes=5;
			left_time=12;
			file.open("levels/1.lvl");
		}
	}
	if (lev==2)
	{
		if (lvlload==1)
		{
			Load();
		    file.open("levels/2.lvl");
		}
		else
		{
			lifes=4;
			left_time=60;
			file.open("levels/2.lvl");
		}
	}
	if (lev==3)
	{
		if (lvlload==1)
		{
			Load();
			file.open("levels/3.lvl");	
		}
		else
		{
			lifes=3;
			left_time=30;
			file.open("levels/3.lvl");	
		}
	}
	if (lev==4)
	{
		if (lvlload==1)
		{
			Load();
			file.open("levels/4.lvl");
		}
		else
		{	
			lifes=2;
			left_time=30;
			file.open("levels/4.lvl");
		}
	}
	if (lev==5)
	{
		if (lvlload==1)
		{
			Load();
			file.open("levels/5.lvl");	
		}
		else
		{
			lifes=1;
			left_time=30;
			file.open("levels/5.lvl");	
		}
	}
	if (lev==6)
	{
		if (lvlload==1)
		{
			Load();
			file.open("levels/1.lvl");
			d_barrier* Enemy = new d_barrier(hgeVector(200, 150), g_tEColors,10,50,hgeVector(280, 350));
			Barriers.push_back(Enemy);
			d_barrier* Enemy1 = new d_barrier(hgeVector(300, 130), g_tEColors,30,70,hgeVector(280, 200));
			Barriers.push_back(Enemy1);
			d_barrier* Enemy2 = new d_barrier(hgeVector(300, 220), g_tEColors,10,70,hgeVector(280, 280));
			Barriers.push_back(Enemy2);
			d_barrier* Enemy3 = new d_barrier(hgeVector(450, 150), g_tEColors,30,70,hgeVector(200, 250));
			Barriers.push_back(Enemy3);	
			d_barrier* Enemy4 = new d_barrier(hgeVector(580, 220), g_tEColors,10,70,hgeVector(280, 280));
			Barriers.push_back(Enemy4);	
			d_barrier* Enemy5 = new d_barrier(hgeVector(620, 150), g_tEColors,10,50,hgeVector(200, 250));
			Barriers.push_back(Enemy5);	
		}
		else
		{			lifes=5;
			left_time=30;
			file.open("levels/1.lvl");
			d_barrier* Enemy = new d_barrier(hgeVector(200, 150), g_tEColors,10,50,hgeVector(280, 350));
			Barriers.push_back(Enemy);
			d_barrier* Enemy1 = new d_barrier(hgeVector(300, 130), g_tEColors,30,70,hgeVector(280, 200));
			Barriers.push_back(Enemy1);
			d_barrier* Enemy2 = new d_barrier(hgeVector(300, 220), g_tEColors,10,70,hgeVector(280, 280));
			Barriers.push_back(Enemy2);
			d_barrier* Enemy3 = new d_barrier(hgeVector(450, 150), g_tEColors,30,70,hgeVector(200, 250));
			Barriers.push_back(Enemy3);	
			d_barrier* Enemy4 = new d_barrier(hgeVector(580, 220), g_tEColors,10,70,hgeVector(280, 280));
			Barriers.push_back(Enemy4);	
			d_barrier* Enemy5 = new d_barrier(hgeVector(620, 150), g_tEColors,10,50,hgeVector(200, 250));
			Barriers.push_back(Enemy5);	
		}
	}
	if (lev==7)
	{
		if (lvlload==1)
		{
			Load();
			file.open("levels/2.lvl");	
			d_barrier* Enemy = new d_barrier(hgeVector(200, 280), g_tEColors,10,50,hgeVector(280, 340));
			Barriers.push_back(Enemy);
			d_barrier* Enemy1 = new d_barrier(hgeVector(370, 150), g_tEColors,50,10,hgeVector(435, 170));
			Barriers.push_back(Enemy1);
		}
		else
		{
			lifes=4;
			left_time=30;
			file.open("levels/2.lvl");	
			d_barrier* Enemy = new d_barrier(hgeVector(200, 280), g_tEColors,10,50,hgeVector(280, 340));
			Barriers.push_back(Enemy);
			d_barrier* Enemy1 = new d_barrier(hgeVector(370, 150), g_tEColors,50,10,hgeVector(435, 170));
			Barriers.push_back(Enemy1);
		}
	}
	if (lev==8)
	{
		if (lvlload==1)
		{
			Load();
			file.open("levels/3.lvl");	
			d_barrier* Enemy = new d_barrier(hgeVector(320, 150), g_tEColors,50,10,hgeVector(400, 170));
			Barriers.push_back(Enemy);
			d_barrier* Enemy1 = new d_barrier(hgeVector(500, 0), g_tEColors,50,10,hgeVector(620, 40));
			Barriers.push_back(Enemy1);
		}
		else
		{
			lifes=3;
			left_time=60;
			file.open("levels/3.lvl");	
			d_barrier* Enemy = new d_barrier(hgeVector(320, 150), g_tEColors,50,10,hgeVector(400, 170));
			Barriers.push_back(Enemy);
			d_barrier* Enemy1 = new d_barrier(hgeVector(500, 0), g_tEColors,50,10,hgeVector(620, 40));
			Barriers.push_back(Enemy1);
		}
	}
	if (lev==9)
	{
		if (lvlload==1)
		{
			Load();
			file.open("levels/4.lvl");
			d_barrier* Enemy = new d_barrier(hgeVector(200, 80), g_tEColors,10,50,hgeVector(280, 250));
			Barriers.push_back(Enemy);	
			d_barrier* Enemy1 = new d_barrier(hgeVector(200, 260), g_tEColors,20,60,hgeVector(280, 430));
			Barriers.push_back(Enemy1);	
			d_barrier* Enemy2 = new d_barrier(hgeVector(200, 440), g_tEColors,30,70,hgeVector(280, 610));
			Barriers.push_back(Enemy2);	
		}
		else
		{
			lifes=2;
			left_time=45;
			file.open("levels/4.lvl");
			d_barrier* Enemy = new d_barrier(hgeVector(200, 80), g_tEColors,10,50,hgeVector(280, 250));
			Barriers.push_back(Enemy);	
			d_barrier* Enemy1 = new d_barrier(hgeVector(200, 260), g_tEColors,20,60,hgeVector(280, 430));
			Barriers.push_back(Enemy1);	
			d_barrier* Enemy2 = new d_barrier(hgeVector(200, 440), g_tEColors,30,70,hgeVector(280, 610));
			Barriers.push_back(Enemy2);
		}
	}
	if (lev==10)
	{
		if (lvlload==1)
		{
			Load();
			file.open("levels/5.lvl");	
			d_barrier* Enemy = new d_barrier(hgeVector(200, 30), g_tEColors,10,50,hgeVector(280, 150));
			Barriers.push_back(Enemy);	
			d_barrier* Enemy1 = new d_barrier(hgeVector(200, 450), g_tEColors,50,10,hgeVector(300, 470));
			Barriers.push_back(Enemy1);
			d_barrier* Enemy2 = new d_barrier(hgeVector(590, 150), g_tEColors,50,10,hgeVector(690, 190));
			Barriers.push_back(Enemy2);
			d_barrier* Enemy3 = new d_barrier(hgeVector(590, 350), g_tEColors,80,30,hgeVector(700, 390));
			Barriers.push_back(Enemy3);
		}
		else
		{
			lifes=1;
			left_time=50;
			file.open("levels/5.lvl");	
			d_barrier* Enemy = new d_barrier(hgeVector(200, 30), g_tEColors,10,50,hgeVector(280, 150));
			Barriers.push_back(Enemy);	
			d_barrier* Enemy1 = new d_barrier(hgeVector(200, 450), g_tEColors,50,10,hgeVector(300, 470));
			Barriers.push_back(Enemy1);
			d_barrier* Enemy2 = new d_barrier(hgeVector(590, 150), g_tEColors,50,10,hgeVector(690, 190));
			Barriers.push_back(Enemy2);
			d_barrier* Enemy3 = new d_barrier(hgeVector(590, 350), g_tEColors,80,30,hgeVector(700, 390));
			Barriers.push_back(Enemy3);
		}
	}
	while (!file.eof())
	{
		int a,b,c;
		file>>a>>b>>c;
		Razvertka(a,b,c);
	}
	file.close();
	level=lev;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_USESOUND, true);
	hge->System_SetState(HGE_FPS, HGEFPS_VSYNC);
	hge->System_SetState(HGE_TITLE, "Game Tutorial");
	if(hge->System_Initiate())
	{
		//Background
		g_tBackground   = hge->Texture_Load("images/list2.jpg");
		g_sBackground   = new hgeSprite(g_tBackground, 0, 0, 1208, 777);
		g_vBGPosition   = hgeVector(0, 0);
		//Player
		Player1 = new c_player(hgeVector(10, 268), hgeVector(5,0));
		g_tEColors  = hge->Texture_Load("images/12.png");
		//Barriers
		init_level(1);		
		//Explosion
		g_tExplosion    = hge->Texture_Load("images/Explosion-Sprite-Sheet.png");
		//Sounds
		g_eBGMusic      = hge->Effect_Load("sounds/1.ogg");
		g_MenueBGMusic      = hge->Effect_Load("sounds/menu.mp3");
		hge->Effect_PlayEx(g_eBGMusic, 40, 0, 0, true);
		g_eExplosion    = hge->Effect_Load("sounds/21410__heigh-hoo__blow.aif");
		//Sprite Exit
		g_tExit			=hge->Texture_Load("images/Exit.png");
		g_sExit			=new hgeSprite(g_tExit,0,0,92,56);
		g_sExit->SetHotSpot(46,28);

		fnt=new hgeFont("font1.fnt");
		fnt->SetColor(ARGB(255,255,0,0));

		tcur=hge->Texture_Load("cursor.png");
		spr= new hgeSprite(tcur,0,0,32,32);
		// Create and initialize the GUI
		gui=new hgeGUI();

		gui->AddCtrl(new hgeGUIMenuItem(1,fnt,g_MenueBGMusic,400,120,0.0f,"new game"));
		gui->AddCtrl(new hgeGUIMenuItem(2,fnt,g_MenueBGMusic,400,200,0.1f,"save game"));
		gui->AddCtrl(new hgeGUIMenuItem(3,fnt,g_MenueBGMusic,400,280,0.2f,"load game"));
		gui->AddCtrl(new hgeGUIMenuItem(4,fnt,g_MenueBGMusic,400,360,0.3f,"continue game"));
		gui->AddCtrl(new hgeGUIMenuItem(5,fnt,g_MenueBGMusic,400,420,0.4f,"Exit"));

		gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
		gui->SetCursor(spr);
		gui->SetFocus(1);
		gui->Enter();


		hge->System_Start();


		hge->Texture_Free(g_tExplosion);
		hge->Effect_Free(g_eExplosion);
		delete g_sBackground;
		delete Player1;
		//delete barriers
		for(std::list<barrier*>::iterator i = Barriers.begin(); i != Barriers.end(); /**/)
		{
			delete (*i);
			i = Barriers.erase(i);
		}

		hge->Texture_Free(g_tEColors);
		for(std::list<explosion>::iterator i = Explosions.begin(); i != Explosions.end(); /**/)
		{
			delete (*i).Animation;
			i = Explosions.erase(i);
		}

	}
	else
	{
		MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}   
	hge->System_Shutdown();
	hge->Release();

	return 0;
}

void CreateExplosion(hgeVector Position)
{
	explosion exp;
	exp.Animation = new hgeAnimation(g_tExplosion,5,10,0,0,118,118);
	exp.Animation->SetHotSpot(78,60);
	exp.Animation->Play();
	exp.Position = Position;
	Explosions.push_back(exp);

	hge->Effect_PlayEx(g_eExplosion,100,0,hge->Random_Float(1,3));
}