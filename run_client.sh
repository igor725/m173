#!/bin/bash

if [ "$1" == "untouched" ]; then
  java -Djava.library.path="./client/natives/" -cp "./client/jars/*" net.minecraft.client.Minecraft "Test$RANDOM" "-"
else
  java -Djava.library.path="./client/natives/" -cp "./client/jars/*" net.minecraft.launchwrapper.Launch "Test%random%" "-" --tweakClass net.minecraft.launchwrapper.VanillaTweaker --gameDir "./client/data/" --version "b1.7.3" --userType "legacy" --server "127.0.0.1" --port 25565
end
