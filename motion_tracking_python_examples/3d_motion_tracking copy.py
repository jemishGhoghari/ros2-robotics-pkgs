import numpy as np
import matplotlib.pyplot as plt

# Function to compute optical flow for a single point
def compute_optical_flow(x, y, Z, v_x, v_y, v_z, omega_x, omega_y, omega_z):
    """
    Computes the optical flow vector for a single point considering rotation around all axes.
    
    Args:
        x, y (float): Normalized coordinates of the point.
        Z (float): Depth at the point.
        v_x, v_y, v_z (float): Linear velocities.
        omega_x, omega_y, omega_z (float): Angular velocities around x, y, z axes.
        
    Returns:
        u (np.array): Optical flow vector [u_x, u_y].
    """
    # Linear motion component
    u_x = (-v_x + x * v_z) / Z
    u_y = (-v_y + y * v_z) / Z
    
    # Rotational motion components for pitch (omega_x), roll (omega_y), and yaw (omega_z)
    u_x_rot = -omega_z * y - omega_y * 0  # Considering rotation around the z-axis and y-axis
    u_y_rot = omega_z * x + omega_x * 0   # Considering rotation around the z-axis and x-axis

    # Total flow = linear + rotational components
    return np.array([u_x + u_x_rot, u_y + u_y_rot])

# Function to compute optical flow for all points in the car's bounding box
def compute_optical_flow_car(x_min, y_min, x_max, y_max, Z, v_x, v_y, v_z, omega_x, omega_y, omega_z):
    """
    Compute the optical flow for a car's bounding box.
    
    Args:
        x_min, y_min, x_max, y_max (float): Coordinates of the bounding box.
        Z (float): Depth at the car.
        v_x, v_y, v_z (float): Linear velocities in 3D space.
        omega_x, omega_y, omega_z (float): Angular velocities in 3D space (rotation).
        
    Returns:
        flow_field (np.array): A grid of optical flow vectors inside the bounding box.
        x_vals, y_vals (np.array): Coordinates grid for the bounding box.
    """
    # Generate a grid of points inside the bounding box
    x_vals, y_vals = np.meshgrid(np.linspace(x_min, x_max, 10), np.linspace(y_min, y_max, 10))
    points = np.vstack([x_vals.ravel(), y_vals.ravel()]).T  # List of points inside bounding box
    
    flow_field = []
    
    for x, y in points:
        # Compute optical flow for each point
        u = compute_optical_flow(x, y, Z, v_x, v_y, v_z, omega_x, omega_y, omega_z)
        flow_field.append(u)
    
    # Convert to a numpy array for easier manipulation
    flow_field = np.array(flow_field).reshape((x_vals.shape[0], x_vals.shape[1], 2))
    
    return flow_field, x_vals, y_vals

