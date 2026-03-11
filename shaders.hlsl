cbuffer FrameCB : register(b0) {
	float aspect;
	float3 _pad;
}

struct vs_out {
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 uv  : TEXCOORD0;
};

float sd_circle(float2 p, float r) {
	return length(p) - r;
}

vs_out vs_main(uint vertexid : SV_VERTEXID) {
	float2 verts[3] = {
	    float2(-1,-1), float2(3,-1), float2(-1,3)
	};
	vs_out o;
	o.col = float4( vertexid == 0, vertexid == 1, vertexid == 2, 1 );
	o.pos = float4(verts[vertexid], 0, 1);
	o.uv  = 0.5 * (verts[vertexid] + 1.0); // map [-1,1] -> [0,1]
	return o;
}

float4 ps_main(vs_out input) : SV_TARGET {
	float2 uv = input.uv;
	uv = (uv * 2.0) - 1.0; // [-1 ,1]
	uv.x *= aspect;

	float d = sd_circle(uv, 0.5);

	float3 col = d > 0.0 ? float3(0.9, 0.6, 0.3) : float3(0.5, 0.8, 1.0);
	col *= 1.0 - exp(-0.6 * abs(d));
	col *= 0.8 + 0.2 * cos(150.0 * d);
	col  = lerp(col, float3(1.0, 1.0, 1.0), 1.0 - smoothstep(0.0, 0.01, abs(d)));
		
	return float4(col, 1.0);
}
