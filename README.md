# MazOS
MazOS is a 32-bit Operating System for x86 processors, designed and built with the aim of better understanding low level computing.
  
  ![Logo](https://github.com/user-attachments/assets/a2c655e3-6a20-488e-a717-b75d240231b2)

## Information
Below are attached some additional documents that further explain, explore and analyse the problem at hand

📘 [Problem Statement](./docs/ProblemStatement.pdf) Defines the problem and sets the constraints

🛠️ [Technical Document](./docs/TechnicalDocument.pdf) Explores Design and Architectural choices made

💼 [Value Proposition](./docs/ValueProposition.pdf) States the importance of MazOS and problems it tries to solve

## Key Features
- custom kernel (although, GRUB is used as the bootloader)
- efficient memory manager using first-fit allocation
- custom process manager that supports concurrent processes and adaptive scheduling that dynamically switches algorithms based on workload characteristics
- a range of drivers including: PS/2 Mouse and Keyboard driver, ATA Disk driver, Sound Blaster 16 audio driver, VGA driver (and support for opening and rendering .png files)
- a fully functioning FAT32 filesytem and ability to perform file operations (open, read, write, close, create, delete, copy and paste)
- some basic POSIX compliant system calls (write, read, sbrk, exit, execve, fork, open and kill)
- intuitive command line interface and Unix-like commands (cd, ls, pwd, cat, rm, mk and touch) plus other custom commands for better usability 
- simple user-space application showcasing the range of features of MazOS: calculator tool, paint app, and snake game


## Project Highlights

**Concurrent Processes**
  
  ![Processes](https://github.com/user-attachments/assets/52907ec6-9f88-4cce-9c8c-0ffab1c677e2)
  * top-right(1): simple counter that is incremented, top-left(2): next prime number calculator, bottom-right(3): simple animation of square, bottom-left(4): scrolling text that reads "Hello Concurrent Processes" 

---

**Keyboard and Display**
  ![keyboard+display](https://github.com/user-attachments/assets/8ffc6323-9ede-44bc-9d5d-74437cfdc57d)
---

**Commnad Line Interface and 'help' command**
  ![cli_help](https://github.com/user-attachments/assets/bce2c99f-1feb-4f94-8074-95befcaa711d)
---

**``ls``, ``mkdir`` and ``cd`` commands**
  ![ls+mkdir+cd](https://github.com/user-attachments/assets/645f989a-b961-4b90-a000-6768dd7d1507)
---

**``touch`` and ``cat`` and file write commands**
  ![touch+cat+echo](https://github.com/user-attachments/assets/cbd15209-705c-4f16-8c24-e22498bc663e)
---

**``rmdir`` and ``rm`` commands**
  ![rmdir+rm](https://github.com/user-attachments/assets/dcd497ed-5842-4f64-9239-791f299efa85)
---

**PNG viewer**

  ![cat](https://github.com/user-attachments/assets/9d9981bf-b975-40de-bdb1-0758d7f61fd4)
---

**Paint user space application using the mouse driver**
  ![paint](https://github.com/user-attachments/assets/08a1acfc-85c8-48f1-b158-4395f5497406)
---

**Calculator user space application**
  ![calc](https://github.com/user-attachments/assets/fed5c55a-ee9d-4de1-b0e8-6ef7896bf2b7)
---

**Snake user space application**
  ![snake](https://github.com/user-attachments/assets/7f4810d8-34d7-4ff6-bb3e-e8d40995e81b)
---

## Usage
Here's how to build and run MazOS 👇

### **Prerequisistes**
> ``g++`` for compiling c++ files

>  ``nasm`` for compiling assembly files into binary

> ``qemu`` used as virtualisation software

### Step 1: Clone the repository
```sh
git clone https://github.com/angh-el/MazO
```

### Step 2: Build the os
```sh
make
```

### Step 2: Run the OS using QEMUS
```sh
make run
```

















  



