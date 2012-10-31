#include "testApp.h"

#define OSC_HOST "localhost"
#define OSC_PORT 8338

using namespace ofxCv;

#pragma mark - OSC

void testApp::clearBundle() {
    bundle.clear();
}

void testApp::sendBundle() {
    sender.sendBundle(bundle);
}

template <>
void testApp::addMessage(string address, float data) {
    ofxOscMessage msg;
    msg.setAddress(address);
    msg.addFloatArg(data);
    bundle.addMessage(msg);
}

template <>
void testApp::addMessage(string address, ofVec2f data) {
    ofxOscMessage msg;
    msg.setAddress(address);
    msg.addFloatArg(data.x);
    msg.addFloatArg(data.y);
    bundle.addMessage(msg);
}

template <>
void testApp::addMessage(string address, ofVec3f data) {
    ofxOscMessage msg;
    msg.setAddress(address);
    msg.addFloatArg(data.x);
    msg.addFloatArg(data.y);
    msg.addFloatArg(data.z);
    bundle.addMessage(msg);
}

template <>
void testApp::addMessage(string address, ofMesh data) {
    ofxOscMessage msg;
    msg.setAddress(address);

    int numOfVertices = data.getNumVertices();
    for (int i = 0; i < numOfVertices; i++) {
        ofVec3f vertex = data.getVertex(i);
        msg.addFloatArg(vertex.x);
        msg.addFloatArg(vertex.y);
        msg.addFloatArg(vertex.z);
    }

    
    bundle.addMessage(msg);
}


template <>
void testApp::addMessage(string address, vector<ofIndexType> data) {
    ofxOscMessage msg;
    msg.setAddress(address);
    
    int numOfVertices = data.size();
    for (int i = 0; i < numOfVertices; i++) {
        msg.addIntArg(data[i]);
    }
    
    bundle.addMessage(msg);
}


#pragma mark - Main

void testApp::setup() {
    ofSetVerticalSync(true);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);

    cam.initGrabber(640, 480);

    // to secure the memory domain of the image
    faceImage.allocate(768, 1024, OF_IMAGE_COLOR);
    faceImage.loadImage("koomote.png");

    tracker.setup();
    tracker.setRescale(.5);

    // setup of the facetracker of the facial image to compose 
    imgTracker.setup();

    // Setup OSC
    string host = OSC_HOST;
    int port = OSC_PORT;
    sender.setup(host, port);
}

void testApp::update() {
    cam.update();
    if (cam.isFrameNew()) {
        imgTracker.update(toCv(faceImage));
        tracker.update(toCv(cam));

        if (tracker.getFound()) {
            clearBundle();

            // position,size, imclination of the face detected with camera
            addMessage("/position", tracker.getPosition());
            addMessage("/scale", tracker.getScale());
            addMessage("/orientation", tracker.getOrientation());

            // mesh based on a camera image
            ofMesh mesh = tracker.getObjectMesh();
            addMessage("/vertices", mesh);
            
            // vertices points 
            addMessage("/triangleIndices", mesh.getIndices());
            sendBundle();
        }
    }
}

void testApp::draw() {
    ofSetColor(255);

    cam.draw(0, 0);
    ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);

    // if face is detected, carry out the following process
    if (tracker.getFound()) {

        // create mesh based on a camera image
        ofMesh objectMesh = tracker.getObjectMesh();

        // create mesh based on face
        ofMesh imgMesh = imgTracker.getObjectMesh();

        // apply mesh of picture based on the present face
        int numOfVertices = objectMesh.getNumVertices();
        for (int i = 0; i < numOfVertices; i++) {
            ofVec3f vertex = objectMesh.getVertex(i);
            imgMesh.setVertex(i, vertex);
        }

        // no-3D perspective
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_DEFAULT, true, -1000, 1000);

        // position,size, imclination of the face detected with camera
        ofVec2f positon = tracker.getPosition();
        float scale = tracker.getScale();
        ofVec3f orientation = tracker.getOrientation();

         ofPushMatrix();
        ofTranslate(positon.x, positon.y);
        ofScale(scale, scale, scale);
        ofRotateX(orientation.x * 45.0f);
        ofRotateY(orientation.y * 45.0f);
        ofRotateZ(orientation.z * 45.0f);

        
        ofSetColor(255, 255, 255, 127);
//        faceImage.getTextureReference().bind();
//        imgMesh.draw();
//        faceImage.getTextureReference().unbind();
        imgMesh.drawWireframe();
        ofPopMatrix();
    }
}

#pragma mark - User Input

void testApp::keyPressed(int key) {
    if (key == 'r') {
        tracker.reset();
    }
}
