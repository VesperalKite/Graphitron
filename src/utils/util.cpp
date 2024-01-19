//
// Created by Yunming Zhang on 1/24/17.
//

#include <graphitron/utils/util.h>

#include <fstream>

namespace graphitron {
    namespace util {

        std::string indent(std::string str, unsigned int num) {
            std::istringstream ss(str);
            std::string indent(num, ' ');
            std::string strIndented;
            std::string line;

            if (std::getline(ss, line)) {
                strIndented += indent + line;
            }
            while (std::getline(ss, line)) {
                strIndented += "\n" + indent + line;
            }
            return strIndented;
        }

        std::vector<std::string> split(const std::string &str, const std::string &delim,
                                       bool keepDelim) {
            std::vector<std::string> results;
            size_t prev = 0;
            size_t next = 0;

            while ((next = str.find(delim, prev)) != std::string::npos) {
                if (next - prev != 0) {
                    std::string substr = ((keepDelim) ? delim : "") + str.substr(prev, next-prev);
                    results.push_back(substr);
                }
                prev = next + delim.size();
            }

            if (prev < str.size()) {
                std::string substr = ((keepDelim) ? delim : "") + str.substr(prev);
                results.push_back(substr);
            }

            return results;
        }

        std::string escape(const std::string &str) {
            std::stringstream oss;

            for (const auto s : str) {
                switch (s) {
                    case '\a':
                        oss << "\\a";
                        break;
                    case '\b':
                        oss << "\\b";
                        break;
                    case '\f':
                        oss << "\\f";
                        break;
                    case '\n':
                        oss << "\\n";
                        break;
                    case '\r':
                        oss << "\\r";
                        break;
                    case '\t':
                        oss << "\\t";
                        break;
                    case '\v':
                        oss << "\\v";
                        break;
                    case '\\':
                        oss << "\\\\";
                        break;
                    case '\'':
                        oss << "\\'";
                        break;
                    case '\"':
                        oss << "\\\"";
                        break;
                    case '\?':
                        oss << "\\?";
                        break;
                    default:
                        oss << s;
                        break;
                }
            }

            return oss.str();
        }

        int loadText(const std::string &file, std::string *text) {
            std::ifstream ifs(file);
            if (!ifs.good()) {
                return 1;
            }

            *text = std::string((std::istreambuf_iterator<char>(ifs)),
                                (std::istreambuf_iterator<char>()));
            return 0;
        }

        std::string trim(const std::string &str, const std::string &ws) {
            const auto strBegin = str.find_first_not_of(ws);
            if (strBegin == std::string::npos)
                return ""; // no content

            const auto strEnd = str.find_last_not_of(ws);
            const auto strRange = strEnd - strBegin + 1;

            return str.substr(strBegin, strRange);
        }

        void variableLoop(std::vector<int>::const_iterator rangesBegin,
                          std::vector<int>::const_iterator rangesEnd,
                          std::vector<int>::iterator indicesBegin,
                          std::vector<int>::iterator indicesEnd,
                          std::function<void()> inner) {
            if (rangesBegin == rangesEnd) {
                inner();
            }
            else {
                int thisRange = *rangesBegin;
                for (*indicesBegin = 0; *indicesBegin < thisRange; ++(*indicesBegin)) {
                    variableLoop(rangesBegin+1, rangesEnd, indicesBegin+1, indicesEnd, inner);
                }
            }
        }

        void printDebugInfo(const std::string &message){
            std::cout << message << std::endl;
        }

        int replaceLine(std::string &str, const std::string &from, const std::string &to) {
            int num = 0;
            int pos = 0;
            while((pos = str.find(from, pos))!=std::string::npos) {
                num++;
                str.replace(pos, from.length(), to);
            }
            return num;
        }
        void replaceSstream(std::stringstream &inbuf, std::stringstream &outbuf, const std::string &from, const std::string &to) {
            std::string line;
            while (std::getline(inbuf, line)) {
                replaceLine(line, from, to);
                outbuf << line << std::endl;
            }
        }
        void replaceFile(std::istream &in, std::ostream &out, const std::string &from, const std::string &to) {
            std::string str;
            while (std::getline(in, str))
            {
                auto num = replaceLine(str, from, to);
                out << str << "\n";
            }
        }
        //replace the key
        void replaceFile(const std::string &in, const std::string &out, const std::string &from, const std::stringstream &wbf) {
            std::ifstream input(in);
            std::ofstream output(out);
            std::string str;
            while (std::getline(input, str))
            {
                auto to = wbf.str();
                auto num = replaceLine(str, from, to);
                output << str << std::endl;
            }
            input.close();
            output.close();
        }

        std::stringstream getReplacedSstreamFromFile(const std::string &filename, const std::map<std::string, std::string> &replaceList) {
            std::ifstream infile(filename);
            std::stringstream inbuf;
            inbuf << infile.rdbuf();
            std::stringstream outbuf;

            std::string line;
            while (std::getline(inbuf, line)) {
                for (auto pair : replaceList) {
                    replaceLine(line, pair.first, pair.second);
                }
                outbuf << line << std::endl;
            }
            return outbuf;
        }
        //insert before key
        void insertFile(const std::string &in, const std::string &out, const std::string &key, const std::stringstream &wbf) {
            std::ifstream input(in);
            std::ofstream output(out);
            std::string line;
            while (std::getline(input, line)) {
                if (line.find(key) != std::string::npos) {
                    output << wbf.str();
                }
                output << line << std::endl;
            }
            input.close();
            output.close();
        }

    }
}
