
# Cgame space shooter

## General Info

This is a simple game in which two players are flying small space ship and shooting lasers into each other to score as many points as possible.

## Controll

One of the players is controling flight direction of his ship with w-s-a-d keys and shooting using "K", another players connects with tcp sockets and sends remote commands - and example of that is client.py script which simulates simple AI. To start game pres Y or T depending which ship you want to fly.

## Under Development

Please mind that this is an on-going project being just one of a few side project of an otherwise very busy single developer.
Tcp client and server mechanism is under development but you can connect to test server using server_test.py script.

## Configuration

For sake of generality of a current solution the assets have to be placed in asset directory 
(it's more configurable for a user, if you dont like some icons you can change it). 
The path to this directory is to be written  to "config.txt" which needs to be placed near the execuable.

## Dependencies

Great thanks to authors of SOIL, GLM, GLFW and GLAD !

## Project
The code is organized into libraries, make file is genered with use of cmake, boost libraries: system and thread have to be avaliable (cmake flags avaliable)
