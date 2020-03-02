local Functions = Debug.Scripting()

function	GetTableGeraltBase()
local lua_table = {}
lua_table.Functions = Debug.Scripting()

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
local current_state = state.idle	-- Current State

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

lua_table.player_ID = 1

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
local key_joystick_threshold = 0.2

lua_table.key_pickup_item = "BUTTON_DPAD_UP"
lua_table.key_prev_consumable = "BUTTON_DPAD_LEFT"
lua_table.key_next_consumable = "BUTTON_DPAD_RIGHT"
lua_table.key_drop_consumable = "BUTTON_DPAD_DOWN"

lua_table.key_notdef5 = "BUTTON_BACK"
lua_table.key_notdef6 = "BUTTON_START"

--Inputs
local mov_input_x = 0.0
local mov_input_z = 0.0
	
local aim_input_x = 0.0
local aim_input_z = 0.0

--Movement
local mov_speed_x = 0.0
local mov_speed_z = 0.0
lua_table.mov_speed_max = 15.0

local rot_speed = 0.0
lua_table.rot_speed_max = 0.0
lua_table.rot_acc_max = 0.0

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
lua_table.evade_duration = 300
lua_table.evade_velocity = 150

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
local time_since_action = 0			-- Time passed since action performed
local current_action_block_time = 0	-- Duration of input block from current action/event (accept new action inputs)
local current_action_duration = 0	-- Duration of current action/event (return to idle)
local action_started_at = 0			-- Marks start of actions (and getting revived)

--Combos
local combo_num = 0							-- Starting at 0, increases by 1 for each attack well timed, starting at 4, each new attack will be checked for a succesful combo. Bad timing or performing a combo resets to 0
local combo_stack = { 'N', 'N', 'N', 'N' }	-- Last 4 attacks performed (0=none, 1=light, 2=heavy). Use push_back tactic.
local rightside = true						-- Last attack side, switches on a succesfully timed attack

--Methods: Short
function PushBack(array, array_size, new_val)	--Pushes back all values and inserts a new one
	for i = 0, array_size - 2, 1
	do
		array[i] = array[i + 1]
	end

	array[array_size - 1] = new_val
end

function GoDefaultState()
	if mov_input_x ~= 0.0 or mov_input_z ~= 0.0
	then
		--Animation to MOVE
		current_state = state.move
	else
		--Animation to IDLE
		current_state = state.idle
		lua_table.Functions:DeactivateParticlesEmission()	--IMPROVE: Make particle emission more complex than de/activating
	end
end

function PerfGameTime()
	return lua_table.Functions:GameTime() * 1000
end

--Methods: Massive
function KeyboardInputs()	--Process Debug Keyboard Inputs
	mov_input_x = 0.0
	mov_input_z = 0.0
	
	aim_input_x = 0.0
	aim_input_z = 0.0
	
	if lua_table.Functions:KeyRepeat("D")
	then
		mov_input_x = 2.0
	elseif lua_table.Functions:KeyRepeat("A")
	then
		mov_input_x = -2.0
	end
	
	if lua_table.Functions:KeyRepeat("S")
	then
		mov_input_z = -2.0
	elseif lua_table.Functions:KeyRepeat("W")
	then
		mov_input_z = 2.0
	end
end

function MovementInputs()	--Process Movement Inputs
	if mov_input_x ~= 0.0 or mov_input_z ~= 0.0
	then
		if current_state == state.idle
		then
			--Animation to MOVE
			lua_table.Functions:ActivateParticlesEmission()
			current_state = state.move
		end

		mov_speed_x = lua_table.mov_speed_max * mov_input_x	--Joystick input directly translates to speed, no acceleration
		mov_speed_z = lua_table.mov_speed_max * mov_input_z

		_x, mov_speed_y, _z = lua_table.Functions:GetLinearVelocity()	--Set velocity
		lua_table.Functions:SetLinearVelocity(mov_speed_x, mov_speed_y, mov_speed_z)

		dir_x, _y, dir_z = lua_table.Functions:GetPosition()	--Rotate to velocity direction

		dir_x = dir_x + mov_speed_x
		dir_z = dir_z + mov_speed_z

		lua_table.Functions:LookAt(dir_x, _y, dir_z)

	elseif current_state == state.move
	then
		--Animation to IDLE
		lua_table.Functions:DeactivateParticlesEmission()
		current_state = state.idle
	end
end

