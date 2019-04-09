
// Nicola Pisanti ( npisanti.com ) - 2018

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform float u_time;

// --------------- FUNCTIONS -----------------------------------------
float rand( in float i, in float t ){
    return fract(sin(dot( vec2(i, t), vec2(12.9898,78.233))) * 43758.5453);
}

float circle_sdf(vec2 st) { 
    return length(st-.5)*2.; 
}

float rect_sdf(vec2 st, vec2 s) { st = st*2.-1.; return max( abs(st.x/s.x), abs(st.y/s.y) ); }

float stroke(float x, float d, float w){ 
    return step(d,x+w) - step(d,x-w); 
}

// -------------------------------------------------------------------


// ----------------------- SHADER CODE -------------------------------

const float u_density = 0.35;    

uniform float u_speed;
uniform float u_max;

#define ROWS 15.0
#define COLS 15.0 

float glyph( vec2 st, float index ){
    const float w = 0.08;
    
    float ra = rand( index, 1.0 );
    
    float e0 = step( 0.5, fract( ra ) );
    float e1 = step( 0.5, fract( ra * 2.0 ) );
    float e2 = step( 0.5, fract( ra * 4.0 ) );
    
    float s0 = stroke( circle_sdf( st + vec2(0.0, ra*0.5) ), 0.8, w);

    float s1 = stroke( rect_sdf( st - vec2(0.0, 0.333*0.5), vec2(1.0, 0.666) ), 0.8, w);
 
    float s2 = stroke( st.x, 0.5, 0.05 ) * stroke( st.y, 0.5, 0.85 );
    
    return s0*e0 + s1*e1 + s2*e2;
}

float write( vec2 st ){
    st.x *= COLS;
    st.y *= ROWS;
    
    float max = COLS * ROWS * u_max;
    
    vec2 tile = floor( st );
    st -= tile;

    float index = tile.x + tile.y*COLS;
    
    float cursor = floor(fract(u_time*u_speed)*max );
    
    float lines = step( index, cursor ); 
    
    float page = floor( u_time*u_speed ) * max;
    index += page;
    
    return glyph( st, index ) * lines;
}

void main(){
	vec2 st = gl_FragCoord.xy/u_resolution.xy;

    float alpha = write( st );
    //float alpha = glyph( st, floor(u_time*0.5) );
    
	vec3 color = mix( vec3(0.0), vec3 (1.0),  alpha );	
	gl_FragColor = vec4(color, 1.0 );
}
