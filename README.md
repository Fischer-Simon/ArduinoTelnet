# Arduino Telnet Library

A simple library providing a telnet server with a command history.

## Usage

Commands are created by inheritance of `ShellCommand` or `ShellCommandGroup`. Only the `execute` method has to be
implemented. An optional help and usage text can also be provided.

See `examples/example.ino` for a simple usage example.

## Optional shell client types

Todo: Provide examples for using.

### BLE

An experimental BLE shell client can be enabled by defining `ARDUINO_TELNET_ENABLE_BLE_SHELL_CLIENT`.

### File script

A shell client reading commands from a file can be enabled by defining `ARDUINO_TELNET_ENABLE_SCRIPT_SHELL_CLIENT`.
