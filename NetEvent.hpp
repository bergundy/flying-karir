#ifndef NETEVENT_HPP
#define NETEVENT_HPP
#include <SDL.h>
#include "generic.hpp"
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
        ar & cords;
    }
    Uint8  id;
    bool   down;
    SDLKey event;
    Cords  cords;

public:
    NetEvent(bool d, Uint8 i, SDLKey key, Cords c) :id(i),event(key),down(d),cords(c) {}
    NetEvent() {}
    SDLKey event_key() const { return event; }
    Uint8 get_id() const { return id; }
    bool is_down() const { return down; }
    Cords get_cords() const { return cords; }
};

#endif
