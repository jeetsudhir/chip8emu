import sys
from PyQt5.QtWidgets import QApplication, QFileDialog
import subprocess
import os


def select_rom_file():
    app = QApplication(sys.argv)
    options = QFileDialog.Options()
    options |= QFileDialog.ReadOnly
    rom_path, _ = QFileDialog.getOpenFileName(None, "Select ROM file", "", "All Files (*)", options=options)
    return rom_path

def main():
    rom_path = select_rom_file()
    if rom_path:
        # Assuming the C++ executable is named chip8Emulator and is in the same directory
        executable_path = os.path.join(os.path.dirname(__file__), "build/chip8Emulator")
        subprocess.run([executable_path, rom_path])
    else:
        print("No ROM file selected or dialog was canceled.")

if __name__ == "__main__":
    main()
