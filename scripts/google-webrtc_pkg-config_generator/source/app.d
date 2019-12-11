import std.stdio;

private auto templatePcFile = "source_dir=<SOURCE_DIR>
build_dir=<BUILD_DIR>
includedir=${source_dir}
libdir=${build_dir}/obj

Name: google-webrtc
Description: googles webrtc implementation
Version: m74
Cflags: -isystem ${includedir} -isystem ${includedir}/third_party/abseil-cpp \\
  -DWEBRTC_POSIX -D_GLIBCXX_USE_CXX11_ABI=1
# got an std::list nullptr error with the debug arguments
# -D_GLIBCXX_DEBUG=1 -D_DEBUG=1 
Libs: -L${libdir} -lwebrtc -ldl
Requires: x11

# Requires.private: X11
# -lX11 -lXcomposite -lXext -lXdamage -lXfixes
";

void main(string[] args)
{
  import std.getopt : defaultGetoptPrinter, getopt;
  import std.path : absolutePath, asNormalizedPath;
  import std.array : replace, array;

  string sourcePath, buildPath;
  auto helpInformation = getopt(args, "sourcePath|s", &sourcePath, "buildPath|b", &buildPath);
  if (helpInformation.helpWanted)
  {
    defaultGetoptPrinter("Some information about the program.", helpInformation.options);
    return;
  }

  sourcePath = absolutePath(sourcePath).asNormalizedPath.array;
  buildPath = absolutePath(buildPath).asNormalizedPath.array;
  checkIfExistsAndIsDir("sourcePath", sourcePath);
  checkIfExistsAndIsDir("buildPath", buildPath);
  auto result = templatePcFile.replace("<SOURCE_DIR>", sourcePath)
    .replace("<BUILD_DIR>", buildPath);
  writeln(result);
}

private:

void checkIfExistsAndIsDir(string name, string path)
{
  import std.file : exists, isDir;
  import std.format : format;

  if (!path.exists)
    throw new Exception(format("%s '%s' does not exist. run the program again with the parameter `--%s`",
        name, path, name));
  if (!path.isDir)
    throw new Exception(format("%s '%s' is not a directory", name, path));
}
