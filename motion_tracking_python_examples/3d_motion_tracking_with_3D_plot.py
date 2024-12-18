import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Function to compute optical flow for a single point (same as before)
def compute_optical_flow(x, y, Z, v_x, v_y, v_z, omega_x, omega_y, omega_z):
    u_x = (-v_x + x * v_z) / Z
    u_y = (-v_y + y * v_z) / Z
    u_x_rot = -omega_z * y - omega_y * 0
    u_y_rot = omega_z * x + omega_x * 0
    return np.array([u_x + u_x_rot, u_y + u_y_rot])

# Function to compute optical flow for all points in the car's bounding box (same as before)
def compute_optical_flow_car(x_min, y_min, x_max, y_max, Z, v_x, v_y, v_z, omega_x, omega_y, omega_z):
    x_vals, y_vals = np.meshgrid(np.linspace(x_min, x_max, 10), np.linspace(y_min, y_max, 10))
    points = np.vstack([x_vals.ravel(), y_vals.ravel()]).T
    flow_field = []
    
    for x, y in points:
        u = compute_optical_flow(x, y, Z, v_x, v_y, v_z, omega_x, omega_y, omega_z)
        flow_field.append(u)
    
    flow_field = np.array(flow_field).reshape((x_vals.shape[0], x_vals.shape[1], 2))
    return flow_field, x_vals, y_vals

# Function to simulate the motion and optical flow over time for the bounding box
def simulate_optical_flow(x_min, y_min, x_max, y_max, Z0, velocities, angular_velocities, time_interval, depth_change_rate):
    v_x, v_y, v_z, a_x, a_y, a_z = velocities
    omega_x, omega_y, omega_z = angular_velocities
    t0, t_ref = time_interval
    time_steps = np.linspace(t0, t_ref, 10)
    
    all_flow_fields = []
    all_x_vals = []
    all_y_vals = []
    trajectory = []
    all_vehicle_positions = []
    
    # Initially, the car and camera are at the same position (vehicle position)
    x_car, y_car, z_car = 0, 0, Z0  # Vehicle initial position in 3D space
    trajectory.append((x_car, y_car, z_car))
    Z = Z0
    
    for t in time_steps:
        # Vehicle movement (for simplicity, assuming linear motion)
        x_car += v_x * (time_steps[1] - time_steps[0])
        y_car += v_y * (time_steps[1] - time_steps[0])
        
        # Update velocities based on acceleration (assuming constant acceleration)
        v_x += a_x * (time_steps[1] - time_steps[0])
        v_y += a_y * (time_steps[1] - time_steps[0])
        v_z += a_z * (time_steps[1] - time_steps[0])
        
        # Update angular velocities (for simplicity, assuming constant angular velocity)
        omega_x += omega_x * (time_steps[1] - time_steps[0])
        omega_y += omega_y * (time_steps[1] - time_steps[0])
        omega_z += omega_z * (time_steps[1] - time_steps[0])

        trajectory.append((x_car, y_car, Z))  # Append updated car position (camera follows the car)
        all_vehicle_positions.append((x_car, y_car, Z))
        
        # Compute optical flow at this time step
        flow_field, x_vals, y_vals = compute_optical_flow_car(x_min, y_min, x_max, y_max, Z, v_x, v_y, v_z, omega_x, omega_y, omega_z)
        
        all_flow_fields.append(flow_field)
        all_x_vals.append(x_vals)
        all_y_vals.append(y_vals)
        
        # Update depth dynamically
        Z = max(1, Z - depth_change_rate * (time_steps[1] - time_steps[0]))
    
    return all_flow_fields, all_x_vals, all_y_vals, time_steps, trajectory, all_vehicle_positions

# Function to plot the 3D positions of the camera and vehicle (camera follows vehicle)
def plot_3d_trajectory(vehicle_positions, title="Vehicle and Camera Trajectory"):
    vehicle_positions = np.array(vehicle_positions)
    
    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection='3d')
    
    # Plot vehicle trajectory in 3D
    ax.plot(vehicle_positions[:, 0], vehicle_positions[:, 1], vehicle_positions[:, 2], label="Vehicle and Camera Trajectory", color='b')
    
    # Camera is at the same position as the vehicle
    ax.scatter(vehicle_positions[0][0], vehicle_positions[0][1], vehicle_positions[0][2], color='r', label="Initial Camera Position", s=100)
    
    ax.set_xlabel('X Coordinate')
    ax.set_ylabel('Y Coordinate')
    ax.set_zlabel('Z Coordinate')
    ax.set_title(title)
    ax.legend()
    plt.show()

# Parameters for simulation
x_min, y_min, x_max, y_max = 0.2, 0.3, 0.5, 0.6  # Bounding box of the car in the image
Z0 = 10.0  # Initial depth of the car (distance from the camera)
velocities = (0.5, 0.1, 2.0, 0.02, 0.01, 0.03)  # Linear velocities (v_x, v_y, v_z) with acceleration (a_x, a_y, a_z)
angular_velocities = (0.05, 0.02, 0.1)  # Angular velocities (omega_x, omega_y, omega_z)
depth_change_rate = 1.0  # Depth reduces over time (simulating forward motion)
time_interval = (0, 5)  # Time interval for simulation (start, end)

# Simulate optical flow for the car's bounding box over time
all_flow_fields, all_x_vals, all_y_vals, time_steps, trajectory, all_vehicle_positions = simulate_optical_flow(
    x_min, y_min, x_max, y_max, Z0, velocities, angular_velocities, time_interval, depth_change_rate
)

# Plot the optical flow at each time step
for i, flow_field in enumerate(all_flow_fields):
    plt.figure(figsize=(8, 6))
    plt.quiver(all_x_vals[i], all_y_vals[i], flow_field[:, :, 0], flow_field[:, :, 1], color='r')
    plt.title(f"Optical Flow at t = {time_steps[i]:.2f} seconds")
    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.grid(True)
    plt.show()

# Plot the 3D trajectory of the vehicle and camera
plot_3d_trajectory(all_vehicle_positions, title="Vehicle and Camera Trajectory in 3D")