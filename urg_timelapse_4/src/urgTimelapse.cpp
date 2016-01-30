//
//  urgTimelapse.cpp
//  urg_timelapse_3
//
//  Created by Ben Snell on 1/17/16.
//
//

// repeated scans are typically within 15 ms (but make this interval a little bigger)

#include "urgTimelapse.h"

urgTimelapse::urgTimelapse() {
    
    timelapseControls.setName("Timelapse Setup");
    timelapseControls.add(alignmentA.set("Frame Align Angle", -3.5, -10., 10.));
    timelapseControls.add(startRotateX.set("Start X Rotation", 0, -180, 180));
    timelapseControls.add(startRotateY.set("Start Y Rotation", 0, -180, 180));
    timelapseControls.add(startRotateZ.set("Start Z Rotation", 0, -180, 180));
    cam.setFarClip(15000);
    
}

// -----------------------------------------------------------------------

// load csv file into ofFile
void urgTimelapse::loadFile(string fileName) {
    
    // CSV file placed in bin/data folder with TXY format
    ofFile file(fileName);
    cout << "Loading file " + fileName << endl;
    
    // alt loading operations?
//    file = ofFile(fileName);
//    file.open(ofToDataPath(fileName), ofFile::ReadOnly, false);

    if (!file.exists()) ofLogError("The file " + fileName + " is missing");
    
    // load buffer with the file
    buffer = file.readToBuffer();
    cout << "File successfully read to buffer" << endl;
    
    // alt loading operations?
//    ofBuffer buffer(filePath);
//    buffer = ofBufferFromFile(ofToDataPath(filePath));
//    ofBufferToFile(ofToDataPath(fileName), buffer);
//    buffer = ofBuffer(file);
//    ofBuffer buffer(file);

}

// -----------------------------------------------------------------------

void urgTimelapse::findFrames(float speed, float periodSize, float startPeriod) {
    
    // reset markers
    frames.clear();
//    nScans = -1;
    
    long thisScanNumber = 0;
    
    frame thisFrame;
    thisFrame.startTime = -1; // start (zero) condition
    
    // holds the current time sought after (to be >=)
    long searchTime = long(startPeriod * periodSize / speed * 1000.); // ms
    
    // loop through all lines to find the frames
    for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        
        string line = *it;
        if (line.empty()) {
            cout << "Empty line found. Stopping at scan # " << thisScanNumber << endl;
//            nScans = thisScanNumber;
            break;
        }
        
        // get time now
        line.resize((int)line.find_first_of(','));
        long timeNow = long(ofToDouble(line)); // ms
        
        // check if timeNow is greater than searchTime
        if (timeNow >= searchTime) {
            
            // new frame is found, so store existing frame (unless it's the zeroth frame)
            if (thisFrame.startTime != -1) {
                
                // update its length
                thisFrame.length = thisScanNumber - thisFrame.start;
                
                // add it to the frame vector
                frames.push_back(thisFrame);
            }
            
            // create a new frame with starting values
            thisFrame.start = thisScanNumber;
            thisFrame.startTime = float(timeNow / 1000.);
            
            // update the search time
            searchTime = long((startPeriod + frames.size() + 1) * periodSize / speed * 1000.);
        }
        
        // increment the scan number
        thisScanNumber++;
    }
    
    cout << frames.size() << " frames have been found." << endl;
    
//    if (nScans == -1) nScans = thisScanNumber
    
    
    // print all frame info
//    cout << "*** BEGIN ALL FRAME DATA ***" << endl;
//    cout << "#" << "\t" << "start" << "\t" << "length" << "\t" << "startTime" << endl;
//    for (long i = 0, end = frames.size(); i < end; i++) {
//        cout << i << "\t" << frames[i].start << "\t" << frames[i].length << "\t" << frames[i].startTime << endl;
//    }
//    cout << "*** END ALL FRAME DATA ***" << endl;
    
}

// -----------------------------------------------------------------------

