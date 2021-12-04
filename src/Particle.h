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
	float	damping;
	float   mass;
	float   lifespan;
	float   radius;
	float   birthtime;
	float rotation = 0.0; 
	void    integrate(float deltaTime);
	void    draw();
	float   age();        // sec
	bool load(string);
	ofColor color;
	ofxAssimpModelLoader lander; 
	bool bLanderLoaded; 
	ofVec3f heading(); 
};


