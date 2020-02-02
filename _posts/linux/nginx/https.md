---
title: https
date: 2019-05-17 17:17
categories: 
tags: 
- nginx
- https
---
[TOC]

本文探索nginx如何支持https请求,后续内容会分为以下部分讲解
* https服务器的配置
* 自建https server
* https原理,交互流程
* nginx https处理方法

# https原理,交互流程
# 自建https服务器
## CA服务器
首先我们需要自建一个CA认证机构
### openssl
首先确认已正确安装openssl,以及openssl版本
![-name](/uploads/nginx/https/openssl.png)
如果版本低于1.0.1f，建议升级，因为1.0.1f版本之下的OpenSSL有一个Heartbleed漏洞。

### 创建CA
**创建文件夹保存CA信息**
```shell
cd && mkdir -p myCA/signedcerts && mkdir myCA/private && cd myCA
```
myCA 用于存放 CA 根证书，证书数据库，以及后续服务器生成的证书，密钥以及请求
signedcerts:保存签名证书的 copy
private: 包含私钥

**配置CA相关参数**,在CA目录运行
```shell
echo '01'>serial && touh index.txt
```
创建caconfig.cnf文件
```shell
vim ~/myCA/caconfig.cnf
```
```shell
# My sample caconfig.cnf file.
#
# Default configuration to use when one is not provided on the command line.
#
[ ca ]
default_ca      = local_ca
#
#
# Default location of directories and files needed to generate certificates.
#
[ local_ca ]
dir             = /home/<username>/myCA                    # 这里要将username替换为你的用户名
certificate     = $dir/cacert.pem
database        = $dir/index.txt
new_certs_dir   = $dir/signedcerts
private_key     = $dir/private/cakey.pem
serial          = $dir/serial
#
#
# Default expiration and encryption policies for certificates.
#
default_crl_days        = 365
default_days            = 1825
default_md              = SHA256
#
policy          = local_ca_policy
x509_extensions = local_ca_extensions
#
#
# Default policy to use when generating server certificates.  The following
# fields must be defined in the server certificate.
#
[ local_ca_policy ]
commonName              = supplied
stateOrProvinceName     = supplied
countryName             = supplied
emailAddress            = supplied
organizationName        = supplied
organizationalUnitName  = supplied
#
#
# x509 extensions to use when generating server certificates.
#
[ local_ca_extensions ]
subjectAltName          = DNS:localhost
basicConstraints        = CA:false
nsCertType              = server
#
#
# The default root certificate generation policy.
#
[ req ]
default_bits    = 2048
default_keyfile = /home/<username>/myCA/private/cakey.pem  # 这里要将username替换为你的用户名
default_md      = SHA256
#
prompt                  = no
distinguished_name      = root_ca_distinguished_name
x509_extensions         = root_ca_extensions
#
#
# Root Certificate Authority distinguished name.  Change these fields to match
# your local environment!
#
[ root_ca_distinguished_name ]
commonName              = MyOwn Root Certificate Authority # CA机构名
stateOrProvinceName     = JS                               # CA所在省份
countryName             = CN                               # CA所在国家（仅限2个字符）
emailAddress            = XXXX@XXX.com                     # 邮箱
organizationName        = XXX                              #
organizationalUnitName  = XXX                              #
#
[ root_ca_extensions ]
basicConstraints        = CA:true
```
**生成CA证书**
```shell
export OPENSSL_CONF=~/myCA/caconfig.cnf       #该命令用于给环境变量 OPENSSL_CONF 赋值为caconfig.cnf。
openssl req -x509 -newkey rsa:2048 -out cacert.pem -outform PEM -days 1825             # 生成 CA 根证书和密钥
```
该命令需要用户设置密码。不要忘记。
以上步骤生成了 CA 自签名根证书，和 RSA 公/私密钥对。证书的格式是 PEM，有效期是1825天。

* myCA/cacert.pem: CA 根证书
* myCA/private/cakey.pem： CA 私钥

## 创建服务器公私钥

