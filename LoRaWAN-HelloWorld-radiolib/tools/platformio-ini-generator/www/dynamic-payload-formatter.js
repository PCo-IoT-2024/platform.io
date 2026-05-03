(() => {
  const $ = (id) => document.getElementById(id);

  function enhanceHero() {
    const lead = document.querySelector(".lead");
    if (lead) {
      lead.innerHTML = 'Generate a tailored <code>platformio.ini</code> and matching <code>payload-formatter.js</code> for the selected radio module, LoRaWAN key model, <a href="https://www.thethingsindustries.com/docs/concepts/ttn/" target="_blank" rel="noopener noreferrer">The Things Networks Sandbox (TTN)</a> device credentials, sensor set, and pin mapping.';
    }
  }

  function populateLorawanKeyModelOptions() {
    const version = $("lorawanVersion");
    if (!version) {
      return;
    }

    const selected = version.value === "1.1.0" ? "1.1.0" : "1.0.x";
    version.replaceChildren();

    [["1.1.0", "LoRaWAN 1.1.0 key model"], ["1.0.x", "LoRaWAN 1.0.x key model"]].forEach(([value, label]) => {
      const option = document.createElement("option");
      option.value = value;
      option.textContent = label;
      version.appendChild(option);
    });

    version.value = selected;
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
    regions.forEach(([value, label]) => {
      const option = document.createElement("option");
      option.value = value;
      option.textContent = label;
      region.appendChild(option);
    });
    region.value = regions.some(([value]) => value === selected) ? selected : "EU868";
  }

  function appendFieldHelp(controlId, text) {
    const label = $(controlId)?.closest("label");
    if (!label || label.querySelector(".field-help")) {
      return;
    }
    const help = document.createElement("span");
    help.className = "field-help";
    help.textContent = text;
    label.appendChild(help);
  }

  function enhanceLorawanProfileFields() {
    appendFieldHelp("radioModule", "Transceiver chip mounted on the ESP32 radio board.");
    appendFieldHelp("lorawanVersion", "Select AppKey + NwkKey for 1.1.0, or AppKey-only for 1.0.x devices.");
    appendFieldHelp("region", "Frequency plan. Use EU868 for this course in Europe.");
  }

  function moveUplinkIntervalToLorawanProfile() {
    const uplinkLabel = $("uplinkInterval")?.closest("label");
    const lorawanGrid = document.querySelector(".device-row-lorawan .compact-grid");
    if (!uplinkLabel || !lorawanGrid || uplinkLabel.parentElement === lorawanGrid) {
      return;
    }
    lorawanGrid.classList.remove("three");
    lorawanGrid.classList.add("four");
    lorawanGrid.appendChild(uplinkLabel);
  }

  function formatterPlaceholder() {
    const out = $("payloadFormatterOutput");
    if (out) {
      out.textContent = "Payload formatter generation is temporarily disabled in this cleanup commit.";
    }
  }

  enhanceHero();
  populateLorawanKeyModelOptions();
  populateRegionOptions();
  moveUplinkIntervalToLorawanProfile();
  enhanceLorawanProfileFields();
  formatterPlaceholder();
})();
