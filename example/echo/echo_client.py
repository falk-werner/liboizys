#!/usr/bin/env python3
# SPDX-License-Identifier: MPL-2.0
# SPDX-FileCopyrightText: Copyright 2024 Falk Werner

import asyncio
import argparse

def create_header(size):
    if (size <= 0) or (size > 0xffffff):
        raise ValueError("invlaid message size")
    header = [0, 0, 0, 0]

    header[1] = (size >> 16) & 0xff
    header[2] = (size >>  8) & 0xff
    header[3] =  size        & 0xff

    return bytearray(header)

def parse_header(header):
    if (header[0] != 0):
        raise ValueError("header must start with a zero byte")
    size = (header[1] << 16) | (header[2] << 8) | (header[3])
    if (size <= 0) or (size > 0xffffff):
        raise ValueError("invlaid message size")
    return size

async def main(endpoint: str, message: str):
    reader, writer = await asyncio.open_unix_connection(endpoint)
    
    data = message.encode()
    size = len(message)

    writer.write(create_header(size))
    writer.write(data)
    await writer.drain()

    header = await reader.readexactly(4)
    size = parse_header(header)
    data = await reader.readexactly(size)

    print(data.decode())

    writer.close()
    await writer.wait_closed()


if __name__ == "__main__":
    parser = argparse.ArgumentParser("echo_client", "Example echo client")
    parser.add_argument("--endpoint", "-e", type=str, required=False, default="/tmp/com_example_echo.sock")
    parser.add_argument("--message", "-m", type=str, required=False, default="Hi, there!")
    args = parser.parse_args()
    asyncio.run(main(args.endpoint, args.message))
