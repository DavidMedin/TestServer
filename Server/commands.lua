function Test(input)
	print("hello from the function test!, and you gave " .. tostring(input) .. " as an input!")
end
function Quit()
	print("quitting")
	ServerQuit()
	-- CmdQuit=1
end
function SendText(string)
	ServerSendText(string)
end
function SendStuff(...)
	for k,v in pairs{...} do
		ServerSendText(v)
	end
end

function Reply(msg)
	ServerReplyText(msg)
end