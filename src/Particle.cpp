#include "Particle.h"


Particle::Particle() {

	// initialize particle with some reasonable values first;
	//
	velocity.set(1.0, 1.0, 1.0);
	acceleration.set(0.0, 0.0, 0.0);
	pos.set(5.0, 5.0, 5.0);
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
void Particle::integrate() {

	
	// interval for this step
	//
	double dt = 1.0 / ofGetFrameRate();

	// update position based on velocity
	//
	//pos = pos + (velocity * dt);
	//lander.setPosition(position.x, position.y, position.z);
	
	//pos.set(position.x, position.y, position.z);
	ofVec3f position = (velocity *dt);
	pos += position;
	cout << position << "\n" << endl;
	cout << pos << "\n" << endl;
	cout << acceleration << "\n" << endl;



	// update acceleration with accumulated paritcles forces
	// remember :  (f = ma) OR (a = 1/m * f)
	//
	//ofVec3f accel = acceleration;    // start with any acceleration already on the particle
	//accel += (forces * (1.0 / mass));
	//velocity += accel * dt;

	// add a little damping for good measure
	//
	//velocity *= damping;

	// clear forces on particle (they get re-added each step)
	//
	//forces.set(0, 0, 0);
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


