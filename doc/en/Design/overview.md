# RealUI System

RealUI system is an efficient embedded solution for display projects based on HoneyGUI.

## RealUI Workflow

The workflow of the RealUI system is mainly divided into four steps.

<center><img src="https://foruda.gitee.com/images/1703054193639447830/2052fd20_9325830.png",alt="GUI flow.png"/></center>

### System

System initialization mainly includes the initialization of the system clock, the initialization of peripherals and the initialization of other modules of the project, such as [PSRAM](/Glossary.rst#term-PSRAM), [LCD](/Glossary.rst#term-LCD), [TP](/Glossary.rst#term-TP), and Bluetooth.

### GUI Server

First, the parts of the GUI port that have been filled in advance are initialized, including the operating system, display, input, and file system. The GUI server thread is then created and the GUI server runs continuously in GUI thread.

### GUI Application

A GUI application is a series of display interfaces consisting of multiple widgets. A GUI app is a series of display interfaces consisting of multiple widgets. In order to run a GUI APP, it needed to be started.

### GUI Server Task

GUI server is the running function of GUI task, and its specific running process is divided into six parts:

1. GUI APP EXIST: First, the currently running GUI APP needs to be obtained. When the GUI detects that there is a running GUI APP, it will proceed to the next step;
2. GET LCD DATA: Get real-time information about the screen;
3. GET TP DATA: Get real-time information about the touchpad and run the touch algorithm;
4. GET [KB](/Glossary.rst#term-KB) DATA: Get real-time information about the keyboard and run the keyboard algorithm;
5. OBJ DRAW: Drawing widgets in the APP, including functional operations and image processing.
6. UPDATE [FB](/Glossary.rst#term-FB): Delivers the drawn results to the screen.

More detailed operation of the GUI APP can be found in the online documentation.
