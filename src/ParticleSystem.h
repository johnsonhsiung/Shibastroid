#pragma once
//  Kevin M. Smith - CS 134 SJSU

#include "ofMain.h"
#include "Particle.h"


//  Pure Virtual Function Class - must be subclassed to create new forces.
//
class ParticleForce {
protected:
public:
	bool applyOnce = false;
	bool applied = false; 
	virtual void updateForce(Particle *) = 0;
};

class ParticleSystem {
public:
	void add(const Particle &);
	void addForce(ParticleForce *);
	void remove(int);
	void update(float deltaTime);
	void setLifespan(float);
	void reset();
	int removeNear(const ofVec3f & point, float dist);
	void draw();
	void stopForces();
	vector<Particle> particles;
	vector<ParticleForce *> forces;
	bool isForcesActive = true;
};



// Some convenient built-in forces
//
class GravityForce: public ParticleForce {
	ofVec3f gravity;
	
public:
	GravityForce(const ofVec3f & gravity);
	void updateForce(Particle *);
};

class TurbulenceForce : public ParticleForce {
	ofVec3f tmin, tmax;
public:
	TurbulenceForce(const ofVec3f & min, const ofVec3f &max);
	void updateForce(Particle *);
};

class ImpulseRadialForce : public ParticleForce {
	float magnitude;
public:
	ImpulseRadialForce(float magnitude); 
	void updateForce(Particle *);
};

class ImpulseRingForce : public ParticleForce {
    float magnitude;
public:
    ImpulseRingForce(float magnitude);
    void updateForce(Particle *);
};

class ThrustForce : public ParticleForce {
	float magnitude;
	ofVec3f dir; 
	bool isAngular;

public:
	ThrustForce(const ofVec3f &dir, float magnitude, bool isAngular);
	void updateForce(Particle *);
};

class ImpulseForce : public ParticleForce {
	float forceMagnitude;
	ofVec3f normal;
public:
	ImpulseForce(float forceMagnitude, ofVec3f normal);
	void updateForce(Particle *);
};
