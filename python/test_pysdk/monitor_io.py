#!/usr/bin/env python3
"""
I/O Monitoring Script for Database Operations

This script monitors I/O pressure during database operations to help identify
bottlenecks and optimize performance.
"""

import os
import time
import psutil
import subprocess
import threading
from datetime import datetime
import json

class IOMonitor:
    def __init__(self, log_file="io_monitor.log"):
        self.log_file = log_file
        self.monitoring = False
        self.monitor_thread = None
        self.stats = []
        
    def start_monitoring(self):
        """Start I/O monitoring in background thread"""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        print(f"I/O monitoring started. Logging to {self.log_file}")
        
    def stop_monitoring(self):
        """Stop I/O monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join()
        print("I/O monitoring stopped.")
        
    def _monitor_loop(self):
        """Main monitoring loop"""
        while self.monitoring:
            try:
                stats = self._collect_io_stats()
                self.stats.append(stats)
                
                # Log to file
                with open(self.log_file, 'a') as f:
                    f.write(f"{stats['timestamp']},{stats['cpu_percent']},{stats['memory_percent']},{stats['disk_io_read']},{stats['disk_io_write']},{stats['disk_usage_percent']}\n")
                
                time.sleep(1)  # Sample every second
                
            except Exception as e:
                print(f"Error in monitoring: {e}")
                time.sleep(1)
    
    def _collect_io_stats(self):
        """Collect current I/O statistics"""
        try:
            # CPU and Memory
            cpu_percent = psutil.cpu_percent(interval=0.1)
            memory = psutil.virtual_memory()
            memory_percent = memory.percent
            
            # Disk I/O
            disk_io = psutil.disk_io_counters()
            disk_io_read = disk_io.read_bytes if disk_io else 0
            disk_io_write = disk_io.write_bytes if disk_io else 0
            
            # Disk usage
            disk_usage = psutil.disk_usage('/')
            disk_usage_percent = disk_usage.percent
            
            return {
                'timestamp': datetime.now().isoformat(),
                'cpu_percent': cpu_percent,
                'memory_percent': memory_percent,
                'disk_io_read': disk_io_read,
                'disk_io_write': disk_io_write,
                'disk_usage_percent': disk_usage_percent
            }
        except Exception as e:
            print(f"Error collecting stats: {e}")
            return {
                'timestamp': datetime.now().isoformat(),
                'cpu_percent': 0,
                'memory_percent': 0,
                'disk_io_read': 0,
                'disk_io_write': 0,
                'disk_usage_percent': 0
            }
    
    def get_current_stats(self):
        """Get current I/O statistics"""
        return self._collect_io_stats()
    
    def print_summary(self):
        """Print monitoring summary"""
        if not self.stats:
            print("No monitoring data available")
            return
            
        print("\n=== I/O Monitoring Summary ===")
        print(f"Duration: {len(self.stats)} seconds")
        
        # Calculate averages
        cpu_avg = sum(s['cpu_percent'] for s in self.stats) / len(self.stats)
        memory_avg = sum(s['memory_percent'] for s in self.stats) / len(self.stats)
        disk_usage_avg = sum(s['disk_usage_percent'] for s in self.stats) / len(self.stats)
        
        # Calculate I/O totals
        total_read = self.stats[-1]['disk_io_read'] - self.stats[0]['disk_io_read']
        total_write = self.stats[-1]['disk_io_write'] - self.stats[0]['disk_io_write']
        
        print(f"Average CPU: {cpu_avg:.1f}%")
        print(f"Average Memory: {memory_avg:.1f}%")
        print(f"Average Disk Usage: {disk_usage_avg:.1f}%")
        print(f"Total Disk Read: {total_read / (1024*1024):.2f} MB")
        print(f"Total Disk Write: {total_write / (1024*1024):.2f} MB")
        
        # Find peak values
        peak_cpu = max(s['cpu_percent'] for s in self.stats)
        peak_memory = max(s['memory_percent'] for s in self.stats)
        
        print(f"Peak CPU: {peak_cpu:.1f}%")
        print(f"Peak Memory: {peak_memory:.1f}%")

def monitor_database_operation(operation_func, operation_name="Database Operation"):
    """
    Monitor I/O during a database operation
    
    Args:
        operation_func: Function to execute and monitor
        operation_name: Name of the operation for logging
    """
    monitor = IOMonitor(f"io_monitor_{operation_name.lower().replace(' ', '_')}.log")
    
    print(f"Starting I/O monitoring for: {operation_name}")
    monitor.start_monitoring()
    
    try:
        # Execute the operation
        start_time = time.time()
        result = operation_func()
        end_time = time.time()
        
        print(f"Operation completed in {end_time - start_time:.2f} seconds")
        
    finally:
        monitor.stop_monitoring()
        monitor.print_summary()
    
    return result

def get_system_info():
    """Get basic system information"""
    print("=== System Information ===")
    
    # CPU info
    cpu_count = psutil.cpu_count()
    cpu_freq = psutil.cpu_freq()
    print(f"CPU Cores: {cpu_count}")
    if cpu_freq:
        print(f"CPU Frequency: {cpu_freq.current:.0f} MHz")
    
    # Memory info
    memory = psutil.virtual_memory()
    print(f"Total Memory: {memory.total / (1024**3):.1f} GB")
    print(f"Available Memory: {memory.available / (1024**3):.1f} GB")
    
    # Disk info
    disk = psutil.disk_usage('/')
    print(f"Total Disk: {disk.total / (1024**3):.1f} GB")
    print(f"Free Disk: {disk.free / (1024**3):.1f} GB")
    print(f"Disk Usage: {disk.percent:.1f}%")

def check_io_bottlenecks():
    """Check for common I/O bottlenecks"""
    print("\n=== I/O Bottleneck Check ===")
    
    # Check disk I/O
    try:
        disk_io = psutil.disk_io_counters()
        if disk_io:
            print(f"Disk I/O - Read: {disk_io.read_bytes / (1024**2):.1f} MB, Write: {disk_io.write_bytes / (1024**2):.1f} MB")
    except:
        print("Could not get disk I/O stats")
    
    # Check memory pressure
    memory = psutil.virtual_memory()
    if memory.percent > 80:
        print(f"WARNING: High memory usage: {memory.percent:.1f}%")
    
    # Check disk space
    disk = psutil.disk_usage('/')
    if disk.percent > 90:
        print(f"WARNING: Low disk space: {disk.percent:.1f}% used")
    
    # Check CPU load
    cpu_percent = psutil.cpu_percent(interval=1)
    if cpu_percent > 80:
        print(f"WARNING: High CPU usage: {cpu_percent:.1f}%")

if __name__ == "__main__":
    # Example usage
    print("I/O Monitoring Tool")
    print("==================")
    
    get_system_info()
    check_io_bottlenecks()
    
    # Example: Monitor a simple operation
    def example_operation():
        print("Running example operation...")
        time.sleep(5)  # Simulate work
        print("Example operation completed")
    
    monitor_database_operation(example_operation, "Example Operation") 