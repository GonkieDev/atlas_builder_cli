# Sprite Atlas CLI Utility

This is something I did quickly on an afternoon so it's still very WIP.
Converts all images in a given path to an atlas.

## Platforms
- Works on Windows only for now

## TODO:
- Handle case where more than one X by Y atlas is needed
- Linux/Mac support
- Use own `string.h`
- Check if output path exists before everything
- Check if output file already exists and give warning/change name
- pass image extensions as a parameter
- add arguments to build.bat (RELEASE or DEBUG)
- better atlas algorithm

#### Depencies/Libs for building from source
- `stb_image.h` and `stb_image_write.h` are used and come with the repo
- stdlib
- `windows.h` on windows

#### Credits
- `github.com/nothings` for `stb_image`
- Ryan Fleury for atlas algorithm