#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <regex>
#include <cctype>
#include "util.hpp"
#include "common.hpp"

struct stat info;

bool pathExists(const std::string& target)
{
    if (stat(target.c_str(), &info) != 0) {
        return false;
    }
    else if (info.st_mode & S_IFDIR) {
        return true;
    }
    else {
        return false;
    }
}

int ensureDirExists(std::string strDestName)
{
    // ensure directory path to the file exists
    size_t pos = -1;
    while ((pos = strDestName.find('/', pos + 1)) != std::string::npos) {
        std::string dirname = strDestName.substr(0, pos + 1);

        DIR* d = opendir(dirname.c_str());
        if (!d) {
            // we can use '/' liberally here - Windows doesn't seem to care as per
            // <https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/mkdir-wmkdir?view=vs-2019>
            // "In Windows NT, both the backslash ( \) and the forward slash (/ ) are valid path delimiters in character strings in run-time routines."
            int err = mkdir(dirname.c_str(), 0755);
            if (err != 0) {
                return err;
            }
        }
        else {
            closedir(d);
        }
    }

    return 0;
}

template <typename T>
std::string valueToString(T num)
{
    std::ostringstream convert;
    convert << std::setprecision(8) << num;
    return convert.str();
}

static double roundOff(double n)
{
    double d = n * 10.0;
    int i = d + 0.5;
    d = (float)i / 10.0;
    return d;
}

std::string formatFileSize(size_t size)
{
    static const char *SIZES[] = { "B", "K", "M", "G" };
    int div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

    double size_d = (float)size + (float)rem / 1024.0;
    std::string result = valueToString(roundOff(size_d)) + SIZES[div];
    std::replace(result.begin(), result.end(), '.', ',');

    return result;
}

bool stringFindIC(const std::string& strHaystack, const std::string& strNeedle)
{
    auto it = std::search(
        strHaystack.begin(), strHaystack.end(),
        strNeedle.begin(), strNeedle.end(),
        [](char ch1, char ch2) {
            return std::toupper(ch1) == std::toupper(ch2);
        });
    return (it != strHaystack.end());
}

bool stringEqualIC(const std::string& str1, const std::string& str2)
{
    return str1.size() == str2.size() && std::equal(str1.begin(), str1.end(), str2.begin(), [](const char& ch1, const char& ch2) {
        return std::toupper(ch1) == std::toupper(ch2);
    });
}

void stringToLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::string stringToLowerCopy(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void formatBytes(std::ostream& out, const unsigned char *data, size_t dataLen, bool format) {
    out << std::setfill('0');
    for(size_t i = 0; i < dataLen; ++i) {
        out << std::hex << std::setw(2) << (int)data[i];
        if (format) {
            out << (((i + 1) % 16 == 0) ? "\n" : " ");
        }
    }
    if (format) {
        out << std::endl;
    }
}
