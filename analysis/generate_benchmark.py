import os

# Configuration
sizes = list(range(40, 120))
counts = [1_000, 1_000_000, 10_000_000]
modes = ["Static", "Dynamic", "Reference"]

lines = []

# Header: comment for generated vectors
lines.append("// Generated vectors")
for count in counts:
    count_million = count // 1_000_000 or (count // 1_000) // 1000  # handle 1_000 => 0
    label = f"{count_million}M" if count >= 1_000_000 else f"{count // 1_000}K"
    lines.append(f"auto numbers{label} = make_numbers<data_type>({count});")

# Section: benchmark calls
lines.append("\n// Benchmark calls")
for size in sizes:
    for count in counts:
        count_million = count // 1_000_000 or (count // 1_000) // 1000
        label = f"{count_million}M" if count >= 1_000_000 else f"{count // 1_000}K"
        numbers_var = f"numbers{label}"
        for mode in modes:
            lines.append(f"run_benchmark<{size}, kallkod::Mode::{mode}>({numbers_var}, out);")

# Write to file
output_file = "benchmark_calls.inc"
with open(output_file, "w") as f:
    f.write("\n".join(lines) + "\n")

# Summary
print("Done! Generated:")
print(f" - {len(counts)} numbers vectors")
print(f" - {len(sizes) * len(counts) * len(modes)} benchmark calls")
print(f"Use this in main.cpp: #include \"{output_file}\"")
