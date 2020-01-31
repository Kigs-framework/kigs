local Sample7Script = { logo=0; startingTime=0 }

function Sample7Script:init()

	local logoSP = CoreModifiable("logo","UIImage")
	self.logo=logoSP:get()
	self.logo.Texture = "KigsHD.png"
	self.logo.Dock = {0.5,0.5}
	self.logo.Anchor = {0.5,0.5}
	self.logo.Priority = 10
	
	self.target:addItem(logoSP)
	
	self.logo:init()
end

function Sample7Script:update(current_time)
	self.logo.Dock = {0.5+0.3*math.sin(current_time-self.startingTime),0.5+0.3*math.cos((current_time-self.startingTime)*0.7)}
end

function Sample7Script:CoreMethodreset(current_time)
	self.startingTime = current_time
end

return Sample7Script