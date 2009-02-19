#include <iostream>
#include <cstdlib>
#include <math.h>
#include <SDL.h>
#include "CSurface.h"
#include "CEvent.h"
#include <SDL_rotozoom.h>
#include <vector>

#define PI 3.14159265

using namespace std;
typedef vector<SDL_Surface*>::size_type vsurf_sz;

class Direction {
	public:
	double x_factor;
	double y_factor;

	public:
	void CalcFactor(double);
};

void Direction::CalcFactor(double degree) {
	static const double arcus = 2. * PI / 360; 
	
	x_factor = cos( degree * arcus );
	y_factor = sin( degree * arcus );

	//cout << x_factor << "\t" << y_factor << endl;
}
	
// this is classic oo right there
class Point {

	public:
	double x;
	double y;
	double last_x;
	double last_y;
	double space_max_x;
	double space_max_y;

	private:
	double rotate;
	double accelerating;
	vector<SDL_Surface*> ship_surf;


	public:
	double degree;
	double degree_vector;
	double speed;
	double rotate_speed;
	double max_speed;

	public:
	Point();
	void NextPoint();
	void Rotate(double);
	void Rotate();
	void LoadSurface(char*);
  SDL_Surface* GetSurface();
	void Accelerate();
	void Decelerate();
	void Accelerating(double);
};

Point::Point() : accelerating(0), max_speed(0), degree(0), degree_vector(0), rotate(0),  rotate_speed(0), speed(0) {}

void Point::Accelerating(double i) {
	accelerating += i;
}

void Point::Accelerate() { 
//	cout << "degree: " << degree << " degree_vector: " << degree_vector << " delta: " << (degree - degree_vector) << endl;

	//cout << "DEG-DEGVEC: " << (degree - degree_vector) << endl;
	//cout << "degree: " << degree << " degree_vector: " << degree_vector << endl;


	// if direction changes we summerizs the vectors
	double new_speed = 1.2;

	double orig_x = speed * cos((degree_vector * PI / 180));
	double orig_y = speed * sin((degree_vector * PI / 180));

	double new_x = new_speed * cos((degree * PI / 180));
	double new_y = new_speed * sin((degree * PI / 180));

	double sum_x = orig_x + new_x;
	double sum_y = orig_y + new_y;

	double real_new_speed = sqrt(sum_x * sum_x + sum_y * sum_y);
	
//	cout << "degree_vector: " << degree_vector << endl;
	double new_degree = atan(sum_y / sum_x) * 180 / PI;

//	if (sum_x > 0 &&

//	cout << "new_degree: " << new_degree << endl;
	cout << "degree: " << new_degree << " x: " << sum_x << " y: " << sum_y << endl;
	//new_degree = degree;

	if (sum_x < 0 && sum_y >0)
		new_degree += 180;
	else if (sum_x < 0 && sum_y < 0)
		new_degree += 180;
	else if (sum_x > 0 && sum_y < 0) 
		new_degree += 360; 

//	cout << "HEH: " << (degree - new_degree) << endl;
	
	
/*
	if ((degree - new_degree) > 200) {
		new_degree = degree;
	}

	if ((degree - new_degree) < -200) {
		new_degree = degree;
	}
*/
//	cout << "orig_x: " << orig_x << " orig_y: " << orig_y << endl;
//	cout << "new_x: " << new_x << " new_y: " << new_y << endl;
//	cout << "sum_x: " << sum_x << " sum_y: " << sum_y << endl;
//	cout << "degree: " << degree << endl;
//	cout << "new_degree: " << new_degree << endl;

	degree_vector = new_degree;
	if (speed < max_speed) 
		speed = speed + real_new_speed;

//	cout << "speed: " << speed << endl;
}

void Point::Decelerate() { 
//	degree_vector = degree - 180;
	if (speed > 0) 
		speed = speed - 0.1;

	cout << "speed: " << speed << endl;
}

void Point::Rotate(double i) { 
	rotate += i;
}

void Point::LoadSurface(char* filename) {

	SDL_Surface* origin =  CSurface::OnLoad(filename);

	for (vsurf_sz i = 0; i <= 360; i++) {
		SDL_Surface* tmp_surf = rotozoomSurface(origin, i, 1.0, 0);
		ship_surf.push_back(tmp_surf);	
	}
	
}

SDL_Surface* Point::GetSurface() {
	return ship_surf[(vsurf_sz)degree];
}

void Point::Rotate() { 
	degree += rotate;
	//cout << degree << endl;
	
	if (degree < 0) 
		degree = 360 - rotate_speed;
	else if (degree > 360) 
		degree = rotate_speed;
	//cout << degree << endl;
}

void Point::NextPoint() {
	Direction d;
	Rotate();

	//cout << "accelerating: " << accelerating << endl;
	
	if (accelerating > 0) 
		Accelerate();
	else if (accelerating < 0)
		Decelerate();

	//cout << "rotate: " << rotate << endl;
	//cout << "degree_vector: " << degree_vector << endl;
	
	d.CalcFactor(degree_vector);
	
	last_x = x;
	last_y = y;

	x -= speed * d.y_factor;
	y -= speed * d.x_factor;

	if (x > space_max_x) 
		x = 0;
	else if (x < 0) 
		x = space_max_x;

	if (y > space_max_y) 
		y = 0;
	else if (y < 0) 
		y = space_max_y;

}



