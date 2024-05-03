use clap::{Parser};

use tokio::io::{self, BufReader, AsyncBufReadExt, AsyncReadExt, AsyncWriteExt};
use tokio::signal::unix::{signal, SignalKind};
use tokio::net::UnixStream;

#[derive(Debug, Parser)]
#[clap(name = "chat-client", version)]
struct App {
    #[arg(short='e', long="endpoint", default_value_t=String::from("/tmp/com_example_chat.sock"))]
    endpoint: String,

    #[arg(short='u', long="user", default_value_t=String::from("James"))]
    user: String
}

async fn next_message(stream: &mut UnixStream) -> io::Result<String> {
    let mut header: [u8;4] = [0,0,0,0];
    stream.read_exact(&mut header).await?;

    let size: usize =
        ((header[1] as usize) << 16) | 
        ((header[2] as usize) <<  8) | 
         (header[3] as usize);

    let mut data: Vec::<u8> = vec!(0; size);
    stream.read_exact(&mut data).await?;

    let message = String::from_utf8(data).unwrap();
    Ok(message)
}

#[tokio::main(flavor = "current_thread")]
async fn main() -> io::Result<()> {
    let args = App::parse();

    let mut shutdown_requested = signal(SignalKind::interrupt())?;

    let mut stream = UnixStream::connect(args.endpoint).await?;

    let stdin = io::stdin();
    let reader = BufReader::new(stdin);
    let mut lines = reader.lines();

    loop {
        tokio::select! {
            Ok(Some(line)) = lines.next_line() => {
                let data = line.as_bytes();
                let header: [u8;4] = [0, 
                    ((data.len() >> 16) & 0xff).try_into().unwrap(),
                    ((data.len() >>  8) & 0xff).try_into().unwrap(),
                    (data.len() & 0xff).try_into().unwrap()
                ];
                stream.write_all(&header).await?;
                stream.write_all(data).await?;            
            },
            Ok(message) = next_message(&mut stream) => {
                println!("{}", message);
            }
            _ = shutdown_requested.recv() => {
                break;
            }
        }
    }

    // Stdin blocks for technical reasons
    // see https://tikv.github.io/doc/tokio/io/struct.Stdin.html
    println!("Press Enter to exit...");
    Ok(())
}
