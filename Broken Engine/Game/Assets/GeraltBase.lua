local Functions = Debug.Scripting ()

function	GetTableGeraltBase ()
local lua_table = {}
lua_table.Functions = Debug.Scripting ()

--State Machine
local state = {
	dead = -2,
	down = -1,
	idle = 0,
	move = 1,
	light = 2,
	heavy = 3,
	evade = 4,
	ability = 5,
	ultimate = 6,
	item = 7,
	revive = 8
}
local current_state = state.idle

--LOCAL: Variable Stats
local current_health = 100.0

--Stats Base (Character default stats)
lua_table.stat_max_health_base = 100
lua_table.stat_damage_base = 100
lua_table.stat_speed_base = 100

--Stats Improved (Stats after modification alterations)
local stat_max_health_alt = 0
local stat_damage_alt = 0
local stat_speed_alt = 0

--Mods: Stats (Multiplier of base Stats)
local stat_health_mod = 1
local stat_damage_mod = 1
local stat_speed_mod = 1

--Mods: Special Effects (Multiplier of stats for effect application)
local regen_bool = false
lua_table.regen_val = 0
local crit_bool = false
lua_table.crit_chance = 0
lua_table.crit_damage = 0

--Controls
local key_state = {
	key_idle = "IDLE",
	key_down = "DOWN",
	key_repeat = "REPEAT",
	key_up = "UP"
}

lua_table.key_ultimate_1 = "AXIS_TRIGGERLEFT"
lua_table.key_ultimate_2 = "AXIS_TRIGGERRIGHT"

lua_table.key_interact = "BUTTON_LEFTSHOULDER"
lua_table.key_use_item = "BUTTON_RIGHTSHOULDER"

lua_table.key_light = "BUTTON_X"
lua_table.key_heavy = "BUTTON_Y"
lua_table.key_evade = "BUTTON_A"
lua_table.key_ability = "BUTTON_B"

lua_table.key_move = "AXIS_LEFT"
lua_table.key_aim = "AXIS_RIGHT"
lua_table.key_joystick_sensibility = 1.0
local key_joystick_threshold = 0.01

lua_table.key_pickup_item = "BUTTON_DPAD_UP"
lua_table.key_prev_consumable = "BUTTON_DPAD_LEFT"
lua_table.key_next_consumable = "BUTTON_DPAD_RIGHT"
lua_table.key_drop_consumable = "BUTTON_DPAD_DOWN"

lua_table.key_notdef5 = "BUTTON_BACK"
lua_table.key_notdef6 = "BUTTON_START"

--Movement
local mov_speed_x = 0.0
local mov_speed_y = 0.0
lua_table.mov_speed_max = 50.0
local mov_acc_x = 0.0
local mov_acc_y = 0.0
lua_table.mov_acc_max = 0.0

local rot_speed = 0.0
lua_table.rot_speed_max = 0.0
lua_table.rot_acc_max = 0.0

--Aiming
local aim_x = 0.0
local aim_y = 0.0

--Light Attack
lua_table.light_attack_damage = 0
lua_table.light_attack_cost = 0

lua_table.light_attack_block_time = 400		--Input block duration	(block new attacks)
lua_table.light_attack_combo_start = 600	--Combo timeframe start
lua_table.light_attack_combo_end = 800		--Combo timeframe end
lua_table.light_attack_end_time = 1000		--Attack end (return to idle)

--Heavy Attack
lua_table.heavy_attack_damage = 0
lua_table.heavy_attack_cost = 0

lua_table.heavy_attack_block_time = 400		--Input block duration	(block new attacks)
lua_table.heavy_attack_combo_start = 600	--Combo timeframe start
lua_table.heavy_attack_combo_end = 800		--Combo timeframe end
lua_table.heavy_attack_end_time = 1000		--Attack end (return to idle)

--Evade
lua_table.evade_cost = 0
lua_table.evade_duration = 0
lua_table.evade_acceleration = 0

