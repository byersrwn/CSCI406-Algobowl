#!/usr/bin/env python3

from pathlib import Path
from re import findall
from subprocess import Popen, PIPE

# Group input file ID regex
group_regex = r"^output_from_(\d+)_to_(\d+)\.txt$"

# Path to the project root directory
root_dir = Path(__file__).parent.parent

# Resolve paths
verifier = root_dir.joinpath("build/verifier").resolve()

input_file = root_dir.joinpath("data/inputs/input_group728.txt")
outputs = root_dir.joinpath("data/outputs")

# Ensure the binaries exist
if not verifier.is_file():
    print(
        f"Verifier binary {verifier} does not exist (You need to build the project first)"
    )
    exit(1)

# Ensure directories exist
outputs.mkdir(exist_ok=True)

# Initialize groups
groups = []
for output_file in outputs.iterdir():
    # Skip if not a file or not a valid input file
    if not output_file.is_file() or not output_file.name.endswith(".txt"):
        continue

    # Extract the group ID from the input file name
    match = findall(group_regex, output_file.name)

    # Skip if the input file name does not match the group ID regex
    if not match:
        continue

    # Add the group ID to the set
    groups.append(
        {
            "id": match[0][0],
            "output_file": output_file.resolve(),
        }
    )

# Iterate over batches
for group_index in range(len(groups)):
    # Extract the group information
    group = groups[group_index]
    group_id = group["id"]
    output_file = group["output_file"]

    # Spawn the verifier process
    process = Popen([verifier, input_file, output_file], stdout=PIPE, stderr=PIPE)

    # Wait for the process to finish
    stdout, stderr = process.communicate()

    # Check if the process exited with a non-zero exit code
    if process.returncode != 0:
        # Print an error message
        print(
            f"Verifier process {process.pid} for group {group_id} exited with non-zero exit code {process.returncode}"
        )

        # Save the stdout and stderr
        output_file.parent.joinpath(
            f"stdout-{group_id}.txt"
        ).write_text(stdout.decode("utf-8"))
        output_file.parent.joinpath(
            f"stderr-{group_id}.txt"
        ).write_text(stderr.decode("utf-8"))

        print(f"Output file for group {group_id} is invalid")
