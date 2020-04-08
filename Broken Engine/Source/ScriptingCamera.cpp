#include "ScriptingCamera.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ResourceScene.h"
#include "GameObject.h"
#include "Components.h"
#include "ModuleRenderer3D.h"
#include "ScriptData.h"

using namespace Broken;
ScriptingCamera::ScriptingCamera() {}

ScriptingCamera::~ScriptingCamera() {}

int ScriptingCamera::GetPosInFrustum(float x, float y, float z, float fovratio1, float fovratio2)
{
	ComponentCamera* cam = App->renderer3D->active_camera;
	int camlevel = 0;

	if (cam)
	{
		// --- Create subdivisions of the frustum ---
		Frustum sub1 = cam->frustum;
		Frustum sub2 = cam->frustum;

		sub1.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio1, cam->frustum.AspectRatio());
		sub2.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio2, cam->frustum.AspectRatio());

		// --- Check in which subdivision we are ---
		if ((int)cam->frustum.Contains({ x, y, z }) == true)
		{
			camlevel = 1;

			if ((int)sub1.Contains({ x, y, z }) == true)
			{
				camlevel = 2;

				if ((int)sub2.Contains({ x, y, z }) == true)
				{
					camlevel = 3;
				}
			}
		}
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Current Active camera is NULL");

	return camlevel;
}

luabridge::LuaRef ScriptingCamera::GetFrustumPlanesIntersection(float x, float y, float z, float fovratio, lua_State* L)
{
	// Top, Bottom, Left, Right
	double top = 1.0, bot = 1.0, left = 1.0, right = 1.0;	//Considered to be inside the frustum (at planes' negative side) by default
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam)
	{
		// --- Create subdivisions of the frustum ---
		Frustum sub1 = cam->frustum;
		sub1.SetVerticalFovAndAspectRatio(cam->GetFOV() * DEGTORAD * fovratio, cam->frustum.AspectRatio());

		float3 pos = { x, y, z };

		if (sub1.TopPlane().IsOnPositiveSide(pos))	//MathGeoLib Considers the positive side of the planes the part outside of the frustum (planes look towards outside the frustum)
			top = 0.0;
		if (sub1.BottomPlane().IsOnPositiveSide(pos))
			bot = 0.0;
		if (sub1.LeftPlane().IsOnPositiveSide(pos))
			left = 0.0;
		if (sub1.RightPlane().IsOnPositiveSide(pos))
			right = 0.0;
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Current Active camera is NULL");

	luabridge::LuaRef table = luabridge::newTable(L);
	table.append(top);
	table.append(bot);
	table.append(left);
	table.append(right);

	return table;
}

luabridge::LuaRef ScriptingCamera::WorldToScreen(float x, float y, float z, lua_State* L) {
	luabridge::LuaRef ret(L, luabridge::newTable(L));
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam) {
		float2 sc_coords = cam->WorldToScreen(float3(x, y, z));

		ret.append(sc_coords.x);
		ret.append(sc_coords.y);
	}

	return ret;
}

luabridge::LuaRef ScriptingCamera::ScreenToWorld(float x, float y, float distance, lua_State* L) {
	luabridge::LuaRef ret(L, luabridge::newTable(L));
	ComponentCamera* cam = App->renderer3D->active_camera;

	if (cam) {
		float3 wrld_coords = cam->ScreenToWorld(float2(x, y), distance);

		ret.append(wrld_coords.x);
		ret.append(wrld_coords.y);
		ret.append(wrld_coords.z);
	}

	return ret;
}