--Ability
lua_table.ability_cost = 0
lua_table.ability_duration = 0

--Revive/Death
lua_table.revive_time = 5000	-- Time to revive
lua_table.down_time = 10000		-- Time until death (restarted by revival attempt)

lua_table.being_revived = false	-- Revival flag (managed by rescuer character)

local stopped_death = false		-- Death timer stop flag
local death_started_at = 0		-- Death timer start
local death_stopped_at = 0		-- Death timer stop
local revive_started_at = 0		-- Revive timer start

--Actions
local current_action_block_time	-- Duration of input block from current action/event (accept new action inputs)
local current_action_duration	-- Duration of current action/event (return to idle)
local action_started_at = 0			-- Marks start of actions (and getting revived)

--Combos
local combo_num = 0						-- Starting at 0, increases by 1 for each attack well timed, starting at 4, each new attack will be checked for a succesful combo. Bad timing or performing a combo resets to 0
local combo_stack = { 'N', 'N', 'N', 'N' }		-- Last 4 attacks performed (0=none, 1=light, 2=heavy). Use push_back tactic.
local rightside = true					-- Last attack side, switches on a succesfully timed attack

--Methods: Short
function PushBack (array, array_size, new_val)	--Pushes back all values and inserts a new one
	for i = 0, array_size - 2, 1
	do
		array[i] = array[i + 1]
	end

	array[array_size - 1] = new_val
end

function GoDefaultState ()
	if mov_input_x ~= 0.0 or mov_input_y ~= 0.0
	then
		--Animation to MOVE
		current_state = state.move
	else
		--Animation to IDLE
		current_state = state.idle
	end
end

--Methods: Massive
function KeyboardInputs ()	--Process Debug Keyboard Inputs
	mov_input_x = 0.0
	mov_input_y = 0.0
	
	aim_input_x = 0.0
	aim_input_y = 0.0
	
	if lua_table.Functions:KeyRepeat ("D")
	then
		mov_input_x = 2.0
	elseif lua_table.Functions:KeyRepeat ("A")
	then
		mov_input_x = -2.0
	end
	
	if lua_table.Functions:KeyRepeat ("S")
	then
		mov_input_y = 2.0
	elseif lua_table.Functions:KeyRepeat ("W")
	then
		mov_input_y = -2.0
	end
end

function MovementInputs ()	--Process Movement Inputs
	if mov_input_x ~= 0.0 or mov_input_y ~= 0.0
	then
		if current_state == state.idle
		then
			--Animation to MOVE
			current_state = state.move
		end

		mov_speed_x = lua_table.mov_speed_max * mov_input_x	--Joystick input directly translates to speed, no acceleration
		mov_speed_y = lua_table.mov_speed_max * mov_input_y
 
		-----------------------
		--lua_table.Functions:SetVelocity (mov_speed_x * dt, 0.0, mov_speed_y * dt)
		--lua_table.Functions:LookTo (mov_speed_x, 0.0, mov_speed_y)
		-----------------------
		lua_table.Functions:Translate (-mov_speed_x * dt, 0.0, 0.0)
		lua_table.Functions:Translate (0.0, 0.0, -mov_speed_y * dt)
		-----------------------

	elseif current_state == state.move
	then
		--Animation to IDLE
		current_state = state.idle
	end
end