// fill mesh with the first capture
// scans are spaced out according to the speed of the rotations (degrees/sec; default = 225/64) and the timestamp of each step
// each period is 180 degrees since we're recording both sides of the lidar every scan
void urgTimelapse::fillFirstFrame(float speed, float periodSize, bool bClockwise, float alignmentAngle, float startPeriod) {
    
    // clear mesh for good measure
    firstFrame.clear();
    
    // ------------------------------------
    // ------- FIND SCANS IN FRAME --------
    // ------------------------------------
    
    // holds first time (in seconds)
    float timeZero;
    
    // interval of interest of the scans
    long frameStart = -1;
    long frameLength = -1;
    
    // find max interval within which to search one period
    float overfillAmt = 1.25;
    long maxSearchInterval = long(periodSize * 1/speed * 10. * overfillAmt); // overfillAmt times ideal number of scans in one frame
    
    // start and stop times to search for
    float startTime = startPeriod * periodSize / speed * 1000.;
    float stopTime = (startPeriod * periodSize + periodSize) / speed * 1000.;
    
    // For the first frame, find the start and stop scans
    // Note: last line may be empty
    long thisScanNumber = 0;
    for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        
        // get the first number (time) in this line
        string line = *it;
        if (line.empty()) {
            cout << "Skipping empty line" << endl;
            // increment counter here?
            continue;
        }
        line.resize((int)line.find_first_of(','));
        float timeNow = ofToFloat(line); // in milliseconds
//        cout << timeNow << endl;
        
        // check if this scan is the start of the frame so long as it has not yet been found
        if (frameStart == -1) {
            if (timeNow >= startTime) {
                frameStart = thisScanNumber;
                timeZero = timeNow / 1000.;
            }
        }
        
        // check if it's the end of the interval
        if (frameStart != -1) {
            if (timeNow >= stopTime) {
                frameLength = thisScanNumber - frameStart;
                break;
            }
        }
        
        // increment the counter
        thisScanNumber++;
        if (thisScanNumber > frameStart + maxSearchInterval) {
            cout << "Couldn't find end of first frame. Try increasing search interval." << endl;
            break;
        }
    }
    
    // ------------------------------------
    // --------- FILL FIRST FRAME ---------
    // ------------------------------------
    
    // reset counter
    thisScanNumber = 0;
    
    // add all scans to the mesh
    ofBuffer::Line it = buffer.getLines().begin();
    advance(it, frameStart);
    ofBuffer::Line end = buffer.getLines().begin();
    advance(end, frameStart + frameLength);
    for (it; it != end; ++it) {
        
        // get this line and split up the measurements
        string line = *it;
        if (line.empty()) {
            cout << "Skipping empty line" << endl;
            continue;
        }
        vector<string> values = ofSplitString(line, ",");
        
        // get timeNow
        float timeNow = ofToFloat(values[0]) / 1000. - timeZero; // in seconds
        
        // put all the points in a scan in the mesh
        for (int i = minIndex; i < maxIndex; i++) {
            
            // find the x and y coordinates
            float px = ofToFloat(values[2 * i + 1]);
            float py = ofToFloat(values[2 * i + 2]);
            
            // find the vector to this point
            ofVec3f thisPoint(px, py, 0.);
            
            // remove points too close
            double sqDist = thisPoint.distanceSquared(ofVec3f(0.,0.,0.));
            if (sqDist < minSqDist2Cam) continue;
            
            // rotate this point 90 degrees about the z axis to orient it upwards
            thisPoint.rotate(90., ofVec3f(0., 0., 1.));
            
            // apply the alignment angle stretch or compression to realign the two chuncks
            float alignmentFactor = (float)i / 682. * alignmentAngle;
            thisPoint.rotate(alignmentFactor, ofVec3f(0., 0., 1.));
            
            // rotate this point about the y axis an amount proportional to this scanNumber and the number of scans in this frame (since this changes)
            float rotationAmt = (float)thisScanNumber / (float)frameLength * periodSize;
//            cout << rotationAmt << endl;

            // if clockwise is true, rotate in negative direction
            if (bClockwise) rotationAmt *= -1.;
            thisPoint.rotate(rotationAmt, ofVec3f(0., 1., 0.));
            
            // add the point to the mesh with a color
            firstFrame.addVertex(thisPoint);
            firstFrame.addColor(ofFloatColor(1.));
        }
        
        thisScanNumber++;
    }
}

