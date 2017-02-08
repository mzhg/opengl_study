#include "simple_sdk.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#if 1
#include <GL\glew.h>
#else
#include <GLES2/gl2.h>
#endif

#include "simple_sdk_common.h"

#ifdef __cplusplus
extern "C"{
#endif

	void update_billboard_buffer(Vertex* pData, Matrix4& rotate, const Billboard& info, int width, int height)
	{
		float hw = width * info.scale * 0.5f;
		float hh = height * info.scale * 0.5f;

		pData[0].position.x = -hw;
		pData[0].position.y = -hh;
		pData[0].position.z = -info.depth;

		pData[1].position.x = +hw;
		pData[1].position.y = -hh;
		pData[1].position.z = -info.depth;

		pData[2].position.x = -hw;
		pData[2].position.y = +hh;
		pData[2].position.z = -info.depth;

		pData[3].position.x = +hw;
		pData[3].position.y = +hh;
		pData[3].position.z = -info.depth;

		pData[0].texcoord.x = -1.0f;
		pData[0].texcoord.y = -1.0f;

		pData[1].texcoord.x = +1.0f;
		pData[1].texcoord.y = -1.0f;

		pData[2].texcoord.x = -1.0f;
		pData[2].texcoord.y = +1.0f;

		pData[3].texcoord.x = +1.0f;
		pData[3].texcoord.y = +1.0f;

		rotate = rotationYawPitchRoll(info.yaw, info.roll, info.pitch);
	}

#ifdef __cplusplus
}
#endif