# C++ Code Generator Project

## Overview

This project generates C++ class structures from a simplified input syntax. It automates the creation of class definitions, member variables, and getters/setters, saving development time and reducing boilerplate code.

## Features

- **Class Generation**: Define classes with protected and public members.
- **Member Variables**: Specify member variable types, names, and access levels.
- **Constants**: Automatically generate static constants based on predefined prefixes.
- **Validation**: Ensures that names follow C++ naming conventions and are not reserved keywords.
- **Error Reporting**: Detects and reports duplicate names and invalid entries.
- **Error Resilience**: Handles bad input gracefully by skipping invalid entries and ensuring the generated output is a valid and compilable C++ class.

## File Structure

### Key Files

- **`gencode.cpp`**: The main program file that parses input and generates C++ code.
- **`input.txt`**: Defines the class structures, member variables, and constants in a simplified syntax.
- **`output.txt`**: The generated C++ code based on the input.

### Input File Format (`input.txt`)

- Use `class <name>` to define a new class.
- Specify member variables with `<type> - <name1> <name2> ...`.
- Use `pro` for protected members and `pub` for public members.
- Define static constants using `names <prefix> - <name1> <name2> ...`.
- End class definitions with `end`.

### Example Input:
```plaintext
class cat
string - name colour
int - weight description
names catdsc - fat hairy orange black
end

class person
string - firstname lastname
int - ssn balance
const cat * - pet
pub
string - favourite_colour
end
```

## Handling Bad Input

If the input contains errors such as duplicate member names, invalid types, or reserved keywords, the program will:

- Log the errors and line numbers to the console for debugging.
- Skip over the problematic lines while continuing to process the rest of the input.
- Ensure that the resulting `output.txt` file contains only valid C++ class definitions.

### Example of Bad Input:
```plaintext
class cat
string - name name
int - weight description
names catdsc - fat hairy weight black tabby white three_legged
end

class person
string - firstname lastname lastname city state phone
int - ssn balance
const cat * - pet
pub
string - int
end
```

### Generated Output for Bad Input:
Despite the errors, the output will include only valid constructs:
```cpp
class cat
{
 protected:
   string name;
   int weight, description;

 public:
   static const int catdsc_fat = 1, catdsc_hairy = 2, catdsc_black = 3, catdsc_tabby = 4, catdsc_white = 5, catdsc_three_legged = 6;

   string get_name() const
   { return name; }

   void set_name(const string & v)
   { name = v; }

   int get_weight() const
   { return weight; }

   void set_weight(const int & v)
   { weight = v; }
};
```

### Example Console Errors:
(unreleated to above code) When bad input is detected, errors will be logged to the terminal with details about the issue:
```plaintext
Script started on Mon Nov  4 19:00:07 2024
jrl351@rabbit:~/ECE318/Labs/gen % a.out
Error on line 1: Duplicate or invalid member name
Error on line 6: Duplicate or invalid member name
Error on line 10: 'int' is not a valid C++ name. (ex. Reserved word)
Error on line 10: Duplicate or invalid member name
Error on line 14: 'void' is not a valid C++ name. (ex. Reserved word)
Error on line 14: Duplicate or invalid member name
Error on line 16: Duplicate or invalid constant name
jrl351@rabbit:~/ECE318/Labs/gen % exit
exit
```
These messages help users identify and correct issues in their input files.

## How It Works

1. **Parsing**:
   - Reads `input.txt` to parse class definitions, member variables, and constants.
   - Validates names to ensure they are valid C++ identifiers and not reserved keywords.

2. **Code Generation**:
   - Creates class structures with member variables.
   - Generates getters and setters for each variable.
   - Outputs the resulting C++ code to `output.txt`.

## Getting Started

### Prerequisites

- A C++ compiler supporting C++11 or later.
- Basic understanding of C++ class structures.

### Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/TheVorant/Class_Generator.git
   cd Class_Generator
   ```
2. Compile the program:
   ```bash
   g++ -o gencode gencode.cpp
   ```
3. Run the program:
   ```bash
   ./gencode input.txt output.txt
   ```

## Contributions

Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch:
   ```bash
   git checkout -b feature-branch
   ```
3. Commit changes:
   ```bash
   git commit -m "Add new feature"
   ```
4. Push to the branch:
   ```bash
   git push origin feature-branch
   ```
5. Create a pull request.


---

Feel free to explore the code, suggest improvements, or create new features! Happy Generating!
