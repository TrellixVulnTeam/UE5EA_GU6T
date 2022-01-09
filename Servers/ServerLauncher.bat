echo WScript.sleep 500 >sss.vbs

start DbServerLauncher.bat
sss.vbs

start GateServerLauncher.bat
sss.vbs

start LoginServerLauncher.bat