local Functions = Debug.Scripting ()

function	GetTableCameraScript ()
local lua_table = {}
lua_table["Functions"] = Debug.Scripting ()

-----------------------------------------------------------------------------------------
-- Inspector Variables
-----------------------------------------------------------------------------------------

-- Absolute Distance from Target
lua_table.camera_distance = 150 

-- Angle of the camera in degrees (0 Horizontal 90 Vertical)
lua_table.camera_angle = 50

-- Smoothing Speed from (0 to 1)
lua_table.smooth_speed = 0.2

-----------------------------------------------------------------------------------------
-- Camera Variables
-----------------------------------------------------------------------------------------

-- Camera position
local camera_position_x = 0
local camera_position_y = 0
local camera_position_z = 0

-- Camera offset 
local offset_x = 0
local offset_y = 0
local offset_z = 0

local camera_angle_for_offset = 0 -- 90-lua_table.camera_angle

-- Camera rotation (z hardcoded for now bc reasons)
local rotation_x = 0 -- 180 - (lua_table.camera_angle) 
local rotation_y = 0
local rotation_z = -180

-- Camera Target 
local target_position_x = 0
local target_position_y = 0
local target_position_z = 0

-- Player distance from camera target (probably won't be used)
local player_distance_from_camera_target = 0 --unused 

-- Camera Desired position (target + offset)
local desired_position_x = 0
local desired_position_y = 0
local desired_position_z = 0

-- Camera state
local state = -- not in use rn
{
	STATIC = 1, 
	DYNAMIC = 2		
}
local current_state = state.DYNAMIC -- Should initialize at awake(?)

-- Zoom layers
local zoom = -- not in use rn
{
	LAYER_1 = 1, 
	LAYER_2 = 2, 
	LAYER_3 = 3
}
local current_zoom = zoom.LAYER_1 -- Shoul initialize at awake(?)

-----------------------------------------------------------------------------------------
-- Gameplay Variables
-----------------------------------------------------------------------------------------
-- Gameplay Mode
local gameplay = 
{
	SOLO = 1, 
	DUO = 2, 
	TRIO = 3, 
	QUARTET = 4
}
local current_gameplay = 0 -- Should AUTOMATICALLY initialize at awake (hardcoded right now)

-----------------------------------------------------------------------------------------
-- Player Variables
-----------------------------------------------------------------------------------------
-- P1
local P1_id = 0

local P1_pos_x = 0
local P1_pos_y = 0
local P1_pos_z = 0

local prev_P1_pos_x = 0
local prev_P1_pos_y = 0
local prev_P1_pos_z = 0

-- P2
local P2_id = 0

local P2_pos_x = 0
local P2_pos_y = 0
local P2_pos_z = 0

local prev_P2_pos_x = 0
local prev_P2_pos_y = 0
local prev_P2_pos_z = 0

-- P3
-- P4

-----------------------------------------------------------------------------------------
-- Methods
-----------------------------------------------------------------------------------------

-- Get Position Offset from Distance and Angle Method

function GetYfromDistAndAng(c_distance, c_angle) --given hypotenuse and angle returns contiguous side
	local c_angle_rad
	local c_angle_cos
	local y_dist 

	c_angle_rad = math.rad(c_angle)
	c_angle_cos = math.cos(c_angle_rad)
	y_dist = c_distance*c_angle_cos

	return y_dist
end

function GetXfromDistAndAng(c_distance, c_angle) --given hypotenuse and angle returns opposite side
	local c_angle_rad
	local c_angle_sin
	local x_dist 

	c_angle_rad = math.rad(c_angle)
	c_angle_sin = math.sin(c_angle_rad)
	x_dist = c_distance*c_angle_sin

	return x_dist
end

-- Centroid Methods

function Centroid2P(p1, p2)
	return (p1 + p2) / 2 
end

function Centroid3P(p1, p2, p3)
	return (p1 + p2 + p3) / 3 
end

-- Camera Movement smoothing NEEDS A LIMIT WHERE IT STOPS SMOOTHING
function Asymptotic_Average(pos, target_pos, speed)
	return pos + (target_pos - pos)*speed
end

-- Get gameplay mode method (1,2,3 or players)(Eventually) 
-- function GetCurrentGameplay()

-- Handle Camera Movement Method
function HandleCameraMovement()

	-- 1 Player Target Calculations
	if current_gameplay == gameplay.SOLO
	then
		-- Gets position from Player 1 gameobject Id
		P1_pos_x = lua_table["Functions"]:GetGameObjectPosX(P1_id)
		P1_pos_y = lua_table["Functions"]:GetGameObjectPosY(P1_id)
		P1_pos_z = lua_table["Functions"]:GetGameObjectPosZ(P1_id)

		-- Target is P1 position
		target_position_x = P1_pos_x
		target_position_y = P1_pos_y		-- Kind of redundant but organized
		target_position_z = P1_pos_z
	
	-- 2 Players Target Calculations
	elseif current_gameplay == gameplay.DUO
	then
		-- Gets position from Player 1 gameobject Id
		P1_pos_x = lua_table["Functions"]:GetGameObjectPosX(P1_id)
		P1_pos_y = lua_table["Functions"]:GetGameObjectPosY(P1_id)
		P1_pos_z = lua_table["Functions"]:GetGameObjectPosZ(P1_id)

		-- Gets position from Player 2 gameobject Id 
		P2_pos_x = lua_table["Functions"]:GetGameObjectPosX(P2_id)
		P2_pos_y = lua_table["Functions"]:GetGameObjectPosY(P2_id)
		P2_pos_z = lua_table["Functions"]:GetGameObjectPosZ(P2_id)

		-- Target is Midpoint between P1 and P2 positions
		target_position_x = Centroid2P(P1_pos_x, P2_pos_x)
		target_position_y = Centroid2P(P1_pos_y, P2_pos_y)
		target_position_z = Centroid2P(P1_pos_z, P2_pos_z)
	end
	-- 3 Players Target Calculations
	-- 4 Players Target Calculations

	-- Offset from Distance and Angle
	-- offset_x = 
	offset_y = GetYfromDistAndAng(lua_table.camera_distance, camera_angle_for_offset)
	offset_z = GetXfromDistAndAng(lua_table.camera_distance, camera_angle_for_offset) -- since camera only has a direction for now, only Z is affected. Else the value would be split between x and z depending on direction

	--Start 
	if is_start == true
	then
		-- Camera position is Target + Offset
		camera_position_x = target_position_x + offset_x
		camera_position_y = target_position_y + offset_y 	-- Kind of redundant but conceptually organized
		camera_position_z = target_position_z + offset_z
		
	-- Update 
	elseif is_update == true
	then
		-- Desired position is target + offset
		desired_position_x = target_position_x + offset_x
		desired_position_y = target_position_y + offset_y
		desired_position_z = target_position_z + offset_z

		-- Camera position is an averaged position between desired position and self position (the averaging depends on "smooth_speed")
		camera_position_x = Asymptotic_Average(camera_position_x, desired_position_x, lua_table.smooth_speed)
		camera_position_y = Asymptotic_Average(camera_position_y, desired_position_y, lua_table.smooth_speed)
		camera_position_z = Asymptotic_Average(camera_position_z, desired_position_z, lua_table.smooth_speed)
	end

	-- Setting Camera Position
	lua_table["Functions"]:SetPosition(camera_position_x, camera_position_y, camera_position_z)
end

-- Handle Zoom layers method

-- Main Code
function lua_table:Awake ()
	lua_table["Functions"]:LOG ("This Log was called from Camera Script on AWAKE")

	-- Gameplay mode (Comment/Uncomment for now until we have a way to manage it automatically)
	-- current_gameplay = gameplay.SOLO
	current_gameplay = gameplay.DUO --Hardcoded af

	if current_gameplay == 0
	then 
		lua_table["Functions"]:LOG ("Camera: Gameplay mode set to NULL")

	elseif current_gameplay == gameplay.SOLO
	then
		lua_table["Functions"]:LOG ("Camera: Gameplay mode set to SOLO")

		-- Player 1 id
		P1_id= lua_table["Functions"]:FindGameObject("gerardo1")--exact name of gameobject 

		if P1_id == 0 
		then
			lua_table["Functions"]:LOG ("Camera: Null Player 1 id, check name of game object inside script")
		else
			lua_table["Functions"]:LOG ("Camera: Player 1 id successfully recieved")
		end

	elseif current_gameplay == gameplay.DUO
	then
		lua_table["Functions"]:LOG ("Camera: Gameplay mode set to DUO")
		
		-- Player 1 id
		P1_id= lua_table["Functions"]:FindGameObject("gerardo1")--exact name of gameobject 

		if P1_id == 0 
		then
			lua_table["Functions"]:LOG ("Camera: Null Player 1 id, check name of game object inside script")
		else
			lua_table["Functions"]:LOG ("Camera: Player 1 id successfully recieved")
		end

		-- Player 2 id
		P2_id= lua_table["Functions"]:FindGameObject("gerardo2")--exact name of gameobject 

		if P1_id == 0 
		then
			lua_table["Functions"]:LOG ("Camera: Null Player 1 id, check name of game object inside script")
		else
			lua_table["Functions"]:LOG ("Camera: Player 2 id successfully recieved")
		end
	 end
	 
	 camera_angle_for_offset = 90-lua_table.camera_angle
	 rotation_x = 180 - lua_table.camera_angle --because soemthing is wrong with the motor view that is inverted
end

function lua_table:Start ()
	lua_table["Functions"]:LOG ("This Log was called from Camera Script on START")
	local is_start = true

	HandleCameraMovement()

	-- LookAt
	-- lua_table["Functions"]:LookAt(target_position_x, 0, 0, false)
	lua_table["Functions"]:RotateObject(rotation_x, rotation_y, rotation_z)	

	is_start = false
end

function lua_table:Update ()
	dt = lua_table["Functions"]:dt ()
	is_update = true
	
	HandleCameraMovement()

	--debug
	-- result = lua_table["Functions"]:GetPositionInFrustum(20000, 0, 0)

	-- lua_table["Functions"]:LOG (result)
	is_update = false
end
	return lua_table
end


-- REFERENCE CODE
-- local Functions = Debug.Scripting ()

-- function	GetTablelua_tabletest ()
-- local lua_table = {}
-- lua_table["position_x"] = 0
-- lua_table["Functions"] = Debug.Scripting ()

-- function lua_table:Awake ()
-- 	lua_table["position_x"] = 30
-- 	lua_table["Functions"]:LOG ("This Log was called from LUA testing a table on AWAKE")
-- end

-- function lua_table:Start ()
-- 	lua_table["Functions"]:LOG ("This Log was called from LUA testing a table on START")
-- end

-- function lua_table:Update ()
	-- dt = lua_table["Functions"]:dt ()

	-- if lua_table["Functions"]:KeyRepeat ("W") then lua_table["Functions"]:Translate (0.0, 0.0, 50.0 * dt) end
	-- if lua_table["Functions"]:KeyRepeat ("A") then lua_table["Functions"]:Translate (50.0 * dt, 0.0 , 0.0) end
	-- if lua_table["Functions"]:KeyRepeat ("S") then lua_table["Functions"]:Translate (0.0, 0.0, -50.0 * dt) end
	-- if lua_table["Functions"]:KeyRepeat ("D") then lua_table["Functions"]:Translate(-50.0 * dt,0.0 , 0.0) end
	-- if lua_table["Functions"]:KeyRepeat ("Q") then lua_table["Functions"]:LOG ("Q is pressed") end
	-- if lua_table["Functions"]:IsGamepadButton(1,"BUTTON_DPAD_LEFT","DOWN") then lua_table["Functions"]:LOG ("Button BACK DOWN") end
	-- if lua_table["Functions"]:IsGamepadButton(2,"BUTTON_A","DOWN") then lua_table["Functions"]:LOG ("PLAYER 2 button A DOWN") end
	
	-- --Testing axis
	-- if lua_table["Functions"]:IsJoystickAxis(1,"AXIS_RIGHTX","POSITIVE_DOWN") then lua_table["Functions"]:LOG ("Joystick Left X POSITIVE Down") end
	-- if lua_table["Functions"]:IsJoystickAxis(1,"AXIS_RIGHTX","NEGATIVE_DOWN") then lua_table["Functions"]:LOG ("Joystick Left X NEGATIVE Down") end
	-- if lua_table["Functions"]:IsJoystickAxis(1,"AXIS_RIGHTY","POSITIVE_DOWN") then lua_table["Functions"]:LOG ("Joystick Left Y POSITIVE Down") end
	-- if lua_table["Functions"]:IsJoystickAxis(1,"AXIS_RIGHTY","NEGATIVE_DOWN") then lua_table["Functions"]:LOG ("Joystick Left Y NEGATIVE Down") end
	
	-- --lua_table["Functions"]:LOG ("Joystick Left X: " .. lua_table["Functions"]:GetAxisValue(1,"AXIS_RIGHTX"))
	-- --lua_table["Functions"]:LOG ("Joystick Left Y: " .. lua_table["Functions"]:GetAxisValue(1,"AXIS_RIGHTY"))
	
	-- if lua_table["Functions"]:IsTriggerState(1,"AXIS_TRIGGERLEFT","DOWN") then lua_table["Functions"]:StopControllerShake(1) end
	-- if lua_table["Functions"]:IsTriggerState(1,"AXIS_TRIGGERRIGHT","DOWN") then lua_table["Functions"]:ShakeController(1,0.3,2000) end
	
	-- lua_table["Functions"]:LOG ("Joystick Left X: " .. lua_table["Functions"]:GetAxisValue(1,"AXIS_LEFTX", 0.3))
	
-- end

-- return lua_table
-- end
