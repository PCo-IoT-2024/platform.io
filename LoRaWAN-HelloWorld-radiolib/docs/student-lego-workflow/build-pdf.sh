#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

OUTPUT="student-lego-workflow.pdf"

pandoc \
  metadata.yaml \
  00-course-context.md \
  01-technology-primer-for-managers.md \
  02-installation-windows-macos.md \
  03-system-overview.md \
  04-lorawan-and-ttn.md \
  05-generator-workflow.md \
  06-firmware-architecture.md \
  07-sensors-and-measurements.md \
  08-analog-calibration.md \
  09-power-and-deep-sleep.md \
  10-build-test-troubleshoot.md \
  11-student-tasks.md \
  --toc \
  --number-sections \
  --pdf-engine=xelatex \
  -o "${OUTPUT}"

echo "Created ${SCRIPT_DIR}/${OUTPUT}"
