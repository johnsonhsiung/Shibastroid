
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   Alex Wolski
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"
#include <glm/gtx/intersect.hpp>



void ofApp::setup() {

	//set up easy cam 
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.enableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();
	theCam = &cam;

	//set up tracking cam
	trackingCam.setPosition(100, 100, 100); 

	//set up onboard cam 
	onboardCam.setNearClip(0.1);

	// setup lighting, can change values once we finalize with sliders
	//
	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(1, 1);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.setSpecularColor(ofFloatColor(1, 1, 1));

	keyLight.rotate(45, ofVec3f(0, 1, 0));
	keyLight.rotate(-45, ofVec3f(1, 0, 0));
	keyLight.setPosition(5, 5, 5);

	fillLight.setup();
	fillLight.enable();

	fillLight.setScale(.05);


	fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
	fillLight.rotate(-10, ofVec3f(1, 0, 0));
	fillLight.rotate(-45, ofVec3f(0, 1, 0));
	fillLight.setPosition(-5, 5, 5);
	fillLight.setAreaLight(1, 1);

	rimLight.setup();
	rimLight.enable();

	rimLight.setScale(.05);
	

	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
	rimLight.rotate(180, ofVec3f(0, 1, 0));
	rimLight.setPosition(0, 5, -7);
	rimLight.setAreaLight(1, 1);


	mars.loadModel("geo/Terrain.obj");
	mars.setScaleNormalization(false);

	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	gui.add(gravity.setup("Gravity", 5, 0, 20));
	gui.add(thrust.setup("Thrust", 50, 1, 150));
	gui.add(intersectDeltaTime.setup("Time between checks for intersects", 0.5, 0.2, 1000000));
	gui.add(keyLightPosition.setup("Keylight pos", ofVec3f(-1000, -102, 5), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(fillLightPosition.setup("Filllight pos", ofVec3f(1000, -143, 5), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(rimLightPosition.setup("rimLight pos", ofVec3f(143, 204, 755), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));


	gui.add(restitution.setup("Bounciness", 0.3, 0.0, 1.0));



	bDrawGui = false;

	//  Create Octree for testing.
	
	octree.create(mars.getMesh(0), 20);
	//load shiba 
	if (landerParticle.load("geo/Shib Ship.obj")) {
		bboxList.clear();
		for (int i = 0; i < landerParticle.lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(landerParticle.lander.getMesh(i)));
		}
		bLanderLoaded = true;
		sys.add(landerParticle); 
	}
	else {
		ofLogFatalError("Can't load shib ship.");
		ofExit();
	}

	//add forces 
	gravityForce = new GravityForce(ofVec3f(0, -5, 0));
	sys.addForce(gravityForce);

	turbulenceForce = new TurbulenceForce(ofVec3f(-6.0, -6.0, -6.0), ofVec3f(6.0, 6.0, 6.0));
	sys.addForce(turbulenceForce);
	timeLastFrame = ofGetElapsedTimef();
}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	float deltaTime = ofGetElapsedTimef() - timeLastFrame;
	//update all the forces on our shiba 
	sys.update(deltaTime);

	timeLastFrame = ofGetElapsedTimef();
	Particle currentLander = sys.particles[0];

	//draw the altitude, laggy if checks every frame, so updates based on a slider 
	float deltaLanderRayIntersectTime = ofGetElapsedTimef() - timeLastFrameIntersect; 
	if (bDrawAltitude && (deltaLanderRayIntersectTime > intersectDeltaTime)) {
		ofVec3f landerIntersectPoint; 
		landerRayIntersectOctree(landerIntersectPoint);
		landerAltitude = currentLander.pos.y - landerIntersectPoint.y;
		timeLastFrameIntersect = ofGetElapsedTimef(); 
	}

	//update cameras
	if (theCam == &trackingCam)
		trackingCam.lookAt(currentLander.pos);
	else if (theCam == &onboardCam)
	{
		ofVec3f currentLanderPos = currentLander.pos;
		onboardCam.setPosition(currentLanderPos);
		ofVec3f downwardSlant(0, -1, 0);
		onboardCam.lookAt(currentLanderPos + currentLander.heading() +downwardSlant);
	}
	else if (theCam == &shibaCam)
	{
		ofVec3f currentLanderPos = currentLander.pos;
		shibaCam.setPosition(currentLanderPos.x, currentLanderPos.y + 20, currentLanderPos.z); 
		shibaCam.lookAt(currentLanderPos);
	}


	
	//Check collisions if altitude is between the y-velocity. It gets laggy if we check all the time. Now it's only laggy sometimes
	if (landerAltitude < (currentLander.velocity.y * -deltaTime && landerAltitude > (currentLander.velocity.y * deltaTime))) {
		ofVec3f min = currentLander.lander.getSceneMin() + currentLander.lander.getPosition();
		ofVec3f max = currentLander.lander.getSceneMax() + currentLander.lander.getPosition();
		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
		
		if (colBoxList.size() != 0) {
	
			cout << currentLander.velocity.y << "\n" << endl;

			/*if (currentLander.velocity.y > -1.0) {
				sys.isForcesActive = false; 
				//currently this happens everytime because after applying impulse force, the velocity slows down
				//maybe we need some way to have it stop checking for collisions once you applied the impulseforce? At least until you exit the collisions. 
			}
			else {
				
				
			}*/
			//this force isn't enough to make it bounce up, even when i multiplied it by -gravity. Can check ImpulseForce class in particle system. 
			
			ofVec3f collisionCenter(0.0f);

			for (TreeNode node: colBoxList)
			{
				Vector3 center = node.box.center();
				collisionCenter.x += center.x();
				collisionCenter.y += center.y();
				collisionCenter.z += center.z();
			}
			
			ofVec3f normal = (currentLander.pos - collisionCenter).normalize();
			float force = (restitution * currentLander.velocity.length()) + gravity;
			impulseForce = new ImpulseForce(force, normal);
			sys.addForce(impulseForce);
		}

	}
	


	//update light positions based on sliders 
	keyLight.setPosition(keyLightPosition);
	rimLight.setPosition(rimLightPosition);
	fillLight.setPosition(fillLightPosition);


		
	
}
//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(ofColor::black);

	glDepthMask(false);
	//might have to draw background image in here?
	if (bDrawGui) gui.draw();
	glDepthMask(true);

	theCam->begin();
	ofPushMatrix();
	ofEnableLighting();              // shaded mode
	mars.drawFaces();
	ofMesh mesh;
	if (bLanderLoaded) {
		sys.draw(); 
	}

	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 1);
	}


	ofPopMatrix();
	theCam->end();

	if (bDrawAltitude) {
		string str;
		str += "Altitude: " + std::to_string(landerAltitude);
		ofSetColor(ofColor::white);
		ofDrawBitmapString(str, ofGetWindowWidth() - 170, 25);
	}
}



