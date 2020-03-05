local result = 
{
   item("button", "UIButtonImage", {Priority=10, UpTexture="Up.png", DownTexture="Down.png",Dock={0.9,0.1},Anchor={0.5,0.5}}).items(
      connect("this","ClickUp" ,"../UIItem:panel","Click")
   ),
   item("text","UIText",{Priority=10,Text="Click button ==>",Dock={0.8,0.1},Anchor={0.5,0.5},SizeX=-1.0,SizeY=-1.0,Font="Calibri.ttf",FontSize=24.0,MaxWidth=200})
}
return result