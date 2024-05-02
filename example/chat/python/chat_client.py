#!/usr/bin/env python3
# SPDX-License-Identifier: MPL-2.0
# SPDX-FileCopyrightText: Copyright 2024 Falk Werner

import messages_pb2

import asyncio
import argparse
from aioconsole import get_standard_streams

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

async def read_commandline(user: str, writer):
    reader, _ = await get_standard_streams()

    while True:
        line = await reader.readline()
        line = line.decode().strip()

        message = messages_pb2.chat_message()
        message.user = user
        message.content = line
        message = message.SerializeToString()

        size = len(message)

        writer.write(create_header(size))
        writer.write(message)
        await writer.drain()

async def main(endpoint: str, user: str):
    reader, writer = await asyncio.open_unix_connection(endpoint)
    asyncio.create_task(read_commandline(user, writer))
    
    while True:
        header = await reader.readexactly(4)
        size = parse_header(header)
        data = await reader.readexactly(size)

        message = messages_pb2.chat_message()
        message.ParseFromString(data)

        print(f"{message.user}: {message.content}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser("echo_client", "Example chat client")
    parser.add_argument("--endpoint", "-e", type=str, required=False, default="/tmp/com_example_chat.sock")
    parser.add_argument("--user", "-u", type=str, required=False, default="Jack")
    args = parser.parse_args()
    try:
        asyncio.run(main(args.endpoint, args.user))
    except KeyboardInterrupt as err:
        pass
