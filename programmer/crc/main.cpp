#include <jlib/jlib/util/std_util.h>
#include <jlib/jlib/util/str_util.h>
#include <libhbcheck/libhbcheck.h>
#include <libstc/disassembler/hex80.h>
#include <libstc/disassembler/intel8051is.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <list>

namespace po = boost::program_options;

// 定义允许的CRC算法
const std::vector<std::string> allowed_formats = {"hb_sum", "crc8", "crc16", "crc32"};

// 自定义验证函数（可选：自动转换为小写）
void validate_format(const std::string& val) {
    std::string normalized_val = boost::algorithm::to_lower_copy(val);  // 可选：转换为小写
    auto it = std::find(allowed_formats.begin(), allowed_formats.end(), normalized_val);
    if (it == allowed_formats.end()) {
        throw po::validation_error(
            po::validation_error::invalid_option_value,
            "format",
            val);
    }
}

bool validate_hex_impl(const std::string& val) {
    if (val.size() > 2) {
        std::string hex_part = val.substr(2);
        jlib::trim(hex_part);
        if (!hex_part.empty() &&
            std::all_of(hex_part.begin(), hex_part.end(), ::isxdigit)) {
            return true;
        }
    }
    return false;
}

void validate_ldr_size(const std::string& val) {
    if (!validate_hex_impl(val)) {
        throw po::validation_error(
            po::validation_error::invalid_option_value,
            "ldr_size",
            val);
    }
}

void validate_meta_size(const std::string& val) {
    if (!validate_hex_impl(val)) {
        throw po::validation_error(
            po::validation_error::invalid_option_value,
            "meta_size",
            val);
    }
}

void validate_fill(const std::string& val) {
    if (!validate_hex_impl(val)) {
        throw po::validation_error(
            po::validation_error::invalid_option_value,
            "fill_value",
            val);
    }
}

void validate_build_time(const std::string& val) {
    if (!validate_hex_impl(val)) {
        throw po::validation_error(
            po::validation_error::invalid_option_value,
            "app_build_timestamp",
            val);
    }
}

void validate_version(const std::string& val) {
    if (!validate_hex_impl(val)) {
        throw po::validation_error(
            po::validation_error::invalid_option_value,
            "app_version",
            val);
    }
}

