# AI Labs

A collection of practical exercises solved in C++ from my  *Artificial Intelligence* (SI) course in the 2025 Summer semester at the University of Wrocław.  
Each list of lab exercises is self-contained in its own folder with source code, problems specs, additional scripts, data, and tests.

## Lab Folder Structre

### Labs 1-3 follow the following structure

- **Makefile**  
  
- **README.md**  
  Contains instructions on using the makefile, the problems specification, sample cases, my comments about the exercise and sometimes explanations on running the code or additional scripts.
- **src/**  
    Contains source code for solutions of exercises, each in it's own file.
- **data/** (optional)  
  Any raw or large input files.
- **utils/**
  Additional scipts or modifications of solutions for additional purposes and the **validator**.
- **tests/**  
  Paired input/expected‐output files for each task. Intended only for use by the **validator**.
  Test files follow the naming scheme:

- `inX_Y.txt` – input for **exercise X**, test **Y**  
- `outX_Y.txt` – expected output for **exercise X**, test **Y**

---

### Validator usage

The validator script automates running your solution on all test cases for a given exercise and compares the output with the expected results.

Syntax:

`utils/validator.sh <exercise-number> <program> [program-args...]`

---

### Labs 4, 5 follow the following structure

- **Makefile**  
  
- **README.md**  
  Contains instructions on using the makefile, the problems specification, sample cases, my comments about the exercise and sometimes explanations on running the code or additional scripts.
- **include/**  
  Public header files defining common utilities, or abstract bases.
- **src/**  
  Implementation files for the solutions.
- **tests/**  
  TODO
  