#include <iostream>
#include <cstdlib>
#include <vector>
#include <list>
#include <math.h>

#include <SDL.h>
#include <SDL_rotozoom.h>
#include <SDL_framerate.h>
#include <SDL_gfxPrimitives.h>

#include "CSurface.h"
#include "CEvent.h"
#include "generic.hpp"
#include "Ship.hpp"
#include "Vector.hpp"
#include "UDP_Sock.hpp"
#include "NetEvent.hpp"

using namespace std;
	
class DirectionDrawer : public CEvent {
	private:
	SDL_Surface*	Surf_Display;
	SDL_Surface* 	Surf_BG;
	SDL_Surface* 	Surf_Explosion;
	int Running;
	int Pause;

	private:
	vector<Ship> ships;
	double explosion_frames;
	vector<Explosion> explosions;
    UDP_Sock Socket;
    Uint8 myId;
    int loop;

	public:
	DirectionDrawer();
    void ExecNetEvent(NetEvent&);
    bool PrepSDL(char*);
	void Init();
	void MainLoop();
	void Render();
	void OnEvent(SDL_Event*);
	void OnKeyDown(SDLKey, SDLMod, Uint16);
	void OnKeyUp(SDLKey, SDLMod, Uint16);
	void DrawVec(Vector, Cords,Uint8);
	void FindCollisions(void);
};

DirectionDrawer::DirectionDrawer() {
	Surf_Display	= NULL;
	Surf_BG		= NULL;
	Surf_Explosion	= NULL;
	Running		= 1;
	Pause		= -1;
}

void DirectionDrawer::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
    Cords cords;
    for (ship_iter a = ships.begin(); a != ships.end(); a++) 
        if (a->ship_id == myId)
    {
        NetEvent baba(true,myId, sym, *a);
        Socket.snd(baba);
    }
	switch(sym) { 
		case SDLK_ESCAPE: 
			Running = 0;
			break;

		case SDLK_p: 
			Pause *= -1;
			break;

		case SDLK_z:
			Init();
			break;

// player 1
		case SDLK_SPACE: 
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 1) {
					if (a->CanFire())
						ships.push_back(a->Fire());
					break;
				}
			break;

		case SDLK_RIGHT:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 1) {
					a->Rotate((-1 * a->rotate_speed));
					break;
				}
			break;

		case SDLK_LEFT:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 1) {
					a->Rotate(a->rotate_speed);
					break;
				}
			break;

		case SDLK_DOWN:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 1) {
					a->Accelerating(-1);
					break;
				}
			break;

		case SDLK_UP: 
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 1) {
					a->Accelerating(1);
					break;
				}
			break;



// player 2 		

		case SDLK_LCTRL: 
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 2) {
					if (a->CanFire())
						ships.push_back(a->Fire());
					break;
				}
			break;

		case SDLK_d:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 2) {
					a->Rotate((-1 * a->rotate_speed));
					break;
				}
			break;

		case SDLK_a:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 2) {
					a->Rotate(a->rotate_speed);
					break;
				}
			break;

		case SDLK_s:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 2) {
					a->Accelerating(-1);
					break;
				}
			break;

		case SDLK_w: 
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 2) {
					a->Accelerating(1);
					break;
				}
			break;

	}
}

void DirectionDrawer::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) {
    Cords cords;
    for (ship_iter a = ships.begin(); a != ships.end(); a++) 
        if (a->ship_id == myId)
        {
            NetEvent baba(false,myId, sym, *a);
            Socket.snd(baba);
        }
    
    for (ship_iter a = ships.begin(); a != ships.end(); a++) {
        Ship::keymap::const_iterator it = a->member_callback.find(sym);
        if (it != a->member_callback.end()) {
            cout << sym << endl;
            a->Call(sym,-1);
        }
    }
	switch(sym) { 
// player 1
		case SDLK_RIGHT:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 1) {
					a->Rotate((a->rotate_speed));
					break;
				}
			break;

		case SDLK_LEFT:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 1) {
					a->Rotate(-1 * a->rotate_speed);
					break;
				}
			break;

		case SDLK_DOWN:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 1) {
					a->Accelerating(1);
					break;
				}
			break;

