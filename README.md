# Secret Store

Allows storing secrets in an encrypted file (at `~/.sneak`). This project works on both Windows and Linux (and presumably MacOS, but I haven't checked). This project requires a C++ 17 compiler.

Use `secretstore write` to write a secret, and `secretstore read` to read it.

Cryptography uses [libsodium](https://github.com/jedisct1/libsodium) as provided by [libsodium-cmake](https://github.com/robinlinden/libsodium-cmake.git)
