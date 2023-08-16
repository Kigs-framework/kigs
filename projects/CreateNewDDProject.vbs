'CreateNewDDProject.vbs

Option Explicit

'Dim logFile
Const TristateFalse = 0 ' the value for ASCII
Const Overwrite = True

Sub ReplaceNames(Filename, OldText, NewText)

	Dim f
	f=FileSystem.GetExtensionName(Filename)
	If f = "data" Or f = "js" Or f = "mem" Or f = "png" Or f = "ttf" Or f = "webp" Or f = "jar" Then 
		Exit Sub
	End If

	'logFile.WriteLine "ReplaceNames " + Filename
	
	Const TemporaryFolder = 2

	Dim OriginalFile, TempFile, Line
	Dim TempFilename


	Dim tempFolder: tempFolder = FileSystem.GetSpecialFolder(TemporaryFolder)
	TempFilename = FileSystem.GetTempName

	If FileSystem.FileExists(TempFilename) Then
		FileSystem.DeleteFile TempFilename
	End If

	Set TempFile = FileSystem.CreateTextFile(TempFilename, Overwrite, TristateFalse)
	Set OriginalFile = FileSystem.OpenTextFile(Filename)

	Do Until OriginalFile.AtEndOfStream
		Line = OriginalFile.ReadLine

		If InStr(Line, OldText) > 0 Then
			Line = Replace(Line, OldText, NewText)
		End If 

		TempFile.WriteLine(Line)
	Loop

	OriginalFile.Close
	TempFile.Close

	FileSystem.DeleteFile Filename
	Dim newFileName 
	newFileName=Replace(Filename, OldText, NewText)

	FileSystem.MoveFile TempFilename, newFileName
End Sub

Sub ChangeAllFiles(FolderName,OldText,NewText)


	Dim Folder
	Set Folder = FileSystem.GetFolder(FolderName)
    Dim colFiles
	Set colFiles = Folder.Files
	Dim fullpath
	Dim objFile,Subfolder 
	
	For Each objFile in colFiles
		if Mid(objFile.Name,len(objFile.Name)-3,4) <> ".pfx" Then
			fullpath = FolderName + "/" + objFile.Name
			ReplaceNames fullpath, OldText, NewText  
		End If
	Next

    For Each Subfolder in Folder.SubFolders

        ChangeAllFiles Subfolder, OldText, NewText

    Next

End Sub

Sub RenameAllFolders(FolderName,OldText,NewText)

	Dim newFolderName 
	newFolderName=Replace(FolderName, OldText, NewText)
	if StrComp(FolderName,newFolderName) <> 0 Then
		Dim FolderFullPath
		FolderFullPath = FileSystem.GetAbsolutePathName(FolderName)
		Dim newFolderFullPathName 
		newFolderFullPathName=Replace(FolderFullPath, OldText, NewText)
		
		FileSystem.MoveFolder FolderFullPath, newFolderFullPathName
		
		FolderName=newFolderName
	End If
	
	
	Dim Folder
	Set Folder = FileSystem.GetFolder(FolderName)
    Dim colFiles
	Set colFiles = Folder.Files
	Dim fullpath
	Dim objFile,Subfolder 

    For Each Subfolder in Folder.SubFolders

        RenameAllFolders Subfolder, OldText, NewText

    Next

End Sub

Function CreateGUID
  Dim TypeLib
  Set TypeLib = CreateObject("Scriptlet.TypeLib")
  CreateGUID = Mid(TypeLib.Guid, 2, 36)
End Function



dim startFolder,OldText,NewText,lowerOldText,lowerNexText

Dim FileSystem
Set FileSystem = CreateObject("Scripting.FileSystemObject")

'Set logFile = FileSystem.CreateTextFile("CreateProjectLog.txt", Overwrite, TristateFalse)

Dim strAnswer
strAnswer = InputBox("Please enter the new project name:", "New Kigs Data Driven Project")

'The location of the zip file.
Dim ZipFile
ZipFile=FileSystem.GetAbsolutePathName("./DDProject.zip")
'The folder the contents should be extracted to.
Dim ExtractTo
ExtractTo=strAnswer + "\"

'If the extraction location does not exist create it.

If NOT FileSystem.FolderExists(ExtractTo) Then
   FileSystem.CreateFolder(ExtractTo)
End If

Dim ExtractFolder
ExtractFolder=FileSystem.GetAbsolutePathName(ExtractTo)

'Extract the contants of the zip file.
Dim objShell, FilesInZip
Set objShell = CreateObject("Shell.Application")
Set FilesInZip=objShell.NameSpace(ZipFile).items
objShell.NameSpace(ExtractFolder).CopyHere(FilesInZip)
Set objShell = Nothing

startFolder = ExtractTo
OldText = "DDProjectName"
NewText = strAnswer

' first change all folder names
RenameAllFolders startFolder, OldText, NewText 
ChangeAllFiles startFolder, OldText, NewText 

' for android do it also in lower caps
lowerOldText =  Lcase(OldText)
lowerNexText = Lcase(NewText)
RenameAllFolders startFolder, lowerOldText, lowerNexText 
ChangeAllFiles startFolder, lowerOldText, lowerNexText 


' change Guid
ChangeAllFiles startFolder, "B1EEDF0E-DA7E-4E8E-9AAA-7E33F37F6A31", CreateGUID
ChangeAllFiles startFolder, "224F564A-CD63-446F-9120-BEACDD4B6D34", CreateGUID
ChangeAllFiles startFolder, "AA4971FB-28BD-45AD-8602-DA8CF20C0FC7", CreateGUID
ChangeAllFiles startFolder, "6CE311F1-90B6-4CAD-8E5C-A1A5FFECD6DE", CreateGUID
' iOS
ChangeAllFiles startFolder, "A11655AB-3658-4E48-8469-31FCB0045FD0", CreateGUID
ChangeAllFiles startFolder, "F0809D00-19A8-46BC-9AAF-433E87E02A81", CreateGUID
ChangeAllFiles startFolder, "FE3D9154-E038-4AB4-AEC2-7E9E0676429F", CreateGUID
' WUP
ChangeAllFiles startFolder, "e01d4331-70cf-481d-8639-f0e749ded9d1", CreateGUID

' cmake app project guid
ChangeAllFiles startFolder, "E40B7E25-3BB8-48B2-8CFF-E12BEE76CFEB", CreateGUID

'logFile.Close

Wscript.Quit