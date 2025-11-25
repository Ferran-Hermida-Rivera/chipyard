import numpy as np
import matplotlib.pyplot as plt
import re

# This script takes as input the following log file
# Outputs of the ubenchmarks with performance counters collected should be in this log
log_path = '/scratch/acui/chipyard-saws/sims/verilator/results.log'


# Here are how the variables are correlated to the log outputs:
# C_cycle -> Cycle
# corewidth -> 8
# C_flush -> Flush
# C_bm -> Branch Mispredict
# C_fence -> Fence Retired
# C_ret -> Uops Retired
# C_issued -> Uops Issued
# C_rec -> Recovery
# C_iblk -> I$ Blocked
# C_db -> D$ Blocked

def parse_log_file(log_path):
    """Parse the log file and extract performance counter data for multiple ubenchmarks."""
    with open(log_path, 'r') as f:
        content = f.read()
    
    # Split by Ubenchmark sections
    ubenchmarks = {}
    ubench_sections = re.split(r'Ubenchmark:\s+(\S+)', content)[1:]  # Skip first empty split
    
    for i in range(0, len(ubench_sections), 2):
        ubench_name = ubench_sections[i].strip()
        ubench_data = ubench_sections[i + 1] if i + 1 < len(ubench_sections) else ""
        
        # Split by KnobConfig sections within this ubenchmark
        knob_configs = []
        config_sections = re.split(r'KnobConfig:\s+(.+?)\n', ubench_data)[1:]  # Skip first empty split
        
        for j in range(0, len(config_sections), 2):
            config_name = config_sections[j].strip()
            config_data = config_sections[j + 1] if j + 1 < len(config_sections) else ""
            
            # Extract counter values
            try:
                counters = {}
                counters['Cycle'] = int(re.search(r'Cycle:\s+(\d+)', config_data).group(1))
                counters['Branch Mispredict'] = int(re.search(r'Branch Mispredict:\s+(\d+)', config_data).group(1))
                counters['Flush'] = int(re.search(r'Flush:\s+(\d+)', config_data).group(1))
                counters['Fence Retired'] = int(re.search(r'Fence Retired:\s+(\d+)', config_data).group(1))
                counters['Uops Retired'] = int(re.search(r'Uops Retired:\s+(\d+)', config_data).group(1))
                counters['Uops Issued'] = int(re.search(r'Uops Issued:\s+(\d+)', config_data).group(1))
                counters['Recovering'] = int(re.search(r'Recovering:\s+(\d+)', config_data).group(1))
                counters['I$ Blocked'] = int(re.search(r'I\$ Blocked:\s+(\d+)', config_data).group(1))
                counters['D$ blocked'] = int(re.search(r'D\$ blocked:\s+(\d+)', config_data).group(1))
                counters['Fetch Bubble'] = int(re.search(r'Fetch Bubble:\s+(\d+)', config_data).group(1))
                
                knob_configs.append({
                    'name': config_name,
                    'counters': counters
                })
            except (AttributeError, ValueError) as e:
                print(f"Warning: Failed to parse config '{config_name}' in ubenchmark '{ubench_name}': {e}")
                continue
        
        if knob_configs:
            ubenchmarks[ubench_name] = knob_configs
    
    return ubenchmarks

