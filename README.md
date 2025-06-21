



### Building

1. **Install dependencies** (example for Ubuntu/Debian):

   ```bash
   sudo apt-get install libgtkmm-4.0-dev libcairomm-1.0-dev g++
   ```

2. **Clone the repository**:

   ```bash
   git clone <your-repo-url>
   cd CustomShell
   ```

3. **Build the project**:

   ```bash
   g++ -std=c++17 main.cpp MyWindow.cpp CLIWindow.cpp NetworkingWindow.cpp FileManagementWindow.cpp SystemInformationWindow.cpp -o AeroShell `pkg-config --cflags --libs gtkmm-4.0 cairomm-1.0`
   ```

4. **Run the application**:

   ```bash
   ./AeroShell
   ```

### Windows

- Use MSYS2 or install GTKmm for Windows.
- Update the build command to use `g++.exe` and the appropriate include/library paths.



## Project Structure

- `main.cpp` - Application entry point and CSS loader.
- `MyWindow.*` - Main dashboard window.
- `CLIWindow.*` - Command-line shell window.
- `NetworkingWindow.*` - Networking tools window.
- `FileManagementWindow.*` - File management window.
- `SystemInformationWindow.*` - System info window.
- `style.css` - Custom CSS for UI styling.

## Customization

- Edit `style.css` to change the look and feel of the application.
- Add more commands or panels by extending the respective window classes.

