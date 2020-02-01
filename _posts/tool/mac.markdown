---
title: mac
date: 2019-11-09 21:04
categories: 
tags: 
---

# 基本配置
## 基本颜色
* iterm2 preference->profile->color->colorpresets
* brew install coreutils   && gdircolors --print-database > ~/.dir_colors
```
if brew list | grep coreutils > /dev/null ; then
  PATH="$(brew --prefix coreutils)/libexec/gnubin:$PATH"
  alias ls='ls -F --show-control-chars --color=auto'
  eval `gdircolors -b $HOME/.dir_colors`
fi
```
追加到bash_profile末尾

* (solarized dir颜色)[https://github.com/seebi/dircolors-solarized] 选择256 dircolors.256dark > ~/.dir_colors
重启 结束
