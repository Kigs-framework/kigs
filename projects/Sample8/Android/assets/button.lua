local result = 
{
	item("camup_txt","UIText",{Priority=11,Text="Cam up",Dock={0.5,0.05},Anchor={0.5,0.5},SizeX=-1.0,SizeY=-1.0,Font="Calibri.ttf",FontSize=24.0,MaxWidth=64}).items(
		item("camup_btn", "UIButtonImage", {Priority=10, UpTexture="Up.png", DownTexture="Down.png",Dock={0.5,0.5},Anchor={0.5,0.5},Sliced={5.0,5.0},SizeModeX="Add",SizeModeY="Add"}).items(
		connect("this","ClickUp" ,"Node3D:Camera","MoveUp")
		)
    ),
  	item("camdown_txt","UIText",{Priority=11,Text="Cam down",Dock={0.5,0.95},Anchor={0.5,0.5},SizeX=-1.0,SizeY=-1.0,Font="Calibri.ttf",FontSize=24.0,MaxWidth=64}).items(
		item("camdown_btn", "UIButtonImage", {Priority=10, UpTexture="Up.png", DownTexture="Down.png",Dock={0.5,0.5},Anchor={0.5,0.5},Sliced={5.0,5.0},SizeModeX="Add",SizeModeY="Add"}).items(
		connect("this","ClickUp" ,"Node3D:Camera","MoveDown")
		)
  )
}
return result