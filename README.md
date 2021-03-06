# Woody-woodpacker

## Overview

A packer is a program that encrypts and compress an executable.
At runtime, the encrypted executable will decrypt / decompress itself to be executed.
It's often used to hide a malicious payload, in order to bypass antiviruses.

This project is a simple packer for Elf64 executables. It's pretty detectable, but as I did it for educational purpose, it'll be okay for this time!


## Compilation & Usage

```
git clone https://github.com/lfalkau/woody-woodpacker
cd woody-woodpacker
make
./woody-woodpacker <elf64 file>
```

## Encryption

This packer assumes that the given executable contains a `.text` section and encrypts it with **[Chacha20](https://datatracker.ietf.org/doc/html/rfc7539)**.
Note that to be decrypted at runtime, the encryption key must be present somewhere in the executable, thus it's rather obfuscation than encryption.

## Injection

Several methods can be used to inject the stub. Here's the one that I choosed:
- Write the stub at the end of the file, after the section table, padded to be page aligned.
- Modify a .`note.*` section header to reference our stub (althrough it's not mandatory for our stub to be contained in a section, it was pretty convenient for me to do it this way).
- Modify a `PT_NOTE` program header to refer our new section and to be loadable.
- Update the header entrypoint to our injected segment.

## Ressources

[Chacha20 RFC](https://datatracker.ietf.org/doc/html/rfc7539)

[Chacha20  - Computerphile](https://www.youtube.com/watch?v=UeIpq-C-GSA&pp=ugMICgJmchABGAE=)

[Elf - Wikipedia](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)

[Executable and Linkable Format](http://www.skyfree.org/linux/references/ELF_Format.pdf)

[Executable packer](https://www.youtube.com/watch?v=5RK7sYTOeNk)

## Todo

- [ ] Replace chacha block macros with functions in the asm stub, to make the packed binary lighter.
- [ ] Use offsets in the stub, in order to pack binaries compiled with ASLR.
- [ ] Compress the binary, then uncompress it at runtime with the stub.
