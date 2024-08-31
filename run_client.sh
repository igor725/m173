#!/bin/bash

CLIENT_DIR="./client"
JMC_CLASSPATH="$CLIENT_DIR/jars/libs/*:$CLIENT_DIR/jars/client.jar"

if [ "$1" == "untouched" ]; then
  java -Djava.library.path="$CLIENT_DIR/natives/" -cp "" net.minecraft.client.Minecraft "Test$RANDOM" "-"
else
  java -Djava.library.path="$CLIENT_DIR/natives/" -cp "$CLIENT_DIR/jars/*" net.minecraft.launchwrapper.Launch "Test%random%" "-" --tweakClass net.minecraft.launchwrapper.VanillaTweaker --gameDir "$CLIENT_DIR/data/" --version "b1.7.3" --userType "legacy" --server "127.0.0.1" --port 25565
fi
