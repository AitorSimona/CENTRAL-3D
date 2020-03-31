function GetTableAudioTest()
local lua_table = {}
lua_table.System = Scripting.System()
lua_table.Audio = Scripting.Audio()
lua_table.Input = Scripting.Inputs()

function lua_table:Awake()
end

function lua_table:Start()
	--lua_table.Audio:SetVolume(10.0)
end

function lua_table:Update()
	--if lua_table.Input:KeyDown("a") == true then lua_table.Audio:PlayAudioEvent("music") end
	if lua_table.Input:KeyDown("a") == true then lua_table.Audio:PlayAudioEvent("Spikeroog") end
end

return lua_table
end