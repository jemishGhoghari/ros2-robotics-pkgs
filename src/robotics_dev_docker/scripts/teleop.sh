#!/bin/bash
ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null 2>&1 && pwd )"

bash $ROOT/run_detached.sh $HOME/workspaces/isaac_ros-dev /usr/local/bin/scripts/teleop-entrypoint.sh