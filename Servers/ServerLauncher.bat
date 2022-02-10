echo WScript.sleep 1000 >sss.vbs

start DbServerLauncher.bat
sss.vbs

start CenterServerLauncher.bat
sss.vbs

start GateServerLauncher.bat
sss.vbs

start LoginServerLauncher.bat