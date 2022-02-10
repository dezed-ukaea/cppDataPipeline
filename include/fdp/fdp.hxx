#ifndef __FDP__
#define __FDP__

#include <ghc/filesystem.hpp>

#include "fdp/registry/datapipeline.hxx"

namespace FDP {

    class DataPipeline {

        public:
            // 'initialise' method for the API

            ~DataPipeline();

            explicit DataPipeline(
                    const std::string &config_file_path,
                    const std::string &script_file_path,
                    std::string token = "",
                    spdlog::level::level_enum log_level = spdlog::level::info);


            DataPipeline(DataPipeline &&rhs) noexcept;
            DataPipeline &operator=(DataPipeline &&rhs) noexcept;

            DataPipeline(const DataPipeline &rhs);
            DataPipeline &operator=(const DataPipeline &rhs);

            ghc::filesystem::path link_read(std::string &data_product);
            ghc::filesystem::path link_write(std::string &data_product);
            void finalise();

        private:
            class impl;

            std::shared_ptr< DataPipeline::impl > pimpl_;
    };
}; // namespace FDP

#endif
