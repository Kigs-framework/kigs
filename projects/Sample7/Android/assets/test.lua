local Sample7Script = { logo=0; startingTime=0; 
WRAP_METHODS = {"reset"}; }

function Sample7Script:init()

	-- create an UIImage instance named logo (get a smart pointer)
	local logoSP = CoreModifiable("logo","UIImage")
	-- self.logo is now the CoreModifiable object 
	self.logo=logoSP:get()
	-- set CoreModifiable attributes directly
	self.logo.TextureName = "KigsHD.png"
	self.logo.Dock = {0.5,0.5}
	self.logo.Anchor = {0.5,0.5}
	self.logo.Priority = 10
	-- then add logo (smart pointer) to target
	self.target:addItem(logoSP)
	
	-- and init logo
	self.logo:init()
	
end

function Sample7Script:reset(current_time)
   self.startingTime = current_time
   local rotate = self.logo.RotationAngle
   rotate = rotate + 0.1
   self.logo.RotationAngle = rotate
end

function Sample7Script:update(current_time)
	self.logo.Dock = {0.5+0.3*math.sin(current_time-self.startingTime),0.5+0.3*math.cos((current_time-self.startingTime)*0.7)}
end

return Sample7Script