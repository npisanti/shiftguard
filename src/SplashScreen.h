
#pragma once

#include "ofMain.h"
#include "ofxPDSP.h"

namespace np{
    
class SplashScreen{
    
public:
    SplashScreen();
    
    int buffersize;
    int samplerate;
    int numBuffers;
    
    void setup( pdsp::Engine & engine, int w, int h );
    
    void draw();
    
    bool done() const { return started; }
    
    void keyPressed( int key );
    
    void mousePressed( int x, int y );

private:
    int boxw;
    int boxh;
    int boxgap;
    int boxy;
    int width; 
    int height;
    
    int selected;
    
    pdsp::Engine * engine;
    bool started;
    
    std::vector<std::string> devices;
    
    void startEngine();
};
    
}