//		case SDLK_UP: 
//			for (ship_iter a = ships.begin(); a != ships.end(); a++) {
//                a->Accelerating(-1);
//                break;
//           }
//			break;



// player 2
		case SDLK_d:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 2) {
					a->Rotate((a->rotate_speed));
					break;
				}
			break;

		case SDLK_a:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 2) {
					a->Rotate(-1 * a->rotate_speed);
					break;
				}
			break;

		case SDLK_s:
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 2) {
					a->Accelerating(1);
					break;
				}
			break;

		case SDLK_w: 
			for (ship_iter a = ships.begin(); a != ships.end(); a++) 
				if (a->ship_id == 2) {
					a->Accelerating(-1);
					break;
				}
			break;

	}
}

void DirectionDrawer::ExecNetEvent(NetEvent& event) {
    Uint8 id   = event.get_id();
    SDLKey sym = event.event_key();
    bool down  = event.is_down();
    ship_iter a;
    for (ship_iter iter = ships.begin(); iter != ships.end(); ++iter) 
        if (iter->ship_id == id) {
            a = iter;
        }
	switch(sym) { 
// player 1
		case SDLK_RIGHT:
            if (!down) {
                a->Rotate((a->rotate_speed));
            }
            else {
                a->Rotate((-1 * a->rotate_speed));
            }
			break;

		case SDLK_LEFT:
            if (!down) {
                a->Rotate(-1 * a->rotate_speed);
            }
            else {
                a->Rotate((a->rotate_speed));
            }
			break;

		case SDLK_DOWN:
            if (!down) {
                a->Accelerating(1);
            }
            else {
                a->Accelerating(-1);
            }
			break;

		case SDLK_UP: 
            if (!down) {
                a->Accelerating(-1);
            }
            else {
                a->Accelerating(1);
            }
			break;

		case SDLK_SPACE: 
                cout << "preparing to fire" << endl;
                if (a->CanFire()) {
                    cout << "firing!" << endl;
                    //ships.push_back(a->Fire());
                    Ship firebaba = a->Fire();
                    cout << firebaba.fire_damage;
                    ships.push_back(firebaba);
                    cout << "succesfully pushed back" << endl;
                }
			break;

// player 2
		case SDLK_d:
            if (!down) {
                a->Rotate((a->rotate_speed));
            }
            else {
                a->Rotate((-1 * a->rotate_speed));
            }
			break;

		case SDLK_a:
            if (!down) {
                a->Rotate(-1 * a->rotate_speed);
            }
            else {
                a->Rotate((a->rotate_speed));
            }
			break;

		case SDLK_s:
            if (!down) {
                a->Accelerating(1);
            }
            else {
                a->Accelerating(-1);
            }
			break;

		case SDLK_w: 
            if (!down) {
                a->Accelerating(-1);
            }
            else {
                a->Accelerating(1);
            }
			break;

		case SDLK_LCTRL: 
                if (a->CanFire())
                    ships.push_back(a->Fire());
			break;
	}
}

void DirectionDrawer::Init() { 
	vector<Ship> empty_ship_vec;
	ships = empty_ship_vec;
	explosion_frames = 18;
    loop = 0;

	Ship ship;
	ship.ship_id = 1;
	ship.ShipCords.x = 500;
	ship.ShipCords.y = 500;
	ship.ShipCords.degree = 0;
	ship.acc_speed = 2;
	ship.max_speed = 10;
	ship.rotate_speed = 10;
	ship.fire_cost = 10;
	ship.hit_points = 100;
	ship.LoadSurface("./gfx/Ship1.png",ship.ship_surf);
	ship.LoadSurface("./gfx/fire.png",ship.missile_surf);
    ship.member_callback[SDLK_UP] = &Ship::Accelerating;

	ships.push_back(ship);
// seconds ship 
	Ship ship2;
	ship2.ship_id = 2;
	ship2.ShipCords.x = 500;
	ship2.ShipCords.y = 300;
	ship2.ShipCords.degree = 0;
	ship2.acc_speed = 2;
	ship2.max_speed = 10;
	ship2.rotate_speed = 10;
	ship2.fire_cost = 10;
	ship2.hit_points = 100;
	ship2.LoadSurface("./gfx/Ship2.png",ship2.ship_surf);
	ship2.LoadSurface("./gfx/fire.png",ship2.missile_surf);

	ships.push_back(ship2);
}

