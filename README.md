# Taskbar-Context-Menu-Tweaker

A small program that restore the classic style context menu on the taskbar.

Supported OS:
Windows 10 10240, 10586, 14393 and above, both x86 and x64
Windows Server 2016
May work on PE systems
May work on preview versions (No guarantee)

Developed using VC++ 2015
Doesn't requires any runtime support, doesn't need VC2015 runtime

The DLL should be copied into C:\Windows\System32

The program doesn't have an executable EXE file, it just have a dll file, by using command:
rundll32 TaskbarContextMenuTweaker.dll,Inject
The dll will be injected into explorer.exe and restore the classic context menu
This command has to be executed manually after restart explorer.exe
This command can be add to registry key "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" to achieve start up with OS

There's also a command to detach the dll from explorer.exe:
rundll32 TaskbarContextMenuTweaker.dll,Release

Note:
Inject and Release in above commands are case sensitive!
