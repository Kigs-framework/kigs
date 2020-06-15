local result = 
{
    -- UIText is parent of UIButtonImage so UIButtonImage size is adapted to text size
	item("camup_txt","UIText",{Priority=11,Text="#CamUp",Dock={0.5,0.05},Anchor={0.5,0.5},SizeX=-1.0,SizeY=-1.0,Font="Calibri.ttf",FontSize=24.0,MaxWidth=128, Color={0,0,0.4}}).items(
		-- UIButtonImage have SizeModeX and Y set to "Add" to be 16 points bigger than parent size
		item("camup_btn", "UIButtonImage", {Priority=10, UpTexture="Up.png", DownTexture="Down.png",Dock={0.5,0.5},Anchor={0.5,0.5},Sliced={5.0,5.0},SizeModeX="Add",SizeModeY="Add", SizeX = 16, SizeY = 16}).items(
		-- clickUp signal on this is connected to MoveUp slot on a Node3D called Camera ( the Node3D Camera must exist when lua file is loaded )
		connect("this","ClickUp" ,"Node3D:Camera","MoveUp")
		)
    ),
	-- same process for second button
  	item("camdown_txt","UIText",{Priority=11,Text="#CamDwn",Dock={0.5,0.95},Anchor={0.5,0.5},SizeX=-1.0,SizeY=-1.0,Font="Calibri.ttf",FontSize=24.0,MaxWidth=128, Color={0,0,0.4}}).items(
		item("camdown_btn", "UIButtonImage", {Priority=10, UpTexture="Up.png", DownTexture="Down.png",Dock={0.5,0.5},Anchor={0.5,0.5},Sliced={5.0,5.0},SizeModeX="Add",SizeModeY="Add", SizeX = 16, SizeY = 16}).items(
		connect("this","ClickUp" ,"Node3D:Camera","MoveDown")
		)
	),
	-- add a third button to go to another sequence
	item("next_txt","UIText",{Priority=11,Text="#Next",Dock={0.95,0.5},Anchor={0.5,0.5},SizeX=-1.0,SizeY=-1.0,Font="Calibri.ttf",FontSize=24.0,MaxWidth=128, Color={0,0,0.4}}).items(
		item("next_btn", "UIButtonImage", {	Priority=10, UpTexture="Up.png", DownTexture="Down.png",Dock={0.5,0.5},Anchor={0.5,0.5},Sliced={5.0,5.0},
											SizeModeX="Add",SizeModeY="Add", SizeX = 16, SizeY = 16 , Parameter="SecondSequence.xml", ClickUpAction ="ChangeSequence" })
	)
}
return result