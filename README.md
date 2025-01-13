# ReInput - Unreal Engine MultiKeyboard




## Overview

ReInput is a plugin for Unreal Engine that enables support for multiple keyboards on a single system. This project was originally developed in August 2023 during my downtime between jobs. At the time, I was planning to bundle a suite of controller functionality into a library called ReInput, and list on both the Unity and Unreal asset stores. However, I haven’t had the time to see that through.



## Installation

1. Drop the plugin folder into your project's `Plugins` directory.

2. Restart Unreal Engine



### Debug Messages

The plugin includes on-screen debug messages to quickly demonstrate that everything works. You can disable them by deleting the lines in the MultiKeyboardDevice.cpp file.



## Known Issues

### Input Conflicts in Editor

In its current state, the plugin may cause inputs while in the editor to trigger unintended actions. I recommend disabling the plugin while working in the editor and re-enabling it when creating a packaged build. A fix for this is planned I just haven't gotten around to it.



### Windows-Specific Implementation

Currently, this plugin leverages Windows-specific libraries, meaning it will not function on macOS or Linux. I’m looking into a cross-platform implementation.



## Usage

If you use this in a project of yours all I ask is that you provide credit to either this project (as "ReInput") or me (as "Joe [ReRezd]")
