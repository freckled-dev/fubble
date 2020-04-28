
## search for precompiled packages

https://docs.conan.io/en/latest/reference/commands/consumer/search.html
```
conan search harfbuzz/2.6.4@bincrafters/stable -r bincrafters -q "os=Windows AND compiler.version=16 AND compiler.runtime=MD"
conan search google-webrtc/m79@
```

## more information about proccesses

https://docs.conan.io/en/latest/reference/conanfile/other.html#running-commands
`CONAN_PRINT_RUN_COMMANDS`

