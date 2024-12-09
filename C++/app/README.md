# GUI of Archiver project (C++)

## **About program**

AntimAR is an archiver for Windows — a tool for archiving and managing archives.

This program is available in two versions:
* AntimarAR.exe — a version with a graphical interface (GUI);
* AAR.exe — a console version launched from the command line and working in text mode.

Some distinctive features of AntimAR:
* Graphical interface with drag-and-drop support;
* Ability to use command line interface;
* Work with archives of the following formats - ZIP, TAR;
* The ability to set a password for the archive.

## **Graphical shell and command line modes**

AntimAR can be used in two ways: in the graphical shell mode (with the standard interface of Windows programs) and in the command line.

Archiving files in a graphical shell:
* Open AntimAR;
* In the file tree, find and select the desired file or folder;
* Click the button in the context menu 'Commands' -> 'Add to archive' or press the key combination Alt + A or click the 'Add' button on the toolbar;
* In the window that opens, select the path where you want to save the archive and the archiving format;
* Click the 'OK' button.

Extract files in a graphical shell::
* Open AntimAR;
* In the file tree, find and select the desired archive;
* Click the button in the context menu 'Commands' -> 'Extract to folder' or press the key combination Alt + E or click the 'Extract to' button on the toolbar;
* In the dialog box that opens, select the folder where you want to extract the archive;
* Click the 'Select folder' button.

<------------------------------------------------------------------------------>

Archiving files in command line mode:
* Run the executable file AAR.exe from the command line in the following format: 
`AAR.exe <Mode of operation> -s <Source path> -d <Destination path> -f <Data format>`, example `AAR.exe compress -s D:\test\files -d D:\test\compress -f zip`
* Mode of operation takes the values ​​compress;
* Source path - path to folder or file;
* Destination path - the path where the archive will be saved;
* Date format - archiving format (tar or zip).

Extract files in command line mode:
* Run the executable file AAR.exe from the command line in the following format: `AAR.exe <Mode of operation> -s <Source path> -d <Destination path>`, example `AAR.exe decompress -s D:\test\compress\files.zip -d D:\test\decompress`
* Mode of operation takes the values de​​compress
* Source path - path to archive
* Destination path - the path where the files will be extracted

## **Interface**
The interface consists of several parts: context menu, toolbar and file tree.

![](/images/appearance.png)

**Context menu**

**File:**
* Open archive - opening an archive in a file tree
* Copy - copying elements
* Paste - inserting elements
* Highlite all - select all items open in the file tree
* None highlite - removal of selection
* Exit - exit the application

![](/images/file.png)

**Commands:**
* Add to an archive - adding files to archive
* Extract to folder - extract files to folder
* Show file contents - show file contents
* Delete files/folders - delete file or folder
* Rename files/folders - rename file or folder
* Show information - shows the information contained in the file

![](/images/commands.png)

**Help:**
* Manual - brief manual for the application
* About the program - brief information about the application

![](/images/help.png)

**ToolBar:**
* Add - adding files to archive
* Extract to - extract files to folder
* View - show file contents
* Delete - delete file or folder
* Information - shows information about a file or folder

![](/images/toolbar.png)