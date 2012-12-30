-- The minimum distance that the stack should be away from the disc.
STACK_MIN_DISTANCE = 10

-- The minimum distance between two players in the stack.
STACK_SEPERATION = 2

--
-- GetEarlierDiscIntercept
--
-- Internal function for the lua code. Not for C code to call.
-- Retrieves a list of all intercepts that are going to occur and returns the
-- one which is going to happen in the shortest amount of time.
--
function GetEarliestDiscIntercept()
    -- Retrieving the table of intercepts is not cheap but also not too 
    -- expensive as it is done on a pre computed disc path.
    intercept_table = callback_calculate_all_intercept_times()
    
    -- If there are no intercepts in the table then return 0.
    -- Otherwise return the element in the table that corresponds to the
    -- intercept which is going to occur soonest.
    if #(table) == 0 then
        return 0
    else
        min_time = -1
        min_data = 0
        for index, data in ipairs(intercept_table) do
            new_time = data["intercept_time"]
            
            -- Just keep track of the minimum time so far.
            if new_time < min_time or min_time == -1 then
                min_data = data
                min_time = new_time
            end
        end
        
        return min_data
    end
end

--
-- ShouldIChase
--
-- Determines whether a player should attempt to chase the disc when it is 
-- thrown.
--
-- Return: 1 if the player should chase and 0 otherwise.
--
function process_ShouldIChase(team_id, player_id)
    -- Find the earliest intercept time.
    earliest_intercept = GetEarlierDiscIntercept()
    
    -- We don't need to set the desired location of the player as the intercept
    -- disc state will handle that for us.
    if earliest_intercept["player_index"] == player_id and 
       earliest_intercept["team_index"] == team_id then
        return 1
    else
        return 0
    end
end

--
-- WillWeCatch
--
-- Find the most likely player to get the disc and decide on which team they are
-- in. If it's our team then return true otherwise return false.
--
function process_WillWeCatch(team_id, player_id)
    -- Find the earliest intercept time.
    earliest_intercept = GetEarlierDiscIntercept()
    
    -- Check the team index of the most likely player to intercept the disc.
    if earliest_intercept["team_index"] == team_id then
        return 1
    else
        return 0
    end
end
    
--
-- DecideWhoToMark
--
-- This function picks the nearest player not currently being marked by someone
-- else and designates them as the person that this player is marking.
--
function process_DecideWhoToMark(team_id, player_id)
    -- Retrieve all the player marks.
    marks = callback_get_all_taem_marks()
    
    -- Create an array of all the marking options.
    possible_marks = {}
    for ii = 0,6 do
        possible_marks[ii] = false
    end
    
    -- Fill in the players who are already being marked.
    for index, data in ipairs(marks) do
        possible_marks[data["mark_player_index"]] = true
    end
    
    -- Just pick the first free person for now
    -- @@@DAT - Sort this out so that we mark the next closest or something
    -- else sensible.
    for ii = 0,6 do
        if possible_marks[ii] == false then
            callback_set_desired_mark(ii)
            return 1
        end
    end
    
    return 0
end
    
--
-- DecideWhereToWait
--
-- Determines whether the disc will get caught or not. If not then it uses the
-- landing place as the intercept point.
--
-- Sets up a vertical stack on the intercept point by picking the next free spot
-- in the stack and running to that.
--
function process_DecideWhereToWait(team_id, player_id)
    -- The final disc location is either going to be the first intercept point
    -- of all the players or the place where it lands.
    disc_intercept = GetEarliestDiscIntercept()
    
    final_pos = {}
    if disc_intercept == 0 then
        -- Use the discs landing point as a location to set up on.
        data = callback_get_disc_final_pos()
        final_pos["x"] = data["x"]
        final_pos["y"] = data["y"]
    else
        -- Use the earliest intercept as the set up point.
        final_pos["x"] = disc_intercept["x"]
        final_pos["y"] = disc_intercept["y"]
    end
    
    stack_positions = {}
    for ii = 0,6 do
        stack_positions[ii] = {(final_pos["x"] + STACK_MIN_DISTANCE + (ii * STACK_SEPERATION)), false}
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
end
    
--
-- SwitchToVertStack
--
-- No processing requried as this is just a transition to move into the vertical
-- stack automaton.
--
function process_SwitchToVertStack(team_id, player_id)
    return 0
end