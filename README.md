# REPOSITORY DESCRIPTION
This repository contains a small program allowing to enable and disable the monitors from a system tray icon or the command-line.

# :warning: WARNING
ShutdownMonitor v1.0.0 uses directly the X11 backend, which seems to be incompatible with Plasma version 5.27.0 (and probably higher).
In Plasma version 5.27.0 and higher use the "Display parameters" to enable or disable a screen.

# FEATURES
Here is a list of the current features of the program:
  - Enable or disable a monitor for the system tray in two clics
  - Disable monitor from the command-line
  - Restore the initial state when quitting
  - Light and dark themes
  
# FUTURE DEVELOPMENTS
Despite the intagration of screen enabling and disabling into Plasma, I may try to interface Shutdown monitor with KScreen, as the interface is much simpler than the current implementation with "Display configuration" applet in system tray and the "Display parameters" configuration panel.

Here are some ideas I plan to implement later:
  - Identify monitors when showing the system tray icon context menu
  - Enable initially disabled monitors
  
# COMPILATION AND INSTALLATION
You can select which interfaces are compiled using the following arguments in qmake command line:
  - Disable the command-line interface using `CONSOLE=no`
  - Disable the system tray interface using `SYSTRAY=no`
You can configure the prefix using `PREFIX=/prefix`
  
To build the program, use
```
$ qmake-qt5 ShutdownMonitor.pro PREFIX=/usr/local CONSOLE=yes SYSTRAY=yes
$ make
```

Then, to install the program, use
```
$ make install
```

# USAGE
## System tray interface
Right click on the system tray icon to make the context menu appear.
In the context menu, you can toogle a monitor by clicking on it.
A blue monitor ![](https://github.com/pasccom/ShutdownMonitor/blob/master/icons/light/enabled-monitor.png) is an enabled monitor, while
a black monitor![](https://github.com/pasccom/ShutdownMonitor/blob/master/icons/light/disabled-monitor.png) is a disabled monitor.

## Command-line interface
Hereafter is a table describing command-line options:
| Short | Long form         | Arguments   | Description                                                    |
| :---- | :---------------- | :---------- | :------------------------------------------------------------- |
| `-t`  | `--toggle-output` |  `<output>` | The outputs to disable before starting (comma-separated list). |
|       |                   |             | This switch can also be repeated to list multiple outputs.     |
| `-l`  | `--list-outputs`  |             | List outputs and quit.                                         |

# LICENSING INFORMATION
ShutdownMonitor is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ShutdownMonitor is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ShutdownMonitor. If not, see http://www.gnu.org/licenses/
