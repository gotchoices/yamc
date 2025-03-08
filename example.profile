# Example YAMC profile for machine configuration
# This profile sets up basic configuration for a Linux server
# 
# Format: Each line is a YAMC command without the hostname
# Lines starting with # are comments and are ignored

# First, set up SSH access for root
sshroot

# Set timezone and locale as root user
-u root timezone America/Los_Angeles

# User preferences
pref

# Run a test to verify everything is working
test

# Another test showing argument passing
test args arg1 arg2 "argument with spaces"

# Test with environment variables
-e test_var1=hello -e test_var2=world test env