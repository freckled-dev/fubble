# notes on qt5

## logging and fedora

by default it's disabled to log with `console.log` and `console.debug`. other logs still work, eg: `console.info`.
https://stackoverflow.com/questions/36338000/qml-console-log-and-console-debug-dont-write-to-console

## qml

When using the `Q_PROPERTY` I have to give the full typename with all namespaces noted.
Same applies to signals.
Don't do `typedef`s or `using`s.
- https://stackoverflow.com/questions/19889163/using-qt-properties-with-custom-types-defined-in-namespaces
- https://wiki.qt.io/How_to_use_a_C_class_declared_in_a_namespace_with_Q_PROPERTY_and_QML
- https://stackoverflow.com/questions/24231470/qmetapropertyread-unable-to-handle-unregistered-datatype-treeiteminspectori

