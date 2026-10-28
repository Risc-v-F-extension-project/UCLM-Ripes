# Floating-point data directives

`.float` emits four bytes per value and `.double` emits eight bytes, in
little-endian order. Decimal literals (including scientific notation) are
converted numerically to the corresponding floating-point representation.

Literals prefixed with `0x` or `0b` instead specify the exact IEEE 754 bit
pattern. These patterns are unsigned and must fit in 32 bits for `.float` or
64 bits for `.double`. Prefixes and hexadecimal digits are case-insensitive.
Invalid digits, empty patterns, and overflowing patterns are rejected.
Raw patterns can represent any bit sequence, including infinities and NaNs.

```asm
.float 5.0, 0x40A00000, 0b01000000101000000000000000000000
.double 5.0, 0x4014000000000000
```

Each value in the first line represents float 5.0, and each value in the second
line represents double 5.0. In contrast, `.float 0x5` stores the raw pattern
`0x00000005`, rather than converting the integer 5 to floating point.
