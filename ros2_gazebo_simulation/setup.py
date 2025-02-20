from setuptools import find_packages, setup
from glob import glob

package_name = 'ros2_gazebo_simulation'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (f"share/{package_name}/launch", glob("launch/*.py")),
        (f"share/{package_name}/rviz", glob("rviz/*.rviz")),
        (f"share/{package_name}/urdf", glob("urdf/*.xacro")),
        (f"share/{package_name}/urdf/mech", glob("urdf/mech/*.xacro")),
        (f"share/{package_name}/urdf/controllers", glob("urdf/controllers/*.xacro")),
        (f"share/{package_name}/urdf/robots", glob("urdf/robots/*.xacro")),
        (f"share/{package_name}/urdf/sensors", glob("urdf/sensors/*.xacro")),
        (f"share/{package_name}/worlds", glob("worlds/*.world")),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='jemish',
    maintainer_email='jemishghoghari50@gmail.com',
    description='ROS2 Gazebo Simulation',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
        ],
    },
)
