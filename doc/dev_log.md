# 开发日志
## 未完成目标
* 安装失败时，界面上报错。
* 读取json格式失败时，进行异常处理.
## 开发记录
2023.11.10
* 初步设计MainPage,DependencyInstaller的GUI界面。
* 建立QApplication-Core-Widget三层架构：
    * **QApplication**（应用）提供事件循环，**Widget**（页面）显示GUI，**Core**（内核）将二者联系起来。
    * Core中存储QApplication指针字段，控制程序的运行和结束。
    * Core与所有页面通过指针双向连接，同时支持双向的信号槽通信。需要注意的是，不同页面之间无法直接通信，需要一个页面向Core发起请求，Core根据该请求与指定的另一个页面通信。
    * Core管理程序的所有页面，它与所有页面通过指针和信号连接，两个页面之间的通信需要通过Core调度。

2023.11.11
* 在Dependency Installer中，使用QProcess与bash交互，检测和安装前置软件。
* 将前置软件(Dependencies)列在  requirements.txt中，计划使用如下命令进行安装:
    ```bash
    xargs sudo apt install -y < requirements.txt
    ```
    xargs 将requirements.txt中的每个条目作为sudo apt install的参数传入。
    在安装的过程中，本机(Ubuntu20.04LTS)需要作以下改动：
    * python-dev -> python-dev-is-python3
    * python -> python3
    * python-pip -> python3-pip
* QProcess运行shell命令
    QProcess继承于QIODevice，支持启动外部程序，此处介绍几个重要的函数:
    下面使用一个例子来说明QProcess的使用：
    ```cpp
    //this : MyWidget
    QProcess* proc = new QProcess(this);
    proc->setProgram("bash");//设置外部程序为bash
    proc->setArguments(QStringList() << "-c" << "pwd(or whatever command)");//设置参数，如果以bash为参数，需要将-c [命令]作为选项
    /*
    *等价于proc->start("bash",QStringList() << "-c" << "command");
    *启动进程，同时将进程以rw模式打开
    */
    proc->start();
    proc->waitForFinished(1000);//等待1000ms直到进程运行结束
    proc->close();//作为QIODevice的派生类，进程可以关闭，停止通信。
    ```
    其他的一些函数：
    [signals]:
    **finished**: 进程完成的信号
* QProcess中调用sudo命令
    安装前置软件需要superuser权限，需要使用sudo apt install命令，但是sudo命令在终端获取密码，这对于GUI程序非常不便。
    因此，计划在GUI对话框中引导用户输入密码(pwd)，然后QProcess调用如下命令：
    ```bash
    echo [pwd] | sudo -S apt install [dependencies]
    ```
    其中，-S参数表示从标准输入(stdin)读取密码而非终端

2023.11.12
* Linux命令行文本处理命令学习
    * **head**\
    输入为文件或标准输入，默认标准输出前10行,\
    可以使用选项-n更改默认值，有两种用法：
    ```bash
    head filename -n 6 #输出前6行
    ```
    ```bash
    head filename -n -6 #输出除了最后6行的内容
    ```
    * **tail**\
    与head相反，默认标准输出后10行\
    可以使用选项-n 更改默认值，
    ```bash
    tail filename -n 6 #输出末尾6行
    ```
    ```bash
    tail filename -n +6 #输出从第6行到末尾
    ```
    * **cut**
    cut命令接收文件或标准输入，对表格中的列进行处理。
        * 使用-c选项按字符截取表格中的列:
        ```bash
        cut tabname -c 1-3 #截取表格中的1-3字符的列
        ```
        * 使用-f选项按以tab(\t)分隔的条目（域）截取:
        ```bash
        cut tabname -f 1-3 #截取表格中1-3条目的列
        ```
        * 使用-d选项配合-f选项制定分隔符，取代默认的tab，比如:
        ```bash
        cut tabname -f 1-3 -d ;#截取表格中以;分隔的1-3条目
        ```
    * **comm**\
    comm命令接收两个有序文件作为输入，标准输出分为3行：
        * file1 unique;
        * file2 unique;
        * file1 and file2 in common

        可以利用comm进行比较，输出没有安装的软件包，比如:
        ```bash
        comm installed.txt requirements.txt -13
        ```
        -13选项表示省略1，3列，只输出第2列，刚好是requirements.txt有，而installed.txt没有的，对应的就是没有安装的软件包

