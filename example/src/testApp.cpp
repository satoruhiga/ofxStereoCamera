#include "testApp.h"

#include "ofxStereoCamera.h"

ofxStereoCamera<ofEasyCam> cam;

vector<ofVec3f> points;

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	ofBackground(0);
	
	cam.setup(ofGetWidth(), ofGetHeight());
	cam.setScale(1, -1, 1);
	
	cam.setPhysicalFocusDistance(120);
	
	cam.setFocusDistance(50);
	cam.setNearClip(0.1);
	
	float s = 1000;
	for (int i = 0; i < 3000; i++)
	{
		ofVec3f v;
		v.x = ofRandom(-s, s);
		v.y = ofRandom(-s, s);
		v.z = ofRandom(-s, s);
		points.push_back(v);
	}
}

//--------------------------------------------------------------
void testApp::update()
{
	cam.setFocusDistance(cam.getGlobalPosition().length());
	
	cam.update();
	
	cam.beginLeft();
	drawScene();
	cam.endLeft();
	
	cam.beginRight();
	drawScene();
	cam.endRight();

}

void testApp::drawScene()
{
	glPushMatrix();
	
	ofNoFill();
	ofBox(100);
	
	glEnable(GL_POINT_SMOOTH);
	
	ofDrawAxis(100);
	
	static GLfloat distance[] = { 0.0, 0.0, 1.0 };
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distance);
	
	glPointSize(3000);
	
	glBegin(GL_POINTS);
	for (int i = 0; i < points.size();i ++)
	{
		glVertex3fv(points[i].getPtr());
	}
	glEnd();
	
	glPopMatrix();
}

//--------------------------------------------------------------
void testApp::draw()
{
	cam.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	string str;
	str += "[1] SIDE_BY_SIDE\n";
	str += "[2] LINE_BY_LINE\n";
	str += "[3] ANAGLYPH\n\n";
	str += "[4] PARALLEL\n";
	str += "[5] TOE_IN\n";
	
	ofSetColor(255);
	ofDrawBitmapString(str, 10, 20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	if (key == '1')
	{
		cam.setOutputMode(ofxStereoCameraOutputMode::SIDE_BY_SIDE);
	}
	else if (key == '2')
	{
		cam.setOutputMode(ofxStereoCameraOutputMode::LINE_BY_LINE);
	}
	else if (key == '3')
	{
		cam.setOutputMode(ofxStereoCameraOutputMode::ANAGLYPH);
	}
	
	if (key == '4')
	{
		cam.setViewMode(ofxStereoCameraViewMode::PARALLEL);
	}
	else if (key == '5')
	{
		cam.setViewMode(ofxStereoCameraViewMode::TOE_IN);
	}

	if (key == 'f')
	{
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}