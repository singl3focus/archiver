extern crate clap;

use clap::{Parser, ValueEnum};

use flate2::read::GzDecoder;
use flate2::write::GzEncoder;
use flate2::Compression;
use tar::Archive;

use zip_archive::Archiver;

use aes::cipher::{KeyIvInit, StreamCipher};
use aes::Aes256;
use hex::decode;

use std::fs::File;
use std::io::{Read, Write};
use std::path::{Path, PathBuf};
use std::{io, str};

/* ____________________[CONST]____________________ */

type Result<T> = std::result::Result<T, Box<dyn std::error::Error>>;

const INIT_VECTOR: &str = "1a2b3c4d5e6f70717273747576777879"; // Test Usage, Go to GitHub :)

const DEFAULT_ARCHIVE_NAME: &str = "archive";

const ZIP: &str = "zip";
const TAR: &str = "tar";
const GZ: &str = "gz";
const TARGZ: &str = "tar.gz";
const TGZ: &str = "tgz";

const ENCRYPT: &str = "enc";

// call_err println error message and call exit(1)
fn call_err<T: std::fmt::Display>(err_value: T) -> ! {
    eprintln!("Error: {err_value}");
    std::process::exit(1);
}

// get_base_name get returns the name of the file without an extension
fn get_base_name(filename: &str) -> String {
    let parts: Vec<&str> = filename.split('.').collect();
    String::from(parts[0])
}

// get_file_extension return last extension in filepath
fn get_file_extension(filepath: &str) -> Option<&str> {
    Path::new(filepath).extension().and_then(|ext| ext.to_str())
}

/* ____________________[APP]____________________ */

#[derive(Parser)]
#[command(name = "antim")]
#[command(version = "0.6.1")]
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

    #[arg(short, long, value_parser = [ZIP, TAR],
        value_name = "Data format. Needed only for compressing data.\n In case there is a decompression, data format will be define automatically")]
    format: Option<String>,

    #[arg(short, long, aliases = vec!["pass"],
        value_name = "Password. It is necessary to encrypt a new archive or decrypt an existing archive.")]
    password: Option<String>,
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

#[derive(Copy, Clone, PartialEq, Eq, PartialOrd, Ord, ValueEnum)]
enum Mode {
    // Compress data
    Compress,
    // Decompress data
    Decompress,
}

impl Cli {
    fn validate(&self) {
        if !(self.source.exists()) {
            call_err("Invalid source path.");
        }

        if self.destination.is_file() {
            call_err("Destination path must be directory, not file.");
        }

        if self.mode == Mode::Compress && self.format == None {
            call_err("Data format is empty");
        }
    }
}

/* --------------------------------------------- */

fn main() {
    let cli: Cli = Cli::parse();
    cli.validate();

    let src: std::result::Result<&str, &str> = cli.source.to_str().ok_or("Invalid source path");
    let src_path: &str = match src {
        Ok(s) => s,
        Err(e) => call_err(e),
    };

    let dst: std::result::Result<&str, &str> =
        cli.destination.to_str().ok_or("Invalid destination path");
    let dst_path: &str = match dst {
        Ok(d) => d,
        Err(e) => call_err(e),
    };

    let df: String = match cli.format {
        Some(dataformat) => dataformat,
        None => "".to_string(),
    };

    let pass: String = match cli.password {
        Some(password) => password,
        None => "".to_string(),
    };

    match cli.mode {
        Mode::Compress => match compression_distribution(src_path, dst_path, &df, pass) {
            Ok(_) => println!("Compress data has been successful."),
            Err(e) => call_err(format!("Compress data has been unsuccessful: {e}.")),
        },
        Mode::Decompress => match decompression_distribution(src_path, dst_path, pass) {
            Ok(_) => println!("Decompress data has been successful."),
            Err(e) => call_err(format!("Decompress data has been unsuccessful: {e}.")),
        },
    }
}

