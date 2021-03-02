How to simulate the environment for an autonomous vehicle in CARLA
------------------------------------------------------------------
------------------------------------------------------------------
Step 1: Download CARLA
https://github.com/carla-simulator/carla/blob/master/Docs/download.md
I used version 0.9.10
------------------------------------------------------------------
Step 2: Unzip the download
After this step, CARLA is ready to use
------------------------------------------------------------------
Step 3: Test run CARLA
Run the "CarlaUE4.exe"
It should take about 30 seconds to load
Move with WASD, hold LMB to look around
If it runs poorly, restart it with the setting "-quality-level=Low" (launch it from cmd with this flag) - this improves performance by a lot
Optionally use -ResX=200 -ResY=200 to change resolution - has no real impact on performance
------------------------------------------------------------------
Step 4: Run example Python scripts
Example Python scripts are in \PythonAPI\examples
To run a script, you require certain Python packages (numpy, matplotlib, etc.) and the carla package
The carla package is included in the download at \PythonAPI\carla\dist and uses Python 3.7
If you have multiple Python versions installed, use the command "py -3.7 xxx.py" to run a script with the right version
For example the scripts "spawn_npc.py" and "manual_control.py" should be working and have noticeable effect
To change map, run config.py from \PythonAPI\util with the right argument
If another script gives a timeout error, you probably have to specify the host (localhost) and port (2000) in the command line arguments
------------------------------------------------------------------
Step 5: Run custom Python script
Go to \PythonAPI\custom (if it is empty, copy .py from this files directory)
Run main.py (with python 3.7)