# Taskbar-Context-Menu-Tweaker

![Image](/screenshot.png)

[AskVG](https://www.askvg.com/tip-get-rid-of-dark-context-menu-in-windows-10-taskbar/)

[Registry tweaks](https://www.askvg.com/registry-tweaks-for-customizing-windows-10-taskbar-context-menu/)

[[Windows 10 Tip] Restore Classic Windows 7 Style Context Menus in Explorer and Desktop](https://www.askvg.com/windows-10-tip-restore-classic-context-menu-in-explorer-and-desktop/)

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
1. Windows 10 10240, 10586, 14393, 10593, 16299, 17134 (both x86 and x64, up to 1803)
1. Windows Server 2016
1. __MAY__ work on technical preview versions
1. __MAY__ work on Windows PE systems (No guarantee)
1. Arm and Arm64 are not supported yet but will be supported in the future, feel free to post an issue if you really want one

Developed with VC++ 2017

__NO runtime dependencies!__ Work without VC2017 runtime!

# Usage:
1. Choose the correct version, e.g. 64 bit dll only work with 64 bit Windows 10
2. Copy the dll into `C:\Windows\System32` (Not SysWOW64, ALWAYS System32)
3. Inject the dll by using the following command:
`rundll32 TaskbarContextMenuTweaker.dll,Inject`
4. That's all, the contect menu has been set to classic style

# Customization
Customizing appearance of the context menu can be done with some registry tweaks.
1. Open `regedit.exe` with administrative privilege
2. Navigate to `HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer`
3. Create a DWORD value `ContextMenuConfig` if it does not exist.
4. Its value would be set to any of the following:
 * 0 - to show classic context menu in Taskbar (Default)
 * 1 - to show dark modern (immersive) context menu in Taskbar
 * 2 - to show icons in menu
5. Create a String value `ToggleMenuTitle` if you want to add your name or 
other text string in Taskbar context menu



# Note:
1. After restart explorer.exe, the context menu changes back to Windows 10 modern style. 
You have to manually execute the injection command. 
(Maybe I should make a light-weight background process which automatically perform this!)
1. Add the command to registry key `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run` to achieve automatic injection during starting up.
1. There's also a command for detaching the dll from explorer.exe:
`rundll32 TaskbarContextMenuTweaker.dll,Release`
1. "Inject" and "Release" are case sensitive!
