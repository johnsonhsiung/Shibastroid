
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


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {

	
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
	//	ofSetWindowShape(1024, 768);

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
	fillLight.setSpotlight();
	fillLight.setScale(.05);
	fillLight.setSpotlightCutOff(15);
	fillLight.setAttenuation(2, .001, .001);
	fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
	fillLight.rotate(-10, ofVec3f(1, 0, 0));
	fillLight.rotate(-45, ofVec3f(0, 1, 0));
	fillLight.setPosition(-5, 5, 5);

	rimLight.setup();
	rimLight.enable();
	rimLight.setSpotlight();
	rimLight.setScale(.05);
	rimLight.setSpotlightCutOff(30);
	rimLight.setAttenuation(.2, .001, .001);
	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
	rimLight.rotate(180, ofVec3f(0, 1, 0));
	rimLight.setPosition(0, 5, -7);


	mars.loadModel("geo/Terrain.obj");
	mars.setScaleNormalization(false);

	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	gui.add(thrust.setup("Thrust", 5, 1, 50));
	gui.add(intersectDeltaTime.setup("Time between checks for intersects", 0.5, 0.2, 1000000));
	gui.add(keyLightPosition.setup("Keylight pos", ofVec3f(5, 5, 5), ofVec3f(-100, -100, -100), ofVec3f(100, 100, 100)));
	gui.add(fillLightPosition.setup("Filllight pos", ofVec3f(-5, 5, 5), ofVec3f(-100, -100, -100), ofVec3f(100, 100, 100)));
	gui.add(rimLightPosition.setup("rimLight pos", ofVec3f(0, 5, -7), ofVec3f(-100, -100, -100), ofVec3f(100, 100, 100)));

	gui.add(restitution.setup("Bounciness", 0.3, 0.0, 1.0));

	gui.add(normalOfPointScaling.setup("Normal Scaling", 3.0, 1.0, 10.0));


	bHide = false;

	//  Create Octree for testing.
	//

	cout << "Creating Octree" << endl;
	uint64_t startTime = ofGetCurrentTime().getAsMilliseconds();

	octree.create(mars.getMesh(0), 20);

	uint64_t endTime = ofGetCurrentTime().getAsMilliseconds();
	cout << "Finished in " << (endTime - startTime) << "ms" << endl;

	cout << "Number of Verts: " << mars.getMesh(0).getNumVertices() << endl;

	testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));

	//load shiba 
	if (landerParticle.load("geo/Shib Ship.obj")) {
		bboxList.clear();
		for (int i = 0; i < landerParticle.lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(landerParticle.lander.getMesh(i)));
		}
		bLanderLoaded = true;
		sys.add(landerParticle); 
		cout << "Loaded";
		
	}
	else {
		cout << "Can't load obj";
	}

	//add forces 
	gravityForce = new GravityForce(ofVec3f(0, -1.6, 0));
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
			//not sure how to get the normal of the contact point. Can you help with this part? 
			//We need the normal of that point and it takes in an index of the point to get it
			//I changed colBoxList to be a vector of treeNodes so we can get the indexes, but not sure what to do after that. 
			cout << landerAltitude << "\n" << endl;
			float scale = normalOfPointScaling;
			if (currentLander.velocity.y < 1.0) {
				sys.isForcesActive = false; 
			}
			else {
				impulseForce = new ImpulseForce(restitution, currentLander.velocity, octree.mesh.getNormal(colBoxList[0].points[0]));
				sys.addForce(impulseForce);
			}
			
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
	if (!bHide) gui.draw();
	glDepthMask(true);

	theCam->begin();
	ofPushMatrix();
	ofEnableLighting();              // shaded mode
	mars.drawFaces();
	ofMesh mesh;
	if (bLanderLoaded) {
		sys.draw(); 
		/**if (bLanderSelected) {

			ofVec3f min = landerParticle.lander.getSceneMin() + landerParticle.lander.getPosition();
			ofVec3f max = landerParticle.lander.getSceneMax() + landerParticle.lander.getPosition();

			Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
			ofSetColor(ofColor::white);
			Octree::drawBox(bounds);

			// draw colliding boxes
			//
			ofSetColor(ofColor::red);
			for (int i = 0; i < colBoxList.size(); i++) {
				Octree::drawBox(colBoxList[i]);
			}
		}**/
	}

	

	// recursively draw octree
	//
	ofDisableLighting();
	rimLight.draw();
	fillLight.draw();
	keyLight.draw();
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

	// if point selected, draw a sphere
	//
	if (pointSelected) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - theCam->getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
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


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {
	const int OF_KEY_SPACE = 32;


	switch (key) {
	case OF_KEY_SPACE:
	{
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
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
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
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
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

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
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
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

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
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	cout << "Ray Intersect with Octree" << endl;
	uint64_t startTime = ofGetCurrentTime().getAsMilliseconds();

	pointSelected = octree.intersect(ray, octree.root, selectedNode);

	uint64_t endTime = ofGetCurrentTime().getAsMilliseconds();
	cout << "Finished in " << (endTime - startTime) << "ms" << endl;

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
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
	/*
	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
	

		/*if (bounds.overlap(testBox)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "OK" << endl;
		}


	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}*/
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
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



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//

void ofApp::dragEvent2(ofDragInfo dragInfo) {
	/*
	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
	if (lander.loadModel(dragInfo.files[0])) {
		lander.setScaleNormalization(false);
//		lander.setScale(.1, .1, .1);
	//	lander.setPosition(point.x, point.y, point.z);
		lander.setPosition(1, 1, 0);

		bLanderLoaded = true;
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		cout << "Mesh Count: " << lander.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
	*/
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//

void ofApp::dragEvent(ofDragInfo dragInfo) {
	/*
	if (lander.loadModel(dragInfo.files[0])) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		//		lander.setRotation(1, 180, 1, 0, 0);

				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = cam.getPosition();
		glm::vec3 camAxis = cam.getZAxis();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = lander.getSceneMin();
			glm::vec3 max = lander.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for lander while we are at it
			//
			landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
		*/
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
