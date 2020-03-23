# SDL2 VGA Terminal 

It is an emulation of the VGA terminal using SDL2.

the VGA fonts are related to a `vgabios` project (http://savannah.nongnu.org/projects/vgabios/).

At the moment support only mode 3.

It should be on 16 colors in the classic way, but can support more thanks to SDL2.

It is just a matter of fonts and a terminal grid for displaying texts.


## Compiling

Use `vcpkg` and install `SDL2`.

## Usage

SDL2 Video sub-system has to be initialized before using `VgaTerminal` class.

## TODO 

- decouple vga modes
- doxygen / document the code
- use streams ?
- read

- try to send OS command and display back.
