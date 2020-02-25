# matrix

[https://matrix.org]

## synapse

### links

install: https://github.com/matrix-org/synapse/blob/master/INSTALL.md
client-server-api:
- example: https://matrix.org/docs/guides/client-server-api
- spec: https://matrix.org/docs/spec/client_server/latest

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

### settings

```ini
allow_guest_access: true
enable_registration: true

# for tests disable ratelimit. the value zero does NOT disable the ratelimit
rc_message:
  per_second: 1000
  burst_count: 1000

rc_registration:
  per_second: 1000
  burst_count: 1000
  address:
    per_second: 1000
    burst_count: 1000
  account:
    per_second: 1000
    burst_count: 1000
  failed_attempts:
    per_second: 1000
    burst_count: 1000
  per_second: 1000
  burst_count: 1000
```


