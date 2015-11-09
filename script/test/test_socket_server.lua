local fish = require "lualib.fish"
local socket = require "lualib.socket"
local mongo = require "lualib.mongo"
local util = require "lualib.util"

fish.Start(function ()
	fish.Log("test socket server")

	socket.Listen("127.0.0.1",10000,function (source)
		socket.Start(source)
		while true do
			local data = socket.Read(source)
			if data == false then
				fish.Log("socket close")
				return
			end

			socket.Send(source,data)
			fish.Sleep(100)
			socket.Close(source)
			fish.Log("socket close")
			return
		end
	end)
end)