// ---------------------------------------------------------------------

// graphs 360 degree panorama
// slide is the rotation here
void urgTimelapse::drawFirstFrame() {
    
//    cam.begin();
    
    ofPushMatrix();

    // rotate the original frame
    ofRotate(startRotateX, 1, 0, 0);
    ofRotate(startRotateY, 0, 1, 0);
    ofRotate(startRotateZ, 0, 0, 1);
    
    firstFrame.drawVertices();

    ofPopMatrix();

//    cam.end();

}

// ---------------------------------------------------------------------

void urgTimelapse::reset() {
    
    frameNum = 0;
}

// ---------------------------------------------------------------------

void urgTimelapse::run(float speed, float periodSize, bool bClockwise, float alignmentAngle, float startPeriod) {
    
    // exit function if frame number is -1
    if (frameNum == -1) return;
    
    // reset frame (MUST DO THIS)
    renderedFrame.clear();
    
    // ------------------------------------
    // ------------ FILL FRAME ------------
    // ------------------------------------
    
    // reset counter
    int scanNum = 0;
    
    // add scans to the mesh as dictated by the frames
    ofBuffer::Line it = buffer.getLines().begin();
    advance(it, frames[frameNum].start);
    ofBuffer::Line end = buffer.getLines().end();
    for (it, end; it != end; ++it) {
        
        // get line
        string line = *it;
        vector<string> values = ofSplitString(line, ",");
        
        // put all the points in a scan in the mesh
        for (int i = minIndex; i < maxIndex; i++) {
            
            // find the x and y coordinates
            float px = ofToFloat(values[2 * i + 1]);
            float py = ofToFloat(values[2 * i + 2]);
            
            // find the vector to this point
            ofVec3f thisPoint(px, py, 0.);
            
            // remove points too close
            double sqDist = thisPoint.distanceSquared(ofVec3f(0.,0.,0.));
            if (sqDist < minSqDist2Cam) continue;
            
            // rotate this point 90 degrees about the z axis to orient it upwards
            thisPoint.rotate(90., ofVec3f(0., 0., 1.));
            
            // apply the alignment angle stretch or compression to realign the two chuncks
            float alignmentFactor = (float)i / 682. * alignmentAngle;
            thisPoint.rotate(alignmentFactor, ofVec3f(0., 0., 1.));
            
            // rotate this point about the y axis an amount proportional to this scanNumber and the number of scans in this frame (since this changes)
            float rotationAmt = (float)scanNum / (float)frames[frameNum].length * periodSize;
            
            // if clockwise is true, rotate in negative direction
            if (bClockwise) rotationAmt *= -1.;
            // if period is 180 and frame is odd, rotate 180 degrees
            if ((int)periodSize == 180 && (frameNum % 2)) rotationAmt += 180.;
            thisPoint.rotate(rotationAmt, ofVec3f(0., 1., 0.));
            
            // add the point to the mesh with a color
            renderedFrame.addVertex(thisPoint);
            renderedFrame.addColor(ofFloatColor(1.));
        }
        
        scanNum++;
        if (scanNum >= frames[frameNum].length) break;
    }
    
    cout << "rendered frame # " << frameNum << endl;
    
    // increment the frame number
    frameNum++;
    if (frameNum >= frames.size()) {
        // stop running this operation
        frameNum = -1;
    }
    
    
    // NOW RENDER
    ofPushMatrix();
    
    // rotate the original frame
    ofRotate(startRotateX, 1, 0, 0);
    ofRotate(startRotateY, 0, 1, 0);
    ofRotate(startRotateZ, 0, 0, 1);
    
    renderedFrame.drawVertices();
    
    ofPopMatrix();
    
    // SAVE IMAGE
    ofSaveFrame();
    
}

// ---------------------------------------------------------------------

