#!/bin/bash

CLIENT_VER=173
CLIENT_DIR="./client"
CLIENT_DAT="$CLIENT_DIR/data/$CLIENT_VER/"
JMC_CLASSPATH="$CLIENT_DIR/jars/libs/*:$CLIENT_DIR/jars/client$CLIENT_VER.jar"

if [ "$1" == "untouched" ]; then
  java -Djava.library.path="$CLIENT_DIR/natives/" -cp "$JMC_CLASSPATH" net.minecraft.client.Minecraft "Test$RANDOM" "-"
else
  java -Djava.library.path="$CLIENT_DIR/natives/" -cp "$JMC_CLASSPATH" net.minecraft.launchwrapper.Launch "Test%random%" "-" --tweakClass net.minecraft.launchwrapper.VanillaTweaker --gameDir "$CLIENT_DAT" --version "b1.7.3" --userType "legacy" --server "127.0.0.1" --port 25565
fi
