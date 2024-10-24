import subprocess
import platform

# Check if the operating system is Linux
if platform.system() != "Linux":
    print("Network namespaces are only supported on Linux.")
    exit()

NS1 = "ns1"
NS2 = "ns2"
NS3 = "ns3"
BRIDGE = "br0"

IP_NS1 = "17.0.0.1/24"
IP_NS2 = "17.0.0.2/24"
IP_NS3 = "17.0.0.3/24"

VETH1 = "veth-ns1"
VETH1_BR = "veth-ns1-br"
VETH2 = "veth-ns2"
VETH2_BR = "veth-ns2-br"
VETH3 = "veth-ns3"
VETH3_BR = "veth-ns3-br"

# Delete existing namespaces if they exist
subprocess.run(f"sudo ip netns delete {NS1}".split())
subprocess.run(f"sudo ip netns delete {NS2}".split())
subprocess.run(f"sudo ip netns delete {NS3}".split())

# Delete existing bridge if it exists
subprocess.run(f"sudo ip link set {BRIDGE} down".split())
subprocess.run(f"sudo brctl delbr {BRIDGE}".split())

# Create network namespaces
subprocess.run(f"sudo ip netns add {NS1}".split(), check=True)
subprocess.run(f"sudo ip netns add {NS2}".split(), check=True)
subprocess.run(f"sudo ip netns add {NS3}".split(), check=True)

# Create bridge
subprocess.run(f"sudo brctl addbr {BRIDGE}".split(), check=True)

# Create veth and connect
subprocess.run(
    f"sudo ip link add {VETH1} type veth peer name {VETH1_BR}".split(), check=True
)
subprocess.run(f"sudo ip link set {VETH1} netns {NS1}".split(), check=True)
subprocess.run(f"sudo brctl addif {BRIDGE} {VETH1_BR}".split(), check=True)

subprocess.run(
    f"sudo ip link add {VETH2} type veth peer name {VETH2_BR}".split(), check=True
)
subprocess.run(f"sudo ip link set {VETH2} netns {NS2}".split(), check=True)
subprocess.run(f"sudo brctl addif {BRIDGE} {VETH2_BR}".split(), check=True)

subprocess.run(
    f"sudo ip link add {VETH3} type veth peer name {VETH3_BR}".split(), check=True
)
subprocess.run(f"sudo ip link set {VETH3} netns {NS3}".split(), check=True)
subprocess.run(f"sudo brctl addif {BRIDGE} {VETH3_BR}".split(), check=True)

# Assign ip for each namespace
subprocess.run(
    f"sudo ip netns exec {NS1} ip addr add {IP_NS1} dev {VETH1}".split(), check=True
)
subprocess.run(
    f"sudo ip netns exec {NS2} ip addr add {IP_NS2} dev {VETH2}".split(), check=True
)
subprocess.run(
    f"sudo ip netns exec {NS3} ip addr add {IP_NS3} dev {VETH3}".split(), check=True
)

# Set up the interfaces
subprocess.run(f"sudo ip link set {BRIDGE} up".split(), check=True)
subprocess.run(f"sudo ip link set {VETH1_BR} up".split(), check=True)
subprocess.run(f"sudo ip link set {VETH2_BR} up".split(), check=True)
subprocess.run(f"sudo ip link set {VETH3_BR} up".split(), check=True)
subprocess.run(f"sudo ip netns exec {NS1} ip link set {VETH1} up".split(), check=True)
subprocess.run(f"sudo ip netns exec {NS2} ip link set {VETH2} up".split(), check=True)
subprocess.run(f"sudo ip netns exec {NS3} ip link set {VETH3} up".split(), check=True)

# Ping each other
subprocess.run(
    f"sudo ip netns exec {NS1} ping -c 3 {IP_NS2.split('/')[0]}".split(), check=True
)
subprocess.run(
    f"sudo ip netns exec {NS2} ping -c 3 {IP_NS3.split('/')[0]}".split(), check=True
)
subprocess.run(
    f"sudo ip netns exec {NS3} ping -c 3 {IP_NS1.split('/')[0]}".split(), check=True
)