2023.11.15
* 解决使用apt list重定向至文件时产生的警告:```WARNING: apt does not have a stable CLI interface. Use with caution in scripts.``` 
    产生该警告的根本原因是，apt 是apt-get的前端，它很好的支持命令行交互，但是无法提供稳定的脚本输出。然而，程序中需要使用apt list来列出已经安装软件的列表，如果将所有std error output都重定向消除：
    ```bash
    apt list $dependency > result.log 2> /dev/null
    ```
    其中，>表示重定向标准输出，2>表示重定向标准错误输出，而重定向目的地/dev/null表示丢弃所有标准错误输出。
* 处理两种最常见的错误：
    如前所述，DependencyInstaller将丢弃安装过程中所有的错误。然而，为了提高软件的易用性，软件将在安装之前处理最常见的两种错误：
        * 密码错误
        * 网络未连接
    如果以上两种错误出现，将终止安装，并在界面上显示出错误。
        * 密码错误
        * 网络未连接

2023.11.16
* 解决安装过程中GUI冻结的问题
安装过程中GUI冻结的原因在于程序阻塞在了waitForFinished函数，无法相应GUI事件，这将带来极差的用户体验。参考了https://doc.qt.io/archives/qq/qq27-responsive-guis.html 之后，找到了几种可行的解决方案。
    * 使用一个工作线程，将需要长时间阻塞的安装工作移动到与主线程分离的新线程，重新实现QThread类或调用QObject::moveToThread()函数。
    * 使用一个局部的事件循环。waitForFinished函数之所以会造成GUI阻塞，是因为在等待进程执行完毕时，并没有使用一个新的事件循环来相应GUI事件，因此可以构造一个局部的QEventLoop对象，连接信号和槽，在进程开始时调用QEventLoop::exec()等待进程结束。本方法可以简单解决GUI冻结问题。
    * 并行编程。继承QRunnable并覆写run()方法，同时使用QFutureWatcher观察进度。
最终确定使用第二种方法，为Core类增加QEventLoop* 类成员eventLoop，使用该eventLoop来相应长时间阻塞线程时的GUI事件。
* 重构代码
原本计划的前后端分离架构，会造成大量的信号槽连接，大大降低代码可读性。因此，仍然将软件模块化，每个模块是前后端集成的整体（除了Core只有后端），同时，每个窗体模块共享Core中的事件循环，用于处理其长时间操作时的GUI事件。
## Bug Report
* 可能存在GUI界面在不同电脑上显示不同的适配问题。
* 不确定对于不同版本的Ubuntu,requirements.txt是否需要改动,同时，requirements.txt必须为有序文件。
* (**Solved**)MainPage一开始黑屏，DependencyInstaller显示后才一起显示,初步怀疑是MainPage初始化时还没有事件循环。
* (**Solved**)下载软件包过程中，出现GUI冻结导致程序无响应
* (**Solved**)execute wrong sequence of switchGUI* function.(may need to checkout Qt source code)
* ChoiceGuide选择程序集时，多选会出现问题。


2023.11.19
* 完成DependencyInstaller的所有逻辑部分，增加子类PyLibInstaller，使用pip3检测和安装Python库。
* 注：requirements.txt和py_requirements.txt必须有序，否则会出现前置软件检测问题。
2023.11.20
* 使用JSON文件格式存储选择分支的信息，具有可拓展性。
* 初步搭建ChoiceGuide 前端界面，完成部分页面切换逻辑。
2023.11.23
* 完成ChoiceGuide的页面逻辑和数据存储
* 从现在开始，代码中的所有注释都为中文
## 学习记录
2023.11.11
* 检查Ubuntu系统是否安装某软件包的几种方式:

apt 是Ubuntu最常用的包管理工具，它可以自动检测和安装依赖，使用apt list命令列出已安装的包。
```bash
apt list vim
```
apt-cache policy命令用于显示对应包的详细信息。
```bash
apt-cache policy vim
```
dpkg 是debian系linux包管理工具，然而与apt相比，dpkg无法自动检测和安装依赖，可以使用dpkg -l列出所有的软件包，通过pipeline grep [package-name] 筛选出指定的软件包。
```bash
dpkg -l |grep -i vim
```
