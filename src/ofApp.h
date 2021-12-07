#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "Particle.h";
#include "ParticleSystem.h";



class ofApp : public ofBaseApp{


	public:
		
		float timeLastFrame;
		float timeLastFrameIntersect;

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void gotMessage(ofMessage msg);
		void savePicture();
		void togglePointsDisplay();
		void setCameraTarget();
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool landerRayIntersectOctree(ofVec3f &pointRet);
		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);

		ofEasyCam cam;
		ofCamera *theCam; 
		ofCamera trackingCam; 
		ofCamera onboardCam;
		ofCamera shibaCam; 

		ofLight keyLight, fillLight, rimLight; 

		ofxVec3Slider keyLightPosition, fillLightPosition, rimLightPosition; 


		ofxAssimpModelLoader mars;
		ofLight light;
		Box boundingBox, landerBounds;
		Box testBox;
		vector<TreeNode> terrainColBoxList;
		vector<TreeNode> islandColBoxList;
		vector<TreeNode> platformColBoxList;
		vector<TreeNode> waterColBoxList;
		bool bLanderSelected = false;
		Octree terrainOctree;
		Octree islandOctree;
		Octree platformOctree;
		Octree waterOctree;
		TreeNode selectedNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;


		ofxIntSlider numLevels;
		ofxFloatSlider thrust; 
		ofxFloatSlider intersectDeltaTime;
		ofxFloatSlider restitution;
		ofxFloatSlider gravity; 
		ofxPanel gui;

		bool bDisplayPoints = false;
		bool pointSelected = false;
		bool bDisplayLeafNodes = false;
		bool bDisplayOctree = false;
		bool bDisplayBBoxes = false;
		bool bDrawAltitude = false; 
		bool bDrawGui = false; 
		bool bLanderLoaded = false;

		
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;
		float landerAltitude;
	

		vector<Box> bboxList;

		const float selectionRange = 4.0;

		Particle landerParticle; 
		ParticleSystem sys; 

		GravityForce *gravityForce; 
		TurbulenceForce *turbulenceForce; 
		ImpulseForce *impulseForce; 

		ofSoundPlayer soundPlayer; 

		ofImage backgroundImage; 
};
