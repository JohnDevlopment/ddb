-- add functions to "string"
string["char"] = function(self, idx)
    assert(type(idx) == "number", idx .. " is not a valid integer!")
    return self:sub(idx, idx)
end

string["append"] = function(self, str)
    self = self .. str
    return self
end

string["length"] = function(self)
    return #self
end

-- add functions to "io"
io["writef"] = function(out, fmt, ...)
    local str = string.format(fmt, ...)
    out:write(str)
end

io["cout"] = function(...)
    local stdout = io.stdout
    local nextarg
    local str = ""

    local n = select("#", ...)
    for i=1,n,1 do
        str = str .. tostring(select(i, ...))
    end
    io.write(str)
end

io["cerr"] = function(...)
    local oldhandle = io.output()
    local nextarg
    local str = ""

    -- switch to stderr as output handle
    io.output(io.stderr)

    local n = select("#", ...)
    for i=1,n,1 do
        str = str .. tostring(select(i, ...))
    end
    io.write(str)

    io.output(oldhandle)
end

cout = io.cout
cerr = io.cerr
cin = io.read
fprintf = io.writef

function _assert(exp, msg)
    assert(exp, "assertion failed: " .. msg)
end

function _loadfile(file)
    local temp = loadfile(file)
    _assert(temp ~= nil, "_loadfile: NULL chunk")
    return temp
end

function _load(chunk)
    _assert(chunk, "_load: 'chunk' parameter is NULL!")
    local temp = load(chunk)
    _assert(temp ~= nil, "_load: NULL chunk")
    return temp
end
