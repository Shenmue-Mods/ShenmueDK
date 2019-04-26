# Shenmue Developer Kit for C++
Shenmue I & II HD Modding SDK for C++

This library is a port of the [ShenmueDKSharp](https://github.com/philyeahz/ShenmueDKSharp).

How to contribute:
- Feel free to create pull requests if you want to contribute some fixes or features.
- If you found an bug just create an issue on GitHub.
- For feature requests just create an issue on GitHub.

## Usage
See the [github wiki](https://github.com/philyeahz/ShenmueDK/wiki).

## File Formats
For more informations about the file formats see the [wulinshu wiki](https://wulinshu.com/wiki/index.php/Main_Page).

### Containers

| Name| Read | Write | Description | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| AFS | :heavy_check_mark: | :heavy_check_mark: | Archive | |
| IDX | :heavy_check_mark: | :x: | AFS Archive Reference Names | |
| PKF | :heavy_check_mark: | :heavy_check_mark: | Archive (mainly textures for PKS) | |
| PKS (IPAC) | :heavy_check_mark: | :heavy_check_mark: | Archive | |
| SPR | :heavy_check_mark: | :heavy_check_mark: | Sprite texture Container | |
| GZ | :heavy_check_mark: | :heavy_check_mark: | GZip | |
| TAD/TAC | :heavy_check_mark: | :heavy_check_mark: | d3t TAD/TAC container with hash mapping | Filename coverage based on [wulinshu hash database](https://wulinshu.raymonf.me/#/) |

### Textures/Images

| Name| Read | Write | Description | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| PVRT | :heavy_check_mark: | :x: | PowerVR Texture | |
| DDS | :x: | :x: | DirectDraw_Surface |  |
| JPEG | :x: | :x: | JPEG format |  |
| BMP | :x: | :x: | Bitmap format |  |
| PNG | :x: | :x: | PNG format |  |

#### PVR color formats

| Value | Name | Read | Write | Description |
| -------------| ------------- | ------------- | ------------- | ------------- | 
| 0x00 | ARGB1555 | :heavy_check_mark: | :heavy_check_mark: | Format consisting of one bit of alpha value and five bits of RGB values. |
| 0x01 | RGB565 | :heavy_check_mark: | :heavy_check_mark: | Format without alpha value and consisting of five bits of RB values and six bits of G value. |
| 0x02 | ARGB4444 | :heavy_check_mark: | :heavy_check_mark: | Format consisting of four bits of alpha value and four bits of RGB values. |
| 0x03 | YUV422 | :heavy_check_mark: | :heavy_check_mark: | YUV422 format |
| 0x04 | BUMP | :heavy_check_mark: | :heavy_check_mark: | Bump map with positiv only normal vectors (S and R direction angles) |
| 0x05 | RGB555 | :heavy_check_mark: | :heavy_check_mark: | for PCX compatible only |
| 0x06 | ARGB8888 | :heavy_check_mark: | :heavy_check_mark: | Format consisting of 1 byte of alpha value and 1 byte of RGB values. (Palettize only!) |
| 0x80 | DDS_RGB24 | :heavy_check_mark: | :heavy_check_mark: | RGB24 format (DXT1) |
| 0x81 | DDS_RGBA32 | :heavy_check_mark: | :heavy_check_mark: | RGBA32 format (DXT3) |

#### PVR data formats

| Value | Name | Read | Write | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| 0x01 | SQUARE_TWIDDLED | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x02 | SQUARE_TWIDDLED_MIPMAP | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x03 | VECTOR_QUANTIZATION | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x04 | VECTOR_QUANTIZATION_MIPMAP | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x05 | PALETTIZE_4BIT | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x06 | PALETTIZE_4BIT_MIPMAP | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x07 | PALETTIZE_8BIT | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x08 | PALETTIZE_8BIT_MIPMAP | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x09 | RECTANGLE | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x0A | RECTANGLE_MIPMAP | :x: | :x: | Reserved: Can't use. |
| 0x0B | RECTANGLE_STRIDE | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x0C | RECTANGLE_STRIDE_MIPMAP | :x: | :x: | Reserved: Can't use. |
| 0x0D | RECTANGLE_TWIDDLED | :heavy_check_mark: | :heavy_check_mark: | Should not be supported.  |
| 0x0E | BMP | :x: | :x: | No information. |
| 0x0F | BMP_MIPMAP | :x: | :x: | No information. |
| 0x10 | VECTOR_QUANTIZATION_SMALL | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x11 | VECTOR_QUANTIZATION_SMALL_MIPMAP | :heavy_check_mark: | :heavy_check_mark: |  |
| 0x80 | DDS | :heavy_check_mark: | :heavy_check_mark: | DDS format |
| 0x87 | DDS | :heavy_check_mark: | :heavy_check_mark: | DDS format |

### Models/Animation

| Name| Read | Write | Description | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| MT5 | :x: | :x: | Model Container | |
| MT6 | :x: | :x: | Model Container | |
| MT7 | :x: | :x: | Model Container | |
| MOTN | :x: | :x: | Motion data (Animation sequences) | |
| OBJ | :x: | :x: | Wavefront OBJ | |

### Audio

| Name| Read | Write | Description | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| SND | :x: | :x: | Dreamcast sound file | |
| XWMA | :x: | :x: | Xbox WMA (XAudio2) file | |

### Subtitles/Text

| Name| Read | Write | Description | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| SRF | :x: | :x: | Cinematic subtitles file | |
| FONTDEF | :x: | :x: | Font definition file | |
| SUB | :x: | :x: | Subtitles file | |
| GLYPHS | :x: | :x: | Font glyph file | |
| FON | :x: | :x: | Disk font file | |

### Mapinfo
| Name| Read | Write | Description | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| CHRD | :x: | :x: | | |
| CHRM | :x: | :x: | | |
| COLS | :x: | :x: | Collisions (sm1) | |
| DOOR | :x: | :x: | Door portals? | |
| ECAM | :x: | :x: | | |
| FLDD | :x: | :x: | Collisions (sm2)| |
| LGHT | :x: | :x: | Lighting data | |
| MAPR | :x: | :x: | | |
| MAPT | :x: | :x: | | |
| SCEX | :x: | :x: | Cutscenes and maybe other stuff| |
| SNDP | :x: | :x: | Sound program | |
| WTHR | :x: | :x: | Weather data | |

### Other

| Name| Read | Write | Description | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| ATH | :x: | :x: | Sequence Data | |
| SRL | :x: | :x: | Scroll Data | |
| IWD | :x: | :x: | LCD Table | |
| WDT | :x: | :x: | Weather Data | |
| UI | :x: | :x: | UI Json | |
| CHR | :x: | :x: | Character | |
| MVS | :x: | :x: | MVS data | |
| DYM | :x: | :x: | Dynamics Info | |
| CRM | :x: | :x: | Character Model | |
| CHT | :x: | :x: | Character Properties | |
| CSV | :x: | :x: | Comma-separated values | |
| EMU | :x: | :x: | Emulator file | |

## Credits
Contributors:
- [LemonHaze](https://github.com/LemonHaze420)

Other:
- [Shenmue-Mods](https://github.com/Shenmue-Mods/Shenmue-Mods) - Modding knowledge database
- [Raymonf](https://wulinshu.raymonf.me/#/) - Wulinshu TAD hash database
