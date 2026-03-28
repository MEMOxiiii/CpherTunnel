"""
Split CpherTunnel category packet files into individual .hpp files.
Each packet class gets its own file. Category files become aggregators.
"""
import re
import os

PACKETS_DIR = r"d:\project\minecraft\minecraft_software\cphertunnel\include\cphertunnel\protocol\packets"

def class_to_filename(class_name):
    """Convert CamelCase class name to snake_case filename."""
    # Insert underscore before uppercase letters
    s = re.sub(r'(?<=[a-z0-9])([A-Z])', r'_\1', class_name)
    s = re.sub(r'(?<=[A-Z])([A-Z][a-z])', r'_\1', s)
    return s.lower() + '.hpp'


def extract_classes(filepath):
    """Extract all Packet subclasses from a category file."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    lines = content.split('\n')
    classes = []
    
    i = 0
    while i < len(lines):
        line = lines[i]
        # Match class definition line
        m = re.match(r'^class\s+(\w+)\s*:\s*public\s+Packet\s*\{', line)
        if m:
            class_name = m.group(1)
            start = i
            brace_count = line.count('{') - line.count('}')
            i += 1
            while i < len(lines) and brace_count > 0:
                brace_count += lines[i].count('{') - lines[i].count('}')
                i += 1
            # i now points to the line after the closing };
            # But we might need to include the }; line
            end = i
            class_lines = lines[start:end]
            class_body = '\n'.join(class_lines)
            classes.append((class_name, class_body))
        else:
            i += 1
    
    return classes


def get_includes(filepath):
    """Extract include lines and namespace from a category file."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    lines = content.split('\n')
    includes = []
    for line in lines:
        if line.startswith('#include') or line.startswith('// '):
            if '#pragma once' not in line:
                includes.append(line)
        if 'namespace cphertunnel' in line:
            break
    
    # Just return standard includes needed
    return '#include "../packet.hpp"'


def create_individual_file(class_name, class_body, output_dir):
    """Create an individual .hpp file for a packet class."""
    filename = class_to_filename(class_name)
    filepath = os.path.join(output_dir, filename)
    
    # Don't overwrite existing individual files (they were manually created)
    if os.path.exists(filepath):
        return filename, True  # already exists
    
    # Determine needed includes
    includes = ['#include "../packet.hpp"']
    if 'packet_types.hpp' in class_body or any(t in class_body for t in [
        'DataItem', 'PropertySyncData', 'NetworkItemStackDescriptor',
        'SerializedAbilitiesData', 'AttributeData', 'InventorySource',
        'InventoryAction', 'ActorLink', 'FullContainerName', 'ScoreboardId',
        'SubChunkPos', 'SubChunkPosOffset', 'ItemStackRequestSlotInfo',
        'MapDecoration', 'MovementAttributesComponent', 'PlayerInputTick',
        'ItemStackNetIdVariant', 'writeDataItemList', 'readDataItemList'
    ]):
        includes.append('#include "../packet_types.hpp"')
    
    if 'NbtTag' in class_body or 'readRootCompound' in class_body or 'writeRootCompound' in class_body:
        includes.append('#include "../nbt.hpp"')
    
    header = f"// CpherTunnel - {class_name}\n#pragma once\n"
    header += '\n'.join(includes)
    header += '\nnamespace cphertunnel {\n\n'
    footer = '\n\n} // namespace cphertunnel\n'
    
    content = header + class_body + footer
    
    with open(filepath, 'w', encoding='utf-8', newline='\n') as f:
        f.write(content)
    
    return filename, False


def process_category_file(cat_path):
    """Process a single category file: extract classes, create individual files."""
    cat_name = os.path.basename(cat_path)
    print(f"\nProcessing {cat_name}...")
    
    classes = extract_classes(cat_path)
    print(f"  Found {len(classes)} packet classes")
    
    individual_files = []
    for class_name, class_body in classes:
        filename, existed = create_individual_file(class_name, class_body, PACKETS_DIR)
        status = "EXISTED" if existed else "CREATED"
        print(f"  {status}: {filename} ({class_name})")
        individual_files.append(filename)
    
    # Rewrite category file as aggregator (just includes)
    if individual_files:
        cat_base = cat_name.replace('.hpp', '')
        agg_content = f"// CpherTunnel - {cat_base} (aggregator)\n"
        agg_content += "#pragma once\n"
        agg_content += '\n'.join(f'#include "{f}"' for f in individual_files)
        agg_content += '\n'
        
        with open(cat_path, 'w', encoding='utf-8', newline='\n') as f:
            f.write(agg_content)
        print(f"  Rewrote {cat_name} as aggregator with {len(individual_files)} includes")
    
    return classes


def main():
    category_files = [
        'sound_packets.hpp',
        'resource_pack_packets.hpp', 
        'camera_packets.hpp',
        'command_packets.hpp',
        'movement_packets.hpp',
        'inventory_packets.hpp',
        'ui_packets.hpp',
        'world_packets.hpp',
        'entity_packets.hpp',
        'play_packets.hpp',
        'login_packets.hpp',
        'misc_packets.hpp',
    ]
    
    total = 0
    for cat_file in category_files:
        cat_path = os.path.join(PACKETS_DIR, cat_file)
        if os.path.exists(cat_path):
            classes = process_category_file(cat_path)
            total += len(classes)
        else:
            print(f"WARNING: {cat_file} not found!")
    
    print(f"\n{'='*50}")
    print(f"Total packet classes extracted: {total}")
    print(f"{'='*50}")


if __name__ == '__main__':
    main()