function ActionInputs ()	--Process Action Inputs

	combo_achieved = false

	if lua_table.Functions:IsGamepadButton (1, lua_table.key_light, key_state.key_down)		--Light Input
	then
		--action_started_at = lua_table.Functions:Time ()						--Set timer start mark
		
		if current_state <= state.move	--IF Idle or Moving
		then
			combo_num = 1				--Register combo start
		elseif current_state == state.light or current_state == state.heavy	--IF previous action was a light or heavy (potential combo)
		then
			if time_since_action > lua_table.light_attack_combo_end			--If too late for combo
			then
				combo_num = 1
			elseif time_since_action > lua_table.light_attack_combo_start	--If inside combo timeframe
			then
				combo_num = combo_num + 1
				if combo_num > 3
				then
					--combo_achieved = CheckCombo ()	--Check combo performed	(ATTENTION: This should handle the animation, combo_num resseting, setting timers, state, bla bla)
				end
			else															--If too early for combo
				combo_num = 1
			end
		end

		if combo_achieved ~= true	--If no combo was achieved with the input, do the attack normally
		then
			current_action_block_time = lua_table.light_attack_block_time	--Set duration of input block (no new actions)
			current_action_duration = lua_table.light_attack_end_time		--Set duration of the current action (to return to idle/move)

			PushBack(combo_stack, 4, 'L')

			--Animation to LIGHT
			current_state = state.light
		end

	elseif lua_table.Functions:IsGamepadButton (1, lua_table.key_heavy, key_state.key_down)	--Heavy Input
	then
		--action_started_at = lua_table.Functions:Time ()						--Set timer start mark
		
		if current_state <= state.move	--IF Idle or Moving
		then
			combo_num = 1				--Register combo start
		elseif current_state == state.heavy or current_state == state.heavy	--IF previous action was a light or heavy (potential combo)
		then
			if time_since_action > lua_table.heavy_attack_combo_end			--IF too late for combo
			then
				combo_num = 1
			elseif time_since_action > lua_table.heavy_attack_combo_start	--IF inside combo timeframe
			then
				combo_num = combo_num + 1
				if combo_num > 3
				then
					--combo_achieved = CheckCombo ()	--Check combo performed	(ATTENTION: This should handle the animation, combo_num resseting, setting timers, state, bla bla)
				end
			else															--If too early for combo
				combo_num = 1
			end
		end

		if combo_achieved ~= true	--If no combo was achieved with the input, do the attack normally
		then
			current_action_block_time = lua_table.heavy_attack_block_time	--Set duration of input block (no new actions)
			current_action_duration = lua_table.heavy_attack_end_time		--Set duration of the current action (to return to idle/move)

			PushBack(combo_stack, 4, 'H')

			--Animation to LIGHT
			current_state = state.heavy
		end

	elseif lua_table.Functions:IsGamepadButton (1, lua_table.key_evade, key_state.key_down)	--Evade Input
	then
		--action_started_at = lua_table.Functions:Time ()							--Set timer start mark

		--Do Evade
		current_state = state.evade

	elseif lua_table.Functions:IsGamepadButton (1, lua_table.key_ability, key_state.key_down)	--Ability Input
	then
		--action_started_at = lua_table.Functions:Time ()							--Set timer start mark

		--Do Ability
		current_state = state.ability

	elseif lua_table.Functions:IsTriggerState (1, lua_table.key_ultimate_1, key_state.key_down) and lua_table.Functions:IsTriggerState (1, lua_table.key_ultimate_2, key_state.key_down)	--Ultimate Input
	then
		--action_started_at = lua_table.Functions:Time ()							--Set timer start mark

		--Do Ultimate
		current_state = state.ultimate

	elseif lua_table.Functions:IsGamepadButton (1, lua_table.key_use_item, key_state.key_down)	--Object Input
	then
		--action_started_at = lua_table.Functions:Time ()							--Set timer start mark

		--Do Use_Object
		current_state = state.item

	elseif lua_table.Functions:IsGamepadButton (1, lua_table.key_interact, key_state.key_down)	--Revive Input
	then
		--action_started_at = lua_table.Functions:Time ()							--Set timer start mark

		--Do Revive
		current_state = state.revive

	end
end

