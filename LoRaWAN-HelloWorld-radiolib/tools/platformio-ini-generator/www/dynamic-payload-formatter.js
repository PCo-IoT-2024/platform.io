(() => {
  const $ = (id) => document.getElementById(id);
  const prompt = "Press “Generate payload formatter”.";
  let generated = "";
  let hasGenerated = false;

  const out = $("payloadFormatterOutput");
  if (out) {
    out.textContent = prompt;
    new MutationObserver(() => {
      if (!hasGenerated && out.textContent !== prompt) {
        out.textContent = prompt;
      }
    }).observe(out, { childList: true, characterData: true, subtree: true });
  }

  function checked(id) {
    return !!$(id)?.checked;
  }

  function add(lines, enabled, block) {
    if (enabled) {
      lines.push(block.trimEnd(), "");
    }
  }

  function enhanceHero() {
    const lead = document.querySelector(".lead");
    if (!lead) {
      return;
    }

    lead.innerHTML = `Generate a tailored <code>platformio.ini</code> and matching <code>payload-formatter.js</code> for the selected radio module, LoRaWAN version, <a href="https://www.thethingsindustries.com/docs/concepts/ttn/" target="_blank" rel="noopener noreferrer">The Things Networks Sandbox (TTN)</a> device credentials, sensor set, and pin mapping.`;
  }

  function populateLorawanVersionOptions() {
    const version = $("lorawanVersion");
    if (!version) {
      return;
    }

    const selected = version.value || "1.1.0";
    const versions = ["1.1.0", "1.0.4", "1.0.3", "1.0.2", "1.0.1", "1.0.0"];

    version.replaceChildren();

    for (const value of versions) {
      const option = document.createElement("option");
      option.value = value;
      option.textContent = value;
      version.appendChild(option);
    }

    version.value = versions.includes(selected) ? selected : "1.1.0";
    version.dispatchEvent(new Event("change"));
  }

  function populateRegionOptions() {
    const region = $("region");
    if (!region) {
      return;
    }

    const selected = region.value || "EU868";
    const regions = [
      ["EU868", "EU868 — Europe 863–870 MHz"],
      ["US915", "US915 — North America 902–928 MHz"],
      ["EU433", "EU433 — Europe 433 MHz"],
      ["AU915", "AU915 — Australia 915–928 MHz"],
      ["CN470", "CN470 — China 470–510 MHz"],
      ["AS923", "AS923-1 — Asia-Pacific 923 MHz"],
      ["AS923_2", "AS923-2 — Asia-Pacific variant"],
      ["AS923_3", "AS923-3 — Asia-Pacific variant"],
      ["AS923_4", "AS923-4 — Asia-Pacific variant"],
      ["KR920", "KR920 — South Korea 920–923 MHz"],
      ["IN865", "IN865 — India 865–867 MHz"]
    ];

    region.replaceChildren();

    for (const [value, label] of regions) {
      const option = document.createElement("option");
      option.value = value;
      option.textContent = label;
      region.appendChild(option);
    }

    region.value = regions.some(([value]) => value === selected) ? selected : "EU868";
  }

  function appendFieldHelp(controlId, text) {
    const control = $(controlId);
    const label = control?.closest("label");
    if (!control || !label || label.querySelector(".field-help")) {
      return;
    }

    const help = document.createElement("span");
    help.className = "field-help";
    help.textContent = text;
    label.appendChild(help);
  }

  function enhanceLorawanProfileFields() {
    appendFieldHelp("radioModule", "Transceiver chip mounted on the ESP32 radio board.");
    appendFieldHelp("lorawanVersion", "Must match the LoRaWAN MAC version configured in TTN.");
    appendFieldHelp("region", "Frequency plan. Use EU868 for this course in Europe.");
  }

  function moveUplinkIntervalToLorawanProfile() {
    const uplinkInput = $("uplinkInterval");
    const uplinkLabel = uplinkInput?.closest("label");
    const lorawanGrid = document.querySelector(".device-row-lorawan .compact-grid");
    const hardwareTitle = document.querySelector(".device-row-hardware .device-copy h4");
    const hardwareText = document.querySelector(".device-row-hardware .device-copy p");

    if (!uplinkLabel || !lorawanGrid || uplinkLabel.parentElement === lorawanGrid) {
      return;
    }

    lorawanGrid.classList.remove("three");
    lorawanGrid.classList.add("four");
    lorawanGrid.appendChild(uplinkLabel);

    if (hardwareTitle) {
      hardwareTitle.textContent = "Radio wiring";
    }

    if (hardwareText) {
      hardwareText.textContent = "The pin map connects the ESP32 to the LoRa module. These pins describe the physical SPI and interrupt/reset wiring used by RadioLib.";
    }
  }

  function commonFormatterHelpers() {
    return `function bytesToString(bytes) {
  return String.fromCharCode.apply(null, bytes || []);
}

function toNumber(value) {
  const number = Number(value);
  return Number.isFinite(number) ? number : null;
}

function parseCsvNumbers(decodedString) {
  if (!decodedString || decodedString.length === 0) {
    return [];
  }

  return decodedString.split(",").map(toNumber);
}

function numberOrDefault(value, fallback) {
  return value === null || value === undefined ? fallback : value;
}

function makeInfoResult(input, decodedString) {
  return {
    data: {
      f_port: input.fPort,
      payload_raw: decodedString,
      message: decodedString,
      level: "info"
    }
  };
}

function makeWarningResult(input, decodedString) {
  return {
    data: {
      f_port: input.fPort,
      payload_raw: decodedString,
      message: decodedString,
      level: "warning"
    },
    warnings: ["Warning: " + decodedString]
  };
}

function makeErrorResult(input, decodedString) {
  return {
    data: {
      f_port: input.fPort,
      payload_raw: decodedString,
      message: decodedString,
      level: "error"
    },
    errors: ["Error: " + decodedString]
  };
}`;
  }

  function generatePayloadFormatter() {
    hasGenerated = true;

    const lines = [];
    lines.push(`// TTN / The Things Stack uplink payload formatter.
// Generated by tools/platformio-ini-generator.
// Includes only the selected sensor fPorts plus common diagnostics.

${commonFormatterHelpers()}

function decodeUplink(input) {
  const decodedString = bytesToString(input.bytes || []);
  const variables = parseCsvNumbers(decodedString);

  switch (input.fPort) {`);

    add(lines, checked("hasGps"), `    case 1: { // GPS location
      const latitude = numberOrDefault(variables[0], 0);
      const longitude = numberOrDefault(variables[1], 0);
      const altitude = numberOrDefault(variables[2], 0);
      const hdop = numberOrDefault(variables[3], 0);

      return {
        latitude: latitude,
        longitude: longitude,
        altitude: altitude,
        hdop: hdop,
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          latitude: latitude,
          longitude: longitude,
          altitude: altitude,
          hdop: hdop
        }
      };
    }`);

    add(lines, checked("hasTemperature"), `    case 2: // DS18B20 temperature
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          temperature_c: numberOrDefault(variables[0], 0)
        }
      };`);

    add(lines, checked("hasPh"), `    case 3: // PH4502C pH level
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          ph_level: numberOrDefault(variables[0], 0)
        }
      };`);

    add(lines, checked("hasTds"), `    case 4: // Gravity TDS ppm + compensation temperature
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          tds_ppm: numberOrDefault(variables[0], 0),
          temperature_c: numberOrDefault(variables[1], 0)
        }
      };`);

    add(lines, checked("hasTurbidity"), `    case 5: // Turbidity NTU
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          turbidity_ntu: numberOrDefault(variables[0], 0)
        }
      };`);

    lines.push(`    case 220:
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          message: "Request further downlinks"
        }
      };

    case 221:
      return makeInfoResult(input, decodedString);

    case 222:
      return makeWarningResult(input, decodedString);

    case 223:
      return makeErrorResult(input, decodedString);

    default:
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          message: decodedString,
          level: "error"
        },
        errors: [
          "Error (fatal): fPort " + input.fPort + " in uplink not enabled by this formatter"
        ]
      };
  }
}
`);

    generated = lines.join("\n");

    if (out) {
      out.textContent = generated;
    }

    const copyButton = $("copyFormatterButton");
    const downloadButton = $("downloadFormatterButton");

    if (copyButton) {
      copyButton.disabled = false;
    }

    if (downloadButton) {
      downloadButton.disabled = false;
    }
  }

  function downloadText(filename, content) {
    const blob = new Blob([content], { type: "text/plain;charset=utf-8" });
    const link = document.createElement("a");
    link.href = URL.createObjectURL(blob);
    link.download = filename;
    document.body.appendChild(link);
    link.click();
    link.remove();
    URL.revokeObjectURL(link.href);
  }

  async function copyPayloadFormatter(event) {
    event?.stopImmediatePropagation();
    if (!generated) {
      return;
    }

    await navigator.clipboard.writeText(generated);

    const button = $("copyFormatterButton");
    if (!button) {
      return;
    }

    const oldText = button.textContent;
    button.textContent = "Copied";
    setTimeout(() => {
      button.textContent = oldText;
    }, 1200);
  }

  function downloadPayloadFormatter(event) {
    event?.stopImmediatePropagation();
    if (generated) {
      downloadText("payload-formatter.js", generated);
    }
  }

  enhanceHero();
  populateLorawanVersionOptions();
  populateRegionOptions();
  moveUplinkIntervalToLorawanProfile();
  enhanceLorawanProfileFields();
  $("generateFormatterButton")?.addEventListener("click", generatePayloadFormatter, true);
  $("copyFormatterButton")?.addEventListener("click", copyPayloadFormatter, true);
  $("downloadFormatterButton")?.addEventListener("click", downloadPayloadFormatter, true);
})();
