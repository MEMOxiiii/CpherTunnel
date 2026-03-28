"""
Fix individual packet files by adding standalone types (enums, structs)
that were defined outside classes in the original category files.
Reads from the original mcprotocol source to find these types.
"""
import re
import os

ORIGINAL_DIR = r"d:\project\minecraft\minecraft_software\mcprotocol\include\bedrock\protocol\packets"
PACKETS_DIR = r"d:\project\minecraft\minecraft_software\cphertunnel\include\cphertunnel\protocol\packets"

def extract_standalone_blocks(filepath):
    """Extract all code blocks between classes (enums, structs, etc.)."""
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    blocks = []  # list of (block_text, next_class_name)
    current_block = []
    in_class = False
    brace_depth = 0
    
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()
        
        # Check if this starts a Packet class
        m = re.match(r'^class\s+(\w+)\s*:\s*public\s+Packet\s*\{', stripped)
        if m and brace_depth == 0:
            class_name = m.group(1)
            # Save current block with the class it precedes
            block_text = ''.join(current_block).strip()
            if block_text:
                # Remove leading #pragma, #include, namespace lines
                block_lines = block_text.split('\n')
                filtered = []
                for bl in block_lines:
                    sbl = bl.strip()
                    if (sbl.startswith('#') or sbl.startswith('//') or 
                        sbl.startswith('namespace') or sbl == '' or
                        sbl == '{' or sbl == '}'):
                        continue
                    filtered.append(bl)
                block_text = '\n'.join(filtered).strip()
                if block_text:
                    blocks.append((block_text, class_name))
            current_block = []
            
            # Skip past the class
            brace_depth = line.count('{') - line.count('}')
            i += 1
            while i < len(lines) and brace_depth > 0:
                brace_depth += lines[i].count('{') - lines[i].count('}')
                i += 1
            continue
        
        if brace_depth == 0:
            current_block.append(line)
        i += 1
    
    return blocks


def class_to_filename(class_name):
    """Convert CamelCase to snake_case filename."""
    s = re.sub(r'(?<=[a-z0-9])([A-Z])', r'_\1', class_name)
    s = re.sub(r'(?<=[A-Z])([A-Z][a-z])', r'_\1', s)
    return s.lower() + '.hpp'


def fix_file(filename, prepend_text):
    """Prepend standalone type definitions before the class in an individual file."""
    filepath = os.path.join(PACKETS_DIR, filename)
    if not os.path.exists(filepath):
        print(f"  WARNING: {filename} not found!")
        return
    
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Check if already fixed
    if prepend_text.split('\n')[0].strip() in content:
        print(f"  SKIP: {filename} (already has the type)")
        return
    
    # Insert right after "namespace cphertunnel {\n"
    marker = 'namespace cphertunnel {\n'
    idx = content.find(marker)
    if idx == -1:
        marker = 'namespace cphertunnel {'
        idx = content.find(marker)
    
    if idx == -1:
        print(f"  ERROR: Can't find namespace in {filename}")
        return
    
    insert_pos = idx + len(marker)
    if content[insert_pos:insert_pos+1] == '\n':
        insert_pos += 1
    
    content = content[:insert_pos] + '\n' + prepend_text + '\n\n' + content[insert_pos:]
    
    with open(filepath, 'w', encoding='utf-8', newline='\n') as f:
        f.write(content)
    
    print(f"  FIXED: {filename}")


def main():
    category_files = [
        'play_packets.hpp',
        'login_packets.hpp', 
        'command_packets.hpp',
        'entity_packets.hpp',
        'movement_packets.hpp',
        'world_packets.hpp',
    ]
    
    # Map of class_name -> [(block_text, source_file)]
    all_blocks = {}
    
    for cat_file in category_files:
        original_path = os.path.join(ORIGINAL_DIR, cat_file)
        if not os.path.exists(original_path):
            print(f"WARNING: Original {cat_file} not found!")
            continue
        
        blocks = extract_standalone_blocks(original_path)
        print(f"\n{cat_file}:")
        for block_text, next_class in blocks:
            # Find what types are defined
            type_names = re.findall(r'(?:enum class|struct)\s+(\w+)', block_text)
            print(f"  Types {type_names} -> before {next_class}")
            
            target_file = class_to_filename(next_class)
            fix_file(target_file, block_text)
    
    # Special case: GameRule is used by both StartGamePacket AND GameRulesChangedPacket
    # We need to put GameRule in a shared location
    # Read the GameRule from start_game_packet.hpp and check if it's also needed in game_rules_changed_packet.hpp
    
    # Check if GameRule is already defined in start_game_packet.hpp
    sgp = os.path.join(PACKETS_DIR, 'start_game_packet.hpp')
    grcp = os.path.join(PACKETS_DIR, 'game_rules_changed_packet.hpp')
    
    if os.path.exists(sgp) and os.path.exists(grcp):
        with open(grcp, 'r') as f:
            grcp_content = f.read()
        if 'GameRule' in grcp_content and 'struct GameRule' not in grcp_content:
            # Need to add forward reference or include
            # Since start_game_packet is included before game_rules_changed_packet via login_packets -> play_packets order,
            # and the main header includes them in sequence, the GameRule from start_game_packet should already be visible
            # through the main cphertunnel.hpp header ordering.
            # But within individual file compilation, it's not.
            # Best approach: add #include "start_game_packet.hpp" or move GameRule to packet_types.hpp
            print("\n  NOTE: GameRule is shared between StartGamePacket and GameRulesChangedPacket")
            print("  Adding GameRule include to game_rules_changed_packet.hpp")
            grcp_content = grcp_content.replace(
                '#include "../packet.hpp"',
                '#include "../packet.hpp"\n#include "start_game_packet.hpp"'
            )
            with open(grcp, 'w', encoding='utf-8', newline='\n') as f:
                f.write(grcp_content)


if __name__ == '__main__':
    main()
