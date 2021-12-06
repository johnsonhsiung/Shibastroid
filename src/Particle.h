#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"

class ParticleForceField;

class Particle {
public:
	Particle();

	ofVec3f pos;
	ofVec3f velocity;
	ofVec3f acceleration;
	ofVec3f forces;
	float angularForces; 
	float angVelocity;
	float angAcceleration; 
	float	damping;
	float   mass;
	float rotation; 
	void    integrate(float deltaTime);
	void    draw();
	bool load(string);
	ofColor color;
	ofxAssimpModelLoader lander; 
	bool bLanderLoaded; 
	ofVec3f heading(); 
};


