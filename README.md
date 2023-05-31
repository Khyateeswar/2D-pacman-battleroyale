# projectpacman

-- By MUKKU VAMSI KRISHNA REDDY 2019CS10374 & NALLA KHYATEESWAR NAIDU 2019CS10376---------

-------------------------------------------------------

## Files in this directory
    resources
    readme
    pacman 


-------------------------------------------------------

##  Usage

        0.open terminal in the "resources" directory.
        1.use "make" command for compiling the code.
        2.use "make run " command for executing the game.
        3.now the game starts and then instructions are shown for 10 secs.
        4.To quit the game at any instant press the 'x' button on the right top of the window.
        5.Enjoy the game (:(:
        6.use "make clean" command to remove the executables.

--------------------------------------------------------

##  Description
        1.We need first to open a SDL window.
        2.Then try to use the surface of the window to display an image
        3.Then we add quit button to the window to uit in any instant
        4.Then we start defining objects to display them.
        5.Then we start animating them using spritesheets.
        6.Then we add key inputs to add motion and properly animate them by their orientation.
        7.Then we add sound to the inputs.
        8.We add cherries to the maze which act as fuel for their motion and ammunition and display the count eaten by score.
        9.Then we define bullets and missiles and similar to the player and give them motion.
        10. We use BFS for the missile to follow the player in every instant.
        11. We give  life to them by adding health attributes to players,bullet,cherries and missiles for their existence in the game.
        12.We then create a finish line if all the cherries are eaten and who reaches it first wins.
        13.We then add game poster and instructions to the beginning of the game.
        
        

------------------------------------------------------------

## Installation 

        install SDL library for ubuntu C++ from
            https://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php
            https://zoomadmin.com/HowToInstall/UbuntuPackage/libsdl2-image-dev

------------------------------------------------------------

## References
        https://lazyfoo.net/tutorials
        https://www.cse.iitd.ac.in/~rijurekha/cop290_2021.html
        https://gist.github.com/BoredBored/3187339a99f7786c25075d4d9c80fad5
        http://cppreference.com/
