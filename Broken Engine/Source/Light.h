#ifndef __BE_LIGHT_H__
#define __BE_LIGHT_H__

#include "Color.h"
#include "Math.h"
#include "BrokenCore.h"

namespace BrokenEngine {
	struct BROKEN_API Light {
		Light();

		void Init();
		void SetPos(float x, float y, float z);
		void Active(bool active);
		void Render();

		Color ambient;
		Color diffuse;
		float3 position;

		int ref;
		bool on;
	};
}

#endif