// This Example renamed everything ofApp instead of testApp

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // setup gui
    general.add(bDrawTimeline.set("Draw Timeline", true));
    general.add(bCameraMouseInput.set("Camera Mouse Input", false));
    general.add(bHideCursor.set("Hide Cursor", false));
    general.setName("General Controls");
    
    panel.setup();
    panel.add(general);
    panel.add(timelapse.timelapseControls);
    panel.loadFromFile("settings.xml");
	
    // graphics
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
    ofEnableSmoothing();
	
	glEnable(GL_DEPTH_TEST);
	ofEnableLighting();
    
    //lets you use COMMAND+C and COMMAND+V actions on mac
    ofxTimeline::removeCocoaMenusFromGlut("urg_timelapse_3");
		
    // load timelapse
//    timelapse.loadFile("2015-12-09 11-17-44 recording.csv"); // zebra
    timelapse.loadFile("2015-12-12 02-28-08 recording.csv"); // bedroom

    // find the total number of frames
    timelapse.findFrames(225./64., 180.);
    
    // Timeline setup and playback details
	timeline.setup();
    timeline.setFrameRate(30);
	timeline.setDurationInFrames(timelapse.frames.size());
    timeline.setLoopType(OF_LOOP_NONE);
    
//    cameraTrack = new ofxTLCameraTrack();
//    cameraTrack->setCamera(cam);
//    timeline.addTrack("Camera", cameraTrack);
    
//    cameraTrack->lockCameraToTrack = true;
    
	// each call to "add keyframes" add's another track to the timeline
    timeline.addCurves("Translate X", ofRange(-20000, 20000));
    timeline.addCurves("Translate Y", ofRange(-20000, 20000));
    timeline.addCurves("Translate Z", ofRange(-20000, 20000));
	timeline.addCurves("Rotate X", ofRange(-180, 180));
	timeline.addCurves("Rotate Y", ofRange(-180, 180));
    timeline.addCurves("Rotate Z", ofRange(-180, 180));
    
//	timeline.addColors("Colors");

    // if true, will never skip frames
    timeline.setFrameBased(true);
    
    // fill first frame
    timelapse.fillFirstFrame(225./64., 180., false, -3.5);
    
    cam.setFarClip(15000);
    
}

//--------------------------------------------------------------
void ofApp::update(){

    bCameraMouseInput ? cam.enableMouseInput() : cam.disableMouseInput();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
	ofBackground(0);
    
    cam.begin();
    ofPushMatrix();
    
    // translate to location
    ofTranslate(timeline.getValue("Translate X"), timeline.getValue("Translate Y"), timeline.getValue("Translate Z"));
    
    ofRotate(timeline.getValue("Rotate X"), 1, 0, 0);
    ofRotate(timeline.getValue("Rotate Y"), 0, 1, 0);
    ofRotate(timeline.getValue("Rotate Z"), 0, 0, 1);
    
    // draw timelapse
    if (bRunTimelapse) {
        timelapse.run(225./64., 180., false, -3.5);
    } else {
        timelapse.drawFirstFrame();
    }
    
    ofPopMatrix();
    cam.end();
    
    
    
//    cam.begin();
//	ofPushMatrix();
//	
//	ofPushStyle();
//    // set the color to whatever the last color we encountered was
//	ofSetColor(timeline.getColor("Colors"));
//	
//    // translate to the center of the screen
//	ofTranslate(ofGetWidth()*.5, ofGetHeight()*.66, 40);
//    
//    // Read the values out of the timeline and use them to change the viewport rotation
//	ofRotate(timeline.getValue("Rotate X"), 1, 0, 0);
//	ofRotate(timeline.getValue("Rotate Y"), 0, 1, 0);
//	
//    ofDrawBox(0, 0, 0, 200);
//	
//	ofPopMatrix();
//    cam.end();
//
//	ofPopStyle();
	
    if (bDrawTimeline) timeline.draw();
    if (bDrawPanel) panel.draw();
    if (bDrawPanel || bDrawTimeline) ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
}

//--------------------------------------------------------------

void ofApp::exit() {
    panel.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//    if(key == 'l'){
//        cameraTrack->lockCameraToTrack = !cameraTrack->lockCameraToTrack;
//    }
//    else if(key == 't'){
//        // NOTE: cannot edit keyframes, only add new ones!
//        cameraTrack->addKeyframe();
//    }
    if (key == 'f') ofToggleFullscreen();
    if (key == 'b') bDrawTimeline = !bDrawTimeline;
    if (key == 'n') bDrawPanel = !bDrawPanel;
    if (key == 'p') timeline.play();
    if (key == 'r') {
        if (bRunTimelapse) {
            bRunTimelapse = false;
        } else {
            timelapse.reset();
            bRunTimelapse = true;
            timeline.play();
        }
    }
    if (key == 'c') {
        bHideCursor ? ofShowCursor() : ofHideCursor();
        bHideCursor = !bHideCursor;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
	