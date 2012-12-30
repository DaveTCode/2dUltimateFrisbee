-- The minimum distance that the stack should be away from the disc.
STACK_MIN_DISTANCE = 10

-- The minimum distance between two players in the stack.
STACK_SEPERATION = 2

--
-- AmICatchingPull
--
-- Checks whether the player is the designated catcher and if so returns 1. 
-- Otherwise it returns 0.
--
function process_AmICatchingPull(team_id, player_id)
    am_catching = 0
    print(player_id)
    print(team_id)
    
    -- Just let player 0 catch the pull.
    if player_id == 1 then
        am_catching = 1
    else
        am_catching = 0
    end
    
    return am_catching
end
    
--
-- IsPullCatchable
--
-- Returns 1 if the player designated to catch the pull should attempt to catch
-- it or let it hit the floor.
--
function process_IsPullCatchable(team_id, player_id)
    -- Always attempt to catch the pull.
    return 1
end
    
--
-- IsDiscLandingOutOfBounds
--
-- Returns 1 if the disc will land out of bounds and 0 if it will land in the 
-- pitch.
--
function process_IsDiscLandingOutOfBounds(team_id, player_id)
    -- Retrieve the final resting place of the disc on its current course.
    disc_end_position = callback_get_disc_final_pos()
    
    -- Retrieve the pitch dimensions.
    pitch_dimensions = callback_get_pitch_dimensions()
    
    -- Checks whether the disc is going to land in or out. 
    if (disc_end_position["x"] < 0 or 
        disc_end_position["x"] > pitch_dimensions["length"]) then
        return 1
    elseif (disc_end_position["y"] < 0 or
            disc_end_position["y"] > pitch_dimensions["width"]) then
        return 1
    end
    
    return 0
end
    
--
-- DecideOnPosition
--
-- Called once per player who is NOT catching the pull to determine what their
-- starting position should be based on where the disc is going to land/be 
-- caught.
--
-- Return value is meaningless for this function.
function process_DecideOnPosition(team_id, player_id)
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
-- DecideStartFromBrick
--
-- When the pull has landed out of bounds this is called to determine whether 
-- it should be taken from the brick mark or from the edge where it went out.
--
-- Returns 1 if the pull is to be taken from the brick and 0 otherwise.
--
function process_DecideStartFromBrick(team_id, player_id)
    -- Returns 1 if starting from the brick mark.
    return 1
end

--
-- SwitchToVerticalStack
--
-- Placeholder for moving from pull automaton to vertical stack automaton.
--
-- Returns 1.
--
function process_SwitchToVerticalStack(team_id, player_id)
    return 1
end