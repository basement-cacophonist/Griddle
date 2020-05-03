/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   GriddleAppIcon32px_png;
    const int            GriddleAppIcon32px_pngSize = 4717;

    extern const char*   GriddleAppIcon256px_png;
    const int            GriddleAppIcon256px_pngSize = 19848;

    extern const char*   ArrowDown_png;
    const int            ArrowDown_pngSize = 6117;

    extern const char*   ArrowUp_png;
    const int            ArrowUp_pngSize = 6077;

    extern const char*   ArrowLeft_png;
    const int            ArrowLeft_pngSize = 6168;

    extern const char*   ArrowRight_png;
    const int            ArrowRight_pngSize = 6198;

    extern const char*   TempoDial_png;
    const int            TempoDial_pngSize = 4668;

    extern const char*   PlayButton_png;
    const int            PlayButton_pngSize = 6084;

    extern const char*   StopButton_png;
    const int            StopButton_pngSize = 5767;

    extern const char*   RoundCheckboxChecked_png;
    const int            RoundCheckboxChecked_pngSize = 3990;

    extern const char*   RoundCheckboxUnchecked_png;
    const int            RoundCheckboxUnchecked_pngSize = 3703;

    extern const char*   RestButton_png;
    const int            RestButton_pngSize = 5088;

    extern const char*   Title_png;
    const int            Title_pngSize = 8267;

    extern const char*   Subtitle_png;
    const int            Subtitle_pngSize = 4317;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 14;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
