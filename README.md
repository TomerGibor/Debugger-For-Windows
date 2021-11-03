## Debugger-For-Windows

A command-line-interface debugger for 64-bit Windows.

```bash
tomer@TomerGibor:/mnt/c/Projects/C/Debugger$ ./Debugger.exe ./Tests/test.exe
Create process debug event triggered: event number 3
Entry point is: 0x7ff7e4020000
Load DLL debug event triggered: event number 6
Load DLL debug event triggered: event number 6
Load DLL debug event triggered: event number 6
Load DLL debug event triggered: event number 6
Exception debug event triggered: event number 1
Exception code: 0x80000003
Exception address: 0x7ffc87a406b0
dbg $
```



### Available commands

- `help` | `h` - prints out the available commands.

- `info reg` | `i reg` - prints out the registers' values.

- `info b` | `i b` - prints out the current breakpoints with their indices.

- `info stack [<num_bytes>]` | `i stack [<num_bytes>]` - prints out the bytes currently present in the stack starting from rsp and going into higher addresses. Defaults to 100 bytes, user may also decide how many bytes to print.

- `print <address> <num_bytes>` | `p <address> <num_bytes>` - prints out the `num_bytes` bytes starting from `address`.

- `print str <address>` | `p str <address>` - prints out the ascii chars starting from `address`, going on until reaching a `\0`, or 1000 chars have been printed.

- `run` | `r` - runs the executable.

- `stepi` - single-step (executes one instruction).

- `breakpoint <address>` | `bp <address>` | `b <address>` - puts a breakpoint at that address.

- `continue` | `c` - continues execution until a breakpoint is reached.

- `delete <address>` | `d <address>` - removes the breakpoint at that address.

- `exit` | `quit` - exits the debugger.

  ##### Note

  All numbers may be entered as either hexadecimal (e.g. 0xf00dbabe) or in base 10 (decimal).

  