void ofApp::keyPressed(int key) {
	const int OF_KEY_SPACE = 32;


	switch (key) {
	case OF_KEY_SPACE:
	{
		if (!soundPlayer.isPlaying() && soundPlayer.load("sounds/rocket.wav"))
			soundPlayer.play();
		if (!sys.isForcesActive) {
			sys.isForcesActive = true;
		}
		ThrustForce *up = new ThrustForce(ofVec3f(0, 1, 0), thrust, false);
		sys.addForce(up);
		break;
	}



	case 'B':
	case 'b':
		bDrawAltitude = !bDrawAltitude; 
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bDrawGui = !bDrawGui;
		break;
	case 'L':
	case 'l':
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'r':
		
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		break;
	case 'V':
		break;
	case 'w':
		break;
	case 'z':
	{
		ThrustForce *rotLeft = new ThrustForce(ofVec3f(0, 0, 0), thrust, true);
		sys.addForce(rotLeft);
		break;
	}
	case 'x':
	{
		break;
	}
	case OF_KEY_ALT:

		break;
	case OF_KEY_CONTROL:


		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_UP:
	{
		//Add ThrustForce in forward direction
		ThrustForce *forward = new ThrustForce(sys.particles[0].heading(), thrust, false);
		sys.addForce(forward);
		cout << "after up: " << landerParticle.rotation << "\n" << endl;
		break;
	}
	case OF_KEY_LEFT:
	{
		//Add ThrustForce in left direction

		ofVec2f temp(sys.particles[0].heading().x, sys.particles[0].heading().z);
		temp = temp.rotate(-90);
		ThrustForce *left = new ThrustForce(ofVec3f(temp.x, 0, temp.y), thrust, false);
		sys.addForce(left);

		break;
	}
	case OF_KEY_RIGHT: {
		//Add ThrustForce in right direction

		ofVec2f temp(sys.particles[0].heading().x, sys.particles[0].heading().z);
		temp = temp.rotate(90);
		ThrustForce *right = new ThrustForce(ofVec3f(temp.x, 0, temp.y), thrust, false);
		sys.addForce(right);
		break;
	}
	case OF_KEY_DOWN:
	{
		//Add ThrustForce in backward direction

		ThrustForce *backward = new ThrustForce(sys.particles[0].heading() * -1, thrust, false);
		sys.addForce(backward);
		break;
	}
	default:
		break;
	}
}


void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	case 32:
		if(soundPlayer.isLoaded())
		soundPlayer.stop();
		break;
	case OF_KEY_ALT:

		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_F1:
		theCam = &cam; 
		break;
	case OF_KEY_F2:
		theCam = &trackingCam;
		break;

	case OF_KEY_F3:
		theCam = &onboardCam;
		break;
	case OF_KEY_F4:
		theCam = &shibaCam;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		
		Particle landerParticle = sys.particles[0];
		ofVec3f min = landerParticle.lander.getSceneMin() + landerParticle.lander.getPosition();
		ofVec3f max = landerParticle.lander.getSceneMax() + landerParticle.lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(landerParticle.lander.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}

}

bool ofApp::landerRayIntersectOctree(ofVec3f &pointRet) {
	ofVec3f rayPoint = sys.particles[0].pos; 
	ofVec3f rayDir(0, -1, 0);
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));
	TreeNode intersectNode; 
	if (octree.intersect(ray, octree.root, intersectNode)) {
		pointRet = octree.mesh.getVertex(intersectNode.points[0]);
		return true; 
	}
	return false; 
}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;
	
	if (bInDrag) {
		sys.isForcesActive = false;
		glm::vec3 landerPos = sys.particles[0].lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		sys.particles[0].lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		sys.particles[0].pos = ofVec3f(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;


	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
	sys.isForcesActive = true;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {
	if (theCam != &cam) return;
	theCam->lookAt(sys.particles[0].pos);


}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}


void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}



bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}



//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}
