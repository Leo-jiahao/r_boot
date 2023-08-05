# r_boot
一种面向嵌入式系统的启动引导程序策略,实现了针对固件远程更新、启动跳转、环境配置等基本功能,使用回调机制可以方便扩展,对嵌入式固件更新和启动管理。采用主机/从机架构,支持拓展功能,实现bootloader主要功能,如远程固件更新,参数配置等。

# 主要功能
根据代码的注释和实现,这段代码的主要功能是:

## 一、 主机端(r_boot_master.c):
1. 初始化和创建会话session,注册串口发送函数等。
2. 封装请求命令,通过请求数组发送给从机。
3. 支持同步请求和异步请求,同步等待应答结果。
4. 实现命令的参数组包打包,支持参数字符串和二进制数据。
5. 解析收到的应答,判断执行结果,调用回调函数。
6. 提供读写Flash、跳转、环境变量等常用bootloader命令。
7. 使用环形缓冲区处理串口数据收发。
8. 实现了断点续传的文件传输协议。
9. 定时处理超时重传等事件。
10. 可以扩展自定义命令,强化通用性。

## 2. 从机端(r_boot_slave.c):

1. 实现一个启动引导程序(Bootloader),提供应用程序远程更新、环境变量设置等功能。
2. 使用环形缓冲区实现数据的输入输出缓存。
3. 定义了一些环境变量来存储应用程序状态信息,如当前应用地址、长度、CRC等。
4. 使用回调函数处理输入指令,可添加自定义回调来扩展功能。
5. 提供应用程序上传、跳转接口。
6. 实现基于协议的数据传输与解析。
7. 支持通过串口或其他方式输入数据,调用回调函数处理。
8. 可以工作在等待模式、命令模式、自动运行模式下。
9. 使用CRC校验数据完整性。
10. 提供读取/烧写flash接口。
11. 编写了示例回调函数如环境变量操作,应用上传等
（hahahaha, ai总结的）
