#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace {
constexpr std::size_t char_threshold =
    104857600; // if below 100Mb then std::vector, else std::set as sorting container

class Handler {

public:
  Handler(std::string &outFile) : m_resultFile(outFile) {}

  bool getData(const std::string &path) {

    std::ifstream infile(path);
	
    if(infile.fail()) {
	  std::cerr << "Can't open input file.\n";
      return false;    
    }
	
    std::string line;
    std::string word;

    while (std::getline(infile, line)) {
      for (std::size_t i = 0; i < line.size(); ++i) {
        if (std::isalpha(line[i])) {
          word += std::tolower(line[i]);
          ++m_charCounter;
        } else {
          if (!word.empty()) {
            ++m_storage[word];
            word.clear();
          }
        }
      }
    }

    if(m_storage.empty()) {

      std::cerr << "No words found, exiting.\n";
	  return false;
    }

    return true;
  }

  void process() {

    if (m_charCounter < char_threshold) {
      processLittle();
    } else {
      processBig();
    }
  }

private:

  std::unordered_map<std::string, std::size_t> m_storage;
  using elem = std::pair<std::size_t, std::string>;
  std::size_t m_charCounter{0};
  std::string m_resultFile;

  struct compare {

    bool operator()(const elem &lhs, const elem &rhs) const {

      if (lhs.first == rhs.first) {
        return lhs.second < rhs.second;
      } else
        return lhs.first > rhs.first;
    }
  };

  template <typename T> bool dumpResult(const T &ty) {

    std::ofstream out;
    out.open(m_resultFile, std::ofstream::out);

    if (out.fail()) {
      std::cerr << "Can't create out file.\n";

      return false;
    }

    for (auto it = ty.begin(); it != ty.end(); ++it) {
      out << it->first << " " << it->second << std::endl;
    }

    out.close();

    return true;
  }

  void processLittle() {

    std::vector<elem> stats(m_storage.size());
    std::size_t ind{0};
    for (auto it = m_storage.begin(); it != m_storage.end(); ++it, ++ind) {
      stats[ind] = std::make_pair(it->second, it->first);
    }
    m_storage.clear();
    std::sort(stats.begin(), stats.end(), compare());
    dumpResult(stats);
  }

  void processBig() {

    std::set<elem, compare> stats;

    for_each(m_storage.begin(), m_storage.end(), [&stats](auto &param) {
      stats.emplace(param.second, param.first);
    });

    for (auto it = m_storage.begin(); it != m_storage.end(); ++it) {
      stats.emplace(it->second, it->first);
    }
    m_storage.clear();
    dumpResult(stats);
  }
};
}

int main(int argc, char *argv[])

    try {

  if (argc != 3) {

    std::cerr << "Usage: freq input output.\n";
	return 0;
  }

  std::string in(argv[1]);
  std::string out(argv[2]);

  Handler handler(out);

  if (handler.getData(in)) {

    handler.process();
  }
  
  return 0;
} catch (std::exception &ex) {

  std::cerr << "Exception caught:" << ex.what() << "\n";
  return 1;
} catch (...) {

  std::cerr << "Unknown exception caught.\n";
  return 1;
}
