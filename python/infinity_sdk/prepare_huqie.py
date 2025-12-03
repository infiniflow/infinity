# prepare_huqie.py
"""
Run 'uv run prepare_huqie.py' before running 'uv build'
"""

import shutil
from pathlib import Path
from infinity import rag_tokenizer

def prepare_huqie_file(source_file, target_dir):
    if not source_file.exists():
        print(f"Source file does not exist: {source_file}")
        return False
    
    print(f"Copy {source_file} to {target_dir}")
    shutil.copy2(source_file, target_dir)

    target_file = target_dir / "huqie.txt"
    if target_file.exists():
        print(f"Copying huqie.txt completed")
        print(f"   Size: {target_file.stat().st_size} byte")
    else:
        print(f"Copying huqie.txt failed")
        return False

    print("Generate huqie.txt.trie")
    tokenizer = rag_tokenizer.RagTokenizer()
    tokenizer.load_user_dict(str(target_file))

    trie_file = target_dir / "huqie.txt.trie"
    if trie_file.exists():
        print(f"Generating huqie.txt.trie completed")
        print(f"   Size: {trie_file.stat().st_size} byte")
    else:
        print(f"Generating huqie.txt.trie failed")
        return False

if __name__ == "__main__":
    current_dir = Path.cwd()
    project_root = current_dir.parent.parent
    source_file = project_root / "resource" / "rag" / "huqie.txt"
    target_dir = current_dir / "infinity"
    txt_file = target_dir / "huqie.txt"
    trie_file = target_dir / "huqie.txt.trie"

    if txt_file.exists() and trie_file.exists():
        print(f"Both huqie.txt and huqie.txt.trie exist in {target_dir}")
    else:
        prepare_huqie_file(source_file, target_dir)
