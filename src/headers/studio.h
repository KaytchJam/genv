#pragma once

#include "utility.h"

namespace genv {
    class Studio {
    ///
    bool is_genv_directory = false;
    std::string ref_image_url;
    std::string ref_image_filename;
    /// 
    ///
    public:
        static Studio from_working_dir();
        void process_command(const GenvCommand& command);
    };
}