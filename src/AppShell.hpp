
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxPDSP.h"

#include "dotfrag/Live.h"
#include "routines/downsample.h"
#include "synth/SinePercussion.h"
#include "synth/NoiseDevice.h"
#include "sequence/Tracker.h"

class AppShell {
public:

// platform-specific settings here
int samplerate = 44100;
int buffersize = 512;
int audioID = 0;

// ---------- variables ----------------------------------------

float uiSide, uiCompY, uiX;

ofFbo fbo;

int downsample = 2;

ofx::dotfrag::Live map;        
    ofParameter<float> u_plane;
    ofParameter<ofColor> u_color_a;
    ofParameter<ofColor> u_color_b;

ofFbo rFbo;
ofx::dotfrag::Live readings;        
    ofParameter<float> rSpeed;
    ofParameter<float> rMax;
float rMargin = 6;

pdsp::SequencerSection *    section;
pdsp::Engine                engine;
np::synth::SinePercussion   perc;
np::synth::NoiseDevice      noise;
np::sequence::Tracker       alarm;
np::sequence::Tracker       preshift;
np::sequence::Tracker       shiftseq;
pdsp::Sequence              aftershift;

ofxPanel gui;
bool bDrawGui = false;

ofParameter<bool> testColors { "test colors", false };
ofParameter<ofColor> dangerA { "danger A", ofColor(255) };
ofParameter<ofColor> dangerB { "danger B", ofColor(255) };
ofParameter<ofColor> uiDark { "ui dark", ofColor(130) };
ofParameterGroup colors { "colors", testColors, dangerA, dangerB, uiDark };

ofParameter<float> threshold { "shift threshold", 0.5f, 0.0f, 1.0f };
ofParameter<bool> armed{ "armed", false };

float oldPlane;
float newPlane;

//--------------------------------------------------------------
inline void setup(){

    ofDisableAntiAliasing(); 
    if( !bDrawGui ) ofHideCursor();
    
    ofBackground( 0 );
    ofSetCircleResolution(32);
        
    map.load( ofToDataPath( "map.frag" ) );
    map.timewarp();
    map.uniform( u_plane, "u_plane", false );
    map.uniform( u_color_a, "u_color_a", false );
    map.uniform( u_color_b, "u_color_b", false );
    u_plane = ofRandom( 42 );
    
    readings.load( ofToDataPath( "readings.frag" ) );
    readings.timewarp();
    readings.uniform( u_plane, "u_plane" );
    readings.uniform( rSpeed.set("writing speed", 0.2f, 0.0f, 1.0f), "u_speed" );
    readings.uniform( rMax.set("writing max", 1.0f, 0.0f, 1.0f), "u_max" );

    np::fbo_init_downsampled( fbo, ofGetWidth(), ofGetHeight(), downsample );

    uiSide = fbo.getHeight() * 0.35f;
    uiCompY = fbo.getHeight()*(0.65f);
    uiX = fbo.getWidth() - uiSide;
    rFbo.allocate( uiSide-rMargin*2, uiCompY-rMargin*2 );
    
    // ----------- SEQUENCES ----------
    engine.sequencer.init( 1, 4, 118.0f); // sections, sequences, tempo
    section = &engine.score.sections[0];
    
    alarm.load( ofToDataPath("alarm.seq") );
    preshift.load( ofToDataPath("preshift.seq") );
    shiftseq.load( ofToDataPath("shift.seq") );
    aftershift.bars = 4.0f; // some time before a new shift can happen
   
    section->setCell( 0, &alarm, pdsp::Behavior::OneShot );
    section->setCell( 1, &preshift, pdsp::Behavior::Next );
    section->setCell( 2, &shiftseq, pdsp::Behavior::Next  );
    section->setCell( 3, &aftershift, pdsp::Behavior::OneShot  );

    // ----------- SIGNAL ------------- 
    section->out_trig(0)  >> perc.in("trig"); 
    section->out_value(1) >> perc.in("pitch"); 
    section->out_trig(2)  >> noise.in("trig"); 
    
    perc >> engine.audio_out(0);
    perc >> engine.audio_out(1);
    
    noise.ch(0) >> engine.audio_out(0);
    noise.ch(1) >> engine.audio_out(1);
    
    // -------------- GUI ------------
    gui.setup( "gui", "settings.xml", 20, 20 );
    gui.add( map.parameters );
    gui.add( readings.parameters );
    gui.add( colors );
    gui.add( alarm.parameters );
    gui.add( preshift.parameters );
    gui.add( shiftseq.parameters );
    gui.add( perc.parameters );
    gui.add( noise.parameters );
    gui.add( armed );
    gui.add( threshold );
    gui.loadFromFile( "settings.xml" );
    gui.minimizeAll();
    
    // ----------- ENGINE ------------- 
    engine.listDevices();
    engine.setDeviceID( audioID ); 
    engine.setup( samplerate, buffersize, 3); 
}

//--------------------------------------------------------------
inline void update(){
    int mode = section->meter_current();
    
    if( mode==1 || mode==2 ){
        rSpeed = 0.5f;
        rMax = 1.0f;
    }else{
        rSpeed = 0.15f;
        rMax = 0.2f;
    }
    
    if( mode==1 ){
        float pct = noise.meter();
        u_plane = pct*newPlane + (1.0f-pct)*oldPlane;
        mapColors( 1.0f-pct );
    }else if( mode==2 ){
        float pct = section->meter_percent();
        if( pct>0.0f ){
            u_plane = pct*newPlane + (1.0f-pct)*oldPlane;
            mapColors( pct );
        }
    }else if( testColors ){
        mapColors( 0.0f );
    }else{
        mapColors( 1.0f );
    }    
    
    map.drawTo( fbo );
    
    readings.drawTo( rFbo );
    
    fbo.begin();
        shiftcompass( uiX, uiCompY, uiSide, uiSide );

        ofFill();
        ofSetColor( 0 );
        ofDrawRectangle( uiX, 0, uiSide, uiCompY );
        
        if( mode==1 || mode==2  || testColors ){
            ofSetColor( dangerA );
        }else{
            ofSetColor( 255 );
        }
        rFbo.draw( uiX + rMargin, rMargin );
        
        ofNoFill();
        ofSetColor( uiDark );
        ofDrawRectangle( uiX, uiCompY, uiSide-1, uiSide-1 );
        ofDrawRectangle( uiX, 1, uiSide-1, uiCompY-1 );
    fbo.end();
}

inline void draw(){
    ofSetColor(255);
    fbo.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    if( bDrawGui ) gui.draw();
}

//--------------------------------------------------------------
void mapColors( float transition ){
    u_color_a = dangerA.get().getLerped( ofColor::white, transition );
    u_color_b = dangerB.get().getLerped( uiDark.get(), transition );
}

void beginShift( float direction ){
    section->launchCell( 1 ); 
    readings.setTime( rand()%4242 * 2 );
    oldPlane = u_plane;
    newPlane = oldPlane + direction; 
}

void shiftcompass( int x, int y, int w, int h ){
    int mode = section->meter_current();
    
    ofPushMatrix();
        ofTranslate( x + w*0.5f, y + h*0.5f );

        float radmin = w * 0.02f;
        float radno  = w * 0.48f;
        
        ofSetColor( 0 );
        ofFill();
        ofDrawRectangle( -w*0.5f, -h*0.5f, w, h );
        
        ofNoFill();
        if( mode==1 || mode==2 ){
            ofSetColor( dangerB );
        }else{
            ofSetColor( uiDark );
        }
        
        ofDrawCircle( 0, 0, radmin + radno*(0.5f - threshold*0.5f) );
        ofDrawCircle( 0, 0, radmin + radno*(0.5f + threshold*0.5f) );
        
        static const int n = 32;
        static const float inc = M_TWO_PI / float(n);
        static const float nOffset = 0.7f;
        float speed = map.speed;
        speed = speed*speed*0.03f;
        
        float theta = -M_PI*0.5f; 
        
        // calculate probability ---------------------
        float noise0 = ofNoise( cos( theta )*nOffset, sin( theta )*nOffset, ofGetElapsedTimef()*speed );
        
        float direction = noise0*2.0f - 1.0f;;
        float chance = ofMap( abs(direction), threshold, 1.0f, 0.0f, 1.0f, true );
        

        // check for shifting 
        if( chance > 0.0f && armed && section->meter_percent() == 0.0f && mode < 1 ){
            if( ofRandomuf() < chance ){
                beginShift( direction ); 
            }else{
                section->launchCell( 0 );
            }
        }
        // -------------------------------------------

        
        if( chance > 0.0f ){
            if( mode==3 ){
                ofSetColor( dangerB );
            }else{
                ofSetColor( dangerA );
            }
        } else if( mode==1 || mode==2 ){
            ofSetColor( dangerA );
        } else if( testColors ){ 
            ofSetColor( dangerA );
        } else {
            ofSetColor( 255 );
        }

        for( int i=0; i<n; ++i ){
            float cost = cos( theta );
            float sint = sin( theta );
            float noise = ofNoise( cost*nOffset, sint*nOffset, ofGetElapsedTimef()*speed );            
            float x = cost * ( radmin + radno*noise );
            float y = sint * ( radmin + radno*noise );
            if( i==0 ){
                ofFill();
                ofDrawCircle( x, y, 2);
                ofNoFill();
                ofBeginShape();                
            }
            ofVertex( x, y );
            theta+=inc; 
        }
        ofEndShape( true );
        
    ofPopMatrix();
}

//--------------------------------------------------------------
inline void keyPressed(int key){
    switch( key ){    
        case '1': section->launchCell( 0 ); break;
        
        case '2': 
            beginShift( ofRandom( -1.0f, 1.0f) );
        break;

        case 'g': 
            bDrawGui= !bDrawGui; 
            if(bDrawGui){
                ofShowCursor();
            }else{
                ofHideCursor();
            }
        break;
    }
}

//--------------------------------------------------------------

}; // class ofApp