def compute_tma_metrics(counters, corewidth=8):
    """Compute TMA metrics from performance counters."""
    # Map counters to variables
    C_cycle = counters['Cycle']
    C_flush = counters['Flush']
    C_bm = counters['Branch Mispredict']
    C_fence = counters['Fence Retired']
    C_ret = counters['Uops Retired']
    C_issued = counters['Uops Issued']
    C_rec = counters['Recovering']
    C_iblk = counters['I$ Blocked']
    C_db = counters['D$ blocked']
    C_fetch = counters['Fetch Bubble']
    
    # Compute intermediate values
    M_total = C_cycle * corewidth  # total uop slots
    M_tf = C_flush + C_bm + C_fence  # total flushes
    
    # Avoid division by zero
    if M_tf == 0:
        M_br_mr = 0
        M_nf_r = 0
        M_fl_r = 0
    else:
        M_br_mr = C_bm / M_tf  # branch miss rate
        M_nf_r = (C_bm + C_fence) / M_tf  # non-fence flush ratio
        M_fl_r = C_flush / M_tf  # machine flush ratio
    
    M_rl = 4  # recover length, cycles from decode stage to issued
    
    # Top-level TMA
    Retiring = C_ret / M_total if M_total > 0 else 0
    BadSpec = ((C_issued - C_ret) * M_nf_r + (C_rec + M_rl * C_bm) * corewidth) / M_total if M_total > 0 else 0
    Frontend = C_fetch / M_total if M_total > 0 else 0
    Backend = 1 - Retiring - BadSpec - Frontend
    
    # Lower-level TMA
    MachCl = (C_issued - C_ret) * M_fl_r / M_total if M_total > 0 else 0  # Machine cleared
    BrMispr = ((C_issued - C_ret) * M_br_mr + C_rec) / M_total if M_total > 0 else 0
    Resteer = (C_issued - C_ret) * M_br_mr / M_total if M_total > 0 else 0
    RecovBub = C_rec / M_total if M_total > 0 else 0
    FetchLat = C_iblk * corewidth / M_total if M_total > 0 else 0
    PCRes = Frontend - FetchLat
    CoreBound = Backend - C_db / M_total if M_total > 0 else 0
    Membound = C_db / M_total if M_total > 0 else 0
    
    return {
        'Top-level': {
            'Retiring': Retiring,
            'Bad Speculation': BadSpec,
            'Frontend Bound': Frontend,
            'Backend Bound': Backend
        },
        'Lower-level': {
            'Machine Cleared': MachCl,
            'Branch Misprediction': BrMispr,
            'Resteer': Resteer,
            'Recovery Bubble': RecovBub,
            'Fetch Latency': FetchLat,
            'PC Resolution': PCRes,
            'Core Bound': CoreBound,
            'Memory Bound': Membound
        }
    }

