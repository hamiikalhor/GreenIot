#!/usr/bin/env python3
"""
BLE Mesh Network Log Analyzer

Analyzes serial logs from BLE Mesh Gateway to provide insights into:
- Message delivery rates
- Network latency
- Hop counts
- Node performance
- Relay efficiency

Usage:
    python analyze_mesh_logs.py gateway_log.txt

Author: GreenIoT Vertical Farming Project
Date: November 4, 2025
"""

import re
import sys
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime
from collections import defaultdict

def parse_log(filename):
    """Parse BLE Mesh serial log file"""
    data = []
    relay_stats = defaultdict(int)
    errors = []
    
    print(f"📖 Parsing log file: {filename}")
    
    with open(filename, 'r') as f:
        for line_num, line in enumerate(f, 1):
            # Parse sensor data messages
            if 'Sensor Status received' in line:
                match = re.search(
                    r'from (0x\w+).*Hops: (\d+).*Delay: (\d+)ms.*'
                    r'Temperature: ([\d.]+).*Humidity: ([\d.]+)',
                    line
                )
                if match:
                    timestamp = extract_timestamp(line)
                    data.append({
                        'timestamp': timestamp,
                        'line': line_num,
                        'node': match.group(1),
                        'hops': int(match.group(2)),
                        'delay_ms': int(match.group(3)),
                        'temperature': float(match.group(4)),
                        'humidity': float(match.group(5))
                    })
            
            # Parse relay statistics
            if 'Relaying message' in line:
                match = re.search(r'from (0x\w+)', line)
                if match:
                    relay_stats[match.group(1)] += 1
            
            # Parse errors
            if 'ERROR' in line or 'FAILED' in line:
                errors.append({
                    'line': line_num,
                    'message': line.strip()
                })
    
    df = pd.DataFrame(data)
    print(f"✅ Parsed {len(df)} sensor messages, {len(errors)} errors")
    
    return df, dict(relay_stats), errors

def extract_timestamp(line):
    """Extract timestamp from log line"""
    match = re.search(r'\[(\d{2}:\d{2}:\d{2})\]', line)
    if match:
        return match.group(1)
    return datetime.now().strftime('%H:%M:%S')

def analyze_delivery_rate(df):
    """Analyze message delivery rate"""
    print("\n" + "="*60)
    print("📊 MESSAGE DELIVERY ANALYSIS")
    print("="*60)
    
    # Expected messages (assuming 5-minute interval)
    # This should be configured based on your test setup
    nodes = df['node'].unique()
    
    for node in nodes:
        node_data = df[df['node'] == node]
        count = len(node_data)
        print(f"\n{node}:")
        print(f"  Messages received: {count}")
        
        if count > 1:
            # Calculate time span
            avg_interval = (node_data['delay_ms'].sum() / count) / 1000 / 60
            print(f"  Average interval: {avg_interval:.1f} minutes")

def analyze_latency(df):
    """Analyze network latency"""
    print("\n" + "="*60)
    print("⏱️  NETWORK LATENCY ANALYSIS")
    print("="*60)
    
    print(f"\n📈 Overall Statistics:")
    print(f"  Average delay: {df['delay_ms'].mean():.2f} ms")
    print(f"  Median delay:  {df['delay_ms'].median():.2f} ms")
    print(f"  Min delay:     {df['delay_ms'].min()} ms")
    print(f"  Max delay:     {df['delay_ms'].max()} ms")
    print(f"  Std deviation: {df['delay_ms'].std():.2f} ms")
    
    # Latency by hop count
    print(f"\n📊 Latency by Hop Count:")
    for hops in sorted(df['hops'].unique()):
        hop_data = df[df['hops'] == hops]
        print(f"  {hops} hop(s): {hop_data['delay_ms'].mean():.2f} ms "
              f"(n={len(hop_data)})")

def analyze_hop_distribution(df):
    """Analyze hop count distribution"""
    print("\n" + "="*60)
    print("🔗 HOP COUNT DISTRIBUTION")
    print("="*60)
    
    hop_counts = df['hops'].value_counts().sort_index()
    
    for hops, count in hop_counts.items():
        percentage = (count / len(df)) * 100
        bar = '█' * int(percentage / 2)
        print(f"  {hops} hop(s): {count:3d} messages ({percentage:5.1f}%) {bar}")
    
    print(f"\n  Average hops: {df['hops'].mean():.2f}")
    print(f"  Max hops:     {df['hops'].max()}")

def analyze_sensor_data(df):
    """Analyze sensor readings"""
    print("\n" + "="*60)
    print("🌡️  SENSOR DATA ANALYSIS")
    print("="*60)
    
    print(f"\n📊 Temperature:")
    print(f"  Average: {df['temperature'].mean():.2f}°C")
    print(f"  Min:     {df['temperature'].min():.2f}°C")
    print(f"  Max:     {df['temperature'].max():.2f}°C")
    print(f"  Range:   {df['temperature'].max() - df['temperature'].min():.2f}°C")
    
    print(f"\n💧 Humidity:")
    print(f"  Average: {df['humidity'].mean():.2f}%")
    print(f"  Min:     {df['humidity'].min():.2f}%")
    print(f"  Max:     {df['humidity'].max():.2f}%")
    print(f"  Range:   {df['humidity'].max() - df['humidity'].min():.2f}%")
    
    # Basil growth conditions
    temp_ok = df[(df['temperature'] >= 18) & (df['temperature'] <= 25)]
    hum_ok = df[(df['humidity'] >= 60) & (df['humidity'] <= 70)]
    both_ok = df[(df['temperature'] >= 18) & (df['temperature'] <= 25) &
                  (df['humidity'] >= 60) & (df['humidity'] <= 70)]
    
    print(f"\n🌿 Basil Growth Conditions:")
    print(f"  Temperature optimal: {len(temp_ok)/len(df)*100:.1f}% of time")
    print(f"  Humidity optimal:    {len(hum_ok)/len(df)*100:.1f}% of time")
    print(f"  Both optimal:        {len(both_ok)/len(df)*100:.1f}% of time")

