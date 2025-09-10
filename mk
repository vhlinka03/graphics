#
#
g++ -c Map.cpp
g++ -c Spirit.cpp
g++ -c graphics.cpp
#
g++ graphics.o Spirit.o Map.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
#

