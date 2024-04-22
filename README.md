# REPOSITORY DESCRIPTION
This repository contains a small program allowing to enable and disable the monitors from a system tray icon or the command-line.

# :warning: WARNINGS
ShutdownMonitor v1.0.0 uses directly the X11 backend, which seems to be incompatible with Plasma version 5.27.0 (and probably higher).
This warning does not apply to ShutdownMonitor v2.0.0 and v3.0.0, which can use KScreen as a backend.

I have noticed that shutting down and restoring monitors (especially external ones) using ShutdownMonitor v3.0.0 with KScreen backend
in Plasma version 6.0.4 under Wayland causes some issues.
This is due to the fact that KScreen or Wayland considers the disabled monitors as removed.

# FEATURES
Here is a list of the current features of the program:
  - Enable or disable a monitor for the system tray in two clics
  - Disable monitor from the command-line
  - Restore the initial state when quitting
  - Light and dark themes
  - Works with X11 or KScreen with a simpler interface than the one provided natively by KScreen ("Display configuration" applet in system tray and the "Display parameters" configuration panel)
  
# FUTURE DEVELOPMENTS
Here are some ideas I plan to implement later:
  - Identify monitors when showing the system tray icon context menu
  - Enable initially disabled monitors
  
# COMPILATION AND INSTALLATION
## CMake
As of ShutdownMonitor v3.0.0, CMake becomes the default build system.


You can select which user interfaces are compiled using the following CMake options:
  - `CONSOLE_UI` Command-line interface
  - `SYSTRAY_UI` System tray interface

You can select which backends are compiled using the following Cmake options:
  - `KSCREEN6_BACKEND` KScreen6 backend (for Plasma 6), needs Qt 6
  - `KSCREEN5_BACKEND` KScreen2 backend (for Plasma 5), needs Qt 5
  - `X11_BACKEND` X11 backend, supports both Qt 5 and Qt 6, but see [the warnings](#warning-warnings)

You can configure the prefix using CMake `--prefix` option.

To build the program with all interfaces and backends enabled (which is the default), use
```
$ cmake -DCONSOLE_UI=ON -DSYSTRAY_UI=ON -DX11_BACKEND=ON -DKSCREEN5_BACKEND=ON -DKSCREEN6_BACKEND=ON /path/to/source
$ make
```

Then, to install the program, use
```
$ cmake --install . --prefix /usr/local
```

## qMake
As of ShutdownMonitor v3.0.0, qMake is deprecated.

You can select which interfaces are compiled using the following arguments in qmake command line:
  - Disable the command-line interface using `CONSOLE=no`
  - Disable the system tray interface using `SYSTRAY=no`

You can select which backends are compiled using the following arguments in qmake command line:
  - Disable the KScreen backend using `KSCREEN=no`
  - Disable the X11 backend using `X11=no`

You can configure the prefix using `PREFIX=/prefix`
  
To build the program with all interfaces and backends enabled (which is the default), use
```
$ qmake-qt5 ShutdownMonitor.pro PREFIX=/usr/local CONSOLE=yes SYSTRAY=yes KSCREEN=yes X11=yes
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
| Short | Long form          | Arguments    | Description                                                           |
| :---- | :----------------- | :----------- | :-------------------------------------------------------------------- |
| `-t`  | `--toggle-output`  |  `<output>`  | The outputs to disable before starting (comma-separated list).        |
|       |                    |              | This switch can also be repeated to list multiple outputs.            |
| `-l`  | `--list-outputs`   |              | List outputs and quit.                                                |
|       | `--theme`          | `<theme>`    | The theme to be used by the system tray interface.                    |
|       |                    |              | This option is available only when the systray interface is built in. |
|       | `--list-backends`  |              | Lists the available backend (usable with the \c --backend switch).    |
|       | `--backend`        | `<backend>`  | The backend to be used (if it cannot be used the program will stop).  |
|       |                    |              | By default, the first usable backend is selected.                     |

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
