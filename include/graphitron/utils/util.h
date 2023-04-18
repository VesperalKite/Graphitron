#ifndef GRAPHITRON_UTIL_H
#define GRAPHITRON_UTIL_H


#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <functional>

namespace graphitron {
    namespace util {

/// Turn anything that can be written to a stream into a string.
        template <class T>
        std::string toString(const T &val) {
            std::stringstream sstream;
            sstream << val;
            return sstream.str();
        }

/// Some << overloads to let the join functions print smart-pointer collections.
        template <typename T>
        std::ostream& operator<<(std::ostream &out, const std::shared_ptr<T> dim) {
            return out << *dim;
        }
        template <typename T>
        std::ostream& operator<<(std::ostream &out, const std::unique_ptr<T> dim) {
            return out << *dim;
        }

/// Join the elements between begin and end in a sep-separated string.
        template <typename Iterator>
        std::string join(Iterator begin, Iterator end, const std::string &sep=", ") {
            std::ostringstream result;
            if (begin != end) {
                result << *begin++;
            }
            while (begin != end) {
                result << sep << *begin++;
            }
            return result.str();
        }

/// Join the elements in the collection in a sep-separated string.
        template <typename Collection>
        std::string join(const Collection &collection, const std::string &sep=", ") {
            return join(collection.begin(), collection.end(), sep);
        }

/// Reverse join the elements in the collection in a sep-separated string.
        template <typename Collection>
        std::string rjoin(const Collection &collection, const std::string &sep=", ") {
            return join(collection.rbegin(), collection.rend(), sep);
        }

/// Indent each line in str by num spaces.
        std::string indent(std::string str, unsigned int num);

/// Split the string.
        std::vector<std::string> split(const std::string &str, const std::string &delim,
                                       bool keepDelim = false);

/// Print string with escaped characters.
        std::string escape(const std::string &str);

/// Load text from file, returning 0 if succesfull, false otherwise.
        int loadText(const std::string &file, std::string *text);

/// Trim whitespace from string
        std::string trim(const std::string &str, const std::string &ws = " \t\n");

        template <typename T>
        std::string quote(const T& t) {return "'" + graphitron::util::toString(t) + "'";}

        template <typename T>
        size_t getVectorSize(std::vector<T> vec) {
            return sizeof(T)*vec.size();
        }

/// Iterate over a variable-sized vector of ranges, executing the inner method
/// per inner loop.
        void variableLoop(std::vector<int>::const_iterator rangesBegin,
                          std::vector<int>::const_iterator rangesEnd,
                          std::vector<int>::iterator indicesBegin,
                          std::vector<int>::iterator indicesEnd,
                          std::function<void()> inner);

        void printDebugInfo(const std::string &message);
        int replaceLine(std::string &str, const std::string &from, const std::string &to);
        void replaceFile(std::istream &in, std::ostream &out, const std::string &from, const std::string &to);
        void replaceFile(const std::string &in, const std::string &out, const std::string &from, const std::stringstream &wbf);
        void insertFile(const std::string &in, const std::string &out, const std::string &key, const std::stringstream &wbf);
    }}
#endif //GRAPHITRON_UTIL_H