function ActionInputs()	--Process Action Inputs
	input_given = false
	combo_achieved = false

	if lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_light, key_state.key_down)		--Light Input
	then
		action_started_at = PerfGameTime()				--Set timer start mark
		
		if current_state <= state.move	--IF Idle or Moving
		then
			combo_num = 1				--Register combo start
		elseif current_state == state.light and time_since_action > lua_table.light_attack_combo_start and time_since_action < lua_table.light_attack_combo_end	--IF prev attack light and input on right light timing
		or current_state == state.heavy and time_since_action > lua_table.heavy_attack_combo_start and time_since_action < lua_table.heavy_attack_combo_end		--OR, IF prev attack heavy and input on right heavy timing
		then
			combo_num = combo_num + 1
			if combo_num > 3			--IF 4+ goods attacks
			then
				--combo_achieved = CheckCombo()	--Check combo performed	(ATTENTION: This should handle the animation, combo_num resseting, setting timers, state, bla bla)
			end
		else
			combo_num = 1	--Not good timing since last attack
		end

		if combo_achieved == false	--If no combo was achieved with the input, do the attack normally
		then
			current_action_block_time = lua_table.light_attack_block_time	--Set duration of input block (no new actions)
			current_action_duration = lua_table.light_attack_end_time		--Set duration of the current action (to return to idle/move)

			PushBack(combo_stack, 4, 'L')

			--Animation to LIGHT
			current_state = state.light
		end

		input_given = true

	elseif lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_heavy, key_state.key_down)	--Heavy Input
	then
		action_started_at = PerfGameTime()				--Set timer start mark
		
		if current_state <= state.move	--IF Idle or Moving
		then
			combo_num = 1				--Register combo start
		elseif current_state == state.light and time_since_action > lua_table.light_attack_combo_start and time_since_action < lua_table.light_attack_combo_end	--IF prev attack light and input on right light timing
		or current_state == state.heavy and time_since_action > lua_table.heavy_attack_combo_start and time_since_action < lua_table.heavy_attack_combo_end		--OR, IF prev attack heavy and input on right heavy timing
		then
			combo_num = combo_num + 1
			if combo_num > 3			--IF 4+ goods attacks
			then
				--combo_achieved = CheckCombo()	--Check combo performed	(ATTENTION: This should handle the animation, combo_num resseting, setting timers, state, bla bla)
			end
		else
			combo_num = 1	--Not good timing since last attack
		end

		if combo_achieved ~= true	--If no combo was achieved with the input, do the attack normally
		then
			current_action_block_time = lua_table.heavy_attack_block_time	--Set duration of input block (no new actions)
			current_action_duration = lua_table.heavy_attack_end_time		--Set duration of the current action (to return to idle/move)

			PushBack(combo_stack, 4, 'H')

			--Animation to HEAVY
			current_state = state.heavy
		end

		input_given = true

	elseif lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_evade, key_state.key_down)	--Evade Input
	then
		if mov_input_x ~= 0.0 or mov_input_z ~= 0.0	-- Evade will not perform without a direction input
		then
			action_started_at = PerfGameTime()							--Set timer start mark
			current_action_block_time = lua_table.evade_duration
			current_action_duration = lua_table.evade_duration
			
			_x, mov_speed_y, _z = lua_table.Functions:GetLinearVelocity()	--TODO: Check if truly needed or remove

			magnitude = math.sqrt(mov_input_x ^ 2 + mov_input_z ^ 2)	--Calculate to use unit vector for direction

			--Do Evade
			lua_table.Functions:SetLinearVelocity(lua_table.evade_velocity * mov_input_x / magnitude, mov_speed_y, lua_table.evade_velocity * mov_input_z / magnitude)

			current_state = state.evade
			input_given = true
		end
		
	elseif lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_ability, key_state.key_down)	--Ability Input
	then
		action_started_at = PerfGameTime()							--Set timer start mark

		--Do Ability
		current_state = state.ability
		input_given = true

	elseif lua_table.Functions:IsTriggerState(lua_table.player_ID, lua_table.key_ultimate_1, key_state.key_down) and lua_table.Functions:IsTriggerState (lua_table.player_ID, lua_table.key_ultimate_2, key_state.key_down)	--Ultimate Input
	then
		action_started_at = PerfGameTime()							--Set timer start mark

		--Do Ultimate
		current_state = state.ultimate
		input_given = true

	elseif lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_use_item, key_state.key_down)	--Object Input
	then
		action_started_at = PerfGameTime()							--Set timer start mark

		--Do Use_Object
		current_state = state.item
		input_given = true

	elseif lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_interact, key_state.key_down)	--Revive Input
	then
		action_started_at = PerfGameTime()							--Set timer start mark

		--Do Revive
		current_state = state.revive
		input_given = true
	end

	if input_given	--IMPROVE: This is trashy, but it works for the current particle demonstration
	then
		lua_table.Functions:ActivateParticlesEmission()
	end

	return input_given
