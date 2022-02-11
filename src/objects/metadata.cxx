#include "fdp/objects/metadata.hxx"

namespace FDP {
std::string calculate_hash_from_file(const ghc::filesystem::path &file_path) {
  if (!ghc::filesystem::exists(file_path)) {
    throw std::invalid_argument("File '" + file_path.string() + "' not found");
  }

  std::ifstream file_(file_path, std::ios_base::in | std::ios_base::binary);

  const std::string hash_ = digestpp::sha1().absorb(file_).hexdigest();

  file_.close();

  return hash_;
}

std::string calculate_hash_from_string(const std::string &input) {
  return digestpp::sha1().absorb(input).hexdigest();
}

std::string generate_random_hash() {
  std::string random_string;

  // Use Both the random_device and chrono high resolution clock to generate
  // a good random seed

  std::random_device rd;
  std::mt19937::result_type seed =
      rd() ^
      ((std::mt19937::result_type)
           std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count() +
       (std::mt19937::result_type)
           std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
               .count());

  // Using mt19937 and a uniform distribution generate a 24 character random
  // string
  std::mt19937 gen(seed);
  std::uniform_int_distribution<unsigned> distrib('a', '9');

  for (unsigned long j = 0; j < 24; ++j) {
    random_string += distrib(gen);
  }

  return calculate_hash_from_string(random_string);
}

std::string current_time_stamp(bool file_name) {
  auto t_ = std::time(nullptr);
  auto tm_ = *std::localtime(&t_);

  std::ostringstream oss_;

  if (!file_name) {
    oss_ << std::put_time(&tm_, "%Y-%m-%d %H:%M:%S");
  } else {
    oss_ << std::put_time(&tm_, "%Y%m%d-%H%M%S");
  }

  return oss_.str();
}

std::string remove_local_from_root(const std::string &root){
  return std::regex_replace(root, std::regex(std::string("file:\\/\\/")), "");
}

std::string remove_backslash_from_path(const std::string &path){
  return std::regex_replace(path, std::regex(std::string("\\\\")), "/");
}

bool file_exists( const std::string &Filename )
{
    return access( Filename.c_str(), 0 ) == 0;
}

std::string read_token(const ghc::filesystem::path &token_path){
  std::ifstream key_(token_path, std::ios::in);
  std::string key_str_;
  key_ >> key_str_;
  key_.close();
  return key_str_;
}

}; // namespace FDP