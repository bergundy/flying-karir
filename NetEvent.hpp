#ifndef NETEVENT_HPP
#define NETEVENT_HPP
#include <SDL.h>
#include "generic.hpp"
#include "Ship.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class NetEvent
{
private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & id;
        ar & event; 
        ar & down;
        ar & ship;
    }

public:
    NetEvent(bool d, Uint8 i, SDLKey key, Ship c) :id(i),event(key),down(d),ship(c) {}
    NetEvent() {}

    Uint8  id;
    bool   down;
    SDLKey event;
    Ship   ship;
};

#endif
