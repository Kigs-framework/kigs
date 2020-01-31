local ScriptOnApp = {} 

function ScriptOnApp:update(current_time)
	self.target:HelloFromLua()
end

return ScriptOnApp