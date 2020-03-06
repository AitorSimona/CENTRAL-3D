local Functions = Debug.Scripting ()

function	GetTablelua_tabletest ()
local lua_table = {}
lua_table["position_x"] = 0
lua_table["position_y"] = 23.45
lua_table["Donde esta perry"] = "perryando"
lua_table["active"] = false
lua_table["active2"] = false
lua_table["active3"] = true
lua_table["active4"] = false
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

	lua_table["Functions"]:SetLinearVelocity(1.0,0.0,0.0)
end

return lua_table
end