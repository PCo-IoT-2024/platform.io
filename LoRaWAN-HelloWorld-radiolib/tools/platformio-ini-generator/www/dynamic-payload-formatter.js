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

  function enhanceUsageDocs() {
    const docs = document.querySelector(".docs-card");
    if (!docs) {
      return;
    }

    const heading = docs.querySelector("h2");
    if (heading) {
      heading.textContent = "How to use the generated files";
    }

    const monitorHeading = [...docs.querySelectorAll("h3")]
      .find((h) => h.textContent.trim() === "5. Monitor and TTN setup");

    if (!monitorHeading || docs.querySelector(".payload-formatter-docs")) {
      return;
    }

    const section = document.createElement("section");
    section.className = "payload-formatter-docs";
    section.innerHTML = `
      <h3>5. Use the generated TTN payload formatter</h3>
      <p>The firmware sends compact uplinks as byte payloads. TTN does not automatically know whether fPort 1 contains GPS data, fPort 2 contains temperature, or whether a disabled sensor should be ignored. The generated payload formatter is the translation layer between the device firmware and readable application data in The Things Stack.</p>
      <p>Generate the formatter in the dedicated card above after selecting the sensors. Then copy or download the JavaScript and paste it into the TTN uplink payload formatter for the application or for the specific end device. Regenerate it whenever the selected sensor combination changes.</p>
      <p>After installation in TTN, uplinks are shown as decoded JSON fields such as <code>temperature_c</code>, <code>ph_level</code>, <code>tds_ppm</code>, <code>turbidity_ntu</code>, or GPS coordinates. Diagnostic messages on fPorts 221, 222 and 223 are decoded as information, warnings, or errors.</p>`;

    monitorHeading.parentNode.insertBefore(section, monitorHeading);
    monitorHeading.textContent = "6. Monitor and test";

    const monitorList = monitorHeading.nextElementSibling;
    if (monitorList?.tagName === "UL") {
      monitorList.innerHTML = `
        <li>Open the serial monitor with <code>pio device monitor</code>.</li>
        <li>Check the TTN live data view to verify that uplinks arrive and that the decoded payload contains the expected fields for the selected sensors.</li>`;
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
  moveUplinkIntervalToLorawanProfile();
  enhanceUsageDocs();
  $("generateFormatterButton")?.addEventListener("click", generatePayloadFormatter, true);
  $("copyFormatterButton")?.addEventListener("click", copyPayloadFormatter, true);
  $("downloadFormatterButton")?.addEventListener("click", downloadPayloadFormatter, true);
})();
