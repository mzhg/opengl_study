

uniform sampler2D inputImage;
vec3 LUMINANCE_VECTOR = vec3(0.2125f, 0.7154f, 0.0721f);

out vec4 f4FragColor;
void main()
{
	float logLumSum = 0.0f;
	int x, y;
	ivec2 size = textureSize(inputImage);
	for (y = 0; y<size.y; y++) {
		for (x = 0; x<size.x; x++) {
			logLumSum += (dot(textureOffset(inputImage, vec2(0), ivec2(x, y)).rgb, LUMINANCE_VECTOR) + 0.00001);
		}
	}
	logLumSum /= max(1.0, float(size.x * size.y));
	float val = (logLumSum + 0.00001);
//	imageStore(outputImage, ivec2(0, 0), vec4(val, val, val, val));
	f4FragColor = vec4(val);
}