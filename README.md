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
| AFS | :x: | :x: | Archive | |
| IDX | :x: | :x: | AFS Archive Reference Names | |
| PKF | :heavy_check_mark: | :heavy_check_mark: | Archive (mainly textures for PKS) | |
| PKS (IPAC) | :heavy_check_mark: | :heavy_check_mark: | Archive | |
| SPR | :x: | :x: | Sprite texture Container | |
| GZ | :heavy_check_mark: | :heavy_check_mark: | GZip | |
| TAD/TAC | :heavy_check_mark: | :heavy_check_mark: | d3t TAD/TAC container with hash mapping | Filename coverage based on [wulinshu hash database](https://wulinshu.raymonf.me/#/) |

### Textures/Images

| Name| Read | Write | Description | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| PVRT | :x: | :x: | PowerVR Texture | |
| DDS | :x: | :x: | DirectDraw_Surface |  |
| JPEG | :x: | :x: | JPEG format |  |
| BMP | :x: | :x: | Bitmap format |  |
| PNG | :x: | :x: | PNG format |  |

#### PVR color formats

| Value | Name | Read | Write | Description |
| -------------| ------------- | ------------- | ------------- | ------------- | 
| 0x00 | ARGB1555 | :x: | :x: | Format consisting of one bit of alpha value and five bits of RGB values. |
| 0x01 | RGB565 | :x: | :x: | Format without alpha value and consisting of five bits of RB values and six bits of G value. |
| 0x02 | ARGB4444 | :x: | :x: | Format consisting of four bits of alpha value and four bits of RGB values. |
| 0x03 | YUV422 | :x: | :x: | YUV422 format |
| 0x04 | BUMP | :x: | :x: | Bump map with positiv only normal vectors (S and R direction angles) |
| 0x05 | RGB555 | :x: | :x: | for PCX compatible only |
| 0x06 | ARGB8888 | :x: | :x: | Format consisting of 1 byte of alpha value and 1 byte of RGB values. (Palettize only!) |
| 0x80 | DDS_RGB24 | :x: | :x: | RGB24 format (DXT1) |
| 0x81 | DDS_RGBA32 | :x: | :x: | RGBA32 format (DXT3) |

#### PVR data formats

| Value | Name | Read | Write | Notes |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| 0x01 | SQUARE_TWIDDLED | :x: | :x: |  |
| 0x02 | SQUARE_TWIDDLED_MIPMAP | :x: | :x: |  |
| 0x03 | VECTOR_QUANTIZATION | :x: | :x: |  |
| 0x04 | VECTOR_QUANTIZATION_MIPMAP | :x: | :x: |  |
| 0x05 | PALETTIZE_4BIT | :x: | :x: |  |
| 0x06 | PALETTIZE_4BIT_MIPMAP | :x: | :x: |  |
| 0x07 | PALETTIZE_8BIT | :x: | :x: |  |
| 0x08 | PALETTIZE_8BIT_MIPMAP | :x: | :x: |  |
| 0x09 | RECTANGLE | :x: | :x: |  |
| 0x0A | RECTANGLE_MIPMAP | :x: | :x: | Reserved: Can't use. |
| 0x0B | RECTANGLE_STRIDE | :x: | :x: |  |
| 0x0C | RECTANGLE_STRIDE_MIPMAP | :x: | :x: | Reserved: Can't use. |
| 0x0D | RECTANGLE_TWIDDLED | :x: | :x: | Should not be supported.  |
| 0x0E | BMP | :x: | :x: | No information. |
| 0x0F | BMP_MIPMAP | :x: | :x: | No information. |
| 0x10 | VECTOR_QUANTIZATION_SMALL | :x: | :x: |  |
| 0x11 | VECTOR_QUANTIZATION_SMALL_MIPMAP | :x: | :x: |  |
| 0x80 | DDS | :x: | :x: | DDS format |
| 0x87 | DDS | :x: | :x: | DDS format |

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
- Just me :)

Other:
- [Shenmue-Mods](https://github.com/Shenmue-Mods/Shenmue-Mods) - Modding knowledge database
- [Raymonf](https://wulinshu.raymonf.me/#/) - Wulinshu TAD hash database
