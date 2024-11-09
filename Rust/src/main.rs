extern crate clap;

use clap::Parser;
use clap::ValueEnum;

use flate2::read::GzDecoder;
use flate2::write::GzEncoder;
use flate2::Compression;
use tar::Archive;

use zip_archive::Archiver;

use std::fs::File;
use std::io;
use std::path::Path;
use std::path::PathBuf;

type Result<T> = std::result::Result<T, Box<dyn std::error::Error>>;

#[derive(Parser)]
#[command(name = "antim")]
#[command(version = "0.5.0")]
#[command(about = "Simple archive app")]
#[command(long_about = None)]
#[command(author = "Tursunov Imran <tursunov.imran@mail.ru>")]
struct Cli {
    #[arg(value_enum, value_name = "Mode of operation")]
    mode: Mode,

    #[arg(short, long, aliases = vec!["src"], value_name = "Source path")]
    source: PathBuf,

    #[arg(short, long, aliases = vec!["dst"], value_name = "Destination path")]
    destination: PathBuf,

    #[arg(short, long, value_parser = ["zip", "tar"],
        value_name = "Data format. Needed only for compressing data.\n In case there is a decompression, data format will be define automatically")]
    format: Option<String>,
    // --force - Allow do operation with replace existed

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

impl Cli {
    fn validate(&self) {
        if !(self.source.exists()) {
            eprintln!("Error: Invalid source path.");
            std::process::exit(1);
        }
        if self.destination.is_file() {
            eprintln!("Error: Destination path must be directory, not file.");
            std::process::exit(1);
        }
    }
}

#[derive(Copy, Clone, PartialEq, Eq, PartialOrd, Ord, ValueEnum)]
enum Mode {
    // Compress data
    Compress,
    // Decompress data
    Decompress,
}

/* --------------------------------------------- */

fn main() {
    let cli: Cli = Cli::parse();

    cli.validate();

    let src: &str = cli.source.to_str().unwrap(); // [DANGER] CALL PANICS
    let dst: &str = cli.destination.to_str().unwrap(); // [DANGER] CALL PANICS

    let df: String;
    match cli.format {
        Some(dataformat) => df = dataformat,
        None => df = String::from(""),
    }

    match cli.mode {
        Mode::Compress => match compression_distribution(src, dst, &df) {
            Ok(_) => println!("Compress data has been successful."),
            Err(e) => eprintln!("Error: Compress data has been unsuccessful: {e}."),
        },
        Mode::Decompress => match decompression_distribution(src, dst) {
            Ok(_) => println!("Decompress data has been successful."),
            Err(e) => eprintln!("Error: Decompress data has been unsuccessful: {e}."),
        },
    }
}

/* --------------------------------------------- */

fn compression_distribution(src_path: &str, dst_path: &str, dataformat: &String) -> Result<()> {
    if dataformat.is_empty() {
        return Err(Box::new(std::io::Error::new(
            std::io::ErrorKind::Other,
            "Data format is empty",
        )));
    }

    match dataformat.to_lowercase().as_str() {
        "zip" => compress_to_zip(src_path, dst_path),
        "tar" => commpress_to_tar(src_path, dst_path),
        _ => Err(Box::new(std::io::Error::new(
            std::io::ErrorKind::Other,
            format!("Unsupported dataformat: {}", dataformat),
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

/*
    BUGS:
    - If 'src_path' is file, commpress is bug
*/
fn commpress_to_tar(src_path: &str, dst_path: &str) -> Result<()> {
    let src: PathBuf = PathBuf::from(src_path);
    let mut dest: PathBuf = PathBuf::from(dst_path);

    let file_name_str: String = match src.file_name().and_then(|s| s.to_str()) {
        Some(name) => get_base_name(name),
        None => "archive".to_string(),
    };
    

    if dest.is_dir() {
        dest.push(format!("{}.tar.gz", file_name_str)); // [MAGIC]
    } else {
        dest.set_file_name(format!("{}.tar.gz", file_name_str)); // [MAGIC]
    }


    let tar_gz: File = File::create(&dest)?;

    let enc: GzEncoder<File> = GzEncoder::new(tar_gz, Compression::default());
    let mut tar: tar::Builder<GzEncoder<File>> = tar::Builder::new(enc);

    tar.append_dir_all(dst_path, src_path)?;

    Ok(())
}
/* --------------------------------------------- */

fn get_base_name(path: &str) -> String {
    let parts: Vec<&str> = path.split('.').collect();

    String::from(parts[0])
}

fn get_file_extension(filename: &str) -> Option<&str> {
    Path::new(filename).extension().and_then(|ext| ext.to_str()) // Преобразуем в строку
                                                                 // .map(|ext| ext.to_string()) // Возвращаем расширение как String
}

/* --------------------------------------------- */

fn decompression_distribution(src_path: &str, dst_path: &str) -> Result<()> {
    match get_file_extension(src_path) {
        Some(ext) => match ext {
            "zip" => decompress_from_zip(src_path, dst_path),
            "gz" | "tgz" | "gzip" => decompress_from_tar(src_path, dst_path),
            _ => Err(Box::new(std::io::Error::new(
                std::io::ErrorKind::Other,
                format!("Unsupported dataformat: {}", ext),
            ))),
        },
        None => Err(Box::new(std::io::Error::new(
            std::io::ErrorKind::Other,
            "Extension not found",
        ))),
    }
}

/*
    BUGS:
    - If dir/file already exist func will be change only content in old dir/file
*/
fn decompress_from_zip(src_path: &str, dst_path: &str) -> Result<()> {
    let zip_file: File = File::open(src_path)?;
    let mut archive: zip::ZipArchive<File> = zip::ZipArchive::new(zip_file)?;

    for i in 0..archive.len() {
        let mut file: zip::read::ZipFile<'_> = archive.by_index(i)?;

        let outpath: PathBuf = match file.enclosed_name() {
            Some(path) => path.to_owned(),
            None => continue,
        };

        let full_outpath: PathBuf = PathBuf::from(dst_path).join(outpath);
        
        if file.is_dir() {
            std::fs::create_dir_all(full_outpath)?;
        } else {
            if let Some(parent) = full_outpath.parent() {
                if !parent.exists() {
                    std::fs::create_dir_all(parent)?;
                }
            }
            
            let mut outfile = File::create(full_outpath)?;
            io::copy(&mut file, &mut outfile)?;
        }
    }

    Ok(())
}

fn decompress_from_tar(src_path: &str, dst_path: &str) -> Result<()> {
    let tar_gz: File = File::open(src_path)?;

    let tar: GzDecoder<File> = GzDecoder::new(tar_gz);
    let mut archive: Archive<GzDecoder<File>> = Archive::new(tar);

    archive.unpack(dst_path)?;

    Ok(())
}