fn compression_distribution(
    src_path: &str,
    dst_path: &str,
    dataformat: &String,
    password: String,
) -> Result<()> {
    let is_encryption_needed: bool = !password.is_empty();

    let created_archive_path: String = match dataformat.to_lowercase().as_str() {
        ZIP => compress_to_zip(src_path, dst_path)?,
        TAR => compress_to_tar(src_path, dst_path)?,
        _ => {
            return Err(Box::new(std::io::Error::new(
                std::io::ErrorKind::Other,
                format!("Unsupported dataformat: {}", dataformat),
            )))
        }
    };

    if is_encryption_needed {
        let encrypted_path: String = format!("{created_archive_path}.{ENCRYPT}");

        let mut key = password.as_bytes().to_vec();
        while key.len() < 32 {
            key.push(0); // Дополняем ключ до 32 байтов
        }
        if key.len() > 32 {
            key = key[..32].to_vec(); // Обрезаем лишние байты
        }

        // let key: Vec<u8> = decode(password)?; // Требует Hex кодировку
        let iv: Vec<u8> = decode(INIT_VECTOR)?;

        encrypt_file(
            created_archive_path.as_str(),
            encrypted_path.as_str(),
            &key,
            &iv,
        )?; // Шифруем и сохраняем

        // std::fs::remove_file(dst_path)?; // Удаляем незащищённый файл. // TODO: Fix error - Access is denied. (os error 5).
    }

    Ok(())
}

fn decompression_distribution(src_path: &str, dst_path: &str, password: String) -> Result<()> {
    let mut src_file_ext = match get_file_extension(src_path) {
        Some(ext) => ext,
        None => {
            return Err(Box::new(std::io::Error::new(
                std::io::ErrorKind::Other,
                format!("There is no extension in the source data: {}", src_path),
            )))
        }
    };

    let new_src_path: &str = match src_file_ext {
        ENCRYPT => {
            if password.is_empty() {
                return Err(Box::new(std::io::Error::new(
                    std::io::ErrorKind::Other,
                    format!("File is encrypted but password is not specified: {}", src_path),
                )));
            }

            let mut key = password.as_bytes().to_vec();
            while key.len() < 32 {
                key.push(0); // Дополняем ключ до 32 байтов
            }
            if key.len() > 32 {
                key = key[..32].to_vec(); // Обрезаем лишние байты
            }
            // let key = decode(password)?;
            let iv = decode(INIT_VECTOR)?;

            // Убираем расширение ".enc"
            let decrypted_path = src_path.trim_end_matches(format!(".{ENCRYPT}").as_str());
            decrypt_file(src_path, decrypted_path, &key, &iv)?;

            // TODO: add remove encrypted file

            // Меняем расширение зашифрованного файла. А также путь с зашифрованного файла на расшифрованный
            src_file_ext = get_file_extension(decrypted_path).unwrap(); // * Call panic.

            decrypted_path
        }
        _ => src_path,
    };

    match src_file_ext {
        ZIP => decompress_from_zip(new_src_path, dst_path),
        GZ | TGZ | TARGZ => decompress_from_tar(new_src_path, dst_path),
        _ => Err(Box::new(std::io::Error::new(
            std::io::ErrorKind::Other,
            format!("Unsupported dataformat: {}", src_file_ext),
        ))),
    }
}

/* --------------------------------------------- */

// use zip::{ZipWriter, CompressionMethod, write::FileOptions};
// use std::fs::File;
// use std::path::Path;

// fn create_archive(files: Vec<String>, output: &str) -> Result<(), Box<dyn std::error::Error>> {
//     let path = Path::new(output);
//     let file = File::create(path)?;
//     let mut zip = ZipWriter::new(file);

//     for file_path in files {
//         let options = FileOptions::default()
//             .compression_method(CompressionMethod::Stored)  // Можно использовать CompressionMethod::Deflated
//             .unix_permissions(0o755);
//         zip.start_file(file_path, options)?;
//     }

//     zip.finish()?;
//     Ok(())
// }

/* //TODO: BUGs:
    - If 'src_path' is file, compress is bug (archive be empty)
*/
fn compress_to_zip(src_path: &str, dst_path: &str) -> Result<String> {
    let src: PathBuf = PathBuf::from(src_path);
    let dest: PathBuf = PathBuf::from(dst_path);

    let mut archiver: Archiver = Archiver::new();
    archiver.push(src); // Add dir to queue
    archiver.set_destination(dest); // Add dst path

    // let thread_count: i32 = 4;
    // archiver.set_thread_count(thread_count);

    archiver.archive()?;

    Ok(dst_path.to_string())
}

