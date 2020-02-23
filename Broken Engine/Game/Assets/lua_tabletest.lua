-- Old Didac Code
local LOG = Debug.Scripting ()

function	GetTablelua_tabletest ()
local lua_table = {}
lua_table["position_x"] = 0
lua_table["LOG"] = Debug.Scripting ()
lua_table["GetDT"] = Debug.Scripting ()

function lua_table:Awake ()
	lua_table["position_x"] = 30
	lua_table["LOG"]:LOG ("This Log was called from LUA testing a table on AWAKE")
end

function lua_table:Start ()
	lua_table["LOG"]:LOG ("This Log was called from LUA testing a table on START")
end

function lua_table:Update ()
	lua_table["position_x"] = lua_table["position_x"] + 1

	lua_table["LOG"]:LOG ("This Log was called from LUA testing a table on UPDATE")
	lua_table["LOG"]:LOG ("Position X = " .. lua_table["position_x"])

	lua_table["LOG"]:LOG ("GameDT = " .. lua_table["GetDT"]:GetDT ())
end

return lua_table
end
