//
//  qualityExtractor.h
//  LeapTracker
//
//  Created by Jonas Fehr on 21/12/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxIIR4.h"
#include "ofxOsc.h"



struct QualityFeatures
{
    ofVec3f position;
    float velocity;
    float distance;
    bool active;
    ofVec3f direction;
};


class qualityExtractWindow{
    
public:
    void setup(int _length);
    void add(ofVec3f _position);
    void update();
    void draw(int _x, int _y);
    void drawTracking(int _x, int _y);
    void setTresholdVelocity(ofVec3f _position);
    void setMaxSpeed(ofVec3f _position);
    void sendOsc(ofxOscSender &sender);


    vector <QualityFeatures> winQualityFeatures;
    
    float activity;
    float directivity;
    float energy;
    float consistence;
    string name = "noName";
    
    
private:
    int length;
    float velTreshold;
    float maxMovement;
    float maxVelocity;
    ofVec3f lastPosForMax;
    
    ofxIIR4Vec3f filterPos;

};
class extract{
public:

};
