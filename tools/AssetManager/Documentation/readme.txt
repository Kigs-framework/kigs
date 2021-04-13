AssetManager.exe should be called directly from the kigs\tools\win32_executable folder, so that other tools can be called without path.

Usage : 

AssetManager -i inputFolder -o outputPackage -t intermFolder [-p platform]
-i inputFolder : path of the folder where to find raw assets
-o outputFolder : path of ouput folder where to set transformed assets
-t intermFolder : path of an intermediary folder
-p platform : platform appended to AssetManagerRules filename

inputFolder should contains raw assets and AssetManagerRules[platform].json

If platform is not given, then default AssetManagerRules.json is used.

At the end of the conversion, the file "AMLastDone.txt" if writen in the input folder, and it will be used to check the date for the future AssetManager calls. 

AssetManagerRules[platform].json format is as follow :

[
	{"dir_rule" : ["sprites","{tFolder}\sprites.kpkg","kigsPackager.exe -i {iFullPathName} -o {tFolder}\sprites.kpkg"]},
	{"rule" : ["textures\\.*\.png$","{tPath}\{iShortFileName}.ddx","TextureCompress.exe -i {iFullPathName} -o {tPath} -c AUTO -d PC -x"]},
	{"rule" : ["\.png$","{tPath}\{iShortFileName}.dds","TextureCompress.exe -i {iFullPathName} -o {tPath} -c AUTO -d PC"]},
	{"rule" : ["","{tPath}\{iFileName}","copy {iFullPathName} {tPath}\{iFileName}"]},
	{"action" : ["changed","kigsPackager.exe -i {tFolder} -o {oFolder}\assets.kpkg"]}
]

Each rule or action is executed in the order they appear in the file. If a file was matched by a rule, it is marked as matched and will not be checked by any next rule.
If a directory is matched by a dir_rule, all the hierarchy inside this directory will be marked as matched and will not be processed by other rules.
If AssetManagerRules[platform].json was modified after the last conversion, then intermediate file is empty so all the rules and actions should be executed no matter the modification date of each file.

A "rule" or "dir_rule" object has an array of 3 string parameters : {"rule" : [param1,param2,param3]}
- param1 is a regex to search on the local path name inside inputFolder. ie :  "textures\\.*\.png$" match all filenames ending with .png in a "textures" folder. If the regular expression is matched,
  then the rule can be executed. If param1 is not set, then all previously unmatched files can be treated by the rule.
- param2 is the name and path of the "output" file. If the writing date of this file is older than the writing date of input file, then the rule can be executed. If output is not set or output file
  does not exist, then the rule can be executed.
- param3 is the action to be executed if the rule is valid for the given file.


A "action" object does not take an input file. It has 2 string parameters : {"action" : [param1,param2]}
- param1 can be set to "changed" or "always". "changed" execute the action only if a previous rule or action was executed. "always" always execute the action.
- param2 is the action to be executed.


Path separator must be '\'.
All parameters exept first "action" parameter are preprocessed before use. Some {keyword} are replaced by the value corresponding to the current treated file or action.

{iFolder} is the input folder path
{oFolder} is the output folder path
{tFolder} is the intermediary folder path

{iPath} is the input folder + path of the current file inside input folder
{oPath} is the output folder path + path of the current file inside input folder
{tPath} is the intermediary folder + path of the current file inside input folder

{iExt} is the extension of the input file
{iShortFileName} is the filename of the input file without the extension
{iFileName} is the filename of the input file (with extension)

{iFullPathName} is the input folder + path of the current file inside input folder + filename of the input file

For a directory, {iFileName} is the name of the directory and {iPath} is the full path to parent directory.




  






