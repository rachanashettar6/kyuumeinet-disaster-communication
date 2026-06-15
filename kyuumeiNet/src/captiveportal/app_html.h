#ifndef APP_HTML_H
#define APP_HTML_H

const char* app_html = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SOS Mobile Portal</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;500;600;700;800&family=JetBrains+Mono:wght@400;500;700&display=swap');

        :root {
            --bg-dark: #08080c;
            --bg-glow: radial-gradient(circle at 50% 0%, #200d11 0%, #08080c 75%);
            --card-bg: rgba(16, 16, 26, 0.7);
            --card-border: rgba(255, 255, 255, 0.05);
            
            --primary: hsl(354, 100%, 60%);
            --primary-glow: rgba(255, 51, 68, 0.35);
            --success: hsl(145, 100%, 55%);
            --success-glow: rgba(0, 255, 136, 0.25);
            --warning: hsl(38, 100%, 55%);
            --warning-glow: rgba(255, 170, 0, 0.2);
            --accent: hsl(195, 100%, 55%);
            
            --text-primary: #ffffff;
            --text-secondary: #9ea0b2;
            --text-muted: #64647c;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }

        body {
            background-color: var(--bg-dark);
            background-image: var(--bg-glow);
            color: var(--text-primary);
            font-family: 'Outfit', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 16px;
        }

        .app-card {
            width: 100%;
            max-width: 380px;
            background: var(--card-bg);
            border: 1px solid var(--card-border);
            backdrop-filter: blur(20px);
            border-radius: 28px;
            padding: 28px 24px;
            box-shadow: 0 16px 40px rgba(0,0,0,0.4);
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 20px;
            text-align: center;
        }

        /* --- Header Status Area --- */
        .header-area {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
            width: 100%;
        }

        .status-capsule {
            background: rgba(255, 255, 255, 0.03);
            border: 1px solid rgba(255, 255, 255, 0.05);
            padding: 6px 14px;
            border-radius: 30px;
            display: inline-flex;
            align-items: center;
            gap: 8px;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1px;
            text-transform: uppercase;
        }

        .status-dot {
            width: 8px;
            height: 8px;
            background-color: var(--text-muted);
            border-radius: 50%;
            transition: all 0.3s ease;
        }

        /* Dot States */
        .status-capsule.idle .status-dot {
            background-color: var(--primary);
            box-shadow: 0 0 8px var(--primary);
        }

        .status-capsule.sending .status-dot {
            background-color: var(--warning);
            box-shadow: 0 0 8px var(--warning);
            animation: blink 0.8s infinite alternate;
        }

        .status-capsule.success .status-dot {
            background-color: var(--success);
            box-shadow: 0 0 8px var(--success);
        }

        h1 {
            font-size: 18px;
            font-weight: 800;
            letter-spacing: 1px;
            margin-top: 6px;
            background: linear-gradient(90deg, #ffffff, #a2a2bd);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        .subtitle {
            font-size: 11px;
            color: var(--text-muted);
            font-weight: 700;
            letter-spacing: 1.5px;
            text-transform: uppercase;
        }

        /* --- GPS Panel --- */
        .gps-panel {
            background: rgba(255, 255, 255, 0.02);
            border: 1px solid rgba(255, 255, 255, 0.04);
            border-radius: 12px;
            padding: 12px 16px;
            width: 100%;
            text-align: left;
            display: flex;
            flex-direction: column;
            gap: 6px;
        }

        .gps-header {
            display: flex;
            align-items: center;
            gap: 6px;
            font-size: 10px;
            font-weight: 800;
            color: var(--text-secondary);
            letter-spacing: 1px;
        }

        .gps-icon {
            width: 12px;
            height: 12px;
            color: var(--primary);
        }

        .gps-coords {
            font-family: 'JetBrains Mono', monospace;
            font-size: 13px;
            font-weight: 500;
            color: var(--accent);
        }

        .gps-notice {
            font-size: 10px;
            color: var(--text-muted);
            line-height: 1.4;
        }

        /* --- Glowing SOS Button --- */
        .btn-outer {
            position: relative;
            width: 190px;
            height: 190px;
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .sos-btn {
            width: 160px;
            height: 160px;
            border-radius: 50%;
            border: none;
            font-family: 'Outfit', sans-serif;
            font-size: 18px;
            font-weight: 900;
            letter-spacing: 0.5px;
            cursor: pointer;
            background: linear-gradient(135deg, hsl(354, 100%, 55%), hsl(340, 100%, 45%));
            color: #ffffff;
            box-shadow: 0 0 25px var(--primary-glow), inset 0 2px 4px rgba(255,255,255,0.2);
            position: relative;
            z-index: 2;
            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
            outline: none;
        }

        .btn-outer::before, .btn-outer::after {
            content: '';
            position: absolute;
            top: 15px; left: 15px; right: 15px; bottom: 15px;
            border-radius: 50%;
            border: 2px solid var(--primary);
            opacity: 0.35;
            z-index: 1;
            transition: all 0.3s ease;
        }

        .btn-outer::before {
            animation: ripple 2s infinite cubic-bezier(0.1, 0.8, 0.3, 1);
        }

        .btn-outer::after {
            animation: ripple 2s infinite 0.7s cubic-bezier(0.1, 0.8, 0.3, 1);
        }

        .sos-btn:active {
            transform: scale(0.96);
        }

        /* Button Transmitting State */
        .sos-btn.loading {
            background: linear-gradient(135deg, hsl(38, 100%, 50%), hsl(30, 100%, 40%));
            box-shadow: 0 0 25px var(--warning-glow);
        }

        .btn-outer.sending::before, .btn-outer.sending::after {
            border-color: var(--warning);
            animation-duration: 1.2s;
        }

        /* Button Success State */
        .sos-btn.success {
            background: linear-gradient(135deg, hsl(145, 100%, 45%), hsl(140, 100%, 35%));
            box-shadow: 0 0 30px var(--success-glow);
        }

        .btn-outer.success::before, .btn-outer.success::after {
            border-color: var(--success);
            animation: ripple-success 3s infinite alternate cubic-bezier(0.4, 0, 0.2, 1);
        }

        .btn-outer.success::after {
            animation-delay: 1.5s;
        }

        /* --- Acknowledgment Banner --- */
        .ack-box {
            display: none;
            align-items: center;
            gap: 14px;
            padding: 16px;
            background: rgba(0, 255, 136, 0.04);
            border: 1px solid rgba(0, 255, 136, 0.15);
            border-radius: 16px;
            color: var(--text-primary);
            text-align: left;
            width: 100%;
            animation: slideUp 0.4s cubic-bezier(0.4, 0, 0.2, 1) forwards;
        }

        .ack-icon {
            width: 32px;
            height: 32px;
            border-radius: 50%;
            background: rgba(0, 255, 136, 0.08);
            border: 1px solid rgba(0, 255, 136, 0.2);
            color: var(--success);
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 16px;
            font-weight: 700;
        }

        .ack-box h4 {
            font-size: 13px;
            font-weight: 800;
            color: var(--success);
            letter-spacing: 0.5px;
            margin-bottom: 2px;
        }

        .ack-box p {
            font-size: 12px;
            color: var(--text-secondary);
            line-height: 1.3;
        }

        /* --- Cancel Button --- */
        .cancel-btn {
            display: none;
            background: transparent;
            border: 1px solid rgba(255, 51, 68, 0.3);
            color: var(--primary);
            padding: 10px 20px;
            border-radius: 20px;
            cursor: pointer;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1px;
            transition: all 0.2s ease;
            width: 100%;
            outline: none;
            text-transform: uppercase;
        }

        .cancel-btn:hover {
            background: rgba(255, 51, 68, 0.06);
            border-color: var(--primary);
        }

        /* --- Animations --- */
        @keyframes ripple {
            0% { transform: scale(1); opacity: 0.8; }
            100% { transform: scale(1.35); opacity: 0; }
        }

        @keyframes ripple-success {
            0% { transform: scale(1); opacity: 0.5; }
            100% { transform: scale(1.2); opacity: 0; }
        }

        @keyframes blink {
            0% { opacity: 0.3; }
            100% { opacity: 1; }
        }

        @keyframes slideUp {
            from { opacity: 0; transform: translateY(12px); }
            to { opacity: 1; transform: translateY(0); }
        }

        /* --- Secure Context Warning Banner --- */
        .secure-context-banner {
            display: flex;
            align-items: center;
            gap: 10px;
            background: rgba(255, 170, 0, 0.06);
            border: 1px solid rgba(255, 170, 0, 0.2);
            border-radius: 14px;
            padding: 10px 14px;
            width: 100%;
            text-align: left;
            margin-bottom: 4px;
        }

        .secure-context-banner .banner-icon {
            font-size: 16px;
            color: var(--warning);
        }

        .secure-context-banner .banner-text {
            font-size: 11px;
            color: var(--text-secondary);
            line-height: 1.4;
        }

        /* --- Manual GPS Panel --- */
        .manual-gps-panel {
            background: rgba(255, 255, 255, 0.02);
            border: 1px solid rgba(255, 255, 255, 0.05);
            border-radius: 16px;
            padding: 16px;
            width: 100%;
            display: flex;
            flex-direction: column;
            gap: 14px;
            text-align: left;
            animation: slideUp 0.3s ease forwards;
        }

        .panel-header {
            font-size: 11px;
            font-weight: 800;
            color: var(--warning);
            letter-spacing: 1px;
            text-transform: uppercase;
        }

        .paste-box-container {
            display: flex;
            gap: 8px;
            width: 100%;
        }

        .paste-box-container input {
            flex: 1;
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 10px 12px;
            color: var(--text-primary);
            font-family: inherit;
            font-size: 13px;
            outline: none;
            transition: border-color 0.2s;
        }

        .paste-box-container input:focus {
            border-color: var(--accent);
        }

        .btn-parse {
            background: var(--accent);
            border: none;
            border-radius: 10px;
            color: var(--bg-dark);
            font-weight: 700;
            font-size: 12px;
            padding: 0 16px;
            cursor: pointer;
            transition: opacity 0.2s;
        }

        .btn-parse:active {
            opacity: 0.8;
        }

        .input-coords-row {
            display: flex;
            gap: 12px;
            width: 100%;
        }

        .coord-field {
            flex: 1;
            display: flex;
            flex-direction: column;
            gap: 6px;
        }

        .coord-field label {
            font-size: 10px;
            font-weight: 700;
            color: var(--text-secondary);
            text-transform: uppercase;
        }

        .coord-field input {
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 10px 12px;
            color: var(--text-primary);
            font-family: 'JetBrains Mono', monospace;
            font-size: 13px;
            outline: none;
            transition: border-color 0.2s;
        }

        .coord-field input:focus {
            border-color: var(--accent);
        }

        /* Tabs and Instructions */
        .instructions-tabs {
            background: rgba(0, 0, 0, 0.15);
            border: 1px solid rgba(255, 255, 255, 0.03);
            border-radius: 12px;
            padding: 10px;
            display: flex;
            flex-direction: column;
            gap: 8px;
        }

        .tab-header {
            display: flex;
            gap: 6px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
            padding-bottom: 6px;
        }

        .tab-btn {
            background: transparent;
            border: none;
            color: var(--text-muted);
            font-family: inherit;
            font-size: 11px;
            font-weight: 700;
            cursor: pointer;
            padding: 4px 8px;
            border-bottom: 2px solid transparent;
            transition: all 0.2s;
        }

        .tab-btn.active {
            color: var(--accent);
            border-bottom-color: var(--accent);
        }

        .tab-content {
            font-size: 11px;
            color: var(--text-secondary);
            line-height: 1.5;
        }

        .toggle-manual-btn {
            background: transparent;
            border: none;
            color: var(--accent);
            cursor: pointer;
            font-size: 11px;
            font-weight: 700;
            text-decoration: underline;
            outline: none;
        }
    </style>
</head>
<body>

    <div class="app-card">
        <div class="header-area">
            <div class="status-capsule idle" id="statusCapsule">
                <span class="status-dot"></span>
                <span id="statusLabel">Status: IDLE</span>
            </div>
            <h1>SOS EMERGENCY BEACON</h1>
            <p class="subtitle">WiFi Link Mode (No Internet Needed)</p>
        </div>

        <div class="secure-context-banner" id="secureContextBanner" style="display: none;">
            <span class="banner-icon">⚠️</span>
            <span class="banner-text">Browser GPS blocked (HTTP link). Copy coordinates from Compass app & paste below.</span>
        </div>

        <div class="btn-outer" id="btnOuter">
            <button class="sos-btn" id="sosBtn" onclick="triggerSOS()">TRIGGER SOS</button>
        </div>

        <div class="gps-panel">
            <div class="gps-header" style="display: flex; justify-content: space-between; align-items: center; width: 100%;">
                <div style="display: flex; align-items: center; gap: 6px;">
                    <svg class="gps-icon" viewBox="0 0 24 24"><path fill="currentColor" d="M12 2C8.13 2 5 5.13 5 9c0 5.25 7 13 7 13s7-7.75 7-13c0-3.87-3.13-7-7-7zm0 9.5c-1.38 0-2.5-1.12-2.5-2.5s1.12-2.5 2.5-2.5 2.5 1.12 2.5 2.5-1.12 2.5-2.5 2.5z"/></svg>
                    <span>LOCATION INSTRUMENT</span>
                </div>
                <button type="button" class="toggle-manual-btn" id="toggleManualBtn" onclick="toggleManualPanel()">Enter Coordinates</button>
            </div>
            <div class="gps-coords" id="gpsLabel">
                GPS: Accessing location sensor...
            </div>
        </div>

        <div class="manual-gps-panel" id="manualGpsPanel" style="display: none;">
            <div class="panel-header">
                <span>MANUAL COORDINATES ENTRY</span>
            </div>
            <div class="paste-box-container">
                <input type="text" id="pasteInput" placeholder="Paste Compass coordinates here..." oninput="autoParsePaste()" />
                <button type="button" class="btn-parse" onclick="manualParseCoordinates()">Parse</button>
            </div>
            <div class="input-coords-row">
                <div class="coord-field">
                    <label>Latitude</label>
                    <input type="number" step="0.00001" id="manualLat" placeholder="e.g. 12.97160" oninput="updateManualCoords()" />
                </div>
                <div class="coord-field">
                    <label>Longitude</label>
                    <input type="number" step="0.00001" id="manualLon" placeholder="e.g. 77.59460" oninput="updateManualCoords()" />
                </div>
            </div>
            
            <div class="instructions-tabs">
                <div class="tab-header">
                    <button type="button" class="tab-btn active" id="btnIosTab" onclick="switchTab('ios')">iOS Instructions</button>
                    <button type="button" class="tab-btn" id="btnAndroidTab" onclick="switchTab('android')">Android Instructions</button>
                </div>
                <div class="tab-content" id="iosInst">
                    1. Open the native <b>Compass</b> app.<br>
                    2. Look at the bottom to see your Lat/Lon (e.g. 12°58'17"N 77°35'40"E).<br>
                    3. Tap and hold the coordinates to <b>Copy</b>.<br>
                    4. Return here and paste into the box above.
                </div>
                <div class="tab-content" id="androidInst" style="display: none;">
                    1. Open <b>Google Maps</b> (GPS works offline).<br>
                    2. Tap the blue dot (your location).<br>
                    3. Copy coordinates from the details sheet.<br>
                    4. Return here and paste into the box above.
                </div>
            </div>
        </div>
        
        <div class="gps-notice">
            *If GPS is blocked, enter your coordinates manually.<br>
            For secure automatic GPS: open the pre-installed PWA.
        </div>

        <div class="ack-box" id="ackLabel">
            <div class="ack-icon">✓</div>
            <div>
                <h4>RESCUE COMMITTED</h4>
                <p>Don't worry! Help is on the way.</p>
            </div>
        </div>

        <button class="cancel-btn" id="cancelBtn" onclick="cancelSOS()">Cancel Broadcast</button>
    </div>

    <script>
        let latitude = 0.0;
        let longitude = 0.0;
        let isPolling = false;
        let pollTimer = null;
        let audioCtx = null;
        let playedAckChime = false;

        function playSound(type) {
            try {
                if (!audioCtx) {
                    audioCtx = new (window.AudioContext || window.webkitAudioContext)();
                }
                if (audioCtx.state === 'suspended') {
                    audioCtx.resume();
                }
                
                const now = audioCtx.currentTime;
                
                if (type === 'trigger') {
                    const osc1 = audioCtx.createOscillator();
                    const gain1 = audioCtx.createGain();
                    osc1.connect(gain1);
                    gain1.connect(audioCtx.destination);
                    osc1.frequency.setValueAtTime(880, now);
                    gain1.gain.setValueAtTime(0.15, now);
                    gain1.gain.exponentialRampToValueAtTime(0.001, now + 0.15);
                    osc1.start(now);
                    osc1.stop(now + 0.15);

                    const osc2 = audioCtx.createOscillator();
                    const gain2 = audioCtx.createGain();
                    osc2.connect(gain2);
                    gain2.connect(audioCtx.destination);
                    osc2.frequency.setValueAtTime(1100, now + 0.1);
                    gain2.gain.setValueAtTime(0.15, now + 0.1);
                    gain2.gain.exponentialRampToValueAtTime(0.001, now + 0.25);
                    osc2.start(now + 0.1);
                    osc2.stop(now + 0.25);
                } else if (type === 'ack') {
                    const notes = [523.25, 659.25, 783.99]; // C5, E5, G5
                    notes.forEach((freq, idx) => {
                        const osc = audioCtx.createOscillator();
                        const gain = audioCtx.createGain();
                        osc.connect(gain);
                        gain.connect(audioCtx.destination);
                        osc.type = 'triangle';
                        osc.frequency.setValueAtTime(freq, now + idx * 0.12);
                        gain.gain.setValueAtTime(0.2, now + idx * 0.12);
                        gain.gain.exponentialRampToValueAtTime(0.001, now + idx * 0.12 + 0.4);
                        osc.start(now + idx * 0.12);
                        osc.stop(now + idx * 0.12 + 0.4);
                    });
                }
            } catch (e) {
                console.error("Audio error:", e);
            }
        }

        // Auto-request GPS Coordinates on page load
        if (navigator.geolocation) {
            navigator.geolocation.watchPosition(
                (position) => {
                    latitude = position.coords.latitude;
                    longitude = position.coords.longitude;
                    document.getElementById('gpsLabel').innerHTML = 
                        `${latitude.toFixed(5)}, ${longitude.toFixed(5)}`;
                    document.getElementById('gpsLabel').style.color = 'var(--accent)';
                    // Prefill manual inputs if empty
                    if (document.getElementById('manualLat').value === '') {
                        document.getElementById('manualLat').value = latitude.toFixed(5);
                    }
                    if (document.getElementById('manualLon').value === '') {
                        document.getElementById('manualLon').value = longitude.toFixed(5);
                    }
                },
                (error) => {
                    console.log("GPS Error:", error);
                    let errMsg = "GPS Blocked: Swipe to enable location/GPS";
                    if (error.code === error.PERMISSION_DENIED) {
                        errMsg = "GPS Blocked: Enable location permission / GPS settings";
                    } else if (error.code === error.POSITION_UNAVAILABLE) {
                        errMsg = "GPS Signal Lost: Positioning hardware unavailable.";
                    } else if (error.code === error.TIMEOUT) {
                        errMsg = "GPS Timeout: Location request timed out.";
                    }
                    document.getElementById('gpsLabel').innerHTML = errMsg;
                    document.getElementById('gpsLabel').style.color = 'var(--primary)';
                    // Auto-open manual coordinates entry since automatic fails
                    showManualPanel();
                },
                { enableHighAccuracy: true, timeout: 10000, maximumAge: 0 }
            );
        } else {
            document.getElementById('gpsLabel').innerHTML = "GPS sensor not supported by browser";
            document.getElementById('gpsLabel').style.color = 'var(--primary)';
            showManualPanel();
        }

        function toggleManualPanel() {
            const panel = document.getElementById('manualGpsPanel');
            const btn = document.getElementById('toggleManualBtn');
            if (panel.style.display === 'none') {
                panel.style.display = 'flex';
                btn.innerHTML = 'Hide Manual Entry';
            } else {
                panel.style.display = 'none';
                btn.innerHTML = 'Enter Coordinates';
            }
        }

        function showManualPanel() {
            document.getElementById('manualGpsPanel').style.display = 'flex';
            document.getElementById('toggleManualBtn').innerHTML = 'Hide Manual Entry';
        }

        function switchTab(os) {
            document.getElementById('btnIosTab').classList.remove('active');
            document.getElementById('btnAndroidTab').classList.remove('active');
            document.getElementById('iosInst').style.display = 'none';
            document.getElementById('androidInst').style.display = 'none';
            
            if (os === 'ios') {
                document.getElementById('btnIosTab').classList.add('active');
                document.getElementById('iosInst').style.display = 'block';
            } else {
                document.getElementById('btnAndroidTab').classList.add('active');
                document.getElementById('androidInst').style.display = 'block';
            }
        }

        function updateManualCoords() {
            const latVal = parseFloat(document.getElementById('manualLat').value);
            const lonVal = parseFloat(document.getElementById('manualLon').value);
            if (!isNaN(latVal) && !isNaN(lonVal)) {
                latitude = latVal;
                longitude = lonVal;
                document.getElementById('gpsLabel').innerHTML = 
                    `${latitude.toFixed(5)}, ${longitude.toFixed(5)} (MANUAL)`;
                document.getElementById('gpsLabel').style.color = 'var(--warning)';
            }
        }

        function autoParsePaste() {
            const text = document.getElementById('pasteInput').value;
            if (!text) return;
            
            // Regex to match decimal coordinates (e.g. 12.34567, 76.54321)
            const regex = /(-?\d+\.\d+)\s*[\s,;:\/|°'\"]+\s*(-?\d+\.\d+)/;
            let match = text.match(regex);
            
            if (!match) {
                const parsed = parseDMS(text);
                if (parsed) {
                    document.getElementById('manualLat').value = parsed.lat.toFixed(5);
                    document.getElementById('manualLon').value = parsed.lon.toFixed(5);
                    updateManualCoords();
                    document.getElementById('pasteInput').value = '';
                    document.getElementById('pasteInput').placeholder = '✓ Successfully parsed coordinates!';
                    setTimeout(() => {
                        document.getElementById('pasteInput').placeholder = 'Paste Compass coordinates here...';
                    }, 3000);
                    return;
                }
            }
            
            if (match) {
                const latVal = parseFloat(match[1]);
                const lonVal = parseFloat(match[2]);
                if (!isNaN(latVal) && !isNaN(lonVal)) {
                    document.getElementById('manualLat').value = latVal.toFixed(5);
                    document.getElementById('manualLon').value = lonVal.toFixed(5);
                    updateManualCoords();
                    document.getElementById('pasteInput').value = '';
                    document.getElementById('pasteInput').placeholder = '✓ Successfully parsed coordinates!';
                    setTimeout(() => {
                        document.getElementById('pasteInput').placeholder = 'Paste Compass coordinates here...';
                    }, 3000);
                }
            }
        }

        function manualParseCoordinates() {
            autoParsePaste();
        }

        function parseDMS(str) {
            try {
                // Matches degrees° minutes' seconds" (N/S/E/W)
                const dmsRegex = /(\d+)\s*°\s*(\d+)\s*'\s*([\d.]+)\s*"\s*([NSEWnsew])/g;
                let matches = [...str.matchAll(dmsRegex)];
                if (matches.length >= 2) {
                    let lat = 0, lon = 0;
                    for (let i = 0; i < 2; i++) {
                        const deg = parseFloat(matches[i][1]);
                        const min = parseFloat(matches[i][2]);
                        const sec = parseFloat(matches[i][3]);
                        const dir = matches[i][4].toUpperCase();
                        
                        let decimal = deg + (min / 60.0) + (sec / 3600.0);
                        if (dir === 'S' || dir === 'W') {
                            decimal = -decimal;
                        }
                        
                        if (dir === 'N' || dir === 'S') lat = decimal;
                        if (dir === 'E' || dir === 'W') lon = decimal;
                    }
                    if (lat !== 0 && lon !== 0) {
                        return { lat, lon };
                    }
                }
            } catch (e) {
                console.error("DMS parsing error:", e);
            }
            return null;
        }

        // Check if the current environment is secure (HTTPS or localhost)
        function checkSecureContext() {
            if (!window.isSecureContext) {
                document.getElementById('secureContextBanner').style.display = 'flex';
                showManualPanel();
            }
        }

        // Check current status on page load to sync with ESP32 state
        window.addEventListener('DOMContentLoaded', () => {
            checkCurrentStatus();
            checkSecureContext();
        });

        function checkCurrentStatus() {
            fetch('/status?t=' + Date.now())
                .then(response => response.json())
                .then(data => {
                    const btn = document.getElementById('sosBtn');
                    const btnOuter = document.getElementById('btnOuter');
                    const statusCapsule = document.getElementById('statusCapsule');
                    const statusLabel = document.getElementById('statusLabel');
                    const ack = document.getElementById('ackLabel');
                    const cancel = document.getElementById('cancelBtn');

                    if (data.status === "SENDING") {
                        btn.disabled = true;
                        btn.className = "sos-btn loading";
                        btnOuter.className = "btn-outer sending";
                        btn.innerHTML = "SENT";
                        statusCapsule.className = "status-capsule sending";
                        statusLabel.innerHTML = "Status: BROADCASTING";
                        ack.style.display = "none";
                        cancel.style.display = "block";
                        playedAckChime = false;
                        startPollingACK();
                    } else if (data.status === "ACKNOWLEDGED") {
                        btn.disabled = true;
                        btn.className = "sos-btn success";
                        btnOuter.className = "btn-outer success";
                        btn.innerHTML = "SAFE";
                        statusCapsule.className = "status-capsule success";
                        statusLabel.innerHTML = "Status: SAVED";
                        ack.style.display = "flex";
                        cancel.style.display = "block";
                        
                        if (!playedAckChime) {
                            playSound('ack');
                            playedAckChime = true;
                        }
                    } else {
                        btn.disabled = false;
                        btn.className = "sos-btn";
                        btnOuter.className = "btn-outer";
                        btn.innerHTML = "TRIGGER SOS";
                        statusCapsule.className = "status-capsule idle";
                        statusLabel.innerHTML = "Status: IDLE";
                        ack.style.display = "none";
                        cancel.style.display = "none";
                        playedAckChime = false;
                    }
                })
                .catch(err => {
                    console.log("Error checking initial status:", err);
                });
        }

        function triggerSOS() {
            const btn = document.getElementById('sosBtn');
            const btnOuter = document.getElementById('btnOuter');
            const statusCapsule = document.getElementById('statusCapsule');
            const statusLabel = document.getElementById('statusLabel');
            const ack = document.getElementById('ackLabel');
            const cancel = document.getElementById('cancelBtn');

            if (btn.disabled || btn.classList.contains('loading') || btn.classList.contains('success')) return;

            if (latitude === 0.0 || longitude === 0.0) {
                // Auto fallback to demo location (essential for restricted captive portal popups)
                latitude = 12.9716;  // Demo Latitude (e.g. Bangalore)
                longitude = 77.5946; // Demo Longitude
                document.getElementById('gpsLabel').innerHTML = `${latitude.toFixed(5)}, ${longitude.toFixed(5)} (DEMO)`;
            }

            if (isPolling) {
                clearInterval(pollTimer);
                isPolling = false;
            }

            // Play quick double beep confirmation sound
            playSound('trigger');

            btn.disabled = true;
            btn.className = "sos-btn loading";
            btnOuter.className = "btn-outer sending";
            btn.innerHTML = "SENT";
            statusCapsule.className = "status-capsule sending";
            statusLabel.innerHTML = "Status: BROADCASTING";
            ack.style.display = "none";
            cancel.style.display = "none";
            playedAckChime = false;

            // Local HTTP request to ESP32 Node 1
            const url = `/send_sos?lat=${latitude.toFixed(5)}&lon=${longitude.toFixed(5)}&t=${Date.now()}`;

            fetch(url)
                .then(response => {
                    if (response.ok) {
                        cancel.style.display = "block";
                        startPollingACK();
                    } else {
                        throw new Error('Server Error');
                    }
                })
                .catch(error => {
                    btn.disabled = false;
                    btn.className = "sos-btn";
                    btnOuter.className = "btn-outer";
                    btn.innerHTML = "TRIGGER SOS";
                    statusCapsule.className = "status-capsule idle";
                    statusLabel.innerHTML = "Status: OFFLINE";
                    cancel.style.display = "none";
                    console.log("Connection Failed!");
                });
        }

        function startPollingACK() {
            if (isPolling) return;
            isPolling = true;

            pollTimer = setInterval(() => {
                fetch('/status?t=' + Date.now())
                    .then(response => response.json())
                    .then(data => {
                        const btn = document.getElementById('sosBtn');
                        const btnOuter = document.getElementById('btnOuter');
                        const statusCapsule = document.getElementById('statusCapsule');
                        const statusLabel = document.getElementById('statusLabel');
                        const ack = document.getElementById('ackLabel');
                        const cancel = document.getElementById('cancelBtn');

                        if (data.status === "ACKNOWLEDGED") {
                            clearInterval(pollTimer);
                            isPolling = false;
                            
                            btn.disabled = true;
                            btn.className = "sos-btn success";
                            btnOuter.className = "btn-outer success";
                            btn.innerHTML = "SAFE";
                            statusCapsule.className = "status-capsule success";
                            statusLabel.innerHTML = "Status: SAVED";
                            ack.style.display = "flex";
                            cancel.style.display = "block";

                            if (!playedAckChime) {
                                playSound('ack');
                                playedAckChime = true;
                            }
                        } else if (data.status === "IDLE") {
                            // If ESP32 auto-timed out back to IDLE
                            clearInterval(pollTimer);
                            isPolling = false;
                            btn.disabled = false;
                            btn.className = "sos-btn";
                            btnOuter.className = "btn-outer";
                            btn.innerHTML = "TRIGGER SOS";
                            statusCapsule.className = "status-capsule idle";
                            statusLabel.innerHTML = "Status: IDLE";
                            ack.style.display = "none";
                            cancel.style.display = "none";
                            playedAckChime = false;
                        }
                    })
                    .catch(err => {
                        console.log("Polling error:", err);
                    });
            }, 1500);
        }

        function cancelSOS() {
            fetch('/reset_status?t=' + Date.now())
                .then(response => {
                    if (response.ok) {
                        const btn = document.getElementById('sosBtn');
                        const btnOuter = document.getElementById('btnOuter');
                        const statusCapsule = document.getElementById('statusCapsule');
                        const statusLabel = document.getElementById('statusLabel');
                        const ack = document.getElementById('ackLabel');
                        const cancel = document.getElementById('cancelBtn');

                        btn.disabled = false;
                        btn.className = "sos-btn";
                        btnOuter.className = "btn-outer";
                        btn.innerHTML = "TRIGGER SOS";
                        statusCapsule.className = "status-capsule idle";
                        statusLabel.innerHTML = "Status: IDLE";
                        ack.style.display = "none";
                        cancel.style.display = "none";
                        playedAckChime = false;
                        
                        if (isPolling) {
                            clearInterval(pollTimer);
                            isPolling = false;
                        }
                    }
                })
                .catch(err => {
                    console.log("Error resetting status:", err);
                });
        }
    </script>
</body>
</html>)rawliteral";

#endif
