STACK_MIN_DISTANCE = 10
STACK_SEPERATION = 2
STACK_BACK = STACK_MIN_DISTANCE + (7 * 4)
SHORT_SPACE_DEPTH = STACK_BACK + 9
HANDLER_SPACE_DEPTH = STACK_MIN_DISTANCE - 2

--
-- DecideWhereToWait
--
-- Sets the location that a player will wait in this offensive set. This is 
-- calculated as "the next free space in a stack directly in front of the disc.
--
-- Return: Always returns 0 as both paths lead to the same place.
--
function process_DecideWhereToWait(team_id, player_id)
    -- Retrieve the final resting place of the disc on its current course.
    disc_end_position = callback_get_disc_final_pos()
    
    -- Calculate the array of stack positions that are possible. These will be
    -- filled in with which players are heading for them so that we can tell
    -- which positions are free.
    stack_positions = {}
    for ii = 0,6 do
        stack_positions[ii] = {(disc_end_position["x"] + STACK_MIN_DISTANCE + (ii * STACK_SEPERATION)), false}
    end
    
    -- Get the desired positions of all the other players on this team.
    -- This is returned as a table with one value for each player and each value
    -- is in turn a table with one entry for each x and y coordinates.
    player_desired_positions = callback_get_team_desired_positions()

    -- Work out the next free space in front of the disc. Note that we cannot
    -- use ipairs on this table because it ignored the zeroth element.
    for player_index, position in pairs(player_desired_positions) do
        if position["y"] == disc_end_position["y"] then
            for index, pos_tf_table in ipairs(stack_positions) do
                if pos_tf_table[1] == position["x"] then
                    stack_positions[index] = {pos, true}
                end
            end
        end
    end
    
    -- The first free space is the first true value in the stack positions table.
    for index, pos_tf_table in ipairs(stack_positions) do
        if not pos_tf_table[2] then
            -- Set the desired location and inform the player to run at 50%.
            callback_set_player_desired_pos(pos_tf_table[1], disc_end_position["y"])
            callback_set_player_speed(50)
            return 1
        end
    end

    return 1
end

--
-- ChangeCut
--
-- Sets the desired position if the player should change from the cut they are
-- currently attempting.
--
-- Return value is ignored regardless.
--
function process_ChangeCut(team_id, player_id)
    -- Retrieves the current location of the disc.
    disc_position = callback_get_disc_position()

    -- Get the players current position.
    current_position = callback_get_current_position()
    
    -- Pre calculate the distance from the disc.
    dist_from_disc = abs(disc_position["x"] - current_position["x"])
    
    -- Decides which direction we are attacking. If 1 then attack is from left 
    -- to right (or positive coordinates)
    attack_left_to_right = callback_is_attacking_left_to_right()
    
    -- Three checks:
    -- 1. Are we too far from the disc.
    -- 2. Are we too close to the disc.
    -- 3. Are we too close to the sideline.
    --
    -- The first two are handled in an identical way.
    if dist_from_disc > SHORT_SPACE_DEPTH or 
       dist_from_disc < HANDLER_SPACE_DEPTH then
        -- Retrieve the rest of the teams desired locations.
        desired_positions = callback_get_team_desired_positions()
        
        -- Check whether either below of above the stack is free from people
        -- trying to cut into it.
        bottom_free = true
        top_free = true
        for player_index, position in ipairs(desired_positions) do
            if position["y"] > disc_position["y"] + 2 then
                bottom_free = false
            elseif position["y"] < disc_position["y"] - 2 then
                top_free = false
            end
        end
        
        -- If both sides of the stack are free then pick a random side.
        -- If only one side is free then pick that one and if neither are free
        -- then stop cutting.
        if bottom_free or top_free then
            if bottom_free and top_free then
                randomseed(time())
                factor = (random(1,2) == 1) and -1 or 1
            elseif botom_free then
                factor = 1
            else
                factor = -1
            end
            
            -- Based on whether we are attacking left or right create a cut 
            -- that goes to the correct side of the stack.
            if attack_left_to_right then
                callback_set_desired_position(disc_position["x"] + HANDLER_SPACE_DEPTH,
                                              current_position["y"] - 10 * factor)
            else
                callback_set_desired_position(disc_position["x"] - HANDLER_SPACE_DEPTH,
                                              current_position["y"] - 10 * factor)
            end
        else
            -- Neither side was free so stop cutting.
            callback_stop_running()
        end
    --elseif TODO: @@@DAT Decide how to determine if too close to sideline.
    else
        -- The player is fine with their current cut. Continue going.
    end
