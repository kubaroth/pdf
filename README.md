# Pdf Text Extractor

A library to extract text (std::string) from PDF streams.

    #include "text.h"
    int page_number = 123;
    auto data = parse_page("document.pdf", page_number);
    cout << data->text <<endl;

## Building

    mkdir __build && cd __build
    cmake .. -DHUMMUS_PATH=$HOME/toolchains/hummus
    make

The main dependency - PDF-Writer is required to be located in $HOME/toolchains/hummus
to install it:

    git clone https://github.com/galkahana/PDF-Writer.git
    mkdir __build && cd __build
    cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/toolchains/hummus -DCMAKE_BUILD_TYPE=Debug
    make install


## Testing

    ./ctest

The test1 which covers basic cases of extracting text from Pdf Object types such as:
- HexString
- LiteralString
- Array of Literal Strings
- Array of HextStrings
- Common ligatures (fi, ff, fl) - right now are converted to ascii sequences.
- Line positioning is taken into account to combine individual characters into complete words.
- The character lookup table is built from two source:
  - bfchar object (HexStrings) (see ResourcesVisitor for details)
  - Difference object (LiteralStrings)

## Detailed traversal output:

in corresponding visitor bum value of the LOG

    #define LOG 2

### Sanitizers

```
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
```

