// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

use clap::{Parser};
use std::os::unix::net::UnixStream;
use std::io::prelude::*;

#[derive(Debug, Parser)]
#[clap(name = "notify-client", version)]
struct App {
    #[arg(short='e', long="endpoint", default_value_t=String::from("/tmp/oizys_example_notify.sock"))]
    endpoint: String,
}

fn main() -> std::io::Result<()> {
    let args = App::parse();
    let mut stream = UnixStream::connect(args.endpoint)?;

    loop {
        let mut header: [u8;4] = [0,0,0,0];
        stream.read_exact(&mut header)?;
        let size: usize =
            ((header[1] as usize) << 16) | 
            ((header[2] as usize) <<  8) | 
             (header[3] as usize);
    
        let mut data: Vec::<u8> = vec!(0; size);
        stream.read_exact(&mut data)?;
    
        let message = String::from_utf8(data).unwrap();
        println!("{}", message);    
    }
}
