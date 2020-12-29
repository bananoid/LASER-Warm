#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
  
  laserWidth = 800;
  laserHeight = 800;
  laser.setup(laserWidth, laserHeight);
  

  laser.addProjector(dac);

#if defined(USE_LASERDOCK) || defined(USE_HELIOS)
  // NB with laser dock you can pass a serial number,
  // with HeliosDAC you can pass a device name
  dac.setup("Helios 825438257");
#else
  // load the IP address of the Etherdream / IDN DAC
  ofBuffer buffer = ofBufferFromFile("dacIP.txt");
  string dacIp = buffer.getText();
  // if there's no file, then use the default IP address :
  if(dacIp=="") dacIp ="10.0.1.130";
  dac.setup(dacIp);
#endif
  
  
  
  // if you don't want to manage your own GUI for your app you can add extra
  // params to the laser GUI
  laser.addCustomParameter(color.set("color", ofColor(0, 255, 0), ofColor(0), ofColor(255)));
    laser.initGui(true);
    currentLaserEffect = 0;
    numLaserEffects = 8;
   
  //Audio Setup
  soundStream.printDeviceList();
  ofSoundStreamSettings settings;
  auto devices = soundStream.getMatchingDevices("default");
  if(!devices.empty()){
    settings.setInDevice(devices[0]);
  }
  
  left.assign(bufferSize, 0.0);
  right.assign(bufferSize, 0.0);
  volHistory.assign(400, 0.0);
  
  bufferCounter  = 0;
  drawCounter    = 0;
  smoothedVol     = 0.0;
  scaledVol    = 0.0;
  
  settings.setInListener(this);
  settings.sampleRate = 44100;
  settings.numOutputChannels = 0;
  settings.numInputChannels = 2;
  settings.bufferSize = bufferSize;
  soundStream.setup(settings);
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input){
  
  float curVol = 0.0;
  
  // samples are "interleaved"
  int numCounted = 0;

  //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
  for (size_t i = 0; i < input.getNumFrames(); i++){
    left[i]    = input[i*2]*0.5;
    right[i]  = input[i*2+1]*0.5;

    curVol += left[i] * left[i];
    curVol += right[i] * right[i];
    numCounted+=2;
  }
  
  //this is how we get the mean of rms :)
  curVol /= (float)numCounted;
  
  // this is how we get the root of rms :)
  curVol = sqrt( curVol );
  
  smoothedVol *= 0.93;
  smoothedVol += 0.07 * curVol;
  
  bufferCounter++;
  
}

//--------------------------------------------------------------
void ofApp::update(){
    
  float deltaTime = ofClamp(ofGetLastFrameTime(), 0, 0.2);
  elapsedTime+=deltaTime;
    
    // prepares laser manager to receive new points
    laser.update();
  
}


void ofApp::draw() {
  
  ofBackground(40);
  
  int ypos = laserHeight+20;
  ofDrawBitmapString("Current Effect : "+ofToString(currentLaserEffect), 400, ypos+=30);
    ofDrawBitmapString("TAB to change view, F to toggle full screen", 400, ypos+=30);
  ofDrawBitmapString("Left and Right Arrows to change current effect", 400, ypos+=30);
  ofDrawBitmapString("Mouse to draw polylines, 'C' to clear", 400, ypos+=30);
    
    showLaserEffect(currentLaserEffect);

    // sends points to the DAC
    laser.send();

    laser.drawUI();


}


