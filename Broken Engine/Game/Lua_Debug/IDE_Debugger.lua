--set ZBS=C:\Users\didac\Downloads\ZeroBraneStudio
--set LUA_PATH=./?.lua;%ZBS%/lualibs/?/?.lua;%ZBS%/lualibs/?.lua
--set LUA_CPATH=%ZBS%/bin/?.dll;%ZBS%/bin/clibs/?.dll

-- ZBS = "C:/Users/didac/Downloads/ZeroBraneStudio"
-- package.path = package.path .. "./?.lua;" .. ZBS .. "/lualibs/?/?.lua;" .. ZBS .. "/lualibs/?.lua"
-- package.cpath = package.cpath .. ZBS .. "/bin/?.dll;" .. ZBS .. "/bin/clibs53/?.dll"
--require('socket')
local System = Scripting.System()

ZBS = System:GetDebuggingPath()
package.path = package.path .. "./?.lua;" .. ZBS .. "/lualibs/?/?.lua;" .. ZBS .. "/lualibs/?.lua"
package.cpath = package.cpath .. ZBS .. "/bin/?.dll;" .. ZBS .. "/bin/clibs53/?.dll"

DEBUG = require('mobdebug')

DEBUG.start()
DEBUG.on()


test_var = "A green arrow should point to this variable"

local lua_table = {}

function GetTableDebug()
  return lua_table
  
  end
