#pragma once

const uint32_t Polynomial = 0xEDB88320;

/// compute CRC32 (bitwise algorithm)
inline uint32_t crc32_bitwise(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint8_t* current = (const uint8_t*) data;

  while (length-- != 0)
  {
    crc ^= *current++;

    for (int j = 0; j < 8; j++)
    {
      // branch-free
      crc = (crc >> 1) ^ (-int32_t(crc & 1) & Polynomial);

      // branching, much slower:
      //if (crc & 1)
      //  crc = (crc >> 1) ^ Polynomial;
      //else
      //  crc =  crc >> 1;
    }
  }

  return ~crc; // same as crc ^ 0xFFFFFFFF
}

