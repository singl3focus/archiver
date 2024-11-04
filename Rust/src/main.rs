extern crate clap;

use clap::Parser;
use std::fs::File;
use std::io;
use std::path::Path;
use std::path::PathBuf;
use zip_archive::Archiver;

type Result<T> = std::result::Result<T, Box<dyn std::error::Error>>;


#[derive(Parser)]
#[command(name = "antim")]
#[command(version = "0.4.0")]
#[command(about = "Simple archive app")]
#[command(long_about = None)]
#[command(author = "Tursunov Imran <tursunov.imran@mail.ru>")]
struct Cli {
    #[arg(short, long, value_parser = ["compress", "decompress"],
        value_name = "Mode of operation")]
    mode: String,

    #[arg(short, long, value_name = "Source path")]
    source: PathBuf,

    #[arg(short, long, value_name = "Destination path")]
    destination: PathBuf,

    #[arg(short, long, value_parser = ["zip"],
        value_name = "Data format. Needed only for compressing data.\n\t\tIn case there is a decompression, data format will be define automatically")]
    format: Option<String>,
   
    // #[command(subcommand)]
    // command: Option<Commands>,
}

// #[derive(Subcommand)]
// enum Commands {
//     Test {
//         #[arg(short, long)]
//         list: bool,
//     },
// }

fn main() {
    let cli: Cli = Cli::parse();

    if !cli.source.exists() {
        eprintln!("Error: Invalid source path.");
        std::process::exit(1);
    }
    if cli.destination.is_file() {
        eprintln!("Error: Destination path must be directory, not file.");
        std::process::exit(1);
    }

    let src: &str = cli.source.to_str().unwrap(); // [DANGER] CALL PANICS
    let dst: &str = cli.destination.to_str().unwrap(); // [DANGER] CALL PANICS
    
    let dataformat: Option<String>  = cli.format; 
    let df: String;
    match dataformat {
        Some(dataformat) => {
            df = dataformat;
        },
        None => {
            df = String::from("");
        },
    }


    let mode: String = cli.mode.to_lowercase();
    match mode.as_str() {
        "compress" => { 
            match compression_distribution(src, dst, &df) {
                Ok(_) => println!("Compress data has been successful."),
                Err(e) => eprintln!("Error: Compress data has been unsuccessful: {e}."),
            }
        }    
        "decompress" => { 
            match decompression_distribution(src, dst) {
                Ok(_) => println!("Decompress data has been successful."),
                Err(e) => eprintln!("Error: Decompress data has been unsuccessful: {e}."),
            }
        }
        _ => {
            eprintln!("Error: Unsupported mode '{}'.", mode);
            std::process::exit(1);
        }
    }
}

// distribution by compression format
fn compression_distribution(src_path: &str, dst_path: &str, dataformat: &String) -> Result<()> {
    if dataformat.is_empty() {
        return Err(Box::new(std::io::Error::new(
            std::io::ErrorKind::Other,
            "Data format is empty",
        )));
    }

    match dataformat.to_lowercase().as_str() {
        "zip" => compress_to_zip(src_path, dst_path),
        _ => Err(Box::new(std::io::Error::new(
            std::io::ErrorKind::Other,
            "Undefined dataformat",
        ))),
    }
}

fn compress_to_zip(src_path: &str, dst_path: &str) -> Result<()> {
    let src: PathBuf = PathBuf::from(src_path);
    let dest: PathBuf = PathBuf::from(dst_path);

    // let thread_count: i32 = 4;

    let mut archiver: Archiver = Archiver::new();
    archiver.push(src); // Add dir to queue
    archiver.set_destination(dest); // Add dst path
                                    // archiver.set_thread_count(thread_count);

    archiver.archive()
}

fn get_file_extension(filename: &str) -> Option<&str> {
    Path::new(filename).extension().and_then(|ext| ext.to_str()) // Преобразуем в строку
                                                                 // .map(|ext| ext.to_string()) // Возвращаем расширение как String
}

// distribution by decompression format
fn decompression_distribution(src_path: &str, dst_path: &str) -> Result<()> {
    match get_file_extension(src_path) {
        Some(ext) => match ext {
            "zip" => decompress_from_zip(src_path, dst_path),
            _ => Err(Box::new(std::io::Error::new( std::io::ErrorKind::Other,"Undefined dataformat"))),
        },
        None => Err(Box::new(std::io::Error::new( std::io::ErrorKind::Other,"Extension not found"))),
    }
}

fn decompress_from_zip(src_path: &str, dst_path: &str) -> Result<()> {
    _ = dst_path;

    let zip_file: &Path = Path::new(src_path);
    let zip_file: File = File::open(zip_file).unwrap();
    let mut archive: zip::ZipArchive<File> = zip::ZipArchive::new(zip_file).unwrap();

    for i in 0..archive.len() {
        let mut file: zip::read::ZipFile<'_> = archive.by_index(i).unwrap();

        let outpath = match file.enclosed_name() {
            Some(path) => path.to_owned(),
            None => continue,
        };

        if (*file.name()).ends_with("/") {
            std::fs::create_dir_all(&outpath).unwrap();
        } else {
            if let Some(p) = outpath.parent() {
                if !p.exists() {
                    std::fs::create_dir_all(&p).unwrap();
                }
            }
        }

        let mut outfile = File::create(&outpath).unwrap();
        io::copy(&mut file, &mut outfile).unwrap();
    }

    Ok(())
}
