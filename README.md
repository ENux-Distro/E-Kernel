# E-Kernel

E-Kernel is an ring 0 operating system without a filesystem, VFS and interrupts, built from scratch

## Available Commands:

E-Kernel only has 3 commands:

- whoami: Tells you who you are, aka some special
- arch: Prints i386, since the OS is for 32 bit
- print: Reads what you want to print in an REPL then prints it out

## Building

You can build E-Kernel in 3 ways:

1. Manual git clone and make

```sh
git clone --depth 1 https://github.com/ENux-Distro/E-Kernel $(pwd)/E-Kernel # git clones E-Kernel to your current directory
make -C $(pwd)/E-Kernel all # Builds E-Kernel with make
```

2. One liner installation **(WIP)**
```sh
curl -fsSL https://raw.githubusercontent.com/ENux-Distro/E-Kernel/refs/heads/master/install.sh | bash # Downloads and runs the install.sh script
```

3. With the tar archive **(WIP)**

```sh
wget -qO- https://github.com/ENux-Distro/E-Kernel/refs/heads/master/misc/E-Kernel.tar.gz | tar -xz # Downloads and extracts the tar archive to $(pwd)/E-Kernel
make -C $(pwd)/E-Kernel all # Builds E-Kernel with make
```

# Booting E-Kernel

After compiling E-Kernel, you can run with like this.

```sh
make -C $(pwd)/E-Kernel run # Runs E-Kernel.elf with qemu. Assumes E-Kernel is already compiled and the E-Kernel is in the current directory you're working at
```

# RAM Usage

E-Kernel can boot on 2.204 MB's of RAM, making it almost 16x smaller than Linux, granted E-Kernel is nowhere close to Linux