end

--
-- MoveToDiscUpAutomaton
--
-- Just returns 0 as all paths lead to the new automaton.
--
function process_MoveToDiscUpAutomaton(team_id, player_id)
    return 0
end

--
-- AmIInStack
--
-- Checks whether a player is in the stack. Does this by comparing their 
-- position to the disc and seeing whether they fit into any of the stack spots.
--
-- Returns: 0 if the player is in the stack and 1 otherwise.
--
function process_AmIInStack(team_id, player_id)
    -- Retrieves the current location of the disc.
    disc_position = callback_get_disc_position()

    -- Get the players current position.
    current_position = callback_get_current_position()
    
    -- The player can only be in the stack if they have roughly the same y
    -- coordinate as the disc.
    if abs(current_position["y"] - disc_position["y"]) < 2 then
        for ii = 0,6 do
            x = disc_position["x"] + STACK_MIN_DISTANCE + (ii * STACK_SEPERATION)
            
            -- The player is in the stack if they are within a small distance
            -- of the position calculated above.
            if abs(x - current_position["x"]) < 2 then
                return 1
            end
        end 
    end
    
    -- The player is not in the stack so return false.
    return 0
end

--
-- ShouldIClear
--
-- For not this juts returns yes always. TODO: Write a better routine here when
-- we've seen what doesn't work.
--
function process_ShouldIClear(team_id, player_id)
    return 1
end

--
-- ShouldICut
--
-- A player should cut if they are at the back of the stack AND there is noone
-- already cutting into the open spaces. 
--
function process_ShouldICut(team_id, player_id)
    -- Decides which direction we are attacking. If 1 then attack is from left 
    -- to right (or positive coordinates)
    attack_left_to_right = callback_is_attacking_left_to_right()

    -- Retrieves the current location of the disc.
    disc_position = callback_get_disc_position()
    
    -- Get the players current position.
    current_position = callback_get_current_position()
    
    -- Pre calculate the distance from the disc.
    dist_from_disc = abs(disc_position["x"] - current_position["x"])

   -- Get the desired positions of all the other players on this team.
    -- This is returned as a table with one value for each player and each value
    -- is in turn a table with one entry for each x and y coordinates.
    player_desired_positions = callback_get_team_desired_positions()

    -- Retrieve a list of all the current player positions.
    player_positions = callback_get_team_positions()
    
    -- I am allowed to cut if I am the deepest of the stationary players.
    am_deepest = true
    for player_index, position in ipairs(player_positions) do
        if position["x"] == player_desired_positions[player_index]["x"] and
           position["y"] == player_desired_positions[player_index]["y"] then
            if abs(position["x"] - disc_position["x"]) > dist_from_disc then
                am_deepest = false
            end
        end
    end
    
    -- Check whether there is already a cutter running towards the deep space.
    -- If there is then we don't want to cut there.
    deep_cutter_exists = false
    for player_index, position in ipairs(player_desired_positions) do
        if attack_left_to_right == 1 then
            if position["x"] > BACK_STACK + disc_position["x"] then
                deep_cutter_exists = true
            end
        else
            if position["x"] < disc_position["x"] - BACK_STACK then
                deep_cutter_exists = true
            end
        end
    end
    
    -- Only cut if you are the deepest player (currently in the stack) and
    -- there isn't already a deep cutter.
    if am_deepest and deep_cutter_exists then
        -- Special case code needed to handle the fact that the positions are
        -- absolute pitch coordinates and so do not map nicely to have offense
        -- switching ends.
        if (callback_is_attacking_left_to_right() == 1) then
            callback_set_desired_position(position["x"] + 10 ,position["y"])
        else
            callback_set_desired_position(position["x"] - 10 ,position["y"])
        end
        
        return 1
    else
        return 0
    end
end