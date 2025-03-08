# YAMC Test Module

This module provides various test functions to verify that YAMC is working correctly. The tests verify core functionality like file sharing, environment variable passing, and argument handling without making any permanent changes to your system.

## Available Tests

The test module includes three main test functions:

1. **Basic Test** (`setup`): 
   - Tests the basic functionality of YAMC
   - Verifies file sharing between local and remote hosts
   - Tests environment variable passing
   - Creates and displays files on both local and remote systems

2. **Arguments Test** (`args`):
   - Tests passing command-line arguments to module scripts
   - Verifies that arguments are properly received by both local and remote scripts
   - Reports how arguments are handled and passed between systems

3. **Environment Test** (`env`):
   - Tests passing environment variables via the `-e` flag
   - Displays all environment variables available to the scripts
   - Compares environment variables between local and remote execution
   - Useful for debugging environment-related issues

## Usage

Before running these tests, you must first initialize your target host:

```bash
# Initialize host (required once per host)
yamc -h your_remote_host -u username init
```

### Basic Functionality Test

```bash
yamc -h your_remote_host test
```

This runs the default `setup` script which tests basic YAMC functionality including file sharing and environment variable passing.

### Arguments Test

```bash
yamc -h your_remote_host test args arg1 arg2 "argument with spaces"
```

This tests passing arguments to module scripts. The arguments (`arg1`, `arg2`, etc.) are passed to both the local and remote scripts and the test reports how they were received.

### Environment Variable Test

```bash
yamc -h your_remote_host -e test_var1=hello -e test_var2="world" test env
```

This tests passing custom environment variables using the `-e` flag. The test will show how these variables are passed to the remote system.

### Verbose Mode

Add the `-v` flag to any test to see more detailed debug information:

```bash
yamc -h your_remote_host -v test
```

## Interactive Pauses

Each test includes an interactive pause where you can manually inspect the remote system. During this pause, you can:

1. SSH into the remote host in another terminal
2. Examine the mounted directories and files
3. Verify file permissions and contents
4. Press Enter in the original terminal to continue the test

## Expected Results

A successful test will:

1. Create files on both the local and remote systems
2. Display the contents of these files
3. Show environment variables and their values
4. Report "Test completed successfully!" at the end

If you encounter any errors during testing, check:
- SSH key authentication is working properly
- SSHFS is installed on the remote host
- The path structure and permissions are correct
- Network connectivity between hosts

## Cleaning Up

All temporary files and directories are automatically cleaned up when the test completes or if it's interrupted.