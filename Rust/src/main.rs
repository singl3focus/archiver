extern crate clap;

use clap::{Arg, Command};
use std::io;
use std::path::PathBuf;
use std::path::Path;
use std::fs::File;
use zip_archive::Archiver;

type Result<T> = std::result::Result<T, Box<dyn std::error::Error>>;

const APP_NAME: &str = "antim";
const APP_VER: &str = "0.3.0";

fn main() {
    let mode_arg: &str = "mode";
    let src_arg: &str = "src";
    let dst_arg: &str = "dst";
    let format_arg: &str = "format";

    let app = Command::new(APP_NAME)
        .version(APP_VER)
        .author("Tursunov Imran <tursunov.imran@mail.ru>")
        .about("Simple archive app")
        .arg(
            Arg::new("mode")
                .short('m')
                .long("mode")
                .required(true)
                .value_parser(["compress", "decompress"])
                .help("Mode of operation: compress or decompress"),
        )
        .arg(
            Arg::new(src_arg)
                .short('s')
                .long("source")
                .required(true)
                .help("Source path for compress"),
        )
        .arg(
            Arg::new(dst_arg)
                .short('d')
                .long("destination")
                .required(true)
                .help("Destination path of compressed file"),
        )
        .arg(
            Arg::new(format_arg)
                .short('f')
                .long("format")
                .required(false)
                .value_parser(["zip"])
                .help("Data format. Needed only for compressing data.\nIn case there is a decompression, data format will be define automatically"),
        )
        .get_matches();

    let mode: &String = app.get_one::<String>(mode_arg).unwrap();
    let src: &String = app.get_one::<String>(src_arg).unwrap();
    let dst: &String = app.get_one::<String>(dst_arg).unwrap();
    
    let default: &String = &String::from("");
    let dataformat: &String = app.get_one::<String>(format_arg).unwrap_or(default);
    
    if src.is_empty() {
        eprintln!("Error: Source path cannot be empty.");
        std::process::exit(1);
    }
    if dst.is_empty() {
        eprintln!("Error: Destination path cannot be empty.");
        std::process::exit(1);
    }

    
    let mode: String = mode.to_lowercase();
    match mode.as_str() {
        "compress" => {
            match compression_distribution(src, dst, dataformat) {
                Ok(_) => println!("Compress data has been successful."),
                Err(e) => eprintln!("Compress data has been unsuccessful: {e}."),
            } 
        }
        "decompress" => {
            match decompression_distribution(src, dst) {
                Ok(_) => println!("Decompress data has been successful."),
                Err(e) => eprintln!("Decompress data has been unsuccessful: {e}."),
            }
        }
        _ => {
            eprintln!("Error: Unsupported mode '{}'.", mode);
            std::process::exit(1);
        }
    }
    
}

// distribution_by_compression_format
fn compression_distribution(src_path: &str, dst_path: &str, dataformat: &String) -> Result<()> {
    if dataformat.is_empty() {
        return Err(Box::new(std::io::Error::new(std::io::ErrorKind::Other, "Data format is empty")));
    }
    
    match dataformat.to_lowercase().as_str() {
        "zip" => compress_to_zip(src_path, dst_path),
        _ => Err(Box::new(std::io::Error::new(std::io::ErrorKind::Other, "Undefined dataformat")))
    }
}

fn compress_to_zip(src_path: &str, dst_path: &str) -> Result<()> {
    let src: PathBuf = PathBuf::from(src_path);
    let dest: PathBuf = PathBuf::from(dst_path);

    // let thread_count: i32 = 4;
    
    let mut archiver: Archiver = Archiver::new();
    archiver.push(src);        // Add dir to queue
    archiver.set_destination(dest);  // Add dst path
    // archiver.set_thread_count(thread_count);

    archiver.archive()
}

fn get_file_extension(filename: &str) -> Option<&str> {
    Path::new(filename).extension()
        .and_then(|ext| ext.to_str()) // Преобразуем в строку
        // .map(|ext| ext.to_string()) // Возвращаем расширение как String
}

// distribution_by_decompression_format
fn decompression_distribution(src_path: &str, dst_path: &str) -> Result<()> {
    match get_file_extension(src_path) {
        Some(ext) => {
            match ext {
                "zip" => decompress_from_zip(src_path, dst_path),
                _ => Err(Box::new(std::io::Error::new(std::io::ErrorKind::Other, "Undefined dataformat")))
            }
        },
        None => Err(Box::new(std::io::Error::new(std::io::ErrorKind::Other, "Extension not found")))
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
            None => continue
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
