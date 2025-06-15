/*
 * File: components/esp32-wifi-manager/code.js
 *
 * Created on: 12 June 2025 22:10:00
 * Last edited on: 15 June 2025 07:55:00 CDT
 *
 * Version: 7.8.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

function scanNetworks() {
    console.log("Scanning for networks...");
    const list = document.getElementById('networks-list');
    list.innerHTML = "<p>Scanning...</p>";

    fetch('/api/wifi-scan')
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}`);
            }
            return response.json();
        })
        .then(data => {
            if (!Array.isArray(data) || data.length === 0) {
                list.innerHTML = "<p>No networks found.</p>";
                return;
            }

            list.innerHTML = "<h3>Available Networks:</h3>";
            const ul = document.createElement('ul');
            ul.className = "networks";

            data.forEach(net => {
                const li = document.createElement('li');
                const button = document.createElement('button');
                button.textContent = `${net.ssid} (${net.rssi} dBm) - ${net.auth ? "Secure" : "Open"}`;
                button.className = "button";
                button.onclick = () => selectNetwork(net.ssid);
                li.appendChild(button);
                ul.appendChild(li);
            });

            list.appendChild(ul);
        })
        .catch(error => {
            console.error('Error scanning for networks:', error);
            list.innerHTML = `<p>Error scanning for networks: ${error.message}</p>`;
        });
}

function selectNetwork(ssid) {
    const list = document.getElementById('networks-list');
    list.innerHTML = `
        <h3>Connect to "${ssid}"</h3>
        <form id="connect-form">
            <input type="hidden" id="ssid" name="ssid" value="${ssid}">
            <p>Password:</p>
            <input type="password" id="password" name="password"
                   style="width: 80%; padding: 10px; font-size: 1.2rem;"
                   placeholder="Enter Wi-Fi password" required>
            <br/><br/>
            <button type="submit" class="button">Connect</button>
        </form>
    `;

    document.getElementById('connect-form').addEventListener('submit', function(event) {
        event.preventDefault();
        connectToNetwork();
    });
}

function connectToNetwork() {
    const ssid = document.getElementById('ssid').value;
    const password = document.getElementById('password').value;

    if (!ssid) {
        alert("SSID missing.");
        return;
    }

    console.log(`Attempting to connect to SSID: ${ssid}`);
    const list = document.getElementById('networks-list');
    list.innerHTML = `<p>Attempting to connect to ${ssid}...</p>`;

    fetch('/api/connect', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ ssid, password }),
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`Server returned ${response.status}`);
        }
        return response.json();
    })
    .then(data => {
        if (data.success) {
            list.innerHTML = "<p><strong>Success!</strong> The device will now restart and connect to your network.</p>";
        } else {
            list.innerHTML = `<p>Failed to connect. Please <a href="/">try again</a>.</p>`;
        }
    })
    .catch(error => {
        console.error('Connection error:', error);
        list.innerHTML = `<p>Connection error: ${error.message}. Please <a href="/">try again</a>.</p>`;
    });
}
