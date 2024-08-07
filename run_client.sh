#!/bin/bash
java -Djava.library.path=./client/natives/ -cp "./client/jars/*" net.minecraft.client.Minecraft "Test$RANDOM" -
