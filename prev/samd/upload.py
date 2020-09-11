#!/usr/bin/env python3

import os
from pathlib import Path
import mimetypes
import subprocess

root=Path(__file__).parent

#subprocess.run(["arduino-cli","compile","--fqbn", "arduino:samd:nano_33_iot","-u","-p","/dev/cu.usbmodem14401",str(root)])
subprocess.run(f"cd {root} && arduino-cli compile --fqbn arduino:samd:nano_33_iot&&arduino-cli upload -p /dev/cu.usbmodem14401 --fqbn arduino:samd:nano_33_iot",shell=True)
