# benchmark

With clangs `-ftime-trace` I've been able to detect, that `sigslots` compiles around 4x faster.
In a simple program `boost::signals2` took 3.6 seconds. sigslot takes for the same achievement 0.9 seconds.

# how to switch from `boost::signals2` to `sigslot`.

```bash
#!/bin/bash -ex

rg -Il '#include <boost/signals2/signal.hpp>' | xargs sed -i 's+#include <boost/signals2/signal.hpp>+#include <sigslot/signal.hpp>+g'
rg -Il 'boost::signals2::scoped_connection' | xargs sed -i 's+boost::signals2::scoped_connection+sigslot::scoped_connection+g'
rg 'boost::signals2::connection'
rg -Il 'boost::signals2::signal<void(.*)>' | xargs sed -i 's+boost::signals2::signal<void(\(.*\))>+sigslot::signal<\1>+g'
rg -Il '#include <sigslot/signal.hpp>' | xargs sed -i 's+#include <sigslot/signal.hpp>+#include <sigslot/signal.hpp>+g'
```

