import numpy as np
import matplotlib.pyplot as plt

def compute_optical_flow(x, y, Z, v_x, v_y, v_z, omega_z):
    """
    Computes the optical flow u(t) with angular and linear motion.
    
    Args:
        x, y (float): Normalized image coordinates.
        Z (float): Depth at current time step.
        v_x, v_y, v_z (float): Linear velocities in 3D space.
        omega_z (float): Angular velocity around the z-axis.

    Returns:
        u (np.array): Optical flow vector [u_x, u_y].
    """
    # Linear motion component
    u_x = (-v_x + x * v_z) / Z
    u_y = (-v_y + y * v_z) / Z

    # Rotational component (around z-axis)
    u_x_rot = -omega_z * y
    u_y_rot = omega_z * x

    # Total flow = linear + rotational components
    return np.array([u_x + u_x_rot, u_y + u_y_rot])

def simulate_multiple_points(points, Z0, velocities, omega_z, time_interval, depth_change_rate):
    """
    Simulates the optical flow for multiple 3D points over time.

    Args:
        points (list): List of initial [x, y] normalized coordinates for points.
        Z0 (float): Initial depth for all points.
        velocities (tuple): Linear velocities (v_x, v_y, v_z).
        omega_z (float): Angular velocity around z-axis.
        time_interval (tuple): Start and end times (t0, t_ref).
        depth_change_rate (float): Rate of depth change per second.

    Returns:
        trajectories (list): List of trajectories for all points.
    """
    v_x, v_y, v_z = velocities
    t0, t_ref = time_interval
    time_steps = np.linspace(t0, t_ref, 100)  # Discrete time steps

    trajectories = [[] for _ in points]  # Initialize trajectories for all points
    Z = Z0  # Initialize depth

    for t in time_steps:
        for i, (x, y) in enumerate(points):
            # Compute optical flow
            u = compute_optical_flow(x, y, Z, v_x, v_y, v_z, omega_z)
            
            # Update position using the optical flow
            x += u[0] * (time_steps[1] - time_steps[0])  # Δx = u_x * dt
            y += u[1] * (time_steps[1] - time_steps[0])  # Δy = u_y * dt
            
            # Store updated position
            trajectories[i].append((x, y))
            
            # Update the point's position for the next iteration
            points[i] = (x, y)

        # Update depth dynamically
        Z = max(1, Z - depth_change_rate * (time_steps[1] - time_steps[0]))

    return trajectories, time_steps

# Parameters
initial_points = [(0.1, 0.1), (-0.1, -0.1), (0.2, -0.2)]  # Initial normalized coordinates for multiple points
Z0 = 10.0                    # Initial depth
velocities = (-1.0, -0.5, 2.0)  # Linear velocities (v_x, v_y, v_z)
omega_z = 0.1                # Angular velocity around the z-axis
depth_change_rate = 1.0      # Depth reduces over time (simulating forward motion)
time_interval = (0, 10)       # Start and end times (seconds)

# Simulate optical flow for multiple points
trajectories, time_steps = simulate_multiple_points(initial_points, Z0, velocities, omega_z, time_interval, depth_change_rate)

# Plot the trajectories for all points
plt.figure(figsize=(10, 6))
for i, trajectory in enumerate(trajectories):
    trajectory = np.array(trajectory)
    plt.plot(trajectory[:, 0], trajectory[:, 1], label=f"Point {i+1} Trajectory")
    plt.scatter(initial_points[i][0], initial_points[i][1], label=f"Start Point {i+1}", s=50)

plt.xlabel("x (Normalized Coordinate)")
plt.ylabel("y (Normalized Coordinate)")
plt.title("Time-Variant Optical Flow Model: Multiple Points with Dynamic Depth and Rotation")
plt.legend()
plt.grid()
plt.show()