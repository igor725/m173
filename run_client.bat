@echo off
if "%JAVA_HOME%"=="" (
  set JAVA_HOME="D:\Programs\jdk-8.80.0.17"
)

set CLIENT_DIR=./client
set DATA_DIR=%CLIENT_DIR%/data/
set JARS_DIR=%CLIENT_DIR%/jars/
set NATIVES_DIR=%CLIENT_DIR%/natives/

if "%1"=="untouched" (
  "%JAVA_HOME%\bin\java.exe" -Djava.library.path="%NATIVES_DIR%" -cp "%JARS_DIR%/*" net.minecraft.client.Minecraft "Test%random%" "-"
) else (
  "%JAVA_HOME%\bin\java.exe" -Djava.library.path="%NATIVES_DIR%" -cp "%JARS_DIR%/*" net.minecraft.launchwrapper.Launch "Test%random%" "-" --tweakClass net.minecraft.launchwrapper.VanillaTweaker --gameDir "%DATA_DIR%" --version "b1.7.3" --userType legacy
)
