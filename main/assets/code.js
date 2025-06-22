/*
 * File: main/assets/code.js
 * Project:    PianoGuard_DCM-1
 * Version:    7.8.10
 * Author:     Andrew Ballard
 * Date:       Jun 30 2025
 */

function scanNetworks() {
    console.log("JS: scanNetworks()");
    const list = document.getElementById("networks-list");
    list.innerHTML = "<p>Scanning…</p>";

    fetch("/scan", { method: "POST" })
      .then(r => {
        console.log("JS: /scan status", r.status);
        return r.json();
      })
      .then(data => {
        console.log("JS: scan data", data);
        if (!Array.isArray(data) || data.length === 0) {
          list.innerHTML = "<p>No networks found.</p>";
          return;
        }
        list.innerHTML = "<h3>Available Networks:</h3>";
        const ul = document.createElement("ul");
        ul.className = "networks";
        data.forEach(net => {
          const li = document.createElement("li");
          const btn = document.createElement("button");
          btn.textContent = `${net.ssid} (${net.rssi} dBm)`;
          btn.onclick = () => selectNetwork(net.ssid);
          li.appendChild(btn);
          ul.appendChild(li);
        });
        list.appendChild(ul);
      })
      .catch(e => {
        console.error("JS: scan error", e);
        list.innerHTML = `<p>Error: ${e.message}</p>`;
      });
}

function selectNetwork(ssid) {
    console.log("JS: selectNetwork(", ssid, ")");
    const list = document.getElementById("networks-list");
    list.innerHTML = `
      <h3>Connect to "${ssid}"</h3>
      <form id="connect-form">
        <input type="hidden" id="ssid" value="${ssid}">
        <p>Password:</p>
        <input type="password" id="password" required>
        <button type="submit" class="button">Connect</button>
      </form>
    `;
    document.getElementById("connect-form")
            .addEventListener("submit", e => {
      e.preventDefault();
      connectToNetwork();
    });
}

function connectToNetwork() {
    const ssid = document.getElementById("ssid").value;
    const pwd  = document.getElementById("password").value;
    console.log("JS: connectToNetwork()", ssid, pwd ? "***" : "(no passwd)");

    fetch("/connect", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ ssid, password: pwd })
    })
    .then(r => {
      console.log("JS: /connect status", r.status);
      return r.json();
    })
    .then(json => {
      console.log("JS: connect response", json);
      const list = document.getElementById("networks-list");
      if (json.success) {
        list.innerHTML = "<p><strong>Success!</strong> Rebooting…</p>";
      } else {
        list.innerHTML = "<p>Failed to connect</p>";
      }
    })
    .catch(e => {
      console.error("JS: connect error", e);
      document.getElementById("networks-list")
              .innerHTML = `<p>Error: ${e.message}</p>`;
    });
}

// ** This is the one you were missing! **
document.getElementById("scan-button")
        .addEventListener("click", scanNetworks);
