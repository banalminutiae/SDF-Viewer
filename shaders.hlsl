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

	float d = sd_circle(uv, 0.6);
	float aa = fwidth(d);
	float edge = smoothstep(-aa, aa, -d); // 1.0 inside, 0.0 outside
	float wave = 0.5 + 0.5 * sin(d * 30.0 * 4.0); // TODO: Time delta thing
	float3 pattern = edge == 0
		? lerp(float3(0.2,0.3,0.8), float3(1.0,0.6,0.2), wave)
		: lerp(float3(0.8, 0.6, 0.1), float3(0.1, 0.3, 0.8), wave);
	return float4(pattern, 1.0);
}
