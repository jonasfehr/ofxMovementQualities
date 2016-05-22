//
//  ofxMovementQualities
//  LeapTracker
//
//  Created by Jonas Fehr on 21/12/15.
//
//

#include "ofxMovementQualities.h"

void qualityExtractWindow::setup(int _length)
{
    length = _length;
    velTreshold = 100;
    maxMovement = 50;
    maxVelocity = 800;
    
    filterPos.init(0.1400982208, -0.0343775491, 0.0454003083, 0.0099732061, 0.0008485135,  1, -1.9185418203, 1.5929378702, -0.5939699187, 0.0814687111); // 1.5 Hz
    //filterPos.init(0.1851439645, 0.1383283833, 0.1746892243, 0.1046627716, 0.0464383730,     1,     -1.2982434912, 1.4634092217, -0.7106501488, 0.2028836637); //4.5 Hz
    //filterPos.init(0.3730569536, 0.8983119412, 0.9660856693, 0.5189611913, 0.1099005390,         1,       0.8053107424, 0.8110594452,  0.2371869724, 0.0849291749); // 7.5 Hz
    

}


void qualityExtractWindow::add(ofVec3f _position)
{
    
    QualityFeatures newInput;
    int winQuSize = winQualityFeatures.size();
    QualityFeatures lastInput;
    if(winQualityFeatures.size() > 0) lastInput = winQualityFeatures.back();
    
    // Protect of jitter
    float dist = lastInput.position.distance(_position);

    
//   if( dist > maxMovement ) winQualities.clear();// DO SOMETHING idea: set flag and when the next call, calculate midpoint to the previous
    
    
    // Position    
    newInput.position = filterPos.calc(_position);
    
    // Distance
    dist = lastInput.position.distance(newInput.position);
    newInput.distance = dist;
    
    // calculate Velocity

    newInput.velocity = dist / ofGetLastFrameTime();
    
    // detect if active
    if(newInput.velocity > velTreshold){
        newInput.active = true;
    }else{
        newInput.active = false;
    }
    
    // Create unit Vector of direction moving
    ofVec3f direction = newInput.position.operator-(lastInput.position);
    direction.normalize();
    newInput.direction = direction;
    
    // add new points to vector;
    winQualityFeatures.push_back(newInput);
    
    // make window slide
    if(winQualityFeatures.size()>length) winQualityFeatures.erase(winQualityFeatures.begin());
}

void qualityExtractWindow::update(){
    int windowLength = winQualityFeatures.size();
    activity = 0;
    energy = 0;
    consistence = 0;
    
    ofVec3f pointA =   winQualityFeatures[0].position;
    ofVec3f pointB =   winQualityFeatures[windowLength].position;
    
    float pathLength;
    
  //  ofVec3f generalDirection = (pointA.operator-(pointB)).normalized();

    
    for(int i = 0; i<windowLength; i++)
    {
        if (winQualityFeatures[i].active) activity++;
        
        energy += winQualityFeatures[i].velocity;
        
        pathLength += winQualityFeatures[i].distance;
        
     //   consistence += generalDirection.dot(winQualities[i].direction);

    }
    //alternative approach
    int numOfSam = 10;
    int stepsBetween = windowLength/numOfSam;
    for(int i = 0; i<numOfSam; i++)
    {
        if(i < numOfSam-1){
            float ratioDirection = winQualityFeatures[i*stepsBetween].direction.dot(winQualityFeatures[(i+1)*stepsBetween].direction);
            
            consistence += ratioDirection;
            
        }
        
        
        
    }
    
    

    activity /= windowLength;
    
    
    energy /= windowLength;
    energy /= maxVelocity;
    
    directivity = pointA.distance(pointB)/pathLength; // Maybe sqrt to get an exponential curve??
    
    consistence /= numOfSam;
    if(consistence < 0) consistence = 0;
  
}

void qualityExtractWindow::draw(int _x, int _y){
    int gap = 30;

    
    ofPushMatrix();
    ofTranslate(_x, _y);

    
    ofSetColor(255);
    ofDrawBitmapString(name+" size: " + ofToString(length), 0, 0.5*gap);
    ofDrawBitmapString("Activity    : " + ofToString(activity), 0, gap);
    ofDrawBitmapString("Energy      : " + ofToString(energy), 0, 2*gap);
    ofDrawBitmapString("Directivity : " + ofToString(directivity), 0, 3*gap);
    ofDrawBitmapString("Consistence : " + ofToString(consistence), 0, 4*gap);
    ofDrawBitmapString("maxV: " + ofToString(maxVelocity), 0, 5*gap);
    ofDrawBitmapString("maxM: " + ofToString(maxMovement), 0, 5.5*gap);
    ofDrawBitmapString("tVel: " + ofToString(velTreshold), 0, 6*gap);


    ofSetLineWidth(1);
    ofSetColor(255, 255);
    ofNoFill();
    ofDrawRectangle(0, gap+5, 200, 10);
    ofDrawRectangle(0, 2*gap+5, 200, 10);
    ofDrawRectangle(0, 3*gap+5, 200, 10);
    ofDrawRectangle(0, 4*gap+5, 200, 10);


    ofFill();
    ofDrawRectangle(0, gap+5, 200*activity, 10);
    ofDrawRectangle(0, 2*gap+5, 200*energy, 10);
    ofDrawRectangle(0, 3*gap+5, 200*directivity, 10);
    ofDrawRectangle(0, 4*gap+5, 200*consistence, 10);
    
    ofSetColor(255, 50);
    ofDrawRectangle(-4, 0, 208, 6.*gap);
    ofPopMatrix();

}
void qualityExtractWindow::drawTracking(int _x, int _y){
    ofPushMatrix();
    ofTranslate(_x,_y);
    ofPolyline poly;
    for(int i = 0; i < winQualityFeatures.size(); i++){
        poly.addVertex(winQualityFeatures[i].position);

    }
    poly.draw();
    
    ofPopMatrix();
}

void qualityExtractWindow::setTresholdVelocity(ofVec3f _position){
   
    float dist = lastPosForMax.distance(_position);
    lastPosForMax = _position;
    float velocity = dist / ofGetLastFrameTime();
    velTreshold = velTreshold*0.81 + velocity*0.1;
    velTreshold *= 1.1; // add 10%
}

void qualityExtractWindow::setMaxSpeed(ofVec3f _position){
    
    float dist = lastPosForMax.distance(_position);
    lastPosForMax = _position;
    float velocity = dist / ofGetLastFrameTime();
    maxVelocity = maxVelocity*0.8 + velocity*0.2;
    maxMovement = maxMovement*0.8 + dist*0.2;
    cout<< ofToString(velocity)+"\n";
}
void qualityExtractWindow::sendOsc(ofxOscSender &sender){
    ofxOscMessage m;
    m.setAddress("/"+name);
    m.addFloatArg(activity);
    m.addFloatArg(energy);
    m.addFloatArg(directivity);
    m.addFloatArg(consistence);
    
    sender.sendMessage(m);
}