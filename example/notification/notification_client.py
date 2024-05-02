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

async def main(endpoint: str):
    reader, _ = await asyncio.open_unix_connection(endpoint)
    
    while True:
        header = await reader.readexactly(4)
        size = parse_header(header)
        data = await reader.readexactly(size)

        print(data.decode())


if __name__ == "__main__":
    parser = argparse.ArgumentParser("echo_client", "Example notifiy client")
    parser.add_argument("--endpoint", "-e", type=str, required=False, default="/tmp/oizys_example_notify.sock")
    args = parser.parse_args()
    try:
        asyncio.run(main(args.endpoint))
    except KeyboardInterrupt as err:
        pass
