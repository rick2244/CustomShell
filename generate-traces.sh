#!/usr/bin/env bash
# This script is designed to be run on a Linux host (i.e., not in xv6)
# You should probably run this script to see what it does first. To do that, you
# may need to make it executable:
#
# chmod +x generate-traces.sh
#
# And then run it from the *root* of your OS directory.
#
#
# For each user space system call stub, this script will print out information
# about its name, arguments, and return values.
#
# You can add information to the stubs that this script will extract. For
# example, instead of:
#
# int write(int, const void *, int);
#
# You can add argument names:
#
# int write(int fd, const void *buf, int count);
#
# And this script will extract them. By default, pointers are assumed to be
# addresses, but if you want the script to treat them like strings you can add a
# bit of metadata with a C comment:
#
# int mkdir(const char* /*str*/ path);
#
# Here's a few examples from a user/user.h file:
# int exit(int status) __attribute__((noreturn));
# int wait(int *status);
# int pipe(int *pipefd);
# int mkdir(const char* /*str*/ path);


# Before we get to the main part of this program, here's an example of
# generating some code. It includes some headers, defines some functions, etc.

cat <<EOM 
#include "defs.h"
#include "syscall.h"

static int
read_int(int n)
{
  int i;
  argint(n, &i);
  return i;
}


// Function to read a string from the syscall arguments
static char* read_str(int n) {
    static char buf[512]; // Temporary buffer to hold the string
    // Call argstr to populate the buffer
    argstr(n, buf, 512);
    return buf;
}












void
strace(struct proc *p, int syscall_num, int ret_val)
{
  
  // TODO given a system call number, figure out what tracing info to show.
  //      The code generation below should get you close, but you will need to
  //      modify this function and the code generation procedure.

//hint a giant switch case
// Adding a comment so the following generated code doesn't actually run:

    switch(syscall_num){
EOM

# Next, the actual information extraction process.

# Retrieves the type of the argument provided as a printf format specifier
get_type() {
    case "${1}" in
    *'/*str*/'*) echo "%s" ;;
    *"*"*) echo "%p" ;;
    *) echo "%d" ;; # default to int
    esac
}

# Determines which function to call for a given format specifier.
get_conversion() {
    case "${1}" in
        '%p') echo "read_int(${2})" ;;
        '%s') echo "read_str(${2})" ;;
        '%d') echo "read_int(${2})" ;;
    esac
}
# Inspects the user/user.h file to extract system call information
for i in $(sed -n 's/entry("\(.*\)");/\1/p' user/usys.pl); do
    return_value=$(sed -nE \
        "s:\s*([A-Za-z_][A-Za-z0-9_\* ]+)\s+${i}.*:\1:p;"  user/user.h)
    args=$(sed 's/const//g; s/void//g; s/struct//g; s/((noreturn))//g' user/user.h \
        | sed -nE "s:.*\s${i}\((.*)\).*:\1:p")

    # Extract argument names and types for printing
    arg_list=""
    arg_convs=""
    arg_count=0
    export IFS=","
    for arg in $args; do
        arg_name=$(echo "${arg}" | sed -nE 's:.*\b([A-Za-z0-9_]+)\s*:\1:p')
        arg_type=$(get_type "${arg}")
        arg_conv=$(get_conversion "${arg_type}" "${arg_count}")
        arg_list="${arg_list}${arg_name} = ${arg_type}, "
        arg_convs="${arg_convs}${arg_conv}, "
        (( arg_count++ ))
    done

    # Remove extra ', '
    [[ "${arg_count}" -gt 0 ]] && arg_list="${arg_list::-2}"

    # Print out the C code to display system call information. You will probably
    # need to modify this so it can be used to conditionally print out the
    # appropriate info.
    echo -e "      case SYS_${i}:"
echo -n "        printf(\"[%d|%s] ${i}(${arg_list}) = $(get_type "${return_value}")\\n\", " 
    echo "p->pid, p->name, ${arg_convs} ret_val);" # Log the syscall details
    echo "        break;"
#incrementing a
done

# Closing that comment


cat <<EOM
    }
}
EOM
