# `gms-v40-beta-client-mods`

A tool for adding quality-of-life features to the Global MapleStory v40 Beta
client.

## Features

This tool automatically injects itself into the game on launch, providing the
following capabilities without needing to modify any game files:

### UI

- Run multiple clients simultaneously
- Windowed mode
- Arbitrary window resolutions
- Override in-game text
- Skip opening screen logos

### Networking

- Connect to custom login servers
- Disable inbound and outbound cryptography
- Log all inbound and outbound traffic

### Gameplay

- No maximum damage limit
- No chat restrictions (word or spam filters)
- Cash items can be dropped
- Game Masters can create parties

Note that these gameplay features require server support, as they are likely
to be blocked by any defensively-programmed server.

## Compatibility

This tool uses absolute addresses and is only compatible with the following,
unedited game binary:

```
966 656 bytes
File version 1.0.0.1
Last modified Friday, March 25, 2005, 1:17:16 PM
```

|Algo|Digest|
|--|--|
|MD5|6a25b8b89ccd75684b459488c8793baf
|SHA-1|882375e56d0b79ba3966bac71a59670282dc6bbd|
|SHA-256|9a7f62a09d160cc8da559424c857538f4ce13fe7ea1da9c0a1f35e46e982c85f|
|SHA-384|0acad00306075c76ee59d5410aa586cc58c914cd53f2c9da0acd2166f28e6a1416f95f4cb26cb0db6057efff009df6af|
|SHA-512|de18681b9ea29598a27dd6f27901effb8cdbbb925de5f6275a88dd3455d4bfd4a4eb446c96e344a6a61d0cd1009df2601730e4a8430cc25785887245e3a7fcc9|

## Usage

1. Customize the code base to your liking:
    - Update the server IP in `hooking/bootstrap.cpp`
    - Comment out any features in `main.cpp`
2. Install `fmtlib`, e.g. via `vcpkg`
3. Run CMake and compile the binary
4. Copy the output `dinput.dll` into the game folder
5. Launch the game

## License

```
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
```