#ifndef __FDP__
#define __FDP__

#include <memory>
#include <string>

namespace FairDataPipeline {
/**
 * @brief DataPipeline Class:
 * A PIMPL Class for interacting the the FAIR Data Pipeline
 * 
 */
    class DataPipeline {

        public:
            typedef std::shared_ptr< DataPipeline > sptr;
  /**
   * @brief Construct a new Data Pipeline (PIMPL)
   * 
   * @param config_file_path 
   * @param script_file_path 
   * @param token 
   */
           static  sptr construct(
                    const std::string &config_file_path,
                    const std::string &script_file_path,
                    std::string token = "" );


  /**
   * @brief Destroy the Data Pipeline object
   * 
   */
            ~DataPipeline();

            //DataPipeline(DataPipeline &&rhs) noexcept;
            //DataPipeline &operator=(DataPipeline &&rhs) noexcept;


  /**
   * @brief Return a path to a given data product
   * Whilst recording it's meta data for the code run
   * 
   * @param data_product 
   * @return ghc::filesystem::path 
   */
	    std::string link_read(std::string &data_product);

  /**
   * @brief Return a path to be used for a given data product
   * whilst recording it's meta data
   * 
   * @param data_product 
   * @return ghc::filesystem::path 
   */
	    std::string link_write(std::string &data_product);

  /**
   * @brief Finalise the pipeline
   * Record all data products and meta data to the registry
   * update the code run with all appropriate meta data
   * 
   */
            void finalise();

        private:
            explicit DataPipeline(
                    const std::string &config_file_path,
                    const std::string &script_file_path,
                    std::string token = ""
                    );

            DataPipeline(const DataPipeline &rhs) = delete;

            DataPipeline &operator=(const DataPipeline &rhs) = delete;

            class impl;

            std::shared_ptr< DataPipeline::impl > pimpl_;
    };
}; // namespace FairDataPipeline
#endif
