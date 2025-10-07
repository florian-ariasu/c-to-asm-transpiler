# Simple C-to-Assembly Compiler

## Classes / Components
For this project, I implemented the following core components:

- **Lexer** – tokenizes the input C-like source code.  
- **Parser** – builds a simple abstract syntax tree (AST) based on the grammar.  
- **Translator** – converts AST nodes into NASM-style 32-bit assembly instructions.  
- **SymbolTable** – keeps track of declared variables and their register mapping.  
- **Emitter** – formats and outputs the final assembly code.  
- **Main** – entry point for running the transpilation process.  

---

## Feature Description

###### Data Structures
- Used **maps** and **vectors** (or equivalent structures) to maintain symbol mappings, register usage, and output instruction lists.  
- SymbolTable ensures variable-to-register consistency using the predefined mapping convention:  
  - `A → eax`  
  - `B → ebx`  
  - `C → ecx`  
  - `D → edx`  

###### Components
- The **Lexer** splits the input source code into tokens like identifiers, literals, and operators.  
- The **Parser** uses a lightweight grammar to recognize expressions, assignments, and control structures (`if`, `for`, `while`).  
- The **Translator** applies transformation rules from C statements to equivalent ASM instructions based on the assignment guide.  
- The **Emitter** outputs a `.asm` file, keeping consistent indentation, labels, and comments for clarity.  
- The **Main** file acts as the controller that glues everything together — reading input, invoking translation, and writing output.  

###### Instruction Coverage
The compiler currently supports:
- **MOV**, **ADD**, **SUB**, **MUL**, **DIV**  
- **AND**, **OR**, **XOR**  
- **SHL**, **SHR**  
- **CMP** and conditional jumps (**JE**, **JNE**, **JG**, **JL**, etc.)  
- Basic **if/else**, **for**, and **while** constructs.

---

## Implementation Notes

###### Design
- The implementation focuses on **simplicity** and **clarity** rather than performance or optimization.  
- I intentionally avoided complex register allocation algorithms — register mappings are fixed to help understand instruction semantics.  
- Control flow constructs (loops, conditionals) are translated into labels and jumps following the table examples provided in the assignment.  
- Division instructions are preceded by `MOV EDX, 0` to prevent undefined behavior.  

###### OOP / Structural Principles
- **Encapsulation** – internal state of components (lexer, parser, etc.) is hidden.  
- **Modularity** – each file has a single responsibility (lexing, parsing, translating, emitting).  
- **Abstraction** – translation logic is layered, mirroring compiler design stages.  

###### Comments & Code Style
- The code avoids unnecessary comments, relying on clear method and variable names instead.  
- Function and variable names are kept descriptive and consistent in English for readability.  
- The `make` system automates compilation.  

---

## Running the Project

> [!NOTE]  
> The transpiler executable must be named **`transpiler`** and located in the root directory.  

### Building
Use the provided `Makefile` to compile the project:
```bash
make
```

---

## Bonus: Vibe-Coding Mode 💽
This project was developed entirely in **vibe-coding mode** — a late-night coding flow powered by caffeine, lo-fi beats, and pure compiler energy.  
The goal was not only to make a working transpiler but also to feel the *aesthetic* of low-level computing: transforming abstract logic into raw assembly instructions — art from syntax.

---

## Tips for Running the Project  

> [!TIP]  
> To experiment interactively, you can pipe your own C-like input files directly into the transpiler:  
> ```bash
> ./transpiler < input.c > output.asm
> ```  

---

## Licence
This project is licensed under the **MIT Licence**.  
See the [LICENCE](./LICENSE) file for details.