class DirectionDrawer : public CEvent {
	private:
	SDL_Surface*	Surf_Display;
	SDL_Surface* 	Surf_BG;
	SDL_Surface*	Surf_Ship1;
	SDL_Surface*	Surf_Ship2;
	int Running;

	private:
	Point point;
	Point point2;

	public:
	DirectionDrawer();
	bool PrepSDL();
	void Init();
	void MainLoop();
	void Render();
	void OnEvent(SDL_Event*);
	void OnKeyDown(SDLKey, SDLMod, Uint16);
	void OnKeyUp(SDLKey, SDLMod, Uint16);
};

DirectionDrawer::DirectionDrawer() {
	Surf_Display = NULL;
	Surf_Ship1 	 = NULL;
	Surf_Ship2 	 = NULL;
	Surf_BG    	 = NULL;
	Running = 1;
}

void DirectionDrawer::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
	switch(sym) { 
		case SDLK_ESCAPE: 
			Running = 0;
			break;

		case SDLK_RIGHT:
			point.Rotate((-1 * point.rotate_speed));
			break;

		case SDLK_LEFT:
			point.Rotate(point.rotate_speed);
			break;

		case SDLK_DOWN:
			point.Accelerating(-1);
			break;

		case SDLK_UP:
			point.Accelerating(1);
			
			break;

// player 2 		
		case SDLK_d:
			point2.Rotate((-1 * point2.rotate_speed));
			break;

		case SDLK_a:
			point2.Rotate(point2.rotate_speed);
			break;

	}
}

void DirectionDrawer::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) {
	switch(sym) { 
		case SDLK_RIGHT:
			point.Rotate(point.rotate_speed);
			break;

		case SDLK_LEFT:
			point.Rotate((-1 * point.rotate_speed));
			break;

		case SDLK_DOWN:
			point.Accelerating(1);
			break;

		case SDLK_UP:
			point.Accelerating(-1);
			break;
// player 2
		case SDLK_d:
			point2.Rotate(point2.rotate_speed);
			break;

		case SDLK_a:
			point2.Rotate((-1 * point2.rotate_speed));
			break;
	}
}

void DirectionDrawer::Init() { 
	point.x = 500;
	point.y = 500;
	point.speed = 0.1;
	point.max_speed = 6;
	point.rotate_speed = 4.5;
	point.space_max_x = 800;
	point.space_max_y = 600;
	point.LoadSurface("./gfx/Ship1.png");


	point2.x = 100;
	point2.y = 100;
	point2.speed = 0.1;
	point2.max_speed = 6;
	point2.rotate_speed = 4.5;
	point2.space_max_x = 800;
	point2.space_max_y = 600;
	point2.LoadSurface("./gfx/Ship2.png");

}

bool DirectionDrawer::PrepSDL() { 
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return false;

	if ((Surf_Display = SDL_SetVideoMode(800,600,32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
		return false;

	Surf_BG =  CSurface::OnLoad("./gfx/bg.png");

	return true; 
}

void DirectionDrawer::OnEvent(SDL_Event* Event) {
	CEvent::OnEvent(Event);
}

void DirectionDrawer::MainLoop() {
	int i = 0;

	while (Running == 1) {

		SDL_Event event;

		while (SDL_PollEvent(&event))
			OnEvent(&event);

		point.NextPoint();
		point2.NextPoint();

		//cout << point.x << "\t" << point.y << endl;
		Render();
		//SDL_Delay(500);
	}
}

void DirectionDrawer::Render() {
	//Draw_Circle(Surf_Display,point.x,point.y,(rand() % 2), (rand()));
	//Draw_Pixel(Surf_Display,point.x,point.y,(rand()));
	//Draw_Line(Surf_Display,point.last_x,point.last_y,point.x,point.y,(rand()));
	//SDL_FillRect(Surf_Display,NULL,0);
//	CSurface::OnDraw(Surf_Display,Surf_Ship1,point.x,point.y,((int)point.degree * 45),0,45,45);
	//CSurface::OnDraw(Surf_Display,Surf_Point,point.x,point.y, 0, 0, 30, 30);

	//background
	CSurface::OnDraw(Surf_Display,Surf_BG, 0, 0);

	//first ship
	SDL_Surface* tmp_surf = point.GetSurface();
	CSurface::OnDraw(Surf_Display,tmp_surf, (point.x - (tmp_surf->w / 2)), (point.y - (tmp_surf->h /2 )));

	SDL_Surface* tmp2_surf = point2.GetSurface();
	CSurface::OnDraw(Surf_Display,tmp2_surf, (point2.x - (tmp2_surf->w / 2)), (point2.y - (tmp2_surf->h /2 )));

	// sdl stuff
	SDL_Flip(Surf_Display);
}

int main(int argc, char* argv[]) {
	DirectionDrawer Board;

	if (!Board.PrepSDL())
		return 1;

	Board.Init();
	Board.MainLoop();


	return 0;
}
