local System = Scripting.System()

ZBS = System:GetDebuggingPath()
package.path = package.path .. "./?.lua;" .. ZBS .. "/lualibs/?/?.lua;" .. ZBS .. "/lualibs/?.lua"
package.cpath = package.cpath .. ZBS .. "/bin_lua/?.dll;" .. ZBS .. "/bin_lua/clibs53/?.dll"

DEBUG = require('mobdebug')

DEBUG.start()
DEBUG.on()


test_var = "A green arrow should point to this variable"

local lua_table = {}

function GetTableDebug()
  return lua_table
  
  end
