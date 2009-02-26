#ifndef _SHIP_HPP
#define _SHIP_HPP

#include <vector>
#include <list>
#include <map>
#include <SDL.h>
#include <SDL_rotozoom.h>
#include "Vector.hpp"
#include "generic.hpp"
#include "CSurface.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class Ship {
    friend class boost::serialization::access;

	public:
	Cords ShipCords;
	std::list<Cords> Last_ShipCords;
	double rotate;
	double accelerating;

	private:

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & ship_vec;
        ar & rotate;
        ar & accelerating; 
        ar & ShipCords;
    }


	public:
	Vector ship_vec;
	std::vector<Vector> ship_vectors;
	std::vector<SDL_Surface*> ship_surf;
	std::vector<SDL_Surface*> missile_surf;

	int	ship_id;
	int	missile_id;
	double rotate_speed;
	double max_speed;
	double acc_speed; 
	double max_distance;
	size_t distance;

	double fire_wait;
	double fire_cost;
	double fire_damage;

	double hit_points;

    typedef void (Ship::*func_ptr)(double);
    typedef std::map<SDLKey, func_ptr> keymap;
    keymap member_callback;

	public:
    void Call(SDLKey, double);
	Ship();
    SDL_Surface* GetSurface(vsurf_sz);
	void NextShip();
	void Rotate(double);
	void Rotate();
	void LoadSurface(char*, std::vector<SDL_Surface*>&);
	void Accelerate();
	void Accelerating(double);
	void PushCurrentShipCords();
	void PushNewVector(Vector);
	void ClearVectors();
	bool CanFire();
	Ship Fire();
};

typedef std::vector<Ship>::iterator ship_iter;

#endif
