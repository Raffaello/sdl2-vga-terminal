# SDL2 VGA Terminal 

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/83e178fb7d9343b78be651f2797fe3f8)](https://app.codacy.com/manual/Raffaello/sdl2-vga-terminal?utm_source=github.com&utm_medium=referral&utm_content=Raffaello/sdl2-vga-terminal&utm_campaign=Badge_Grade_Settings)

| Windows (x86, x64) |
|---------|
|[![Build status](https://ci.appveyor.com/api/projects/status/67mildjynhnlekk5/branch/master?svg=true)](https://ci.appveyor.com/project/Raffaello/sdl2-vga-terminal/branch/master)|

It is just a VGA font terminal using SDL2.

the VGA fonts are related to [vgabios](http://savannah.nongnu.org/projects/vgabios/) project.

At the moment support only mode 3.

It should be on 16 colors in the classic way, but can support more thanks to SDL2.

It is just a matter of fonts and a terminal grid for displaying texts.

## Compiling

Use `vcpkg` and install `SDL2`.

## Usage

SDL2 Video sub-system has to be initialized before using `VgaTerminal` class.

for extended ASCII chars to use as literal (eg: char 205), it is required to save the source code with 
the codepage 437 encoding (OEM United States / Extended ASCII / DOS Terminal).

There are few examples usage as part of this cmake compilation too.

## Screenshot

This screenshot is also used in the snapshot test.

![alt text](./sdl2-vga-terminal/test/snapshot/VgaTerminal.Snapshot.png "Title")

## Testing

using `GTest` framework.

There is a technique, snapshot testing, to verify some expectation, due to some required raster image to compare with,
there is a `boolean` option `TEST_DUMP_SNAPSHOT`, that will run the test suit for generating the expected result.
It could be a little bit risky, but it is a way to have generated expected results for the test suite.

The test suite take advantages of the option and will be compiled accordingly based on "dumping" or "testing".

please note if you are dumping the snapshot, you have to copy back to the `test/snapshot` directory to make them usable.

The filename generated are based on the test that are running, ideally: `[Test-suite.Test-name].png`

Just as a convention.

## TODO 

Some must and ideas:

-   web assembly example,build experimentation?
-   add tests for the cmake configuration and install files, expect them to be "there".
-   clear with bgCol, and only viewport.
-   decouple vga modes
-   DAC Palette changing
-   doxygen / document the code
-   use streams 
-   read
-   unicode VGA font
-   some functions can be optimized using SDL_Surface functions/SDL functions
