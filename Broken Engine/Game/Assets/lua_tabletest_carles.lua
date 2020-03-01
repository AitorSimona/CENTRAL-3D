local Functions = Debug.Scripting ()

function	GetTablelua_tabletest_carles ()
local lua_table = {}
lua_table.Functions = Debug.Scripting ()

lua_table.mov_speed_max = 15.0

local mov_speed_x = 0.0
local mov_speed_z = 0.0

function KeyboardInputs ()	--Process Debug Keyboard Inputs
	mov_input_x = 0.0
	mov_input_z = 0.0
	
	aim_input_x = 0.0
	aim_input_z = 0.0
	
	if lua_table.Functions:KeyRepeat ("D")
	then
		mov_input_x = 1.0
	elseif lua_table.Functions:KeyRepeat ("A")
	then
		mov_input_x = -1.0
	end
	
	if lua_table.Functions:KeyRepeat ("S")
	then
		mov_input_z = -1.0
	elseif lua_table.Functions:KeyRepeat ("W")
	then
		mov_input_z = 1.0
	end
end

function MovementInputs ()	--Process Movement Inputs
	if mov_input_x ~= 0.0 or mov_input_z ~= 0.0
	then
		mov_speed_x = lua_table.mov_speed_max * mov_input_x
		mov_speed_z = lua_table.mov_speed_max * mov_input_z

		_x, mov_speed_y, _z = lua_table.Functions:GetLinearVelocity ()
        lua_table.Functions:SetLinearVelocity (mov_speed_x, mov_speed_y, mov_speed_z)

        lua_table.Functions:LookAt(0, lua_table.Functions:GetPositionY(), 0, false)
	end
end

function lua_table:Awake ()
	--lua_table.Functions:LOG ("This Log was called from LUA testing a table on AWAKE")
end

function lua_table:Start ()
	--lua_table.Functions:LOG ("This Log was called from LUA testing a table on START")
end

function lua_table:Update ()
	dt = lua_table.Functions:dt ()

    KeyboardInputs()
    
	MovementInputs()

	--lua_table.Functions:LOG (rotX)
	--lua_table.Functions:LOG (posX)
	--lua_table.Functions:LOG (mass)

	--Particles Emission Activation/Deactivation
	--lua_table.Functions:ActivateParticlesEmission()
	--lua_table.Functions:DeactivateParticlesEmission()
end

return lua_table
end