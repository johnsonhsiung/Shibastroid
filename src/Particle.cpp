#include "Particle.h"


Particle::Particle() {

	// initialize particle with some reasonable values first;
	//
	velocity.set(0.0, 0.0, 0.0);
	acceleration.set(0.0, 0.0, 0.0);
	pos.set(0.0, 20.0, 0.0);
	forces.set(0.0, 0.0, 0.0);
	angularForces = 0.0;
	damping = .99;
	mass = 1;
	rotation = 0.0;
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

	rotation += (angVelocity * deltaTime);
	
	float angAccel = angAcceleration;
	angAccel += (angularForces * (1.0 / mass));
	angVelocity += angAccel * deltaTime;
	angVelocity *= damping; 
	// clear forces on particle (they get re-added each step)
	//
	forces.set(0, 0, 0);
	angularForces = 0;
	
	


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

ofVec3f Particle::heading() {
	return glm::normalize(glm::vec3(sin(glm::radians(rotation + 180)), 0, cos(glm::radians(rotation + 180))));
}


