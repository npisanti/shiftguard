
#include "SplashScreen.h"

np::SplashScreen::SplashScreen(){
    boxw = 8*27;
    boxh = 18;
    boxgap = 22;
    boxy = 44;
    
    selected = 0;
    
    samplerate = 44100;
    buffersize = 512;
    numBuffers = 3;
    started = false;
}

void np::SplashScreen::setup( pdsp::Engine & engine, int w, int h ){
    this->engine = &engine;
    width = w;
    height = h;
    
    const auto & list = engine.listDevices();
    
    devices.resize( list.size() );
    for(size_t i=0; i<list.size(); ++i ){
        devices[i] = list[i].name;
    }

    static const int maxlen = 23;
    for(size_t i=0; i<devices.size(); ++i ){
        if( devices[i]=="" ){
            devices[i]="unidentified";
        }
        if(devices[i].length() > maxlen ){
            devices[i].resize(maxlen);
        }
    }
    
    if( devices.size() == 1 ){
        startEngine();
    }
}

void np::SplashScreen::draw(){
    
    ofNoFill();
    
    int yhint = boxy - boxgap;
    
    int bx = (width-boxw)/2;
    
    ofDrawBitmapString( "<< select your soundcard >>", bx, yhint+13 );
    
    for(size_t i=0; i<devices.size(); ++i ){
        int by = boxy + i*boxgap;
        if( int(i) == selected ) ofDrawRectangle( bx, by, boxw, boxh );
        ofDrawBitmapString( devices[i], bx+(8*3), by+13 );
    }
}

    
void np::SplashScreen::keyPressed( int key ){
    switch( key ){
        case OF_KEY_DOWN:
            selected++;
            if( selected >= int(devices.size()) ){ selected = 0; }
        break;
        
        case OF_KEY_UP:
            selected--;
            if( selected < 0 ){ selected = devices.size()-1; }
        break;
        
        case OF_KEY_RETURN:
            startEngine();
        break;
    }
}

    
void np::SplashScreen::mousePressed( int x, int y ){
    
    int bx = (width-boxw)/2;
        
    for(size_t i=0; i<devices.size(); ++i ){
        int by = boxy + i*boxgap;
        
        if( x > bx && x < (bx+boxw) && y > by && y< by+boxh ){
            selected = i;
            startEngine();
        }
    }
}

void np::SplashScreen::startEngine(){
    engine->setDeviceID( selected ); 
    engine->setup( samplerate, buffersize, numBuffers ); 
    started = true;
}