function SecondaryInputs ()	--Process Secondary Inputs
	if lua_table.Functions:IsGamepadButton (1, lua_table.key_pickup_item, key_state.key_down)			--Pickup Item
	then
		--IF consumable (increase counter)
		--ELSEIF gear (replace current gear)
	
	elseif lua_table.Functions:IsGamepadButton (1, lua_table.key_prev_consumable, key_state.key_down)	--Previous Consumable
	then
		--GO TO PREV CONSUMABLE
	
	elseif lua_table.Functions:IsGamepadButton (1, lua_table.key_next_consumable, key_state.key_down)	--Next Consumable
	then
		--GO TO NEXT CONSUMABLE

	elseif lua_table.Functions:IsGamepadButton (1, lua_table.key_drop_consumable, key_state.key_down)	--Drop Consumable
	then
		--DROP CURRENT CONSUMABLE
	end
end

--Main Code
function lua_table:Awake ()
    lua_table.Functions:LOG ("This Log was called from LUA testing a table on AWAKE")
end

function lua_table:Start ()
    lua_table.Functions:LOG ("This Log was called from LUA testing a table on START")
end

function lua_table:Update ()
	dt = lua_table.Functions:dt ()
	if current_state >= state.idle	--IF alive
	then
		if current_health <= 0
		then
			--Animation to DEATH
			--lua_table.Functions:SetVelocity (0.0, 0.0, 0.0)
			--death_started_at = lua_table.Functions:Time ()
			current_state = state.down
		else
			--DEBUG
			--KeyboardInputs ()

			mov_input_x = lua_table.Functions:GetAxisValue(1, lua_table.key_move .. "X", key_joystick_threshold)
			mov_input_y = lua_table.Functions:GetAxisValue(1, lua_table.key_move .. "Y", key_joystick_threshold)

			aim_input_x = lua_table.Functions:GetAxisValue(1, lua_table.key_aim .. "X", key_joystick_threshold)
			aim_input_y = lua_table.Functions:GetAxisValue(1, lua_table.key_aim .. "Y", key_joystick_threshold)

			if current_state > state.move	--IF action currently going on, check action timer
			then
				--time_since_action = lua_table.Functions:Time () - action_started_at
			end

			if current_state <= state.move --or time_since_action > current_action_block_time	--IF state == idle/move or action_input_block_time has ended (Input-allowed environment)
			then
				--ActionInputs ()	--Process major action inputs (attacks, evade, ability, etc)
			end

			if current_state <= state.move	--IF there's no action inputs given, go for other inputs
			then
				MovementInputs ()	--Movement orders
				SecondaryInputs ()	--Minor ctions with no timer or special animations

			else	--ELSE (action being done)
				if time_since_action > current_action_duration	--IF action duration up
				then
					GoDefaultState ()	--Return to move or idle
				end
			end
		end
	elseif current_state == state.down		--IF currently down
	then
		if lua_table.being_revived == true	--IF flag marks that other player is reviving
		then
			if stopped_death == false		--IF stop mark hasn't been done yet
			then
				--death_stopped_at = lua_table.Functions:Time ()	--Mark revival start (for death timer)
				stopped_death = true							--Flag death timer stop
				revive_started_at = death_stopped_at			--Mark revival start (for revival timer)

			-- elseif lua_table.Functions:Time () - revive_started_at > lua_table.revive_time		--IF revival complete
			-- then
			-- 	current_health = stat_max_health_base / 2	--Get half health
			-- 	GoDefaultState ()							--Return to move or idle
			end
		else								--IF other player isn't reviving
			if stopped_death == true		--IF death timer was stopped
			then
				death_started_at = death_started_at + lua_table.Functions:Time () - death_stopped_at	--Resume timer
				stopped_death = false																	--Flag timer resuming

			elseif lua_table.Functions:Time () - death_started_at > lua_table.down_time	--IF death timer finished
			then
				current_state = state.dead			--Kill character
				--lua_table.Functions:Deactivate ()	--Disable character
			end
		end
	end
end

return lua_table
end

-- DÍDAC REFERENCE CODE
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