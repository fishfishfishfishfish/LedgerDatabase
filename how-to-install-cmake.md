1. 查看当前版本
版本太高编译好像不太行，导致empty key。目前建议cmake 3.22.1
```bash
xinyuchen@zju:/usr/local/opt$ cmake --version
cmake version 3.28.3

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```
查看当前位置
```bash
which cmake
/usr/local/bin/cmake
```

```bash
wget https://cmake.org/files/v3.22/cmake-3.22.1-linux-x86_64.tar.gz
tar -xzf cmake-3.22.1-linux-x86_64.tar.gz
```

3. 复制到目标目录
```bash
sudo mkdir -p  /usr/local/opt/cmake-3.22.1/
sudo cp -r cmake-3.22.1-linux-x86_64/* /usr/local/opt/cmake-3.22.1/
```

4. 使用 update-alternatives 管理（推荐｜系统级）

```bash
sudo rm /usr/local/bin/cmake # 如果系统已经有cmake
sudo update-alternatives --install /usr/local/bin/cmake cmake /usr/local/opt/cmake-3.22.1/bin/cmake 10
```
- 数字为优先级，越高越优先


切换版本：
```bash
sudo update-alternatives --config cmake
```

输出示例：
```bash
There are 2 choices for the alternative cmake (providing /usr/bin/cmake).

  Selection    Path                                Priority   Status
------------------------------------------------------------
* 0            /opt/cmake-3.27.9/bin/cmake          20        auto mode
  1            /opt/cmake-3.20.5/bin/cmake          10        manual mode
  2            /opt/cmake-3.27.9/bin/cmake          20        manual mode

Press <enter> to keep the current choice[*], or type selection number:
```
输入编号即可切换。