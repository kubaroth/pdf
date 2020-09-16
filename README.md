# Pdf Text Extractor

A Tool to extract text strings from PDF streams.

## Building

```
mkdir __build && cd __build
cmake .. -DHUMMUS_PATH=$HOME/toolchains/hummus
make
```

The main dependency - PDF-Writer is required to be located in $HOME/toolchains/hummus
to install it:

```
git clone https://github.com/galkahana/PDF-Writer.git
mkdir __build && cd __build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/toolchains/hummus -DCMAKE_BUILD_TYPE=Debug
make install
```

## Testing

```
./test1
```

The test1 which covers basic cases of extracting text from Pdf Object types.
- HexString
- LiteralString
- Array of Literal Strings
- Array of HextStrings
- Common ligatures (fi, ff, fl) are converted to ascii sequence.
- Line positioning is taken into account to combine individual characters into complete words.
- The character lookup table are build from two source:
-- bfchar object (HexStrings)
-- Differences object (LiteralStrings)

## Debugging

in text.h set:

```
#define LOG 2
```

update main.cpp to test specific pdf file

### Sanitizers

```
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
```

