#!/bin/bash
BUILD_DIR="${1:-/home/neochapay/work/nemomobile/locationd/build}"
export LOCATIOND_PLUGIN_PATH="$BUILD_DIR/plugins"
export LD_LIBRARY_PATH="$BUILD_DIR/lib:$LD_LIBRARY_PATH"
export LOCATIOND_ALLOWLIST_DIR=/tmp/locationd-test-allowlist.d
rm -rf "$LOCATIOND_ALLOWLIST_DIR"
mkdir -p "$LOCATIOND_ALLOWLIST_DIR"
touch "$LOCATIOND_ALLOWLIST_DIR/test_location.conf"

for pid in $(pgrep -f "[m]aster/master" 2>/dev/null); do kill "$pid" 2>/dev/null; done
sleep 1

setsid bash -c "export LOCATIOND_PLUGIN_PATH=$BUILD_DIR/plugins LD_LIBRARY_PATH=$BUILD_DIR/lib LOCATIOND_ALLOWLIST_DIR=/tmp/locationd-test-allowlist.d; exec $BUILD_DIR/master/master" </dev/null >/tmp/locationd-master.log 2>&1 &
sleep 4

if timeout 15 "$BUILD_DIR/test/test_location" 2>&1 | grep -q "ALLOWED:"; then
    echo "PASS: location allowed"
    RESULT=0
else
    echo "FAIL: location not allowed"
    RESULT=1
fi

for pid in $(pgrep -f "[m]aster/master" 2>/dev/null); do kill "$pid" 2>/dev/null; done
exit $RESULT