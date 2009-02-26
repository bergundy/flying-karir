#ifndef _VECTOR_HPP
#define _VECTOR_HPP

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class Vector {
    friend class boost::serialization::access;

    private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & force;
        ar & degree;
    }

	public:
	double force;
	double degree;

	public:
	Vector();
	double X();	
	double Y();
	void AddVector(Vector);
	void LimitForce(double max_force);
};

double rad(double);
double deg(double);

#endif
