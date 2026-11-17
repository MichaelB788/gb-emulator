# VRAM Tile Data

The VRAM tile data section is at the beginning of VRAM and is 6144 bytes long,
ranging from [0x8000, 0x97FF]

Given that each tile consists of 16 bytes, this is enough to hold 384 tile
entries.

The two middle hexadecimal numbers of an address in this region give you the
tile ID. These ID's populate the VRAM Tile Map sections.

## Useful Bitmasking Examples

```
0b1000000000000000 0x8000
0b1000011100000000 0x8700
0b1000100000000000 0x8800
0b1000111100000000 0x8F00
0b1001000000000000 0x9000
0b1001011100000000 0x9700
0b0001100000000000 0x1800 // block mask
0b0000111111110000 0x0FF0 // tile id

0b1001011111111111 0x97FF
0b000101111111     383
0b0001111111110000 0x1FF0
```

You could mask the address with `0x1FF0` and index it to VRAM directly to get
the beginning of any tile data entry.

## Rendering Tile Data

When rendering tile data, the low nibble of the tile id can be used to represent
the x coodinate, while the high nibble represents the y coordinate.

Tile entries in the block 2 would need special handling however, since their
tile ID's overlap with the tile ID's of block 0.

Since each tile is 8 pixels long/wide, each point would need to be multiplied by 8.
