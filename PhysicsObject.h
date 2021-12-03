#pragma once

#include "ofMain.h"

class Force {
protected:
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(Particle *) = 0;
};
class PhysicsObject {
public: 
	PhysicsObject(ofxAssimpModelLoader);
	ofVec3f position;
	ofVec3f velocity;
	ofVec3f acceleration;
	ofVec3f forces;
	float damping;
	float masss; 
	void integrate();
	void draw();
	void addForce(Force *);
	void update();
	vector<Force *> forces; 
};

class GravityForce : public Force {
	ofVec3f gravity;
public:
	GravityForce(const ofVec3f & gravity);
	void updateForce(PhysicsObject *);
};