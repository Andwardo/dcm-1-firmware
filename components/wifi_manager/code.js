/*
 * File: components/esp32-wifi-manager/code.js
 *
 * Created on: 12 June 2025 22:10:00
 * Last edited on: 12 June 2025 22:10:00
 *
 * Version: 7.8.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */
function scanNetworks() {
    console.log("Scanning for networks...");
    let list = document.getElementById('networks-list');
    list.innerHTML = "<p>Scanning...</p>";

    fetch('/api/wifi-scan')
        .then(response => response.json())
        .then(data => {
            list.innerHTML = "<h3>Available Networks:</h3>";
            let ul = document.createElement('ul');
            ul.className = "networks";
            data.forEach(net => {
                let li = document.createElement('li');
                let link = document.createElement('a');
                link.href = "#";
                link.textContent = `${net.ssid} (${net.rssi} dBm) - ${net.auth ? "Secure" : "Open"}`;
                link.onclick = function() { selectNetwork(net.ssid); return false; };
                li.appendChild(link);
                ul.appendChild(li);
            });
            list.appendChild(ul);
        })
        .catch(error => {
            console.error('Error scanning for networks:', error);
            list.innerHTML = "<p>Error scanning for networks. Please try again.</p>";
        });
}

function selectNetwork(ssid) {
    document.getElementById('networks-list').innerHTML = `
        <h3>Connect to "${ssid}"</h3>
        <form id="connect-form">
            <input type="hidden" id="ssid" name="ssid" value="${ssid}">
            <p>Password:</p>
            <input type="password" id="password" name="password" style="width: 80%; padding: 10px; font-size: 1.2rem;">
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

    console.log(`Attempting to connect to SSID: ${ssid}`);
    document.getElementById('networks-list').innerHTML = `<p>Attempting to connect to ${ssid}...</p>`;

    fetch('/api/connect', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ ssid, password }),
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            document.getElementById('networks-list').innerHTML = "<p>Success! The device will now restart and connect to your network.</p>";
        } else {
            document.getElementById('networks-list').innerHTML = `<p>Failed to connect. Please <a href="/">try again</a>.</p>`;
        }
    })
    .catch(error => {
        console.error('Connection error:', error);
        document.getElementById('networks-list').innerHTML = `<p>Connection error. Please <a href="/">try again</a>.</p>`;
    });
}