void ofApp :: showLaserEffect(int effectnum) {
    
    
  float left = laserWidth*0.1;
  float top = laserHeight*0.1;
  float right = laserWidth*0.9;
  float bottom = laserHeight*0.9;
  float width = laserWidth*0.8;
  float height = laserHeight*0.8;
  
  switch (currentLaserEffect) {
      
    case 1: {

      // LASER LINES
      int numlines = 10;
      
      for(int i = 0; i<numlines; i++) {
        
        float progress =(float)i/(float)(numlines-1);

        float xpos =left + (width*progress);
                  
        laser.drawLine(ofPoint(xpos, top+height*0.1), ofPoint(xpos, top+height*0.4), ofColor(255));
              
        ofColor c;
        c.setHsb(progress*255, 255, 255);
                
        laser.drawLine(ofPoint(xpos, top+height*0.6), ofPoint(xpos, top+height*0.9), c);
    
      }

      break;

    }
    
      
    case 2: {
      
      // LASER LINES ANIMATING
      int numlines = 10;
      
      for(int i = 0; i<numlines; i++) {
        
        float progress =(float)i/(float)(numlines-1);
        
        float xpos =left + (width*progress) + (sin(elapsedTime*4+i*0.5)*width*0.05);
        
        laser.drawLine(ofPoint(xpos, top+height*0.1), ofPoint(xpos, top+height*0.4), ofColor(255));
        ofColor c;
        c.setHsb(progress*255, 255, 255);
        laser.drawLine(ofPoint(xpos, top+height*0.6), ofPoint(xpos, top+height*0.9), c);
        
      }
      
      break;
      
    }
    
      
    case 3: {
      
      // LASER CIRCLES
      int numCircles = 6;
      
      for(int i = 0; i<numCircles; i++) {
        
        float progress =(float)i/(float)(numCircles-1);
        
        float xpos =left + (width*progress);
        
        laser.drawCircle(ofPoint(xpos, top+height*0.3),30, ofColor(255));
        ofColor c;
        c.setHsb(progress*255, 255, 255);
        
        laser.drawCircle(ofPoint(xpos, top+height*0.7), 30, c);
        
      }
      
      break;
      
    }
      
    case 4: {
      
      // LASER CIRCLES ANIMATING
      int numCircles = 6;
      
      for(int i = 0; i<numCircles; i++) {
        
        float progress =(float)i/(float)(numCircles-1);
        
        float xpos =left + (width*progress) + (sin(elapsedTime*4+i*0.5)*width*0.05);
        
        laser.drawCircle(ofPoint(xpos, top+height*0.3), 30, ofColor::white);
        ofColor c;
        c.setHsb(progress*255, 255, 255);
        
        laser.drawCircle(ofPoint(xpos, top+height*0.7), 30, c);
        
      }
      
      break;
      
    }
      
    case 5: {
      
      // LASER PARTICLES
      int numParticles = 20;
      
      for(int i = 0; i<numParticles; i++) {
        
        float progress =(float)i/(float)(numParticles-1);
        
        float xpos =left + (width*progress) ;
        
        laser.drawDot(ofPoint(xpos, top+height*0.3), ofColor(255));
        ofColor c;
        c.setHsb(progress*255, 255, 255);
        laser.drawDot(ofPoint(xpos, top+height*0.7), c);
        
      }
      
      break;
      
    }
    case 6: {
      
      // LASER PARTICLES ANIMATING
      
      float speed = 1;
      for(int i = 0; i<30; i++) {
    
        ofColor c;
        c.setHsb(i*6,255,255);
        ofPoint p;
        float spread = ofMap(cos(elapsedTime*0.4),1,-1,0.01,0.1);
        p.x = sin((elapsedTime-((float)i*spread)) *1.83f * speed) * 300;
        p.y = sin((elapsedTime-((float)i*spread)) *2.71f *speed) * 300;
        p.x+=laserWidth/2;
        p.y+=laserHeight/2;
                
        laser.drawDot(p, c);
        
      }
      
      break;
      
    }
      
  }
  
  float speed = 1;

  polyLines.clear();
  polyLines.push_back(ofPolyline());
  ofPolyline &poly = polyLines.back();
  
  
  //lets scale the vol up to a 0-1 range
  scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

  //lets record the volume into an array
  volHistory.push_back( scaledVol );
  
  //if we are bigger the the size we want to record - lets drop the oldest value
  if( volHistory.size() >= 400 ){
    volHistory.erase(volHistory.begin(), volHistory.begin()+1);
  }
  
  for (int i = 0; i < volHistory.size(); i++) {
    ofPoint p;
      
    float spread = 0.04;
    float rad = volHistory[i] * 300 + 50 ;
    p.x = sin((elapsedTime-((float)i*spread)) *1.83f * speed) * rad;
    p.y = sin((elapsedTime-((float)i*spread)) *2.71f *speed) * rad;
    p.x+=laserWidth/2;
    p.y+=laserHeight/2;
    
    poly.addVertex(p.x, p.y);
    
  }

  // LASER POLYLINES
  for(size_t i = 0; i<polyLines.size(); i++) {
    laser.drawPoly(polyLines[i], color );
  }
  
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if(key =='c') {
    polyLines.clear();
  } else if (key == OF_KEY_LEFT) {
    currentLaserEffect--;
    if(currentLaserEffect<0) currentLaserEffect = numLaserEffects-1;
  } else if (key == OF_KEY_RIGHT) {
    currentLaserEffect++;
    if(currentLaserEffect>=numLaserEffects) currentLaserEffect = 0;
  }
  if(key=='f') {
        ofToggleFullscreen();
  }
    if(key==OF_KEY_TAB) {
        laser.nextProjector();
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  if(!drawingShape) return;
  
  ofPolyline &poly = polyLines.back();
  poly.addVertex(x, y);

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  polyLines.push_back(ofPolyline());
  drawingShape = true;
}

void ofApp::mouseReleased(int x, int y, int button) {
  if(drawingShape) {
    ofPolyline &poly = polyLines.back();
    poly = poly.getSmoothed(2);
    drawingShape = false;
  }
  // TODO add dot if the line is super short
  
}

//--------------------------------------------------------------
void ofApp::exit(){
  laser.saveSettings();
}