end

function SecondaryInputs()	--Process Secondary Inputs
	if lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_pickup_item, key_state.key_down)			--Pickup Item
	then
		--IF consumable (increase counter)
		--ELSEIF gear (replace current gear)
	
	elseif lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_prev_consumable, key_state.key_down)	--Previous Consumable
	then
		--GO TO PREV CONSUMABLE
	
	elseif lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_next_consumable, key_state.key_down)	--Next Consumable
	then
		--GO TO NEXT CONSUMABLE

	elseif lua_table.Functions:IsGamepadButton(lua_table.player_ID, lua_table.key_drop_consumable, key_state.key_down)	--Drop Consumable
	then
		--DROP CURRENT CONSUMABLE
	end
end

--Main Code
function lua_table:Awake()
    lua_table.Functions:LOG("This Log was called from LUA testing a table on AWAKE")
end

function lua_table:Start()
    lua_table.Functions:LOG("This Log was called from LUA testing a table on START")
end

function lua_table:Update()
	dt = lua_table.Functions:dt()
	if current_state >= state.idle	--IF alive
	then
		if current_health <= 0
		then
			--Animation to DEATH
			--lua_table.Functions:SetVelocity(0.0, 0.0, 0.0)
			--death_started_at = PerfGameTime()
			current_state = state.down
		else
			--DEBUG
			--KeyboardInputs()

			mov_input_x = lua_table.Functions:GetAxisValue(lua_table.player_ID, lua_table.key_move .. "X", key_joystick_threshold)
			mov_input_z = lua_table.Functions:GetAxisValue(lua_table.player_ID, lua_table.key_move .. "Y", key_joystick_threshold)

			aim_input_x = lua_table.Functions:GetAxisValue(lua_table.player_ID, lua_table.key_aim .. "X", key_joystick_threshold)
			aim_input_z = lua_table.Functions:GetAxisValue(lua_table.player_ID, lua_table.key_aim .. "Y", key_joystick_threshold)

			if current_state > state.move	--IF action currently going on, check action timer
			then
				time_since_action = PerfGameTime() - action_started_at
			end

			if current_state <= state.move or time_since_action > current_action_block_time	--IF state == idle/move or action_input_block_time has ended (Input-allowed environment)
			then
				ActionInputs()
			end

			if current_state <= state.move	--IF there's no action being performed
			then
				MovementInputs()	--Movement orders
				--SecondaryInputs()	--Minor ctions with no timer or special animations

			else							--ELSE (action being performed)
				time_since_action = PerfGameTime() - action_started_at

				if time_since_action > current_action_duration	--IF action duration up
				then
					GoDefaultState()	--Return to move or idle
				end
			end
		end
	elseif current_state == state.down		--IF currently down
	then
		if lua_table.being_revived == true	--IF flag marks that other player is reviving
		then
			if stopped_death == false		--IF stop mark hasn't been done yet
			then
				death_stopped_at = PerfGameTime()	--Mark revival start (for death timer)
				stopped_death = true								--Flag death timer stop
				revive_started_at = death_stopped_at				--Mark revival start (for revival timer)

			elseif PerfGameTime() - revive_started_at > lua_table.revive_time		--IF revival complete
			then
				current_health = stat_max_health_base / 2	--Get half health
				GoDefaultState()							--Return to move or idle
			end
		else								--IF other player isn't reviving
			if stopped_death == true		--IF death timer was stopped
			then
				death_started_at = death_started_at + PerfGameTime() - death_stopped_at	--Resume timer
				stopped_death = false																		--Flag timer resuming

			elseif PerfGameTime() - death_started_at > lua_table.down_time	--IF death timer finished
			then
				current_state = state.dead			--Kill character
				--lua_table.Functions:Deactivate()	--Disable character
			end
		end
	end

	lua_table.Functions:LOG("Current state: " .. current_state)
end

return lua_table
end