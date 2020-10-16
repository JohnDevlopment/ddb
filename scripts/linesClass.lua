lines_metatable = {
    __len = function(o)
        return o.count
    end,

    __concat = function(o1, o2)
        if not(o1.count) then
            return tostring(o1) .. tostring(o2:empty())
        else
            return tostring(o2) .. tostring(o1:empty())
        end
    end
}

lines = {
    file = nil, -- file name
    data = nil, -- [Internal] line data
    count = 0, -- line count

    -- returns the line number LINE from the file
    readLine = function(self, line)
        assert(line <= self.count, "'line' parameter exceeds boundaries")
        return self.data[line]
    end,

    -- opens a file and initializes the table data
    open = function(self, file)
        self.file = file
        self.data = self.segFile(file)
        self.count = #self.data
        return self.data.count
    end,

    --[[
    Used to create a new instance of mlLines, initialized to zero.
    ]]
    new = function(self)
        local o = self
        setmetatable(o, lines_metatable)
        return o
    end,

    -- returns true if the table is empty
    empty = function(self)
        if self.count == 0 then
            return true
        else
            return false
        end
    end,

    --[[
    [Internal]
    Opens a handle to FILENAME, reads each line, and closes it. Returns
    a table where each index starting from 1 corresponds to a line in
    FILENAME. Thus, index 1 is equal to line 1 in the file.
    ]]
    segFile = function(filename)
        local input, lines, x, done

        input = io.open(filename, "r")
        lines = {}
        x = 1

        -- read each line and append it to the table
        while not(done) do
            local temp = input:read("l")
            if not(temp) then
                done = 1
            else
                -- append to table
                lines[x] = temp
            end
            x = x + 1
        end

        input:close()

        return lines
    end,
}
