
#pragma once

#include "ofMain.h"

namespace np {

    inline void fbo_init_downsampled( ofFbo & fbo, int w, int h, int downsample ){
        ofFboSettings settings;
        settings.width = w/downsample;
        settings.height = h/downsample;
        settings.minFilter = GL_NEAREST;
        settings.maxFilter = GL_NEAREST;
        fbo.allocate( settings );
        fbo.begin();
            ofClear( 0, 0, 0, 0 );
        fbo.end();
    }
    
}
