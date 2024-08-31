@echo off
if "%JAVA_HOME%"=="" (
  set JAVA_HOME="D:\Programs\jdk-8.80.0.17"
)

set CLIENT_VER=181
set CLIENT_DIR=./client
set DATA_DIR=%CLIENT_DIR%/data/%CLIENT_VER%/
set JARS_DIR=%CLIENT_DIR%/jars/
set NATIVES_DIR=%CLIENT_DIR%/natives/
set JMC_CLASSPATH=%JARS_DIR%/libs/*;%JARS_DIR%/client%CLIENT_VER%.jar

if "%1"=="untouched" (
  "%JAVA_HOME%\bin\java.exe" -Djava.library.path="%NATIVES_DIR%" -cp "%JMC_CLASSPATH%" net.minecraft.client.Minecraft "Test%random%" "-"
) else (
  "%JAVA_HOME%\bin\java.exe" -Djava.library.path="%NATIVES_DIR%" -cp "%JMC_CLASSPATH%" net.minecraft.launchwrapper.Launch "Test%random%" "-" --tweakClass net.minecraft.launchwrapper.VanillaTweaker --gameDir "%DATA_DIR%" --userType legacy
)
