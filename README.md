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

- [Core Documentation](docs/DOCUMENTATION.md) - Beginner-friendly explanation of how the server works
- [Visual Guide](docs/VISUAL_GUIDE.md) - Diagrams illustrating the server architecture
- [Practical Examples](docs/EXAMPLES.md) - Code examples for extending the server

See the [docs README](docs/README.md) for more information.