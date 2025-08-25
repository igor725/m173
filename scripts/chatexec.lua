local scriptInfo = ...

scriptInfo.name = 'Chat script executor'
scriptInfo.description = 'This script executes passed to chat Lua string'
scriptInfo.version = '0.0.1'

local execEnvMeta = {}
execEnvMeta.__index = _G

local function runScript(runner, script)
  if not script or #script == 0 then
    return '\xc2\xa7cAttempt to execute an empty string was made'
  end

  local env = setmetatable({ self = runner }, execEnvMeta)
  local chunk, err = load(script, 'chatexec', 't', env)

  if chunk ~= nil then
    local out = { pcall(chunk) }

    if table.remove(out, 1) == false then
      return '\xc2\xa7cRuntime error\xc2\xa7f: ' .. out[1]
    end

    if #out > 0 then
      for i = 1, #out do
        ---@diagnostic disable-next-line: assign-type-mismatch
        out[i] = tostring(out[i])
      end

      return '\xc2\xa7aScript executed\xc2\xa7f: ' .. table.concat(out, ', ')
    end

    return '\xc2\xa7aScript executed successfully'
  end

  return '\xc2\xa7cCompilation failed\xc2\xa7f: ' .. err
end

local function processEvent(evname, evarg)
  if evname == 'onMessage' then
    local msg = evarg:message()
    if msg:find('^>') then
      local sender = evarg:sender()
      if not sender:isLocal() and not sender:isOperator() then
        return '\xc2\xa7cYou don\'t have permissions to execute Lua code'
      end

      evarg:cancel()
      msg = msg:gsub('^(>=?)', function(str)
        if str == '>=' then
          return 'return '
        end

        return ''
      end)

      sender:chat(runScript(sender, msg))
      return
    end
  end
end

while true do
  processEvent(coroutine.yield())
end
