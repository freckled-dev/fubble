# matrix

[https://matrix.org]

## synapse

### install:

[https://github.com/matrix-org/synapse/blob/master/INSTALL.md]

#### opensuse:
```
zypper install matrix-synapse
```
or by using `pip`
```
zypper install python3-virtualenv
virtualenv -p python3 ./env
source ./env/bin/activate.fish
pip install --upgrade pip
pip install --upgrade setuptools
pip install matrix-synapse
```

### configure and start

```
# ensure you are in the right `virtualenv`
python -m synapse.app.homeserver -c homeserver.yaml --generate-config \
  --server-name=fubble.io --report-stats=no

synctl start
```

