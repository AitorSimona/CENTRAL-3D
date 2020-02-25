local Functions = Debug.Scripting ()

function	GetTablelua_tabletest ()
local lua_table = {}
lua_table["position_x"] = 0
lua_table["Functions"] = Debug.Scripting ()

function lua_table:Awake ()
	lua_table["position_x"] = 30
	lua_table["Functions"]:LOG ("This Log was called from LUA testing a table on AWAKE")
end

function lua_table:Start ()
	lua_table["Functions"]:LOG ("This Log was called from LUA testing a table on START")
end

function lua_table:Update ()
	dt = lua_table["Functions"]:dt ()

	if lua_table["Functions"]:KeyRepeat ("W") then lua_table["Functions"]:Translate (0.0, 0.0, 50.0 * dt) end
	if lua_table["Functions"]:KeyRepeat ("A") then lua_table["Functions"]:Translate (50.0 * dt, 0.0 , 0.0) end
	if lua_table["Functions"]:KeyRepeat ("S") then lua_table["Functions"]:Translate (0.0, 0.0, -50.0 * dt) end
	if lua_table["Functions"]:KeyRepeat ("D") then lua_table["Functions"]:Translate(-50.0 * dt,0.0 , 0.0) end
	if lua_table["Functions"]:KeyRepeat ("Q") then lua_table["Functions"]:LOG ("Q is pressed") end
	if lua_table["Functions"]:IsGamepadButton(1,"BUTTON_DPAD_LEFT","DOWN") then lua_table["Functions"]:LOG ("Button BACK DOWN") end
	if lua_table["Functions"]:IsGamepadButton(2,"BUTTON_A","DOWN") then lua_table["Functions"]:LOG ("PLAYER 2 button A DOWN") end
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
