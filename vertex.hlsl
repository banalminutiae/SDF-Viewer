#include "common.hlsli"

vs_out vs_main(uint vertexid : SV_VERTEXID) {
	float2 verts[3] = {
	    float2(-1,-1), float2(3,-1), float2(-1,3)
	};
	vs_out o;
	o.col = float4(vertexid == 0, vertexid == 1, vertexid == 2, 1);
	o.pos = float4(verts[vertexid], 0, 1);
	o.uv  = 0.5 * (verts[vertexid] + 1.0); // map [-1,1] -> [0,1]
	return o;
}
