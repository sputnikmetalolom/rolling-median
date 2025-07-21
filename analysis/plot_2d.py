import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("build/benchmark_results.csv")

# Assume a single DataCount for a simple 2D plot
data_count = df["DataCount"].unique()[0]
df_filtered = df[df["DataCount"] == data_count]

# Pivot to compare Static vs Dynamic vs Reference
pivot = df_filtered.pivot(index="SIZE", columns="Mode", values="ElapsedSeconds").reset_index()

# Find the first SIZE where Dynamic is faster than Static
faster = pivot[pivot["Dynamic"] < pivot["Static"]]
if not faster.empty:
    first = faster.iloc[0]
    SIZE_faster = first["SIZE"]
    Static_time = first["Static"]
    Dynamic_time = first["Dynamic"]
    diff = Static_time - Dynamic_time
    print(
        f"First Dynamic faster: SIZE={SIZE_faster}, "
        f"Static={Static_time:.4f}, Dynamic={Dynamic_time:.4f}, Diff={diff:.4f}"
    )
else:
    print("No point found where Dynamic is faster than Static.")
    SIZE_faster = None

# Plot benchmark curves
plt.figure(figsize=(12, 6))
plt.plot(pivot["SIZE"], pivot["Static"], label="Static", color="blue")
plt.plot(pivot["SIZE"], pivot["Dynamic"], label="Dynamic", color="orange")
plt.plot(pivot["SIZE"], pivot["Reference"], label="Reference", color="green")

# Mark the first crossover point
if SIZE_faster is not None:
    plt.axvline(
        x=SIZE_faster,
        color="red",
        linestyle="--",
        label=f"First Dynamic Faster @ SIZE={SIZE_faster}"
    )

# Labeling and layout
plt.xlabel("SIZE (Window Length)")
plt.ylabel("Elapsed Time (s)")
plt.title(f"RollingMedian Benchmark — DataCount = {data_count}")
plt.legend()
plt.grid(True)
plt.tight_layout()

# Save output plot
output_path = "build/benchmark_2d_plot_first_faster.png"
plt.savefig(output_path, dpi=300)
print(f"Saved: {output_path}")
