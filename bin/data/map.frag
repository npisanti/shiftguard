
#ifdef GL_ES
precision mediump float;
#endif

#define PI 3.14159265359
#define TWO_PI 6.28318530718

// ---------------------- FUNCTIONS ----------------------------------
  
// Simplex 3D noise
// 	<www.shadertoy.com/view/XsX3zB>
//	by Nikita Miropolskiy

/* discontinuous pseudorandom uniformly distributed in [-0.5, +0.5]^3 */
vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

const float F3 =  0.3333333;
const float G3 =  0.1666667;

float noise(vec3 p) {

	vec3 s = floor(p + dot(p, vec3(F3)));
	vec3 x = p - s + dot(s, vec3(G3));
	 
	vec3 e = step(vec3(0.0), x - x.yzx);
	vec3 i1 = e*(1.0 - e.zxy);
	vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
	vec3 x1 = x - i1 + G3;
	vec3 x2 = x - i2 + 2.0*G3;
	vec3 x3 = x - 1.0 + 3.0*G3;
	 
	vec4 w, d;
	 
	w.x = dot(x, x);
	w.y = dot(x1, x1);
	w.z = dot(x2, x2);
	w.w = dot(x3, x3);
	 
	w = max(0.6 - w, 0.0);
	 
	d.x = dot(random3(s), x);
	d.y = dot(random3(s + i1), x1);
	d.z = dot(random3(s + i2), x2);
	d.w = dot(random3(s + 1.0), x3);
	 
	w *= w;
	w *= w;
	d *= w;
	 
	return dot(d, vec4(52.0));
}

float noise_fractal(vec3 m) {
	return   0.5333333* noise(m)
				+0.2666667* noise(2.0*m)
				+0.1333333* noise(4.0*m)
				+0.0666667* 
                noise(8.0*m);
}

  
float stroke(float x, float d, float w){ 
    float r = step(d,x+w*.5) - step(d,x-w*.5); 
    return clamp(r, 0., 1.); 
}

// -------------------------------------------------------------------
  
uniform vec2 u_resolution;
uniform float u_time;
uniform float u_position;
uniform vec3 u_color_a;
uniform vec3 u_color_b;

void main(){

    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    
    float ratio = u_resolution.x / u_resolution.y;
    st.x *= ratio;
    st.y -= u_time * 0.03;

    float no = noise( vec3( st*2.1, u_position ) ) * 0.7 
             + noise( vec3( st*4.0, u_position ) ) * 0.27
             + noise( vec3( st*30., u_position ) ) * 0.03;
    
    float w = 0.025;
    
    vec3 color = vec3( 0.0 );
    color += u_color_a * stroke( no, 0.1, w ) * 0.5; 
    color += u_color_a * stroke( no, 0.20, w ) * 0.75; 
    color += u_color_b * stroke( no, 0.30, w ) * 0.9; 
    color += u_color_b * stroke( no, 0.40, w ); 

    gl_FragColor = vec4( color, 1.0 );

}
