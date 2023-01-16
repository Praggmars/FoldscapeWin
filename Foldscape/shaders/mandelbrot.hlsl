RWTexture2D<float4> target : register(u0);

#ifndef DOUBLE_PRECISION
#define DOUBLE_PRECISION 1
#endif

#if DOUBLE_PRECISION
#define NUMBER double
#define NUMBER2 double2
#else
#define NUMBER float
#define NUMBER2 float2
#endif

inline NUMBER2 Cpx_Re(NUMBER2 z) { return NUMBER2(z.x, 0.0); }
inline NUMBER2 Cpx_Im(NUMBER2 z) { return NUMBER2(0.0, z.y); }
inline NUMBER2 Cpx_Add(NUMBER2 z1, NUMBER2 z2) { return z1 + z2; }
inline NUMBER2 Cpx_Sub(NUMBER2 z1, NUMBER2 z2) { return z1 - z2; }
inline NUMBER2 Cpx_Mul(NUMBER2 z1, NUMBER2 z2) { return NUMBER2(z1.x * z2.x - z1.y * z2.y, z1.x * z2.y + z1.y * z2.x); }
inline NUMBER2 Cpx_Div(NUMBER2 z1, NUMBER2 z2) { return NUMBER2(z1.x * z2.x + z1.y * z2.y, z1.y * z2.x - z1.x * z2.y) / (z2.x * z2.x + z2.y * z2.y); }
inline NUMBER2 Cpx_Neg(NUMBER2 z) { return -z; }
inline NUMBER2 Cpx_Pos(NUMBER2 z) { return abs(z); }

#if !DOUBLE_PRECISION
inline float angle(float2 z)
{
	if (z.x > 0.0f)
		return atan(z.y / z.x);
	float pi = 3.141592654f;
	if (z.y > 0.0f)
		return pi * 0.5f - atan(z.x / z.y);
	if (z.y < 0.0f)
		return -pi * 0.5f - atan(z.x / z.y);
	if (z.x < 0.0f)
		return atan(z.y / z.x) + pi;
	return 0.0f;
}
inline float2 Cpx_Abs(float2 z) { return float2(length(z), 0.0f); }
inline float2 Cpx_Exp(float2 z) { return float2(cos(z.y), sin(z.y)) * exp(z.x); }
inline float2 Cpx_Log(float2 z) { return float2(log(length(z)), angle(z)); }
inline float2 Cpx_Pow(float2 z1, float2 z2) { return Cpx_Exp(Cpx_Mul(z2, Cpx_Log(z1))); }
inline float2 Cpx_Sin(float2 z) { return float2(sin(z.x) * cosh(z.y), cos(z.x) * sinh(z.y)); }
inline float2 Cpx_Cos(float2 z) { return float2(cos(z.x) * cosh(z.y), -sin(z.x) * sinh(z.y)); }
inline float2 Cpx_Tan(float2 z)
{
	float cosx = cos(z.x);
	float sinhy = sinh(z.y);
	return float2(sin(z.x) * cosx, sinhy * cosh(z.y)) / (cosx * cosx + sinhy * sinhy);
}
inline float2 Cpx_Sinh(float2 z) { return sin(float2(-z.y, z.x)); }
inline float2 Cpx_Cosh(float2 z) { return cos(float2(-z.y, z.x)); }
inline float2 Cpx_Tanh(float2 z) { return tan(float2(-z.y, z.x)); }
#endif


#ifndef FUNCTION
#define FUNCTION (Cpx_Mul(z, z) + c)
#endif

cbuffer Data
{
	double2 cb_center;
	double2 cb_offset;
	double2 cb_scale;
	float4 cb_fractalColor;
	float2 cb_resolution;
	float cb_colorScale;
	uint cb_iterationCount;
	float cb_escapeRadius;
	uint cb_isMandelbrot;
	uint cb_startOnZ;
	uint padding;
};

NUMBER2 Function(NUMBER2 z, NUMBER2 c)
{
	return FUNCTION;
}

float ToRatio(float iterations, float2 z)
{
	return (iterations + 1.0f - log(log(length(z))) / log(2.0f)) / cb_colorScale;
}
float4 ToColor(float ratio)
{
	const float3 colors[5] = {
		float3(0.0f, 7.0f, 100.0f) / 255.0f,
		float3(32.0f, 107.0f, 203.0f) / 255.0f,
		float3(237.0f, 255.0f, 255.0f) / 255.0f,
		float3(255.0f, 170.0f, 0.0f) / 255.0f,
		float3(0.0f, 2.0f, 0.0f) / 255.0f
	};

	float3 c1 = colors[int(fmod(ratio, 5.0f))];
	float3 c2 = colors[int(fmod(ratio + 1.0f, 5.0f))];
	return float4(lerp(c1, c2, fmod(ratio, 1.0f)), 1.0f);
}
float4 FunctionColor(NUMBER2 coord)
{
	NUMBER2 numArray[] = {
		NUMBER2(NUMBER(cb_startOnZ * cb_isMandelbrot) * coord + NUMBER2(cb_offset.x, cb_offset.y)),
		coord
	};
	NUMBER2 z = numArray[1 - cb_isMandelbrot];
	NUMBER2 c = numArray[cb_isMandelbrot];

	for (uint i = 0; i < cb_iterationCount; i++)
	{
		z = Function(z, c);
		float2 zf = float2(z.x, z.y);
		if (length(zf) > cb_escapeRadius)
			return ToColor(ToRatio(float(i), zf));
	}
	return cb_fractalColor;
}

NUMBER2 Transform(uint2 screen)
{
	float2 coord = (screen * 2.0f - cb_resolution.xy) / (cb_resolution.y - 1.0f);
	coord = screen / (cb_resolution - 1.0f) * 2.0f - 1.0f;
	if (cb_resolution.x > cb_resolution.y)
		coord.x *= cb_resolution.x / cb_resolution.y;
	else
		coord.y *= cb_resolution.y / cb_resolution.x;
	return Cpx_Mul(NUMBER2(coord.x, coord.y), NUMBER2(cb_scale.x, cb_scale.y)) + NUMBER2(cb_center.x, cb_center.y);
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	target[DTid.xy] = FunctionColor(Transform(DTid.xy));
}