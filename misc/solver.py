#!/usr/bin/env python3

from multiprocessing import cpu_count
from pathlib import Path
from re import findall
from subprocess import Popen, PIPE

# Group input file ID regex
group_regex = r"^input_group(\d+)\.txt$"

# Path to the project root directory
root_dir = Path(__file__).parent.parent

# Resolve paths
solver_file = root_dir.joinpath("build/solver").resolve()
verifier_file = root_dir.joinpath("build/verifier").resolve()

inputs_folder = root_dir.joinpath("data/inputs")
outputs_folder = root_dir.joinpath("data/outputs")

# Ensure the binaries exist
if not solver_file.is_file():
    print(
        f"Solver binary {solver_file} does not exist (You need to build the project first)"
    )
    exit(1)

if not verifier_file.is_file():
    print(
        f"Verifier binary {verifier_file} does not exist (You need to build the project first)"
    )
    exit(1)

# Ensure directories exist
inputs_folder.mkdir(exist_ok=True, parents=True)
outputs_folder.mkdir(exist_ok=True, parents=True)

# Get the number of available CPU cores
cpu_cores = cpu_count()

# Initialize groups
groups = []
for input_file in inputs_folder.iterdir():
    # Skip if not a file or not a valid input file
    if not input_file.is_file() or not input_file.name.endswith(".txt"):
        continue

    # Extract the group ID from the input file name
    match = findall(group_regex, input_file.name)

    # Skip if the input file name does not match the group ID regex
    if not match:
        continue

    # Compute path
    output_file = outputs_folder.joinpath(f"output_group{match[0]}.txt")

    # Add the group ID to the set
    groups.append(
        {
            "id": match[0],
            "input_file": input_file.resolve(),
            "output_file": output_file.resolve(),
        }
    )

# Iterate over batches
for group_index in range(0, len(groups), cpu_cores):
    # Get the current batch
    batch = groups[group_index : group_index + cpu_cores]

    # Solve inputs
    for group in batch:
        # Extract the group information
        group_id = group["id"]
        input_file = group["input_file"]
        output_file = group["output_file"]

        # Ensure the output file does not exist
        if output_file.is_file():
            print(f"Output file {output_file} already exists")
            continue

        # Spawn the solver process
        process = Popen([solver_file, input_file, output_file], stdout=PIPE, stderr=PIPE)

        # Update the group
        group["solver_process"] = process

    # Wait for all tasks to finish and check their exit codes
    for group in batch:
        # Extract the group information
        group_id = group["id"]
        process = group["solver_process"]
        output_file = group["output_file"]

        # Wait for the process to finish
        stdout, stderr = process.communicate()

        # Check if the process exited with a non-zero exit code
        if process.returncode != 0:
            # Print an error message
            print(
                f"Solver process for group {group_id} exited with non-zero exit code {process.returncode}"
            )

            # Save the stdout and stderr
            output_file.parent.joinpath(
                f"stdout-{group_id}.txt"
            ).write_text(stdout.decode("utf-8"))
            output_file.parent.joinpath(
                f"stderr-{group_id}.txt"
            ).write_text(stderr.decode("utf-8"))

            # Update the group
            group["solved"] = False
        else:
            # Update the group
            group["solved"] = True

    # Verify outputs
    for group in batch:
        # Extract the group information
        group_id = group["id"]
        input_file = group["input_file"]
        output_file = group["output_file"]
        solved = group["solved"]

        # Skip if not solved
        if not solved:
            # Update the group
            group["verified"] = False

            continue

        # Spawn the verifier process
        process = Popen([verifier_file, input_file, output_file], stdout=PIPE, stderr=PIPE)

        # Update the group
        group["verifier_process"] = process

    # Wait for all processes to finish and check their exit codes
    for group in batch:
        # Extract the group information
        group_id = group["id"]
        process = group["verifier_process"]
        input_file = group["input_file"]
        output_file = group["output_file"]

        # Wait for the process to finish
        stdout, stderr = process.communicate()

        # Check if the process exited with a non-zero exit code
        if process.returncode != 0:
            # Print an error message
            print(
                f"Verifier process {process.pid} for group {group_id} exited with non-zero exit code {process.returncode}"
            )

            # Rename the output file to indicate that it is invalid
            output_file.rename(output_file.with_suffix(".invalid.txt"))

            # Save the stdout and stderr
            output_file.parent.joinpath(
                f"verifier_group{group_id}.txt.stdout"
            ).write_text(stdout.decode("utf-8"))
            output_file.parent.joinpath(
                f"verifier_group{group_id}.txt.stderr"
            ).write_text(stderr.decode("utf-8"))

            # Update the group
            group["verified"] = False
        else:
            # Update the group
            group["verified"] = True

    # Print upload instructions
    for group in batch:
        # Extract the group information
        group_id = group["id"]
        verified = group["verified"]

        if not verified:
            print(f"Output file for group {group_id} is invalid")

    # Print the progress
    print(f"Processed {len(batch)} groups out of {len(groups)} ({100 * group_index / len(groups):.2f}%)")
