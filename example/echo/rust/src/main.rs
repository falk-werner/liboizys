// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

use clap::{Parser};
use std::os::unix::net::UnixStream;
use std::io::prelude::*;

#[derive(Debug, Parser)]
#[clap(name = "echo-client", version)]
struct App {
    #[arg(short='e', long="endpoint", default_value_t=String::from("/tmp/com_example_echo.sock"))]
    endpoint: String,

    #[arg(default_value_t=String::from("Hello!"))]
    message: String
}

fn main() -> std::io::Result<()> {
    let args = App::parse();

    let mut stream = UnixStream::connect(args.endpoint)?;

    let data = args.message.as_bytes();
    let header: [u8;4] = [0, 
        ((data.len() >> 16) & 0xff).try_into().unwrap(),
        ((data.len() >>  8) & 0xff).try_into().unwrap(),
        (data.len() & 0xff).try_into().unwrap()
    ];
    stream.write_all(&header)?;
    stream.write_all(data)?;

    let mut header: [u8;4] = [0,0,0,0];
    stream.read_exact(&mut header)?;
    let size: usize =
        ((header[1] as usize) << 16) | 
        ((header[2] as usize) <<  8) | 
         (header[3] as usize);

    let mut data: Vec::<u8> = vec!(0; size);
    stream.read_exact(&mut data)?;

    let message = String::from_utf8(data).unwrap();
    println!("{}: {}", size, message);

    Ok(())
}
