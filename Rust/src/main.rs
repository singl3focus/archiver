extern crate clap;

use clap::{Arg, Command};
use std::path::PathBuf;
use zip_archive::Archiver;

const APP_NAME: &str = "antim";
const APP_VER: &str = "0.2.0";


fn main() {
    let mode_arg: &str = "mode";
    let src_arg: &str = "src";
    let dst_arg: &str = "dst";

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
        .get_matches();

    let mode = app.get_one::<String>(mode_arg).unwrap();
    let src = app.get_one::<String>(src_arg).unwrap();
    let dst = app.get_one::<String>(dst_arg).unwrap();

    if src.is_empty() {
        eprintln!("Error: Source path cannot be empty.");
        std::process::exit(1);
    }
    if dst.is_empty() {
        eprintln!("Error: Source path cannot be empty.");
        std::process::exit(1);
    }
    
    match mode.to_lowercase().as_str() {
        "compress" => compress_to_zip(src, dst),
        "decompress" => decompress_from_zip(src, dst),
        _ => {
            eprintln!("Error: Unsupported mode '{}'. Use 'compress' or 'decompress'.", mode);
            std::process::exit(1);
        }
    }
}

fn compress_to_zip(src_path: &str, dst_path: &str) {
    let src: PathBuf = PathBuf::from(src_path);
    let dest: PathBuf = PathBuf::from(dst_path);

    // let thread_count: i32 = 4;

    let mut archiver = Archiver::new();
    archiver.push(src); // Add dir to queue
    archiver.set_destination(dest); // Add dst path
                                    // archiver.set_thread_count(thread_count);

    match archiver.archive() {
        Ok(_) => println!("Archive the src has been successfull! {}", src_path),
        Err(e) => println!("Cannot archive the directory! {}", e),
    };
}

fn decompress_from_zip(src_path: &str, dst_path: &str) {
    // Эта функция должна быть реализована для разжатия zip-архива
    // println!("Decompressing from {} to {}", src_path, dst_path);
    eprintln!("Error: Not implemented 'decompress_from_zip', args: [{}] [{}]", src_path, dst_path);
}