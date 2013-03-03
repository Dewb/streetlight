//
//  adKinectServer.h
//  adServer
//
//  Created by Michael Dewberry on 12/23/12.
//
//

#ifndef __adServer__adKinectServer__
#define __adServer__adKinectServer__

#include "adserverApp.h"
#include "ofxKinect.h"
#include "ofxSyphon.h"


class adKinectServer : public adDepthCameraServer
{
public:
    void setup();
	void update();
	void drawPreview(int x, int y, int w, int h);
	void exit();
    
    void keyPressed(int key);
    
protected:
    void drawPointCloud();
    
    bool bDrawPointCloud;
    int nearThreshold;
	int farThreshold;
	int angle;
    
	ofxKinect _kinect;
    
    ofPixels _pixels;
    ofTexture _texture;
    ofxSyphonServer _syphonServer;
	
	// used for viewing the point cloud
	ofEasyCam _easyCam;
    
};

#endif /* defined(__adServer__adKinectServer__) */
