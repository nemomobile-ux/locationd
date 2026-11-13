# locationd

`locationd` is a Qt 6 location daemon for NemoMobile providing a GeoClue 1-compatible D-Bus API. It aggregates location data from provider plugins and exposes position, satellite, velocity, address, and reverse-geocoding interfaces to clients (including Qt Positioning).

## Components

- **`locationd`** (`daemon/`) – base service (`org.nemomobile.location`).
- **`master`** (`master/`) – GeoClue Master service (`org.freedesktop.Geoclue.Master`) that creates per-client objects and serves location data.
- **`liblocation`** (`lib/`) – core library: plugin manager, D-Bus adaptors, client/resolver logic, authorization, optional power management.
- **`locationd-agent`** (`agent/`) – user-session authorization agent that shows a notification asking to allow/deny location access.
- **Plugins** (`plugins/`) – location and reverse-geocoding providers.

## Building

Requirements: CMake (>= 3.22), C++20, Qt 6 (Core, DBus, Network, Positioning), Nemo notifications library for the agent (`nemonotifications-qt6`).

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

Optional CMake options:

- `-DENABLE_MCE_DSME=ON` – enable MCE/DSME power management integration (pauses/resumes providers based on display/device state).

## Installation

```bash
sudo cmake --install .
```

Installs binaries, libraries, plugins (`/usr/lib/locationd/plugins`), D-Bus service/config files, and systemd units.

## Running (development)

```bash
export LOCATIOND_PLUGIN_PATH=$PWD/plugins
export LD_LIBRARY_PATH=$PWD/lib
./master/master
```

Run the authorization agent in a user session if interactive permission prompts are desired:

```bash
./agent/locationd-agent
```

## Plugins

Location providers (`ILocationProvider`):

- **gps** – NMEA over serial device.
- **geoip** – network-based coarse location.
- **ofono** – cellular-based location.
- **wifi** – Wi-Fi-based location.

Reverse geocoding (`IReverseGeocodeProvider`):

- **reversegeocode** – OpenStreetMap/Nominatim-based reverse geocoding.

Plugins are loaded from `/usr/lib/locationd/plugins` by default (override with `LOCATIOND_PLUGIN_PATH`).

## Configuration

### GPS plugin (`/etc/locationd/gps.conf`, override with `LOCATIOND_GPS_CONFIG`)

INI keys:

- `device` – serial device path (default `/dev/ttyACM0`)
- `wakeupCommand` – command sent on activation (`\r`, `\n` escapes supported)
- `sleepCommand` – command sent on deactivation

### Authorization (allow/deny)

If no rules exist, access is allowed by default. When rules are present, callers must be explicitly allowed. Decisions can be persisted per-app.

- `LOCATIOND_ALLOWLIST` / `/etc/locationd/allowlist`
- `LOCATIOND_ALLOWLIST_DIR` / `/etc/locationd/allowlist.d/*.conf`
- `LOCATIOND_DENYLIST` / `/etc/locationd/denylist`
- `LOCATIOND_DENYLIST_DIR` / `/etc/locationd/denylist.d/*.conf`

Entries are matched against D-Bus caller name, executable name, or PID. An empty `*.conf` file allows/denies by its basename.

### Authorization agent

`locationd-agent` registers on the session bus and implements `org.freedesktop.Geoclue.Master.AuthorizationAgent`. When a client is not explicitly allowed/denied, the master requests authorization and the agent shows a notification with **Allow once**, **Allow always**, and **Disallow**. “Always” decisions are written to the allow/deny drop-in directories.

### Reverse geocoding

- `LOCATIOND_REVERSE_GEOCODE_URL` – override endpoint (default: `https://nominatim.openstreetmap.org/reverse`)

## D-Bus interfaces

Master: `org.freedesktop.Geoclue.Master` on `/org/freedesktop/Geoclue/Master` (session bus) with `Create`, `RegisterAuthorizationAgent`, `UnregisterAuthorizationAgent`.

Per-client interfaces (GeoClue 1): `org.freedesktop.Geoclue`, `org.freedesktop.Geoclue.MasterClient`, `org.freedesktop.Geoclue.Position`, `org.freedesktop.Geoclue.Address`, `org.freedesktop.Geoclue.Satellite`, `org.freedesktop.Geoclue.Velocity`, `org.freedesktop.Geoclue.ReverseGeocode`.

## Tests

```bash
cd build
ctest
# or
../test/test_allowed.sh $PWD
```

## License

LGPL-2.1-or-later (see source file headers).
