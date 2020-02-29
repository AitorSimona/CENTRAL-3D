local Functions = Debug.Scripting ()

function	GetTablelua_tabletest ()
local lua_table = {}
lua_table["position_x"] = 0
lua_table["Functions"] = Debug.Scripting ()

function lua_table:Awake ()
	lua_table["position_x"] = 30
	lua_table["Functions"]:LOG ("This Log was called from LUA testing a table on AWAKE")
	lua_table["Camera"] = lua_table["Functions"]:FindGameObject("Test")
	lua_table["Functions"]:LOG ("Camera Game Object UID: " .. lua_table["Camera"] )
end

function lua_table:Start ()
	lua_table["Functions"]:LOG ("This Log was called from LUA testing a table on START")
end

function lua_table:Update ()
	dt = lua_table["Functions"]:dt ()
	
	lua_table["Functions"]:TranslateGameObject(lua_table["Camera"],1.0*dt,0.0,0.0)
	lua_table["Functions"]:LOG("Pos_x:"  .. lua_table["Functions"]:GetGameObjectPosX(lua_table["Camera"]))
	lua_table["Functions"]:LOG("Pos_y:"  .. lua_table["Functions"]:GetGameObjectPosY(lua_table["Camera"]))
	lua_table["Functions"]:LOG("Pos_z:"  .. lua_table["Functions"]:GetGameObjectPosZ(lua_table["Camera"]))
	--pos_x, pos_y, pos_z = lua_table["Functions"]:GetGameObjectPos(lua_table["Camera"]);

    --if pos_x ~= nil then lua_table["Functions"]:LOG("Pos_X: " .. pos_x) end
    --lua_table["Functions"]:LOG ("Pos_X" .. posx)
    --lua_table["Functions"]:LOG ("Pos_Y" .. posy)
    --lua_table["Functions"]:LOG ("Pos_Z" .. posz)
    
	--Object Transformations
	--rotX = lua_table["Functions"]:GetRotationX()

	--lua_table["Functions"]:LOG (rotX)

	--Particles Emission Activation/Deactivation
	--lua_table["Functions"]:ActivateParticlesEmission()
	--lua_table["Functions"]:DeactivateParticlesEmission()
	
end

return lua_table
end

-- Old Didac Code
--local LOG = Debug.Scripting ()
--
--function	GetTablelua_tabletest ()
--local lua_table = {}
--lua_table["position_x"] = 0
--lua_table["LOG"] = Debug.Scripting ()
--lua_table["GetDT"] = Debug.Scripting ()
--
--function lua_table:Awake ()
--	lua_table["position_x"] = 30
--	lua_table["LOG"]:LOG ("This Log was called from LUA testing a table on AWAKE")
--end
--
--function lua_table:Start ()
--	lua_table["LOG"]:LOG ("This Log was called from LUA testing a table on START")
--end
--
--function lua_table:Update ()
--	lua_table["position_x"] = lua_table["position_x"] + 1
--
--	lua_table["LOG"]:LOG ("This Log was called from LUA testing a table on UPDATE")
--	lua_table["LOG"]:LOG ("Position X = " .. lua_table["position_x"])
--
--	lua_table["LOG"]:LOG ("GameDT = " .. lua_table["GetDT"]:GetDT ())
--end
--
--return lua_table
--end
