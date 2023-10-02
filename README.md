# Arduino Telnet Library

A simple library providing a telnet server with a command history.

## Usage

Commands are created by inheritance of `ShellCommand` or `ShellCommandGroup`. Only the `execute` method has to be
implemented. An optional help and usage text can also be provided.

See `examples/example.ino` for a simple usage example.
