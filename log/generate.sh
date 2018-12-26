#!/bin/bash
file=$(date +%Y%m%d).markdown
if [ ! -e $file ];then
    cat << END > $file
 ---
 title:$(date +%Y%m%d)
 date: $(date "+%Y-%m-%d %H:%M:%S")
 categories:
 tags:
 ---
## Plan
## 行为日志
\`\`\`
target: xxxxxxx
start: xxx
end: xxxxx
thinking: xxxx
status: xxxx
\`\`\`
## 思考
## 总结
END
fi





