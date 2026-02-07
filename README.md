# c-http-server

## Usage

To compile the project, use the Makefile in the root directory. This will generate the following structure:
- The `bin/` folder contains the compiled binary and a folder for object files (`.o`).
- The `obj/` folder mirrors the `src/` folder, containing the corresponding `.o` files.

To compile and run the project, simply use:

```bash
make
```

## Port Issues

If the port is already in use, set a custom port by declaring the `PORT` environment variable:

```bash
export PORT=9000
```

The default port is 9000.

## Documentation

Comprehensive documentation is available in the [docs](docs) folder. The documentation includes:

- [Server Architecture](docs/SERVER_ARCHITECTURE.md) - Layered architecture design and protocol separation
- [Core Documentation](docs/DOCUMENTATION.md) - Beginner-friendly explanation of how the server works
- [Visual Guide](docs/VISUAL_GUIDE.md) - Diagrams illustrating the server architecture
- [Practical Examples](docs/EXAMPLES.md) - Code examples for extending the server

See the [docs README](docs/README.md) for more information.

## Contribution

To ensure consistent code style, `clang-format` is used. To ensure you will not
forget to execute it, download latest
[`pre-commit-*.pyz`](https://github.com/pre-commit/pre-commit/releases) 
```console
wget https://github.com/pre-commit/pre-commit/releases/download/v4.5.1/pre-commit-4.5.1.pyz
```
and run
```console
python pre-commit-*.pyz install
```
from the project's root directory.

Now, each time you'll execute `git commit` pre-commit will launch
`clang-format` and only allow you to perform the commit if all files are
conformed, otherwise you'll have to add all files that were modified and
repeat.

If you are relying on `clangd` during development, you, probably, want to setup
`compile_commands.json` locally. Recommended way is using `bear`:

```
bear -- make -B BUILD_TYPE=debug bin/server bin/gtest
```
