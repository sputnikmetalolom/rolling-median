import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
import numpy as np
import matplotlib.cm as cm
import matplotlib.colors as mcolors

# Load benchmark results
df = pd.read_csv("build/benchmark_results.csv")

# Pivot each mode separately
pivot_static = df[df["Mode"] == "Static"].pivot_table(
    index="DataCount", columns="SIZE", values="ElapsedSeconds"
)
pivot_dynamic = df[df["Mode"] == "Dynamic"].pivot_table(
    index="DataCount", columns="SIZE", values="ElapsedSeconds"
)
pivot_reference = df[df["Mode"] == "Reference"].pivot_table(
    index="DataCount", columns="SIZE", values="ElapsedSeconds"
)

# Create mesh grid
X, Y = np.meshgrid(pivot_static.columns, pivot_static.index)
Z_static = pivot_static.values
Z_dynamic = pivot_dynamic.values
Z_reference = pivot_reference.values

# Set up 3D figure
fig = plt.figure(figsize=(14, 10))
ax = fig.add_subplot(111, projection="3d")

# Normalize all Z values for consistent coloring
Z_all = np.concatenate([
    Z_static.flatten(),
    Z_dynamic.flatten(),
    Z_reference.flatten()
])
norm = mcolors.Normalize(vmin=np.min(Z_all), vmax=np.max(Z_all))

# Colormaps
cmap_static = cm.Blues
cmap_dynamic = cm.Oranges
cmap_reference = cm.Greens

# Plot surfaces
surf_static = ax.plot_surface(
    X, Y, Z_static,
    facecolors=cmap_static(norm(Z_static)),
    alpha=0.7, edgecolor="k"
)
surf_dynamic = ax.plot_surface(
    X, Y, Z_dynamic,
    facecolors=cmap_dynamic(norm(Z_dynamic)),
    alpha=0.7, edgecolor="k"
)
surf_reference = ax.plot_surface(
    X, Y, Z_reference,
    facecolors=cmap_reference(norm(Z_reference)),
    alpha=0.7, edgecolor="k"
)

# Axis labels and title
ax.set_xlabel("SIZE (Window Length)")
ax.set_ylabel("DataCount")
ax.set_zlabel("Elapsed Time (s)")
ax.set_title("RollingMedian: Static vs Dynamic vs Reference")

# Add colorbars for each surface
fig.colorbar(
    cm.ScalarMappable(cmap=cmap_static, norm=norm),
    ax=ax, shrink=0.5, aspect=5, label="Static"
)
fig.colorbar(
    cm.ScalarMappable(cmap=cmap_dynamic, norm=norm),
    ax=ax, shrink=0.5, aspect=5, label="Dynamic"
)
fig.colorbar(
    cm.ScalarMappable(cmap=cmap_reference, norm=norm),
    ax=ax, shrink=0.5, aspect=5, label="Reference"
)

# Finalize layout and save
plt.tight_layout()
plt.savefig("build/benchmark_surface_compare_3modes.png", dpi=300)
print("Saved: build/benchmark_surface_compare_3modes.png")
