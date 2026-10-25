#!/usr/bin/env python3
"""Convert HoodOS pixel font JSON into compact C++ row-bit lookup tables."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent
OUTPUT = ROOT.parent / 'firmware' / 'HoodOS' / 'assets' / 'HoodFontData.h'
sources = [('TINY', '3x3.json'), ('SMALL', '3x5.json'), ('LARGE', '5x7.json')]
lines = ['// GENERATED FILE - run python3 tools/generate_fonts.py', '#pragma once', '#include <Arduino.h>', '', 'namespace HoodFontData {', 'struct Glyph { uint8_t width; uint8_t height; uint8_t rows[9]; };', '']
for symbol, source in sources:
    data = json.loads((ROOT / source).read_text())
    chars = data['characters']
    fallback = data['fallback']
    lines.append(f'static constexpr uint8_t {symbol}_SPACING = {data["characterSpacing"]};')
    lines.append(f'static constexpr uint8_t {symbol}_HEIGHT = {max(len(c) for c in chars.values())};')
    lines.append(f'static const Glyph {symbol}[95] = {{')
    for code in range(32, 127):
        ch = chr(code)
        glyph = chars.get(ch, fallback)
        height = len(glyph)
        width = max(map(len, glyph))
        if width > 8 or height > 9:
            raise ValueError(f'Glyph {symbol} {ch!r} too large: {width}x{height}')
        rows = []
        for row in glyph:
            b = 0
            for x, pix in enumerate(row):
                if pix == 'x':
                    b |= (1 << (7-x))
            rows.append(b)
        rows += [0] * (9-len(rows))
        lines.append(f'  {{ {width}, {height}, {{ {", ".join("0x%02X"%v for v in rows)} }} }}, // ASCII {code}')
    lines.append('};\n')
lines.append('} // namespace HoodFontData\n')
OUTPUT.write_text('\n'.join(lines))
print(OUTPUT, OUTPUT.stat().st_size)
