# QTrayToolBar
Provides a replacement for the custom toolbar functionality that was removed in Windows 11 
提供Windows 11中删除的自定义工具栏功能的替换 
指定一个目录产生菜单项，代替以前的Quick Launch 

# 与github上的TrayToolBar区别
* TrayToolBar使用C#编写，QTrayToolBar基于Qt，使用C++编写，主要是我不会C#
* 增加了菜单的内每个item的拖动，可以按自己喜欢的顺序排列
* 在设置里可对系统所有的TrayIcon做拖动排序（需要重启），还可以按Delete删除无用的icon（比如已经卸载的程序）
* 针对文件管理器等快捷方式，可以正确显示中文名
* 同时增加多个目录菜单，并且使用不同的图标，方便区别查看