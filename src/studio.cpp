#include <filesystem>
#include <iostream>
#include <fstream>
#include <curl/curl.h>

#include "headers/toolkit/str_slice.h"
#include "headers/studio.h"

#define GENV_FILE_NAME ".genv"

struct HeaderValidator {
    bool is_http;
    bool is_image;
    std::string extension;

    bool all_good() {
        return this->is_http && this->is_image && (this->extension != "");
    }

    static bool http_check(HeaderValidator& hv, StrSlice& slice) {
        hv.is_http = slice.contains("HTTP") && slice.contains("OK");
        return hv.is_http;
    }

    static bool image_check(HeaderValidator& hv, StrSlice& slice) {
        std::string target = "image";
        int index = slice.contains(target);
        hv.is_image = index != StrSlice::npos;

        if (hv.is_image) {
            size_t start = index + target.size();
            hv.extension = StrSlice::from_slice(slice, start, slice.size() - start).to_string();
        }

        return hv.is_image;
    }
};

static size_t assess_header(char* buffer, size_t size, size_t nitems, void* userdata) {
    void** user_data_arr = (void**) userdata;
    
    // get our sent in user data
    HeaderValidator& hv = *((HeaderValidator*) *(user_data_arr));
    FILE** fpp = ((FILE**) *(user_data_arr + 1));
    std::string& filename = *((std::string*) *(user_data_arr + 2));

    bool (*header_funcs[2])(HeaderValidator&, StrSlice&) = {HeaderValidator::http_check, HeaderValidator::image_check};

    const char TOKEN_DELIM = '\n';
    size_t byte = 0;
    size_t tok_start = 0;
    int h_func_idx = hv.is_http + hv.is_image;

    while (byte < nitems && !hv.all_good()) {
        // if the chest is stuffed it'll contain a slice of our buffer that ends at the delimeter
        Chest<StrSlice> chest_slice = StrSlice::slice_on_delim(buffer, tok_start, byte, TOKEN_DELIM);

        // We hit the delimiter, do the current header_func (as indicated by our h_func_idx)
        if (chest_slice.is_stuffed()) {
            StrSlice& slice_ref = chest_slice.open();
            bool passed = header_funcs[h_func_idx](hv, slice_ref);
            h_func_idx += passed;
            tok_start = byte + 1;
        }

        byte++;
    }

    if (*fpp == NULL && hv.all_good()) {
        filename += "." + hv.extension;

        int result = fopen_s(fpp, filename.c_str(), "wb");
        if (result != 0) {
            std::cout << "Error: " << strerror(result) << std::endl;
            return 0;
        }
    }

    return nitems;
}

static size_t write_func(char* buffer, size_t size, size_t nmemb, void* userdata) {
    void** user_data_arr = (void**) userdata;
    FILE* fpp = *((FILE**) *(user_data_arr + 1));

    size_t out = fwrite(buffer, nmemb, size, fpp);
    return nmemb;
}

static bool save_image_url(const std::string& url, std::string file_name) {
    HeaderValidator hv{false, false, ""};
    FILE* fp = NULL;
    void* my_data[3] = { (void*) &hv, (void*) &fp, (void*) &file_name};

    CURL* curl = curl_easy_init();
    if (curl) {
        CURLcode res;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, my_data);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, assess_header);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, my_data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);

        res = curl_easy_perform(curl);
        std::cout << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
    }

    if (hv.all_good()) {
        fclose(fp);
    }

    return true;
}

namespace genv {
    Studio Studio::from_working_dir() {
        std::filesystem::path cwd = std::filesystem::current_path();
        std::cout << "Current working directory: " << cwd.string() << std::endl;
        Studio s;

        for (auto const& entry : std::filesystem::directory_iterator(cwd)) {
            if (std::filesystem::path(entry).filename().string() == GENV_FILE_NAME) {
                //std::cout << "this is a genv directory" << std::endl;
                s.is_genv_directory = true;
                return s;
            }
        }

        std::cout << "this is not a .genv directory" << std::endl;
        return s;
    }

    void Studio::process_command(const GenvCommand& command) {
        switch (command.unwrap_command_type()) {
            case CommandType::BUILD:
            {
                if (this->is_genv_directory) {
                    std::cout << "This is already a .genv directory! Exiting!";
                    exit(EXIT_FAILURE);
                }
            }
                break;
            case CommandType::DIR_NAME:
            {
                std::filesystem::path dir_full_path = (std::filesystem::current_path() / command.unwrap_command_arg());

                bool success = std::filesystem::create_directory(dir_full_path);
                if (!success) {
                    std::cout << "Studio.process_command(): Was unable to create directory: " + dir_full_path.string();
                    exit(EXIT_FAILURE);
                }

                std::ofstream outfile(dir_full_path / GENV_FILE_NAME);
                if (!outfile.is_open()) {
                    std::cout << "Studio.process_command(): Was unable to create .genv configuration file at directory: " + dir_full_path.string();
                    exit(EXIT_FAILURE);
                }

                outfile << "This is a genv file." << std::endl;
                outfile.close();
                break;
            }
            case CommandType::FILE_NAME:
                this->ref_image_filename = command.unwrap_command_arg();
                save_image_url(this->ref_image_url, this->ref_image_filename);
                break;
            case CommandType::END:
                break;
            case CommandType::REFERENCE:
                if (!this->is_genv_directory) {
                    std::cout << "Must call 'ref' in an existing .genv directory" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case CommandType::URL:
                this->ref_image_url = command.unwrap_command_arg();
                break;
            default:
                break;
        }
    }
}