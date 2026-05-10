#include "common.hlsli"

float sd_circle(float2 p, float r) {
	return length(p) - r;
}

float sd_equilateral_triangle(float2 p, float r) {
	const float k = sqrt(3.0);
    p.x = abs(p.x);
    p -= float2(0.5, 0.5*k) * max(p.x + k*p.y, 0.0);
    p -= float2(clamp(p.x, -r, r), -r/k );
    return length(p) * sign(-p.y);
}

float sd_box(float2 p, float2 b) {
	float2 d = abs(p) - b;
	return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float sd_union(float2 p) {
	float box = sd_box(p, float2(0.5, 0.5));
	float circle = sd_circle(p, 0.6);
	
	return min(box, circle);
}

float sd_subtraction(float2 p) {
	float tri = sd_equilateral_triangle(p, 0.7);
	float circle = sd_circle(p, 0.5);


	return max(-circle, tri);
}

float sd_intersection(float2 p) {
	float circle = sd_circle(p, 0.55);
	float box = sd_box(p, float2(0.5, 0.5));

	return max(box, circle);
}

float sd_xor(float2 p) {
	float circle = sd_circle(p, 0.55);
	float box = sd_box(p, float2(0.5, 0.5));

	return max(min(box, circle), -max(box, circle));
}

float4 ps_main(vs_out input) : SV_TARGET {
	float2 uv = input.uv;
	uv = (uv * 2.0) - 1.0; // [-1 ,1]
	uv.x *= aspect;

	float d;
	switch (shape) {
	    case 0: {
		    d = sd_circle(uv, 0.5);
			break;
	    }
	    case 1: {
		    d = sd_equilateral_triangle(uv, 0.9);
			break;
	    }
	    case 2: {
			float2 b = float2(0.3, 0.5);
		    d = sd_box(uv, b);
			break;
	    }
	    case 3: {
		    d = sd_union(uv);
			break;
	    }
	    case 4: {
			d = sd_subtraction(uv);
			break;
		}
	    case 5: {
			d = sd_intersection(uv);
			break;
		}
	    case 6: {
			d = sd_xor(uv);
			break;
	    }
	    default: {
		    d = sd_circle(uv, 0.5);
		    break;
	    }
	}

	float3 col = d > 0.0 ? exterior_color : interior_color;
	col *= 1.0 - exp(-6.0 * abs(d));
	col *= 0.6 + 0.5 * cos(150.0 * d);
	col  = lerp(col, float3(1.0, 1.0, 1.0), 1.0 - smoothstep(0.0, 0.01, abs(d)));
		
	return float4(col, 1.0);
}
