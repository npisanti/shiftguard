
#include "ofMain.h"
#include "AppShell.hpp"
#include "SplashScreen.h"

class ofApp : public ofBaseApp{
    
    AppShell shell;
    np::SplashScreen splash;

    void setup(){
        #ifndef __ARM_ARCH
        ofSetWindowTitle("shiftguard");
        #endif
        
        shell.setup();
        
        splash.setup( shell.engine, shell.fbo.getWidth(), shell.fbo.getHeight() );
    }

    void update(){
        if( splash.done() ){
            shell.update();
        }else{
            shell.fbo.begin();
                ofClear(0, 0, 0, 0 );
                splash.draw();
            shell.fbo.end();
        }
    }

    void draw(){
        shell.draw();
    }

    void keyPressed(int key){
        if( splash.done() ){
            shell.keyPressed( key );
        }else{
            splash.keyPressed( key );
        }
    }
    
    void mousePressed( int x, int y, int button ){
        int fx = ofMap( x, 0, ofGetWidth(), 0, shell.fbo.getWidth() );
        int fy = ofMap( y, 0, ofGetHeight(), 0, shell.fbo.getHeight() );
        
        if( splash.done() ){
            shell.mousePressed( fx, fy );
        }else{
            splash.mousePressed( fx, fy );
        }
    }
    
    void windowResized(int w, int h){
        ofSetWindowShape( w, w*0.6 );
    }

}; // class ofApp

int main(){

#ifdef __ARM_ARCH
    ofGLESWindowSettings settings;
    settings.glesVersion = 2;
    settings.setSize( 800, 480 );
    ofCreateWindow(settings); 
#else        
    ofGLFWWindowSettings settings;

    settings.setSize( 800, 480 );
    settings.decorated = false;
    settings.windowMode = OF_WINDOW;
    ofCreateWindow(settings);

    int sw = ofGetScreenWidth();
    int sh = ofGetScreenHeight();
    
    if( sw > 1920 ){
        int ratio = sw / 800;
        ofSetWindowShape( 400*ratio, 240*ratio );
    }
    ofSetWindowPosition( ofRandom( 0, sw/2), ofRandom( 0, sh/4) );
    
#endif

	ofRunApp( new ofApp() );
}
