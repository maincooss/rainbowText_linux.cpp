// rainbowText_linux.cpp
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <clocale>
#include <locale>
#include <unistd.h>
#include <random>

// 彩虹颜色结构体
struct Color {
    int r, g, b;
    Color(int red, int green, int blue) : r(red), g(green), b(blue) {}
};

// 生成彩虹颜色序列，支持随机相位
std::vector<Color> generateRainbowColors(int numColors, bool randomize = false) {
    std::vector<Color> colors;

    double phaseShift = 0.0;
    if (randomize) {
        static std::mt19937 rng{std::random_device{}()};
        std::uniform_real_distribution<double> dist(0, 2.0 * M_PI);
        phaseShift = dist(rng);  // 随机相位偏移
    }

    for (int i = 0; i < numColors; i++) {
        double position = (double)i / numColors;
        double frequency = 2.0 * M_PI;
        int r = (int)(sin(frequency * position + phaseShift + 0) * 127 + 128);
        int g = (int)(sin(frequency * position + phaseShift + 2) * 127 + 128);
        int b = (int)(sin(frequency * position + phaseShift + 4) * 127 + 128);
        colors.push_back(Color(r, g, b));
    }
    return colors;
}

// 按 UTF-8 分割字符串为完整字符
std::vector<std::string> utf8Split(const std::string& str) {
    std::vector<std::string> result;
    for (size_t i = 0; i < str.size();) {
        unsigned char c = str[i];
        size_t len = 1;
        if ((c & 0x80) == 0) len = 1;          // ASCII
        else if ((c & 0xE0) == 0xC0) len = 2;  // 2字节
        else if ((c & 0xF0) == 0xE0) len = 3;  // 3字节
        else if ((c & 0xF8) == 0xF0) len = 4;  // 4字节
        result.push_back(str.substr(i, len));
        i += len;
    }
    return result;
}

// 获取终端当前编码（简化版，只识别UTF-8）
bool isUtf8Locale() {
    const char* loc = setlocale(LC_CTYPE, "");
    if (!loc) return false;
    std::string s(loc);
    for (auto& c : s) c = std::tolower(c);
    return (s.find("utf-8") != std::string::npos || s.find("utf8") != std::string::npos);
}

// 打印彩虹文本
void printRainbowText(const std::string& text, bool vertical = false, bool randomize = false) {
    std::vector<std::string> chars;
    if (isUtf8Locale()) {
        chars = utf8Split(text);  // UTF-8 按字符分割
    } else {
        // 非 UTF-8：直接按字节输出（适用于 GBK/Big5 等多字节编码）
        for (size_t i = 0; i < text.size(); i++) {
            chars.push_back(text.substr(i, 1));
        }
    }

    std::vector<Color> colors = generateRainbowColors(chars.size(), randomize);

    for (size_t i = 0; i < chars.size(); i++) {
        Color c = colors[i];
        std::cout << "\033[38;2;" << c.r << ";" << c.g << ";" << c.b << "m";
        std::cout << chars[i];
        if (vertical) std::cout << std::endl;
    }

    std::cout << "\033[0m"; // 重置颜色
    if (!vertical) std::cout << std::endl;
}

// 检查是否有管道输入
bool hasPipedInput() {
    return !isatty(fileno(stdin));
}

// 显示帮助信息
void showHelp() {
    std::cout << "彩虹文本输出工具 (Linux 版)" << std::endl;
    std::cout << "用法: rainbow_text [选项] [文本]" << std::endl;
    std::cout << "      echo 文本 | rainbow_text [选项]" << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -h, --help     显示帮助" << std::endl;
    std::cout << "  -v, --vertical 垂直显示" << std::endl;
    std::cout << "  -r, --random   每行随机彩虹渐变" << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  rainbow_text 你好世界" << std::endl;
    std::cout << "  rainbow_text -v 你好世界" << std::endl;
    std::cout << "  rainbow_text -r 你好世界" << std::endl;
    std::cout << "  echo 你好 | rainbow_text -r" << std::endl;
}

int main(int argc, char* argv[]) {
    std::locale::global(std::locale("")); // 设置程序使用系统 locale

    bool vertical = false;
    bool randomize = false;
    std::string text = "彩虹文字";  // 默认文本
    bool hasPipe = hasPipedInput();

    if (argc > 1) {
        text.clear();
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") {
                showHelp();
                return 0;
            } else if (arg == "-v" || arg == "--vertical") {
                vertical = true;
            } else if (arg == "-r" || arg == "--random") {
                randomize = true;
            } else {
                if (!text.empty()) text += " ";
                text += arg;
            }
        }
        if (text.empty() && !hasPipe) {
            text = "彩虹文字";
        }
    }

    if (hasPipe) {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty() && std::cin.eof()) line = "彩虹文字";
            printRainbowText(line, vertical, randomize);
        }
        return 0;
    }

    printRainbowText(text, vertical, randomize);
    return 0;
}
