struct vs_out {
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 uv  : TEXCOORD0;
};

cbuffer FrameCB : register(b0) {
	float aspect;
	float3 interior_color;
	float3 exterior_color;
	int shape;
}
