#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxOsc.h"

class testApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

private:
    void clearBundle();
    template <class T> void addMessage(string address, T data);
    void sendBundle();

private:
    ofVideoGrabber cam;
    ofxFaceTracker tracker;
    ofxFaceTracker imgTracker;
    ofImage faceImage;

    ofxOscSender sender;
    ofxOscBundle bundle;
};
