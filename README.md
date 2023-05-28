<div align="center">
  <img width="187" src="https://user-images.githubusercontent.com/93570324/234292265-889228a8-7a68-4e2d-b891-f75262410af1.png"/>
</div>

<p align="center">
    <b>The AI-native database for next-generation AI apps</b>
</p>
## What is Infinity

Infinity is an AI-native database that can be used with large language models (LLMs) to meet typical To B (business-to-business) applications such as internal enterprise search, specific industry search, internal AI assistants, and chatbots. 


## Build Infinity from source
### Requirements

-   Operating system: Ubuntu 22.04 or higher

-   GCC 11.3 or higher to support C++20

-   CMake 3.10 or higher

-   Git

### Step1 Download Infinity source code

```shell
$ git clone https://github.com/infiniflow/infinity.git
```

### Step2 Install dependency

```shell
$ sudo apt install libboost-dev-all
```
### Step3 Build source code

```shell
$ cd infinity && mkdir build && cd build
$ cmake ..
$ make
```

### Step4 Start up Infinity server

```shell
$ ./build/src/infinity_main
```
