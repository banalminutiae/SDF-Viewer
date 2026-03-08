struct vs_out {
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};

vs_out vs_main(uint vertexid : SV_VERTEXID) {
    vs_out output;

    float2 pos = float2(vertexid & 1, vertexid >> 1);

    output.pos = float4(pos * 4 - 1, 0, 1);

    output.col = float4(
        vertexid == 0,
        vertexid == 1,
        vertexid == 2,
        1
    );

    return output;
}

float4 ps_main(vs_out input) : SV_TARGET {
    return input.col;
}

/*
float4 VsMain(uint id : SV_VertexID) : SV_POSITION
{
    float2 uv = float2((id << 1) & 2, id & 2);
    return float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);
}

| id | (id << 1) & 2 | id & 2 | uv    |
| -- | ------------- | ------ | ----- |
| 0  | 0             | 0      | (0,0) |
| 1  | 2             | 0      | (2,0) |
| 2  | 0             | 2      | (0,2) |

uv * float2(2,-2) + float2(-1,1)

| uv    | clip position |
| ----- | ------------- |
| (0,0) | (-1, 1)       |
| (2,0) | (3, 1)        |
| (0,2) | (-1,-3)       |


 */
