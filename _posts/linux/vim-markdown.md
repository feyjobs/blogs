---
title: vim
date: 2017-07-16 14:43:10
categories: 
- Tool
---

貌似没人介绍过这个vim-markdown插件,随便翻一翻方便查阅
# 安装
如果你使用的是 Vundle,则在你的~/.vimrc里加上这两行配置
```shell
Plugin 'godlygeek/tabular'
Plugin 'plasticboy/vim-markdown'
```
注意 tabular必须放在vim-markdown前面
然后在vim里运行
```shell
:so ~/.vimrc
:PluginInstall
```
嗯,其他两种安装方式,我没用不管了,

# 选项

## 折行
```viml
let g:vim_markdown_folding_disabled = 1
```
通过这句设置vim中markdown文件是否折行.
要想全局设置vim的折行配置,需要使用下面的语句
```viml
set [no]foldenable
```

## 折行风格
## 适配TOC窗体
添加以下代码
```viml
let g:vim_markdown_toc_autofit = 1
```
使得TOC窗体大小更加合适,而不是直接占据半个屏幕

## 代码块语法高亮
你可以使用文件名后缀来标示你所使用的语言,当然如果你希望使用其他不同的标示,则可以通过类似以下语句添加
```viml
let g:vim_markdown_fenced_languages = ['csharp=cs']
```
这样你就可以在代码块中使用csharp来表示cs语言
默认的语言类型有['c++=cpp', 'viml=vim', 'bash=sh', 'ini=dosini']

