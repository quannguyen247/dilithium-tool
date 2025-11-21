
# dilithium-tool — quick usage

This repository contains a small command-line tool that demonstrates signing (encrypting) and verifying (decrypting) files using the included Dilithium codec.

Key points:
- The tool operates on raw file bytes (binary) — it does not rely on file extension, so you can sign/verify files with or without an extension.
- When verifying a signed file, the tool will restore the file with the suffix `_restored` inserted before the original file extension (e.g. `example.exe.signed` -> `example_restored.exe`). If the original filename had no extension, it will append `_restored`.

Build (Windows / cmd.exe):

```
make
```

If your environment on Windows requires a different make tool use `mingw32-make`.

Examples (from repo root):

# 1) Generate a keypair
`dilithium-tool keygen secrets`

This writes `secrets/public.key` and `secrets/secret.key`.

# 2) Sign a file (binary-safe)
`dilithium-tool sign secrets/example.exe secrets/secret.key`

This creates `secrets/example.exe.signed` by default.

# 3) Verify / restore a signed file
`dilithium-tool verify secrets/example.exe.signed secrets/public.key`

Successful verification writes `secrets/example_restored.exe` (note the `_restored` suffix before the extension).

