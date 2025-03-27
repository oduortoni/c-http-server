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