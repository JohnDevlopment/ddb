local _id, _answer, _fdefname

print("hello world")

-- file information
do
    local _subdir, _file, _check

    function _check(str, name)
        assert(name and name ~= "", "Invalid 'name' parameter")
        assert(str and str ~= "", "You did not enter anything for the '" .. name .. "' field")
    end

    -- subdirectory
    cout('Enter the relative path of the file: ')
    _subdir = cin('l')
    _check(_subdir, "subdir")
    _subdir = path.join('..', _subdir)
    assert(os.isdir(_subdir), _subdir .. " doesn't exist")

    -- file
    cout('Enter the name of the file: ')
    _file = cin('l')
    _check(_file, "file")
    _fdefname = _file
    _file = path.join(_subdir, _file)
    assert(not(os.isfile(_file)), _file .. " already exists")

    -- open file; buffers until end-of-line
    _id = io.open(_file, 'w')
    _id:setvbuf("line")

    -- header guard name
    _fdefname = string.upper(_fdefname)
    _fdefname = replaceChars(_fdefname, '.', '_')
end

-- inclusion guard and header files
do
    fprintf(_id, "#ifndef %s\n#define %s\n\n", _fdefname, _fdefname)

    local _locx, _sysx = 1, 1

    -- header include tables
    local _sysincs = {}
    local _localincs = {}

    cout("\nHeaders: each local header is prefixed with a 'l_'. ")
    cout("Each system header is not.\n")
    cout("To quit processing input, press enter without typing anything.\n")

    local _done = false
    while not(done) do
        cout("Header: ")
        _answer = cin('l')

        -- break loop
        if _answer == "" then break end

        -- local for system header?
        if _answer:sub(1, 2) == 'l_' then
            _localincs[_locx] = _answer:sub(3)
            _locx = _locx + 1
        else
            _sysincs[_sysx] = _answer
            _sysx = _sysx + 1
        end
    end

    -- print each system header first
    for i,v in ipairs(_sysincs) do
        fprintf(_id, "#include <%s>\n", v)
    end

    -- print each local header
    if #_sysincs > 0 then
        fprintf(_id, "\n")
        for i,v in ipairs(_localincs) do
            fprintf(_id, "#include \"%s\"\n", v)
        end
    end
end
-- end do

--[[
collectgarbage()

cout("You have the option of defining a class. Do you want a class? (Y/n) ")
_answer = cin(1)
if string.lower(_answer) == 'y' then
    local class, func1, func2, constructors
    local memberfields_prot, memberfields_priv, memberfields_publ

    -- define class
    cin('l')
    cout("What's the name of this class? ")
    _answer = cin('l')

    if _answer == "" then
        cerr("Invalid class name\n")
        goto QuitClass
    end
    class = _answer

    -- func1
    func1 = function(s, clear)
        if clear ~= 0 then
            cin('l')
        end
        cout(s, " (Y/n) ")

        local temp = cin(1)
        if temp == "" then
            return "n"
        end
        return string.upper(temp)
    end

    -- func2
    func2 = function(table, field, out)
        if table[field] then
            fprintf(out, "%s\n", table[field])
        end
    end

    constructors = {}

    -- class fields
    cout("\nOptions, pick yes or no\n")

    if func1("Class has a constructor", 0) == 'Y' then
        constructors["ctor"] = string.format("%s();", class)
    end
    if func1("Class has a copy constructor", 1) == 'Y' then
        constructors["copy"] = string.format("%s(const %s& src);", class, class)
    end
    if func1("Class has a move constructor", 1) == 'Y' then
        constructors["move"] = string.format("%s(%s&& src);", class, class)
    end

    -- data fields (private)
    memberfields_prot, memberfields_priv, memberfields_publ = {}, {}, {}

    _answer = "a"
    cout("Data member names. Prefix with a 'P_' to make it public, ")
    cout("prefix with 'p_' for protected, or no prefix to leave it private.\n")
    cout("To quit processing input, press enter without typing anything.\n")
    while _answer ~= "" do
        _answer = cin('l')
        if _answer ~= "" then
            local temp = _answer:sub(1, 2)
            if temp == "P_" then
                table.insert(memberfields_publ, _answer:sub(3))
            elseif temp == "p_" then
                table.insert(memberfields_prot, _answer:sub(3))
            else
                table.insert(memberfields_priv, _answer)
            end
        end
    end

    fprintf(_id, "class %s {\npublic:\n\t", class)

    func2(constructors, "ctor", _id)
    func2(constructors, "copy", _id)
    func2(constructors, "move", _id)

    fprintf(_id, "};\n")
end
]]

::QuitClass::

-- close header
fprintf(_id, "\n#endif /* %s */", _fdefname)

_id:close()
