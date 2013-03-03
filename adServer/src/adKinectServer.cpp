//
//  adKinectServer.cpp
//  adServer
//
//  Created by Michael Dewberry on 12/23/12.
//
//

#include "adKinectServer.h"

void adKinectServer::setup() {
	
	_kinect.setRegistration(true);
    
	_kinect.init();
	_kinect.open();
    
    _pixels.allocate(_kinect.width, _kinect.height, OF_PIXELS_RGBA);
    _texture.allocate(_kinect.width, _kinect.height, GL_RGBA);
    
    _syphonServer.setName("Kinect " + _kinect.getSerial());
	
	nearThreshold = 255;
	farThreshold = 70;
		
	angle = 0;
	_kinect.setCameraTiltAngle(angle);
	
	bDrawPointCloud = false;
}

void adKinectServer::update() {
	
	_kinect.update();
	
	if(_kinect.isFrameNew()) {
		
        const unsigned char* rgbPix = _kinect.getPixels();
        const unsigned char* depPix = _kinect.getDepthPixels();
        
        int n = _kinect.width * _kinect.height;
        for(int i = 0; i < n; i++) {
            _pixels[4*i]   = rgbPix[3*i];
            _pixels[4*i+1] = rgbPix[3*i+1];
            _pixels[4*i+2] = rgbPix[3*i+2];
            
            if (depPix[i] < nearThreshold && depPix[i] > farThreshold) {
                _pixels[4*i+3] = depPix[i];
            } else {
                _pixels[4*i+3] = 0;
            }
        }
        
        _texture.loadData(_pixels);
        _syphonServer.publishTexture(&_texture);
    }
}

void adKinectServer::drawPreview(int x, int y, int w, int h)
{
	if(bDrawPointCloud) {
		_easyCam.begin();
		drawPointCloud();
		_easyCam.end();
	} else {
        _texture.draw(x,y,w,h);
    }
}

void adKinectServer::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(_kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(_kinect.getColorAt(x,y));
				mesh.addVertex(_kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	glEnable(GL_DEPTH_TEST);
	mesh.drawVertices();
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
}

void adKinectServer::keyPressed (int key) {
    switch (key) {
			
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			_kinect.enableDepthNearValueWhite(!_kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			_kinect.setCameraTiltAngle(angle); // go back to prev tilt
			_kinect.open();
			break;
			
		case 'c':
			_kinect.setCameraTiltAngle(0); // zero the tilt
			_kinect.close();
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			_kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			_kinect.setCameraTiltAngle(angle);
			break;
	}
}

void adKinectServer::exit() {
	_kinect.setCameraTiltAngle(0);
	_kinect.close();
}


