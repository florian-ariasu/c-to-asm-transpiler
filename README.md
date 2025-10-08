# Simple C-to-Assembly Transpiler

## Project Overview

This project is a simple C-to-NASM 32-bit assembly transpiler. It reads a restricted subset of C-like code and converts it into NASM-style assembly instructions. The focus is on clarity and understanding of instruction semantics rather than performance or full C language support.

---

## Core Components

* **Translator** – Parses each line of input and converts assignments, arithmetic operations, and logic operations into NASM assembly instructions.
* **Control Flow Handlers** – Translates `if`, `for`, and `while` statements into labeled assembly blocks with conditional jumps.
* **Register Mapper** – Maps variables `a`, `b`, `c`, `d` to fixed registers (`eax`, `ebx`, `ecx`, `edx`).
* **Main Loop** – Reads input lines, detects control structures, and invokes the translator for each statement.

---

## Feature Description

###### Supported Operations

* Arithmetic: **ADD**, **SUB**, **MUL**, **DIV**
* Logic: **AND**, **OR**, **XOR**
* Shifts: **SHL**, **SHR**
* Comparisons: **CMP** with conditional jumps (**JE**, **JNE**, **JG**, **JL**, **JGE**, **JLE**)

###### Control Flow

* **if** statements
* **for** loops (with initialization, condition, increment)
* **while** loops
* Uses labels (`start_loop`, `end_loop`) to emulate loop control.

###### Register Mapping

* `a → eax`
* `b → ebx`
* `c → ecx`
* `d → edx`

###### Error Handling

* Division by zero is detected and prints an error message.

---

## Implementation Notes

* **Simplicity and Clarity** – Fixed register mapping and line-by-line translation for readability.
* **Parsing Approach** – Basic character-based parsing without a full lexer or AST.
* **Code Style** – Descriptive variable names, consistent indentation, minimal comments.
* **Label Management** – Each control structure creates start and end labels to handle branching.

---

## Development Approach

This project was developed as an experimental learning exercise to explore compiler design concepts and low-level programming techniques.

---

## License

This project is licensed under the **MIT License**.
See the [LICENSE](./LICENSE) file for details.
