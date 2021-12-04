#include "Particle.h"


Particle::Particle() {

	// initialize particle with some reasonable values first;
	//
	velocity.set(0.0, 0.0, 0.0);
	acceleration.set(0.0, 0.0, 0.0);
	pos.set(0.0, 0.0, 0.0);
	forces.set(0.0, 0.0, 0.0);
	lifespan = 5;
	birthtime = 0;
	radius = .1;
	damping = .99;
	mass = 1;
	color = ofColor::aquamarine;
}

void Particle::draw() {
//	ofSetColor(color);
	if (bLanderLoaded) {
		lander.drawFaces();
	}
	
}

// write your own integrator here.. (hint: it's only 3 lines of code)
//
void Particle::integrate(float deltaTime)
{

	// update position based on velocity
	//
	//pos = pos + (velocity * dt);
	//lander.setPosition(position.x, position.y, position.z);
	
	//pos.set(position.x, position.y, position.z);
	pos += (velocity * deltaTime);
	lander.setPosition(pos.x,pos.y,pos.z);


	// update acceleration with accumulated paritcles forces
	// remember :  (f = ma) OR (a = 1/m * f)
	//
	ofVec3f accel = acceleration;    // start with any acceleration already on the particle
	accel += (forces * (1.0 / mass));
	velocity += accel * deltaTime;

	// add a little damping for good measure
	//
	velocity *= damping;

	// clear forces on particle (they get re-added each step)
	//
	forces.set(0, 0, 0);
	cout << pos << "\n" << endl;
	cout << velocity << "\n" << endl;

}

bool Particle::load(string path) {
		//load lander 
	if (lander.loadModel(path)) {
		bLanderLoaded = true; 
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		return true;
	}
	else {
		return false; 
	}

}

//  return age in seconds
//
float Particle::age() {
	return (ofGetElapsedTimeMillis() - birthtime)/1000.0;
}