def plot_top_level_tma(ubenchmark, knob_configs, all_metrics):
    """Create a stacked bar chart comparing top-level TMA metrics across configurations."""
    # Extract configuration names and top-level metrics
    config_names = [config['name'] for config in knob_configs]
    categories = ['Retiring', 'Bad Speculation', 'Frontend Bound', 'Backend Bound']
    
    # Prepare data for plotting (keep as decimal, not percentage)
    data = np.zeros((len(categories), len(config_names)))
    for j, metrics in enumerate(all_metrics):
        for i, cat in enumerate(categories):
            data[i, j] = metrics['Top-level'][cat]
    
    # Set up the plot
    x = np.arange(len(config_names))
    width = 0.6
    fig, ax = plt.subplots(figsize=(10, 7))
    
    # Create stacked bars
    colors = ['#2ecc71', '#e74c3c', '#3498db', '#f39c12']
    bottom = np.zeros(len(config_names))
    
    for i, (cat, color) in enumerate(zip(categories, colors)):
        ax.bar(x, data[i], width, label=cat, color=color, bottom=bottom)
        bottom += data[i]
    
    # Customize the plot
    ax.set_ylabel('Fraction', fontsize=12)
    ax.set_title(f'Top-Level TMA Metrics - {ubenchmark}', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(config_names, rotation=15, ha='right')
    ax.legend(loc='upper left', bbox_to_anchor=(1, 1))
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    ax.set_ylim(0, 1.0)
    
    # Add percentage labels on each segment
    for j in range(len(config_names)):
        cum_height = 0
        for i in range(len(categories)):
            height = data[i, j]
            if height > 0.03:  # Only label if segment is large enough
                ax.text(x[j], cum_height + height/2, f'{height*100:.1f}%', 
                       ha='center', va='center', fontsize=9, fontweight='bold')
            cum_height += height
    
    plt.tight_layout()
    plt.savefig(f'tma_toplevel_{ubenchmark}.png', dpi=300, bbox_inches='tight')
    plt.show()
    print(f"Saved chart as: tma_toplevel_{ubenchmark}.png")

def plot_backend_analysis(ubenchmark, knob_configs, all_metrics):
    """Create a stacked bar chart showing backend breakdown into Core Bound and Memory Bound."""
    # Extract configuration names and backend metrics
    config_names = [config['name'] for config in knob_configs]
    categories = ['Core Bound', 'Memory Bound']
    
    # Prepare data for plotting (keep as decimal, not percentage)
    data = np.zeros((len(categories), len(config_names)))
    for j, metrics in enumerate(all_metrics):
        data[0, j] = metrics['Lower-level']['Core Bound']
        data[1, j] = metrics['Lower-level']['Memory Bound']
    
    # Set up the plot
    x = np.arange(len(config_names))
    width = 0.6
    fig, ax = plt.subplots(figsize=(10, 7))
    
    # Create stacked bars
    colors = ['#9b59b6', '#e67e22']  # Purple for Core Bound, Orange for Memory Bound
    bottom = np.zeros(len(config_names))
    
    for i, (cat, color) in enumerate(zip(categories, colors)):
        ax.bar(x, data[i], width, label=cat, color=color, bottom=bottom)
        bottom += data[i]
    
    # Customize the plot
    ax.set_ylabel('Fraction', fontsize=12)
    ax.set_title(f'Backend Bound Analysis - {ubenchmark}', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(config_names, rotation=15, ha='right')
    ax.legend(loc='upper left', bbox_to_anchor=(1, 1))
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    ax.set_ylim(0, max(0.5, bottom.max() * 1.1))  # Scale y-axis to data
    
    # Add percentage labels on each segment
    for j in range(len(config_names)):
        cum_height = 0
        for i in range(len(categories)):
            height = data[i, j]
            if height > 0.01:  # Only label if segment is large enough
                ax.text(x[j], cum_height + height/2, f'{height*100:.1f}%', 
                       ha='center', va='center', fontsize=10, fontweight='bold', color='white')
            cum_height += height
    
    plt.tight_layout()
    plt.savefig(f'tma_backend_{ubenchmark}.png', dpi=300, bbox_inches='tight')
    plt.show()
    print(f"Saved chart as: tma_backend_{ubenchmark}.png")

def plot_frontend_analysis(ubenchmark, knob_configs, all_metrics):
    """Create a stacked bar chart showing frontend breakdown into Fetch Latency and PC Resolution."""
    # Extract configuration names and frontend metrics
    config_names = [config['name'] for config in knob_configs]
    categories = ['Fetch Latency', 'PC Resolution']
    
    # Prepare data for plotting (keep as decimal, not percentage)
    data = np.zeros((len(categories), len(config_names)))
    for j, metrics in enumerate(all_metrics):
        data[0, j] = metrics['Lower-level']['Fetch Latency']
        data[1, j] = metrics['Lower-level']['PC Resolution']
    
    # Set up the plot
    x = np.arange(len(config_names))
    width = 0.6
    fig, ax = plt.subplots(figsize=(10, 7))
    
    # Create stacked bars
    colors = ['#3498db', '#1abc9c']  # Blue for Fetch Latency, Teal for PC Resolution
    bottom = np.zeros(len(config_names))
    
    for i, (cat, color) in enumerate(zip(categories, colors)):
        ax.bar(x, data[i], width, label=cat, color=color, bottom=bottom)
        bottom += data[i]
    
    # Customize the plot
    ax.set_ylabel('Fraction', fontsize=12)
    ax.set_title(f'Frontend Bound Analysis - {ubenchmark}', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(config_names, rotation=15, ha='right')
    ax.legend(loc='upper left', bbox_to_anchor=(1, 1))
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    ax.set_ylim(0, max(0.5, bottom.max() * 1.1))  # Scale y-axis to data
    
    # Add percentage labels on each segment
    for j in range(len(config_names)):
        cum_height = 0
        for i in range(len(categories)):
            height = data[i, j]
            if height > 0.01:  # Only label if segment is large enough
                ax.text(x[j], cum_height + height/2, f'{height*100:.1f}%', 
                       ha='center', va='center', fontsize=10, fontweight='bold', color='white')
            cum_height += height
    
    plt.tight_layout()
    plt.savefig(f'tma_frontend_{ubenchmark}.png', dpi=300, bbox_inches='tight')
    plt.show()
    print(f"Saved chart as: tma_frontend_{ubenchmark}.png")

# Parse the log file
ubenchmarks = parse_log_file(log_path)

# Process each ubenchmark
for ubenchmark_name, knob_configs in ubenchmarks.items():
    print(f"\n{'#'*70}")
    print(f"# Ubenchmark: {ubenchmark_name}")
    print(f"{'#'*70}\n")
    
    # Compute and display TMA metrics for each configuration
    all_metrics = []
    for config in knob_configs:
        print(f"{'='*60}")
        print(f"Configuration: {config['name']}")
        print(f"{'='*60}")
        
        metrics = compute_tma_metrics(config['counters'])
        all_metrics.append(metrics)
        
        print("\nTop-level TMA Metrics:")
        for metric, value in metrics['Top-level'].items():
            print(f"  {metric:20s}: {value:.4f} ({value*100:.2f}%)")
        
        print("\nLower-level TMA Metrics:")
        for metric, value in metrics['Lower-level'].items():
            print(f"  {metric:20s}: {value:.4f} ({value*100:.2f}%)")
        print()
    
    # Create bar charts for TMA metrics for this ubenchmark
    if all_metrics:
        plot_top_level_tma(ubenchmark_name, knob_configs, all_metrics)
        plot_frontend_analysis(ubenchmark_name, knob_configs, all_metrics)
        plot_backend_analysis(ubenchmark_name, knob_configs, all_metrics)