bool DirectionDrawer::PrepSDL(char* server_addr) { 
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return false;

    if ( server_addr == NULL ) {
        cout << "initializing server ..\n";
        if ( !Socket.create_server() )
            return false;
        myId = 1;
        SDL_Delay(2000);
    }
    else {
        cout << "initializing client ..\n";
        if ( !Socket.create_client(server_addr) )
            return false;
        myId = 2;
    }
        

	if ((Surf_Display = SDL_SetVideoMode(BOARD_X,BOARD_Y,32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
		return false;

	Surf_BG =  CSurface::OnLoad("./gfx/bg.png");
	Surf_Explosion = CSurface::OnLoad("./gfx/explosion.png");

	return true; 
}

void DirectionDrawer::OnEvent(SDL_Event* Event) {
	CEvent::OnEvent(Event);
}

bool Collides(Cords a_cords,SDL_Surface* a_surf, Cords b_cords, SDL_Surface* b_surf) { 
	double ax1 = a_cords.x;
	double ay1 = a_cords.y;
	double ax2 = ax1 + a_surf->w;
	double ay2 = ay1 + a_surf->h;

	double bx1 = b_cords.x;
	double by1 = b_cords.y;
	double bx2 = bx1 + b_surf->w;
	double by2 = by1 + b_surf->h;


	if (ax1 >= bx1 && ax1 <= bx2 && ay1 >= by1 && ay1 <= by2)
		return true;

	if (ax2 >= bx1 && ax2 <= bx2 && ay2 >= by1 && ay2 <= by2) 
		return true;

	return false;
}

void DirectionDrawer::FindCollisions() { 
	// i guess this is a lame collision detection algorithm
	// but i just want to get it working... :)


	// go over all objects
	for (ship_iter a = ships.begin(); a != ships.end(); a++) {
		// and go over all objects again
		for (ship_iter b = ships.begin(); b != ships.end(); b++) {
			// if it isn't the same object, check for collision
			if (a != b) {
				if (Collides(
					a->ShipCords, 
					a->GetSurface((vsurf_sz)a->ShipCords.degree), 
					b->ShipCords, 
					b->GetSurface((vsurf_sz)a->ShipCords.degree)))
				{
					// a is a missile hitting enemy ship b
					if (a->fire_damage > 0 && b->hit_points > 0 and a->missile_id != b->ship_id) {
						// create explosion
						Explosion explosion;
						explosion.Explosion_Cords = b->ShipCords;
						explosion.frames = explosion_frames;
						explosions.push_back(explosion);
		
						// lower hitpoints
						b->hit_points -= a->fire_damage;
						a->hit_points--;
					}
				}
			
			}
		}
	}
}

void DirectionDrawer::MainLoop() {

	while (Running == 1) {
        ++loop;

		SDL_Event event;
        
		while (SDL_PollEvent(&event)) {
			OnEvent(&event);
        }

        NetEvent nevent;
        if (loop == 10) {
            for (ship_iter si = ships.begin(); si != ships.end(); ++si)
                for (ship_iter si = ships.begin(); si != ships.end(); ++si)
                    if (si->ship_id == myId ) {
                        //cout << "sending positions to remote host" << endl;
                        nevent.event_key() = (SDLKey)0;
                        nevent.get_id()    = myId;
                        nevent.is_down()   = false;
                        nevent.get_ship()  = *si;
                        Socket.snd(nevent);
                    }
            loop = 0;
        }
        while (Socket.rcv(nevent)) {
            for (ship_iter si = ships.begin(); si != ships.end(); ++si)
                if (si->ship_id == nevent.get_id() ) {
                    cout << "recieved positions to remote host" << endl;
                    si->ShipCords = nevent.get_ship().ShipCords;
                    si->accelerating = nevent.get_ship().accelerating;
                    si->rotate = nevent.get_ship().rotate;
                    if (nevent.event_key() != 0) ExecNetEvent(nevent);
                    break;
                }
        }

                   //cout << "im still alive!" << endl;
		if (Pause < 0) { 
			for (ship_iter sp = ships.begin(); sp != ships.end(); sp++) 
				sp->NextShip();

			FindCollisions();


			Render();

			SDL_Delay(1000 / FRAME_RATE);
		}
		
		// reseting vectors	
		for (ship_iter sp = ships.begin(); sp != ships.end(); sp++) {
			sp->ClearVectors();

			// deleting expired missiles
			if (sp->max_distance != 0 && sp->distance > sp->max_distance) {
				ships.erase(sp);
				sp--;
			}

		}

		for (ship_iter sp = ships.begin(); sp != ships.end(); sp++)
			if (sp->hit_points <= 0)
				ships.erase(sp--);


	}
}

void DirectionDrawer::DrawVec(Vector v, Cords c,Uint8 color) {
	double x = c.x + ( 100 * v.force * v.X());
	double y = c.y - ( 100 * v.force * v.Y());

	cout << "cx: " << c.x << " cy: " << c.y << " x: " << x << " y: " << y  << endl;
	lineRGBA(Surf_Display,c.x,c.y,x,y,0,color  / 2,color  ,255);
}

void DirectionDrawer::Render() {
	//background
	CSurface::OnDraw(Surf_Display,Surf_BG, 0, 0);


	if (DRAW_VECTORS == 1) {
		for (ship_iter sp = ships.begin(); sp != ships.end(); sp++) 
			DrawVec(sp->ship_vec,sp->ShipCords,100);

		// draw ship_vec
		Uint8 color = 200;
		for (ship_iter sp = ships.begin(); sp != ships.end(); sp++) {
			for (vec_iter i = sp->ship_vectors.begin(); i != sp->ship_vectors.end(); i++) {
				DrawVec(*i,sp->ShipCords,color);
				color += 30;
			}
		}
	}
	
	if (DRAW_TRACE == 1) { 
		// movement traces 
		for (ship_iter sp = ships.begin(); sp != ships.end(); sp++) 
			for (list<Cords>::const_iterator p = sp->Last_ShipCords.begin(); p != sp->Last_ShipCords.end(); p++) { 
				filledCircleRGBA(Surf_Display,p->x,p->y,4,255,255,0,255);
			}
	}

	// ship itself
	for (ship_iter sp = ships.begin(); sp != ships.end(); sp++) {
		SDL_Surface* tmp_surf = sp->GetSurface((vsurf_sz)sp->ShipCords.degree);
		CSurface::OnDraw(Surf_Display,tmp_surf, (sp->ShipCords.x - (tmp_surf->w / 2)), (sp->ShipCords.y - (tmp_surf->h /2 )));
	}


	// draw explosions
	for (exp_iter ex = explosions.begin(); ex != explosions.end(); ex++) {
		CSurface::OnDraw(Surf_Display,Surf_Explosion, ( ex->Explosion_Cords.x - 32 / 2), (ex->Explosion_Cords.y - 42 / 2),(ex->frame * 34),((int)ex->frame  / 6) * 42,34,42);
		if (ex->frame++ > ex->frames) {
			explosions.erase(ex);
			ex--;
		}
	}	

	
	SDL_Flip(Surf_Display);
}

int main(int argc, char* argv[]) {
	DirectionDrawer Board;

	if (!Board.PrepSDL(argv[1]))
		return 1;

	Board.Init();
	Board.MainLoop();


	return 0;
}