//void urgTimelapse::render() {
//    
//    ofPushMatrix();
//
//    // rotate the original frame
//    ofRotate(startRotateX, 1, 0, 0);
//    ofRotate(startRotateY, 0, 1, 0);
//    ofRotate(startRotateZ, 0, 0, 1);
//
//    renderedFrame.drawVertices();
//
//    ofPopMatrix();
//    
//    ofSaveFrame();
//}


// ---------------------------------------------------------------------


// fill mesh with a spherical capture
// scans are spaced out according to the speed of the rotations (degrees/sec; default = 225/64) and the timestamp of each step
// each period is 180 degrees since we're recording both sides of the lidar every scan
//void urgDisplay::fillPointMeshTXYSpherical(float speed, float period, bool bClockwise, float startingPeriod, float numPeriods, float alignmentAngle) {
//    
//    // clear the mesh
//    pointMesh.clear();
//    
//    // if either variable is -1, set it to default or max
//    if (startingPeriod == -1.) startingPeriod = 0;
//    if (numPeriods == -1.) numPeriods = 99999.;
//    
//
//    
//    // set time zero
//    float timeZero;
//    
//    // interval of interest of the scans
//    int startIndex = -1;
//    int endIndex = -1;
//    
//    // first, find the starting points and ending points for the sphere
//    for (int i = 0; i < nScans; i++) {
//        
//        // find the current time
//        float timeNow = csv.getFloat(i, 0) / 1000.; // in seconds
//        
//        // check if this scan is the start of the interval of interest so long as it has not yet been found
//        if (startIndex == -1) {
//            if ((timeNow * speed) >= (startingPeriod * period)) {
//                startIndex = i;
//                timeZero = timeNow;
//            }
//        }
//        
//        // check if it's the end of the interval
//        if (startIndex != -1) {
//            if ((timeNow * speed) >= ((startingPeriod + numPeriods) * period)) {
//                endIndex = i;
//                break;
//            }
//        }
//    }
//    
//    // if endIndex wasn't assigned, assign it to the total number of scans
//    if (endIndex == -1) endIndex = nScans;
//    
//    // ------------------------------------------
//    // ---------- FILL THE POINT MESH -----------
//    // ------------------------------------------
//    
//    // for every scan within the interval, add it to the mesh
//    for (int i = startIndex; i < endIndex; i++) {
//        
//        // get the current time
//        float timeNow = csv.getFloat(i, 0) / 1000. - timeZero;
//        
//        // put all the points in a scan in the mesh
//        for (int j = minIndex; j < maxIndex; j++) { // NOTE: I've been doing this wrong (i.e. wrong order of operations... this is why there are so many points at zero, zero)... it's right here now
//            
//            // find the x and y coordinates
//            float px = csv.getFloat(i, 2 * j + 1);
//            float py = csv.getFloat(i, 2 * j + 2);
//            
//            // find the vector to this point
//            ofVec3f thisPoint(px, py, 0.);
//            
//            
//            
//            // remove points too close
//            double sqDist = thisPoint.distanceSquared(ofVec3f(0.,0.,0.));
//            
//            //            cout << sqDist << endl;
//            
//            if (sqDist < minSqDist2Cam) continue;
//            
//            
//            //            if (px < 100. && py < 100.) continue;
//            
//            // rotate this point 90 degrees about the z axis to orient it upwards
//            thisPoint.rotate(90., ofVec3f(0., 0., 1.));
//            
//            // apply the alignment angle stretch or compression to realign the two chuncks
//            float alignmentFactor = (float)j / 682. * alignmentAngle;
//            thisPoint.rotate(alignmentFactor, ofVec3f(0., 0., 1.));
//            
//            // rotate the point about the y axis an amount proportional to the elapsed time and the speed
//            float rotationAmt = timeNow * speed;
//            // if clockwise is true, rotate in negative direction
//            if (bClockwise) rotationAmt *= -1.;
//            thisPoint.rotate(rotationAmt, ofVec3f(0., 1., 0.));
//            
//            // add the point to the mesh with a color
//            pointMesh.addVertex(thisPoint);
//            pointMesh.addColor(ofFloatColor(1.));
//        }
//    }
//}

