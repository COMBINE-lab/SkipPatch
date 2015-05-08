/*
Usage: timeit commandfile

Will run every command in commandfile,
*/

package main

import (
    "bufio"
    "flag"
    "fmt"
    "io"
    "log"
    "os"
    "os/exec"
    "strings"
    "syscall"
    "time"
)

// input and output filenames
var (
    commandFile     string
    outputFile      string = "timeit.out"
    performanceFile string = "timeit.perf"
    execute         bool = true
)

// holds the command lines and the variable substitutions
type Commands struct {
    vars     map[string][]string
    cmdlines []string
    index    map[string]int
    varnames []string
}

func createCommands() *Commands {
    return &Commands{
        vars:     make(map[string][]string),
        cmdlines: make([]string, 0),
        index:    make(map[string]int),
        varnames: make([]string, 0),
    }
}

// return a slice of the current values of the variables
func (c *Commands) varValues() []string {
    out := make([]string, 0)
    for _, v := range c.varnames {
        out = append(out, c.vars[v][c.index[v]])
    }
    return out
}

func (c *Commands) varMap() map[string]string {
    out := make(map[string]string)
    for _, v := range c.varnames {
        out[v] = c.vars[v][c.index[v]]
    }
    return out
}

func (c *Commands) value(v string) string {
    return c.vars[v][c.index[v]]
}

func (c *Commands) varLen(v string) int {
    return len(c.vars[v])
}

func (c *Commands) variableVars() []string {
    out := make([]string, 0)
    for _, v := range c.varnames {
        if c.varLen(v) > 1 {
            out = append(out, v)
        }
    }
    return out
}


// increment index like an odometer, return true if it wraps around to 0
func (c *Commands) incrementIndex() bool {
    for _, v := range c.varnames {
        c.index[v]++
        if c.index[v] == len(c.vars[v]) {
            c.index[v] = 0
        } else {
            return false
        }
    }
    return true
}

// replace all variables with the replacement given by index
func (c *Commands) replace(cmdline string) string {
    for v, repl := range c.vars {
        cmdline = strings.Replace(cmdline, "#"+v, repl[c.index[v]], -1)
    }
    cmd := exec.Command("sh", "-c", "echo " + cmdline)
    out, err := cmd.CombinedOutput()
    if err != nil {
        log.Fatalln("Invalid command to execute:", cmdline) 
    }
    return string(out)
}

// parse a variable name from a string that looks like "# *name *="
func parseVarName(s string) string {
    return strings.TrimSpace(s[1:])
}

// parse a command file and return a Commands struct with the info
// it contains. The format of this file is:
//
// #cmd=a b
// c d e f g
// #foo=moo moo moo
//  moo moo
// ##
//
// name1: command -f #2 -o #2
// name2:
//
// where a variable definition starts is of the form: "# var = a1 a2 ..." where
// # var and = must be on the same line but a1 a2 ... can be split over
// arbitrary lines. Variable definitions end at a line that equals "##". Each
// remaining non-empty line contans a tag (first word, name1: above) and
// commmand lines with optionally #var patterns that are replaced by the words
// in the variable in turn.
//
func readCommandFile(fname string) *Commands {
    in, err := os.Open(fname)
    if err != nil {
        log.Fatalf("Couldn't open command file %f\n", fname)
    }
    defer in.Close()

    inCommands := false
    C := createCommands()
    varName := ""

    scanner := bufio.NewScanner(in)
    for scanner.Scan() {
        line := strings.TrimSpace(scanner.Text())
        if strings.HasPrefix(line, "//") { continue }
        if !inCommands {
            switch {

            // if line is empty, skip it
            case len(line) == 0:
                continue

            // if line equals ##, we're done with variables
            case line == "##":
                varName = ""
                inCommands = true

            // if line starts with #, extract var name
            case line[0] == '#':
                eqPos := strings.Index(line, "=")
                if eqPos == -1 {
                    log.Fatalf("#var line must have =: %s", line)
                }
                varName = parseVarName(strings.TrimSpace(line[:eqPos]))
                C.vars[varName] = strings.Fields(strings.TrimSpace(line[eqPos+1:]))
                C.index[varName] = 0
                C.varnames = append(C.varnames, varName)

            // if in variable line, append
            case varName != "":
                C.vars[varName] = append(C.vars[varName], strings.Fields(strings.TrimSpace(line))...)

            }
        } else {
            if len(line) > 0 {
                C.cmdlines = append(C.cmdlines, line)
            }
        }
    }

    return C
}

// run the given program with the given arguments and save the
// output to 'save'
func runProgram(program string, args []string, save io.Writer, tag string) *exec.Cmd {
    // run the command
    fmt.Printf("#### %s: %s %s\n", tag, program, strings.Join(args, " "))
    fmt.Fprintf(save, "#### %s: %s %s\n", tag, program, strings.Join(args, " "))
    if (execute) {
        cmd := exec.Command(program, args...)
        out, err := cmd.CombinedOutput()
        if err != nil && len(out) == 0 {
            fmt.Println("Command seemed to fail")
            os.Exit(0)
        }
        fmt.Fprintln(save, string(out))
        return cmd
    }
    return nil
}

func printVars(c *Commands, cmd string) string {
    out := ""
    for i, v := range c.variableVars() {
        if i > 0 {
            out += "\t"
        }
        if strings.Contains(cmd, "#" + v) {
            out += c.value(v)
        } else {
            out += "-"
        }
    }
    return out
}

// for every command, for every sub
func runAll(c *Commands, output, perf io.Writer) {
    already := make(map[string]bool)

    fmt.Fprintln(perf, "TAG\t", strings.Join(c.variableVars(),"\t"), "\tWALL\tSYS\tUSER\tMAX_RSS")

    // for every command line
    for _, cmd := range c.cmdlines {
        done := false
        for !done {
            cmdline := cmd
            cmdline = c.replace(cmdline)
            if !already[cmdline] {
                already[cmdline] = true
                a := strings.Fields(cmdline)
                if len(a) >= 2 {
                    // save time, memory, outputfile
                    start := time.Now()
                    result := runProgram(a[1], a[2:], output, a[0])
                    if result != nil {
                        wall := time.Now().Sub(start).Seconds()
                        rusage := result.ProcessState.SysUsage().(*syscall.Rusage)
                        fmt.Fprintln(perf, a[0], "\t", printVars(c, cmd), "\t",
                            wall, "\t",
                            result.ProcessState.SystemTime().Seconds(), "\t",
                            result.ProcessState.UserTime().Seconds(), "\t",
                            rusage.Maxrss)
                    }
                }
            }
            done = c.incrementIndex()
        }
    }
}


// set up the command line parsing
func init() {
    flag.StringVar(&commandFile, "c", commandFile, "The commands to execute")
    flag.StringVar(&outputFile, "o", outputFile, "File to save stdout to")
    flag.StringVar(&performanceFile, "p", performanceFile, "File to store the memory / time stats")
    flag.BoolVar(&execute, "n", execute, "If false, don't actually run")
}

func main() {
    flag.Parse()
    if commandFile == "" {
        log.Fatalf("Must specify -c option with command file")
    }
    c := readCommandFile(commandFile)
    log.Printf("Read %d command lines and %d variables\n", len(c.cmdlines), len(c.vars))

    out, err := os.Create(outputFile)
    if err != nil {
        log.Fatalf("Couldn't create output file:", outputFile)
    }
    defer out.Close()

    perf, err := os.Create(performanceFile)
    if err != nil {
        log.Fatalf("Couldn't create performance file:", performanceFile)
    }
    defer perf.Close()

    runAll(c, out, perf)
}
