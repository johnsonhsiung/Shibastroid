
// Kevin M.Smith - CS 134 SJSU

#include "ParticleSystem.h"

void ParticleSystem::add(const Particle &p) {
	particles.push_back(p);
}

void ParticleSystem::addForce(ParticleForce *f) {
	forces.push_back(f);
}

void ParticleSystem::remove(int i) {
	particles.erase(particles.begin() + i);
}

void ParticleSystem::reset() {
	for (int i = 0; i < forces.size(); i++) {
		forces[i]->applied = false;
	}
}

void ParticleSystem::stopForces() {
	isForcesActive = false; 
}

void ParticleSystem::update(float deltaTime) {
	// check if empty and just return
	if (particles.size() == 0 || !isForcesActive) return;


	// check which particles have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, we need to use an iterator.
	//

	// update forces on all particles first 
	//
	for (int i = 0; i < particles.size(); i++) {
		for (int k = 0; k < forces.size(); k++) {
			if (!forces[k]->applied)
				forces[k]->updateForce( &particles[i] );
		}
	}

	// update all forces only applied once to "applied"
	// so they are not applied again.
	//
	for (int i = 0; i < forces.size(); i++) {
		if (forces[i]->applyOnce)
			forces[i]->applied = true;
	}

	// integrate all the particles in the store
	//
	for (int i = 0; i < particles.size(); i++)
		particles[i].integrate(deltaTime);

}

// remove all particlies within "dist" of point (not implemented as yet)
//
int ParticleSystem::removeNear(const ofVec3f & point, float dist) { return 0; }

//  draw the particle cloud
//
void ParticleSystem::draw() {
	for (int i = 0; i < particles.size(); i++) {
		particles[i].draw();
	}
}


// Gravity Force Field 
//
GravityForce::GravityForce(const ofVec3f &g) {
	gravity = g;
}

void GravityForce::updateForce(Particle * particle) {
	//
	// f = mg
	//
	particle->forces += gravity * particle->mass;
}

// Turbulence Force Field 
//
TurbulenceForce::TurbulenceForce(const ofVec3f &min, const ofVec3f &max) {
	tmin = min;
	tmax = max;
}

void TurbulenceForce::updateForce(Particle * particle) {
	//
	// We are going to add a little "noise" to a particles
	// forces to achieve a more natual look to the motion
	//
	particle->forces.x += ofRandom(tmin.x, tmax.x);
	particle->forces.y += ofRandom(tmin.y, tmax.y);
	particle->forces.z += ofRandom(tmin.z, tmax.z);
}

// Impulse Radial Force - this is a "one shot" force that
// eminates radially outward in random directions.
//
ImpulseRadialForce::ImpulseRadialForce(float magnitude) {
	this->magnitude = magnitude;
	applyOnce = true;
}

void ImpulseRadialForce::updateForce(Particle * particle) {

	// we basically create a random direction for each particle
	// the force is only added once after it is triggered.
	//
	ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
	particle->forces += dir.getNormalized() * magnitude;
}

ImpulseRingForce::ImpulseRingForce(float magnitude){
    this->magnitude = magnitude;
    applyOnce = true;
}
void ImpulseRingForce::updateForce(Particle * particle) {

    // we basically create a random direction for each particle
    // the force is only added once after it is triggered.
    //
    ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(-0.05,0.05), ofRandom(-1, 1));
    particle->forces += dir.getNormalized() * magnitude;
}


ThrustForce::ThrustForce(const ofVec3f &dir, float magnitude, bool isAngular) {
	applyOnce = true;
	this->magnitude = magnitude;
	this->dir = dir; 
	this->isAngular = isAngular; 
}
void ThrustForce::updateForce(Particle * particle) {
	if (isAngular) 	particle->angularForces += magnitude;

	else
	{
		particle->forces += (dir.getNormalized() * magnitude);
	}
}

ImpulseForce::ImpulseForce(float restitution, ofVec3f velocity, ofVec3f normal) {
	applyOnce = true;
	this->resitituion = resitituion;
	this->velocity = velocity;
	this->normal = normal;
}
void ImpulseForce::updateForce(Particle * particle) {


	particle->forces += (resitituion + 1) * (-velocity.dot(normal) * normal); 
}