# Function to simulate the motion and optical flow over time for the bounding box
def simulate_optical_flow(x_min, y_min, x_max, y_max, Z0, velocities, angular_velocities, time_interval, depth_change_rate):
    """
    Simulates optical flow for the car's bounding box over time with acceleration and 3D rotation.
    
    Args:
        x_min, y_min, x_max, y_max (float): Initial bounding box coordinates.
        Z0 (float): Initial depth at the car.
        velocities (tuple): Linear velocities (v_x, v_y, v_z) with acceleration.
        angular_velocities (tuple): Angular velocities (omega_x, omega_y, omega_z).
        time_interval (tuple): Start and end times (t0, t_ref).
        depth_change_rate (float): Rate of depth change over time.
        
    Returns:
        all_flow_fields (list): List of optical flow fields for each time step.
        all_x_vals, all_y_vals (np.array): Bounding box coordinates over time.
        trajectory (list): List of vehicle's position (x, y) over time.
    """
    v_x, v_y, v_z, a_x, a_y, a_z = velocities  # including acceleration components
    omega_x, omega_y, omega_z = angular_velocities  # angular velocities for 3D rotation
    
    t0, t_ref = time_interval
    time_steps = np.linspace(t0, t_ref, 10)  # Discrete time steps
    
    all_flow_fields = []
    all_x_vals = []
    all_y_vals = []
    trajectory = []

    # Initial position of the car's bounding box (for simplicity, we take the center of the bounding box as the car's position)
    x_car = (x_min + x_max) / 2
    y_car = (y_min + y_max) / 2
    trajectory.append((x_car, y_car))  # Initial trajectory point

    Z = Z0  # Initial depth

    for t in time_steps:
        # Update position using linear velocity (we only update the center of the bounding box as the car's position)
        x_car += v_x * (time_steps[1] - time_steps[0])  # Δx = v_x * dt
        y_car += v_y * (time_steps[1] - time_steps[0])  # Δy = v_y * dt

        # Update linear velocities with acceleration
        v_x += a_x * (time_steps[1] - time_steps[0])
        v_y += a_y * (time_steps[1] - time_steps[0])
        v_z += a_z * (time_steps[1] - time_steps[0])
        
        # Update angular velocities (can include rotations in pitch, roll, and yaw)
        omega_x += omega_x * (time_steps[1] - time_steps[0])
        omega_y += omega_y * (time_steps[1] - time_steps[0])
        omega_z += omega_z * (time_steps[1] - time_steps[0])

        # Store the updated car's position
        trajectory.append((x_car, y_car))
        
        # Compute optical flow at the current depth and position
        flow_field, x_vals, y_vals = compute_optical_flow_car(x_min, y_min, x_max, y_max, Z, v_x, v_y, v_z, omega_x, omega_y, omega_z)
        
        # Store the flow field and coordinates
        all_flow_fields.append(flow_field)
        all_x_vals.append(x_vals)
        all_y_vals.append(y_vals)
        
        # Update the depth
        Z = max(1, Z - depth_change_rate * (time_steps[1] - time_steps[0]))  # Ensure depth does not go below 1

    return all_flow_fields, all_x_vals, all_y_vals, time_steps, trajectory

# Function to plot the optical flow for each time step
def plot_optical_flow(x_vals, y_vals, flow_field, title="Optical Flow in Bounding Box"):
    """
    Plot the optical flow field inside the bounding box.
    
    Args:
        x_vals, y_vals (np.array): Grid of bounding box coordinates.
        flow_field (np.array): Optical flow vectors for each point in the grid.
        title (str): Plot title.
    """
    plt.quiver(x_vals, y_vals, flow_field[:, :, 0], flow_field[:, :, 1], color='r')
    plt.title(title)
    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.grid(True)
    plt.show()

# Function to plot the trajectory of the vehicle
def plot_vehicle_trajectory(trajectory, title="Vehicle Trajectory"):
    """
    Plot the trajectory of the vehicle over time.
    
    Args:
        trajectory (list): List of vehicle positions (x, y) over time.
        title (str): Plot title.
    """
    trajectory = np.array(trajectory)
    plt.plot(trajectory[:, 0], trajectory[:, 1], label="Vehicle Trajectory", color='b', marker='o')
    plt.scatter(trajectory[0, 0], trajectory[0, 1], color='g', label="Start Point", zorder=5)
    plt.title(title)
    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.legend()
    plt.grid(True)
    plt.show()

# Parameters for simulation
x_min, y_min, x_max, y_max = 0.2, 0.3, 0.5, 0.6  # Bounding box of the car in the image
Z0 = 10.0  # Initial depth of the car (distance from the camera)
velocities = (0.5, 0.1, 2.0, 0.02, 0.01, 0.03)  # Linear velocities (v_x, v_y, v_z) with acceleration (a_x, a_y, a_z)
angular_velocities = (0.05, 0.02, 0.1)  # Angular velocities (omega_x, omega_y, omega_z)
depth_change_rate = 1.0  # Depth reduces over time (simulating forward motion)
time_interval = (0, 60)  # Time interval for simulation (start, end)

# Simulate optical flow for the car's bounding box over time
all_flow_fields, all_x_vals, all_y_vals, time_steps, trajectory = simulate_optical_flow(
    x_min, y_min, x_max, y_max, Z0, velocities, angular_velocities, time_interval, depth_change_rate
)

# Plot the optical flow at each time step
for i, flow_field in enumerate(all_flow_fields):
    plot_optical_flow(all_x_vals[i], all_y_vals[i], flow_field, title=f"Optical Flow at t = {time_steps[i]:.2f} seconds")

# Plot the vehicle trajectory over time
plot_vehicle_trajectory(trajectory, title="Vehicle Trajectory Over Time")