def analyze_relay_efficiency(relay_stats):
    """Analyze relay node efficiency"""
    print("\n" + "="*60)
    print("🔄 RELAY NODE EFFICIENCY")
    print("="*60)
    
    if not relay_stats:
        print("  No relay statistics found")
        return
    
    total_relays = sum(relay_stats.values())
    
    print(f"\n  Total messages relayed: {total_relays}")
    print(f"\n  By node:")
    
    for node, count in sorted(relay_stats.items(), key=lambda x: x[1], reverse=True):
        percentage = (count / total_relays) * 100
        bar = '█' * int(percentage / 2)
        print(f"    {node}: {count:3d} messages ({percentage:5.1f}%) {bar}")

def show_errors(errors):
    """Display errors found in log"""
    print("\n" + "="*60)
    print("❌ ERRORS AND WARNINGS")
    print("="*60)
    
    if not errors:
        print("  ✅ No errors found!")
        return
    
    print(f"\n  Found {len(errors)} error(s):")
    for error in errors[:10]:  # Show first 10
        print(f"    Line {error['line']}: {error['message'][:80]}")
    
    if len(errors) > 10:
        print(f"    ... and {len(errors)-10} more")

def generate_plots(df, output_prefix='mesh_analysis'):
    """Generate visualization plots"""
    print("\n" + "="*60)
    print("📈 GENERATING PLOTS")
    print("="*60)
    
    try:
        import matplotlib
        matplotlib.use('Agg')  # Non-interactive backend
        
        # Plot 1: Latency over time
        plt.figure(figsize=(12, 6))
        plt.subplot(2, 2, 1)
        plt.plot(df['delay_ms'], marker='o', linestyle='-', markersize=3)
        plt.xlabel('Message #')
        plt.ylabel('Delay (ms)')
        plt.title('Network Latency Over Time')
        plt.grid(True, alpha=0.3)
        
        # Plot 2: Hop distribution
        plt.subplot(2, 2, 2)
        df['hops'].value_counts().sort_index().plot(kind='bar')
        plt.xlabel('Hop Count')
        plt.ylabel('Frequency')
        plt.title('Hop Count Distribution')
        plt.grid(True, alpha=0.3)
        
        # Plot 3: Temperature over time
        plt.subplot(2, 2, 3)
        plt.plot(df['temperature'], marker='o', linestyle='-', markersize=3)
        plt.axhline(y=18, color='r', linestyle='--', alpha=0.5, label='Min')
        plt.axhline(y=25, color='r', linestyle='--', alpha=0.5, label='Max')
        plt.xlabel('Message #')
        plt.ylabel('Temperature (°C)')
        plt.title('Temperature Readings')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # Plot 4: Humidity over time
        plt.subplot(2, 2, 4)
        plt.plot(df['humidity'], marker='o', linestyle='-', markersize=3)
        plt.axhline(y=60, color='b', linestyle='--', alpha=0.5, label='Min')
        plt.axhline(y=70, color='b', linestyle='--', alpha=0.5, label='Max')
        plt.xlabel('Message #')
        plt.ylabel('Humidity (%)')
        plt.title('Humidity Readings')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        output_file = f'{output_prefix}.png'
        plt.savefig(output_file, dpi=150)
        print(f"  ✅ Plot saved to: {output_file}")
        
    except Exception as e:
        print(f"  ⚠️  Could not generate plots: {e}")

def export_csv(df, filename='mesh_data.csv'):
    """Export data to CSV"""
    print("\n" + "="*60)
    print("💾 EXPORTING DATA")
    print("="*60)
    
    df.to_csv(filename, index=False)
    print(f"  ✅ Data exported to: {filename}")

def main():
    """Main analysis function"""
    if len(sys.argv) < 2:
        print("Usage: python analyze_mesh_logs.py <log_file>")
        print("\nExample: python analyze_mesh_logs.py gateway_log.txt")
        sys.exit(1)
    
    log_file = sys.argv[1]
    
    print("\n" + "="*60)
    print("🔍 BLE MESH NETWORK LOG ANALYZER")
    print("="*60)
    print(f"File: {log_file}")
    print(f"Date: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    
    # Parse log file
    df, relay_stats, errors = parse_log(log_file)
    
    if df.empty:
        print("\n❌ No sensor data found in log file!")
        print("Make sure the log contains 'Sensor Status received' messages.")
        sys.exit(1)
    
    # Run analyses
    analyze_delivery_rate(df)
    analyze_latency(df)
    analyze_hop_distribution(df)
    analyze_sensor_data(df)
    analyze_relay_efficiency(relay_stats)
    show_errors(errors)
    
    # Generate outputs
    generate_plots(df)
    export_csv(df)
    
    print("\n" + "="*60)
    print("✅ ANALYSIS COMPLETE")
    print("="*60)
    print(f"\nSummary:")
    print(f"  Total messages analyzed: {len(df)}")
    print(f"  Average latency: {df['delay_ms'].mean():.2f} ms")
    print(f"  Average hops: {df['hops'].mean():.2f}")
    print(f"  Errors found: {len(errors)}")
    print(f"\n📁 Outputs:")
    print(f"  - mesh_analysis.png (plots)")
    print(f"  - mesh_data.csv (raw data)")
    
if __name__ == '__main__':
    main()

