
#include "ofMain.h"
#include "AppShell.hpp"

class ofApp : public ofBaseApp{
    
    AppShell shell;

    void setup(){
        #ifndef __ARM_ARCH
        ofSetWindowTitle("shiftguard");
        #endif
        
        shell.audioID = 0;
        shell.setup();
    }

    void update(){
        shell.update();
    }

    void draw(){
        shell.draw();
    }

    void keyPressed(int key){
        shell.keyPressed( key );
    }

}; // class ofApp

int main(){

#ifdef __ARM_ARCH
    ofGLESWindowSettings settings;
    settings.glesVersion = 2;
    settings.setSize( 800, 480 );
    ofCreateWindow(settings); 
#else        
	ofSetupOpenGL( 800, 480, OF_WINDOW);	
#endif

	ofRunApp( new ofApp() );
}
