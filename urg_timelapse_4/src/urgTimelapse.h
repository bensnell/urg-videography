//
//  urgTimelapse.h
//  urg_timelapse_3
//
//  Created by Ben Snell on 1/17/16.
//
//

#ifndef __urg_timelapse_3__urgTimelapse__
#define __urg_timelapse_3__urgTimelapse__

#include "ofMain.h"
#include "ofxGui.h"

struct frame {
public:
    long start;
    int length;
    float startTime;
};

// class for displaying and rendering timelapses
class urgTimelapse {

public:
    
    urgTimelapse();
    
    // load file (but not data, which will be accessed as needed)
    ofBuffer buffer;
    void loadFile(string fileName);
    
    // find the number of frames and the interval of scans for each
    void findFrames(float speed, float periodSize, float startPeriod = 0);
    vector<frame> frames;       // holds data pertaining to each frame
//    int nScans;                 // total number of scans
    
    // general params for filling spherical meshes
    int minIndex = 0;           // min index of point in scan
    int maxIndex = 682;         // max index of point in scan (682 since 726 total but last chunk is empty)
    double minSqDist2Cam = 70000.;    // discard points closer than this squared distance to camera
    
    // fill mesh with the first scene
    ofMesh firstFrame;          // first frame of the timelapse
    void fillFirstFrame(float speed, float periodSize, bool bClockwise, float alignmentAngle, float startPeriod = 0);
    
    // draw the first scene
    ofEasyCam cam;
    void drawFirstFrame();
    
    
    ofParameter<float> alignmentA;
    ofParameter<float> startRotateX;
    ofParameter<float> startRotateY;
    ofParameter<float> startRotateZ;
    ofParameterGroup timelapseControls;
    
    
    
    int frameNum = -1;
    ofMesh renderedFrame;       // current frame rendered in timelapse
    void run(float speed, float periodSize, bool bClockwise, float alignmentAngle, float startPeriod = 0);     // render, save, and increment counter
//    void render(); // draw the frame and save
    
    void reset(); // reset the timelapse rendered frame count

    
    
    
    
    
    
    
    
    
    
};

#endif /* defined(__urg_timelapse_3__urgTimelapse__) */