/* //TODO: BUGs:
    - If 'src_path' is file, compress is err (The directory name is invalid. (os error 267).)
*/
fn compress_to_tar(src_path: &str, dst_path: &str) -> Result<String> {
    let src: PathBuf = PathBuf::from(src_path);
    let mut dest: PathBuf = PathBuf::from(dst_path);

    let file_name: String = match src.file_name().and_then(|s| s.to_str()) {
        Some(name) => get_base_name(name),
        None => DEFAULT_ARCHIVE_NAME.to_string(),
    };

    if dest.is_dir() {
        dest.push(format!("{file_name}.{TARGZ}"));
    } else {
        dest.set_file_name(format!("{file_name}.{TARGZ}")); // * Unused
    }

    if !dest.parent().unwrap().exists() {
        // * Maybe Call panic
        std::fs::create_dir_all(dest.parent().unwrap())?; // * Maybe Call panic
    }

    let tar_gz: File = File::create(&dest)?;

    let enc: GzEncoder<File> = GzEncoder::new(tar_gz, Compression::default());
    let mut tar: tar::Builder<GzEncoder<File>> = tar::Builder::new(enc);

    if src.is_dir() {
        match tar.append_dir_all("", &src) {
            Ok(_) => Ok(dest.to_str().unwrap().to_string()), // * Maybe Call panic
            Err(e) => {
                // If compress failed -> We need delete archive file
                std::fs::remove_file(dest)?;

                Err(Box::new(std::io::Error::new(std::io::ErrorKind::Other, e)))
            }
        }
    } else {
        //TODO:
        // match File::open(src)
        // match tar.append_file("", &src) {
        //     Ok(_) => Ok(()),
        //     Err(e) => {
        //         // If compress failed -> We need delete archive file
        //         std::fs::remove_file(&dest)?;

        //         Err(Box::new(std::io::Error::new(
        //             std::io::ErrorKind::Other,
        //             e,
        //         )))
        //     }
        // }
        unimplemented!()
    }
}

/* --------------------------------------------- */

/* //TODO: BUGs:
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

        // if full_outpath.exists() {
        //     eprintln!("Warning: File {} already exists. Skipping.", full_outpath.display());
        //     continue; // Пропускаем файл, если он уже существует
        // } // TODO: add force flag

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

/* //TODO BUGs:
    - If the target directory already exists,
    files will be extracted into it. There is a bug that, if the file already exists, it will overwrite the content.
    To prevent that, consider adding a check and prompt or rename existing files if necessary.
    - Add create parent dir from
*/
fn decompress_from_tar(src_path: &str, dst_path: &str) -> Result<()> {
    let tar_gz: File = File::open(src_path)?;

    let tar: GzDecoder<File> = GzDecoder::new(tar_gz);
    let mut archive: Archive<GzDecoder<File>> = Archive::new(tar);

    archive.unpack(dst_path)?;

    Ok(())
}

/* --------------------------------------------- */

type Aes256Ctr = ctr::Ctr128BE<Aes256>;

fn encrypt_file(input_path: &str, output_path: &str, key: &[u8], iv: &[u8]) -> Result<()> {
    let mut file = File::open(input_path)?;
    let mut buffer = Vec::new();
    file.read_to_end(&mut buffer)?;

    let mut cipher = Aes256Ctr::new(key.into(), iv.into());
    cipher.apply_keystream(&mut buffer);

    let mut output_file = File::create(output_path)?;
    output_file.write_all(&buffer)?;

    Ok(())
}

fn decrypt_file(input_path: &str, output_path: &str, key: &[u8], iv: &[u8]) -> Result<()> {
    let mut file = File::open(input_path)?;
    let mut buffer = Vec::new();
    file.read_to_end(&mut buffer)?;

    let mut cipher = Aes256Ctr::new(key.into(), iv.into());
    cipher.apply_keystream(&mut buffer);

    let mut output_file = File::create(output_path)?;
    output_file.write_all(&buffer)?;

    Ok(())
}
