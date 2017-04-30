# Taskbar-Context-Menu-Tweaker

A small program restores the taskbar context menu to Windows classic style(Depends on your theme, the same style as the context menu shown when you right click a title bar) by injecting a dynamic library into the explorer process. The program doesn't have any executable EXE file.

Supported OS:
Windows 10 10240, 10586, 14393, 10593 and above, both x86 and x64, arm version can be released if needed
Including technical preview versions
Windows Server 2016
May work on Windows PE systems (No guarantee)

Developed using VC++ 2015
No runtime dependencies! Can work without VC2015 runtime!

Usage:
1. Choose the correct version, e.g. 64 bit dll only work with 64 bit Windows 10
2. Copy the dll into C:\Windows\System32 (Not SysWOW64!)
3. Inject the dll by using the following command:
rundll32 TaskbarContextMenuTweaker.dll,Inject
4. That's all, the contect menu has been set to classic style


Note:
1. After restart explorer.exe, the context menu changes back to Windows 10 modern style. You have to manually execute the injection command. (Maybe I should make a light-weight background process that automatically does this!)
2. Add the command to registry key "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" to achieve automatic injection during starting up.
3. There's also a command for detaching the dll from explorer.exe:
rundll32 TaskbarContextMenuTweaker.dll,Release
4. "Inject" and "Release" in commands are case sensitive!
