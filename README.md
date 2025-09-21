# rainbowText_linux.cpp
Output rainbow-colored text on the terminal for linux 
彩虹文本输出工具 (Linux 版)
用法: rainbow_text [选项] [文本]
      echo 文本 | rainbow_text [选项]
选项:
  -h, --help     显示帮助
  -v, --vertical 垂直显示
  -r, --random   每行随机彩虹渐变
示例:
  rainbow_text 你好世界
  rainbow_text -v 你好世界
  rainbow_text -r 你好世界
  echo 你好 | rainbow_text -r
