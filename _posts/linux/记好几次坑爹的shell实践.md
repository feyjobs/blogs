---
title: 记一次坑爹的shell实践.markdown
date: 2018-01-14 01:46
categories: 
- Shell
- Code
---
## 背景介绍
忽然想看eva了！但是本宝宝不想看B站的阉割版(暗牧圣光大法),于是准备下2003重制版,关于为什么不下载2015蓝光版,请联系我，让我给你资源自己去下载试试。
悲催的宝宝开始在我的ubuntu上折腾,然后发现浏览器下载太特么慢了,15k/s的小水管,03版都下到地老天荒啊,于是决定在mac上下载然后scp到ubuntu上,关于为什么不下在mac上
因为mac容量小= =。
尝试一发之后，发现都很稳妥,但是还是有点慢,每个文件都scp好傻逼,想着能不能尝试crontab,这个好说,来写个脚本

## 坑爹
上代码
```shell
#!/bin/sh
files=`ls|grep -e "mkv$"`
PATH=/Users/baidu/Downloads/eva03重制/
for file in $files
do
    echo "\033[44;37m 正在复制$PATH$file \033[0m"
    scp $PATH$file gaohao@192.168.99.184:/home/gaohao/Videos/eva/
    echo "\033[31m 正在删除$PATH$file \033[0m"
    rm $PATH$file
done
```
嗯，非常祥和稳妥脚本,每个下好的文件都会乖乖scp,然后rm,然后静静等下一个轮训
祛痘嘛得！
![命令无法找到](/uploads/shell/command_not_found.png)
这特么是为毛！！

在查了几十分钟后,始终提示我是不是PATH没有设置对,然而本宝宝直接把命令打印出来还是正常复制过去了啊！
最后找到了问题所在
**我特么把PATH重定义了!!**
导致scp,rm等命令没有找到可以执行的路径！
![PATH路径](/uploads/shell/PATH.png)
将PATH重新声明为ROOT功能就正常了

但是！
**为什么echo能正常运行,哪个大神求告诉!**

##学到的
**echo**
echo命令是shell里常用的打印调试命令,今天尝试把echo的打印输出加点花样,我就是这么优秀～
```shell
echo  "\033[字体背景色;字体颜色;字体效果m    something to print \033[0m"
```
### 问题点
如果要在终端加上花样,就需要加上选项-e,也就是这样
echo -e "\033[字体背景色;字体颜色;字体效果m    something to print \033[0m"
### 记录点
具体各个代码表示的效果,小姑娘小伙子们们自己网上找吧！
下面是找到的几个比较常用的,网上抄来的
```shell
#!/bin/bash
#
#下面是字体输出颜色及终端格式控制
#字体色范围：30-37
echo -e "\033[30m 黑色字 \033[0m"
echo -e "\033[31m 红色字 \033[0m"
echo -e "\033[32m 绿色字 \033[0m"
echo -e "\033[33m 黄色字 \033[0m"
echo -e "\033[34m 蓝色字 \033[0m"
echo -e "\033[35m 紫色字 \033[0m"
echo -e "\033[36m 天蓝字 \033[0m"
echo -e "\033[37m 白色字 \033[0m"
#字背景颜色范围：40-47
echo -e "\033[40;37m 黑底白字 \033[0m"
echo -e "\033[41;30m 红底黑字 \033[0m"
echo -e "\033[42;34m 绿底蓝字 \033[0m"
echo -e "\033[43;34m 黄底蓝字 \033[0m"
echo -e "\033[44;30m 蓝底黑字 \033[0m"
echo -e "\033[45;30m 紫底黑字 \033[0m"
echo -e "\033[46;30m 天蓝底黑字 \033[0m"
echo -e "\033[47;34m 白底蓝字 \033[0m"

#控制选项说明
#\033[0m 关闭所有属性
#\033[1m 设置高亮度
#\033[4m 下划线
echo -e "\033[4;31m 下划线红字 \033[0m"
#闪烁
echo -e "\033[5;34m 红字在闪烁 \033[0m"
#反影
echo -e "\033[8m 消隐 \033[0m "

#\033[30m-\033[37m 设置前景色
#\033[40m-\033[47m 设置背景色
#\033[nA光标上移n行
#\033[nB光标下移n行
echo -e "\033[4A 光标上移4行 \033[0m"
#\033[nC光标右移n行
#\033[nD光标左移n行
#\033[y;xH设置光标位置
#\033[2J清屏
#\033[K清除从光标到行尾的内容
echo -e "\033[K 清除光标到行尾的内容 \033[0m"
#\033[s 保存光标位置
#\033[u 恢复光标位置
#\033[?25| 隐藏光标
#\033[?25h 显示光标
echo -e "\033[?25l 隐藏光标 \033[0m"
echo -e "\033[?25h 显示光标 \033[0m"
```
## 某次文件diff
直接上代码
```shell
#!/bin/bash
i=0
ROOT=/home/work/wmq/wmq-dw/pusher/conf/pusher/
for line in `cat ${ROOT}pusher.yml |grep yml|awk -F: '{print $2}'|sort`
do
    name[${i}]=$line
    let i=${i}+1
done

i=0
for line in `ls -l ${ROOT}groups/|awk '{print $9}'|sort`
do
    files[${i}]=$line
    let i=${i}+1
done

for file in ${files[*]}
do
    flag=0
    for save in ${name[*]}
    do
        if [ $file == $save ];then
            flag=1
            break
        fi
    done
    if [ $flag -eq 0 ];then
        echo $file
    fi
done
```
* awk 修改输出换行符 'BEGIN{ORS=' '}{action}'
* 遍历数组${array[*]}
* 对比字符串用== 对比数组用-eq
* for的格式注意 for do done
* if的格式注意if ; then fi
早日熟练掌握shell!