int main(int argc, char* argv[]) {
    std::string input_file, sldr_size, smeta_size, sfill_value, sapp_build_timestamp, sapp_version, meta_path;
    uint32_t ldr_size = 0, meta_size = 0, fill_value = 0, app_size = 0;
    uint32_t app_build_timestamp = 0;
    uint32_t app_version = 0;

    po::options_description desc(
        "Usage: crc -i input_file -l ldr_size -m meta_size -f fill_value -t app_build_timestamp -v app_version -M meta_bin_path\n"
        "Example: crc -i input.hex -l 0x1000 0x200 -f 0x00 -t 0x68C7B8E7 -v 0x01000000 -M meta.bin\n");
    auto init = desc.add_options();
    init = init("help,h", "Show usage");
    init = init("input,i", po::value<std::string>(&input_file), "Input file");
    init = init("ldr_size,l", po::value<std::string>(&sldr_size)->notifier(&validate_ldr_size), "Bootloader size");
    init = init("meta_size,m", po::value<std::string>(&smeta_size)->notifier(&validate_meta_size), "Meta info size");
    init = init("fill_value,f", po::value<std::string>(&sfill_value)->notifier(&validate_fill), "Fill value");
    init = init("app_build_timestamp,t", po::value<std::string>(&sapp_build_timestamp)->notifier(&validate_build_time), "Application build timestamp");
    init = init("app_version,v", po::value<std::string>(&sapp_version)->notifier(&validate_version), "Application version");
    init = init("meta_path,M", po::value<std::string>(&meta_path), "Output Meta Info Binary path");

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(1);
        }

        if (input_file.empty()) {
            std::cout << "Input file is required." << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        }

    } catch (const po::error& e) {
        std::cout << e.what() << std::endl
                  << desc << std::endl;
        exit(1);
    }

    ldr_size = std::strtoul(sldr_size.c_str(), nullptr, 16);
    if (ldr_size == 0) {
        std::cerr << "Invalid bootloader size: " << sldr_size << std::endl;
        return 1;
    }

    fill_value = std::strtoul(sfill_value.c_str(), nullptr, 16);
    if (fill_value > 0xFF) {
        std::cerr << "Invalid fill value: " << sfill_value << std::endl;
        return 1;
    }

    app_build_timestamp = std::strtoul(sapp_build_timestamp.c_str(), nullptr, 16);
    if (app_build_timestamp == 0) {
        std::cerr << "Invalid application build timestamp: " << sapp_build_timestamp << std::endl;
        return 1;
    }

    app_version = std::strtoul(sapp_version.c_str(), nullptr, 16);
    if (app_version == 0) {
        std::cerr << "Invalid application version: " << sapp_version << std::endl;
        return 1;
    }

    if (meta_path.empty()) {
        std::cerr << "Meta info binary path is required." << std::endl;
        return 1;
    }

    // read hex file
    std::string file_content;
    std::vector<uint8_t> bin_data;
    std::ifstream ifs(input_file, std::ios::binary);
    if (!ifs) {
        std::cerr << "Failed to open input file: " << input_file << std::endl;
        return 1;
    }
    ifs.seekg(0, std::ios::end);
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    file_content.resize((size_t)size);
    if (!ifs.read(&file_content[0], size)) {
        std::cerr << "Failed to read input file: " << input_file << std::endl;
        return 1;
    }

    // parse hex file
    std::vector<hex80_record_t> records;
    if (hex80_to_records(file_content, records)) {
        std::cerr << "Failed to convert hex80 to records." << std::endl;
        return 1;
    }
    std::vector<hex80_code_snippet_t> snippets;
    merge_hex80_records(records, snippets);
    if (snippets.empty()) {
        std::cerr << "No code snippets found in hex80 file." << std::endl;
        return 1;
    }

    // validate hex content
    // get a copy of snippets without the first element
    auto cpy = snippets;
    cpy.erase(cpy.begin());
    {
        // check if first snippet at 0x0000 and is `LJMP addr16`
        if (snippets[0].addr != 0 || snippets[0].dat.size() != 3 || snippets[0].dat[0] != 0x02) {
            std::cerr << "First code snippet address is not 0 or is not `LJMP addr16`." << std::endl;
            return 1;
        }

        // check if all other snippets addr bigger than ldr_size + 3
        if (!std::all_of(cpy.begin(), cpy.end(), [ldr_size](const hex80_code_snippet_t& snip) {
                return snip.addr >= ldr_size + 3;
            })) {
            std::cerr << "Some code snippet address is less than bootloader size + 3." << std::endl;
            return 1;
        }
    }

    // merge all snippets's dat to bin_data
    bin_data.insert(bin_data.end(), snippets[0].dat.begin(), snippets[0].dat.end());
    for (const auto& snip : cpy) {
        if (bin_data.size() + ldr_size < snip.addr) {
            // need fill
            size_t need_fill = snip.addr - (bin_data.size() + ldr_size);
            bin_data.insert(bin_data.end(), need_fill, (uint8_t)fill_value);
        }
        bin_data.insert(bin_data.end(), snip.dat.begin(), snip.dat.end());
    }

    // gen meta info and write file
    {
        typedef struct {
            uint32_t size;       // size of the whole application binary
            uint32_t crc;        // crc32 of the whole application binary
            uint32_t timestamp;  // UTC timestamp
            uint32_t version;    // major(8).minor(8).patch(16)
        } app_info_t;

        app_info_t meta;
        meta.size = bin_data.size();
        meta.crc = hb_crc32(bin_data.data(), bin_data.size());
        meta.timestamp = app_build_timestamp;
        meta.version = app_version;

        // to big endian
        meta.size = rev32(meta.size);
        meta.crc = rev32(meta.crc);
        meta.timestamp = rev32(meta.timestamp);
        meta.version = rev32(meta.version);

        // write to file
        std::ofstream ofs(meta_path, std::ios::binary);
        if (!ofs) {
            std::cerr << "Failed to open meta info binary file: " << meta_path << std::endl;
            return 1;
        }

        if (!ofs.write((const char*)&meta, sizeof(meta))) {
            std::cerr << "Failed to write meta info binary file: " << meta_path << std::endl;
            return 1;
        }
    }

    return 0;
}
