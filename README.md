# Taskbar-Context-Menu-Tweaker

# Brief Introduction
A small program brings back the familiar Windows classic style to your taskbar context menu.
(Windows Classic style menu has an appearance which depends on your theme, it looks the context menu popup 
when you right click a title bar).
The tweaker works by injecting a dynamic library into the explorer process and then intercept some Windows APIs.
This program doesn't have any executable EXE file.

The successor of legay `Taskbar-Context-Menu-Tweaker` (Check out legacy branch), the new version employs a new code injection mechanism 
([MinHook](https://www.codeproject.com/Articles/44326/MinHook-The-Minimalistic-x-x-API-Hooking-Libra)) which reduces
number of crashes.


# Supported OS
1. Windows 10 10240, 10586, 14393, 10593 and 16299 (both x86 and x64)
1. Including technical preview versions
1. Windows Server 2016
1. __MAY__ work on Windows PE systems (No guarantee)

Developed with VC++ 2017

__NO runtime dependencies!__ Work without VC2017 runtime!

# Usage:
1. Choose the correct version, e.g. 64 bit dll only work with 64 bit Windows 10
2. Copy the dll into `C:\Windows\System32` (Not SysWOW64, ALWAYS System32)
3. Inject the dll by using the following command:
`rundll32 TaskbarContextMenuTweaker.dll,Inject`
4. That's all, the contect menu has been set to classic style

# Note:
1. After restart explorer.exe, the context menu changes back to Windows 10 modern style. 
You have to manually execute the injection command. 
(Maybe I should make a light-weight background process which automatically perform this!)
1. Add the command to registry key `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run` to achieve automatic injection during starting up.
1. There's also a command for detaching the dll from explorer.exe:
`rundll32 TaskbarContextMenuTweaker.dll,Release`
1. "Inject" and "Release" are case sensitive!
