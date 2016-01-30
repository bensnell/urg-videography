#pragma once

#include "ofMain.h"
#include "ofxTimeline.h"
#include "urgTimelapse.h"
#include "ofxGui.h"

class ofApp: public ofBaseApp{

  public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    void exit();
	
//	ofLight light;
	
//    ofxTLCameraTrack * cameraTrack;
    ofEasyCam cam;
    
	ofxTimeline timeline;
    
    bool bRunTimelapse = false;
    urgTimelapse timelapse;
    
    bool bDrawPanel = false;
    
    ofParameter<bool> bDrawTimeline;
    ofParameter<bool> bCameraMouseInput;
    ofParameter<bool> bHideCursor;
    ofParameterGroup general;
    ofxPanel panel;
    
};
