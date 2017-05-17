# Simple shell

	Unix programming 2017 in NCTU hw3 : simple shell 

# Compile and Run

- make clean
- make
- ./shell

# Features
- Built-in commands
    - `exit`: terminate the shell
    - `export`: set environment variable
    - `unset`: unset environment variable
    - `jobs`: list all current jobs
    - `fg`: put a job to foreground
- `Ctrl-C` and `Ctrl-Z` signal handling
- Standard input/output redirection operators: `<` and `>`
- Create pipeline using pipe operator: `|`
- Filename expansion with `*` and `?` operators
- Job control support: `Ctrl-Z`, `jobs`, `fg`

## Environment Variables
- Will `pirntenv` right after `export` or `unset`

### Usage

```
export <KEY>=<value> [<KEY>=<value>, <KEY>=<value>, ...]
unset <KEY> [<KEY>, <KEY>, ...]
```

## Input Redirection

### Usage

```
cmd < inputfile
cmd > outputfile
cmd < inputfile > outputfile
cmd > outputfile < inputfile
```

## Job Control

### Usage

```
jobs		#list all jobs in background
fg %<jobID>	#foreground the job of jobID
```
