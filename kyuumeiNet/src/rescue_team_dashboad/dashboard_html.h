#ifndef DASHBOARD_HTML_H
#define DASHBOARD_HTML_H

const char* dashboard_html = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <meta name="theme-color" content="#07070a">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
    <link rel="manifest" href="./manifest.json">
    <link rel="icon" href="./icon.svg" type="image/svg+xml">
    <link rel="apple-touch-icon" href="./icon.svg">
    <title>Rescue Command Center</title>
    
    <script>
    // Force HTTP redirect on Surge to avoid Mixed Content block when connecting to local HTTP receiver
    if (window.location.protocol === 'https:' && window.location.hostname.includes('surge.sh')) {
        window.location.href = window.location.href.replace('https:', 'http:');
    }
    </script>

    <style>
        @import url('https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;500;600;700;800&family=JetBrains+Mono:wght@400;500;700&display=swap');

        :root {
            --bg-dark: #07070a;
            --accent: #00ff88;
            --accent-dim: rgba(0, 255, 136, 0.15);
            --danger: #ff3b5c;
            --danger-dim: rgba(255, 59, 92, 0.15);
            --warning: #ffaa00;
            --text-primary: #e8e8ea;
            --text-muted: #6b6b7b;
            --border: rgba(255, 255, 255, 0.06);
            --glass: rgba(255, 255, 255, 0.04);
            --card-border: rgba(255, 255, 255, 0.05);
        }

        * { margin: 0; padding: 0; box-sizing: border-box; }

        body {
            font-family: 'Outfit', sans-serif;
            background: var(--bg-dark);
            color: var(--text-primary);
            min-height: 100vh;
            overflow-x: hidden;
            display: flex;
            justify-content: center;
            align-items: flex-start;
            padding: 16px;
        }

        body::before {
            content: '';
            position: fixed;
            top: -40%; left: 50%;
            transform: translateX(-50%);
            width: 120vw; height: 80vh;
            background: radial-gradient(ellipse, rgba(0,255,136,0.04) 0%, transparent 70%);
            pointer-events: none; z-index: 0;
            transition: background 1s ease;
        }
        body.has-alerts::before {
            background: radial-gradient(ellipse, rgba(255,59,92,0.08) 0%, transparent 70%);
        }

        /* =========================================
           LOCK SCREEN
           ========================================= */
        .lock-screen {
            position: fixed; inset: 0;
            z-index: 9999;
            background: var(--bg-dark);
            display: flex; flex-direction: column;
            align-items: center; justify-content: center;
            transition: opacity 0.5s ease, transform 0.5s ease;
        }
        .lock-screen.hidden {
            opacity: 0; transform: scale(1.1);
            pointer-events: none;
        }

        .lock-logo {
            width: 90px; height: 90px;
            margin-bottom: 28px;
            position: relative;
        }
        .lock-logo .radar-ring {
            position: absolute; border-radius: 50%;
            border: 1.5px solid rgba(0,255,136,0.2);
            top: 50%; left: 50%; transform: translate(-50%,-50%);
        }
        .lock-logo .radar-ring:nth-child(1) { width: 40px; height: 40px; }
        .lock-logo .radar-ring:nth-child(2) { width: 65px; height: 65px; }
        .lock-logo .radar-ring:nth-child(3) { width: 88px; height: 88px; }
        .lock-logo .radar-dot {
            position: absolute; top: 50%; left: 50%;
            width: 10px; height: 10px; margin: -5px;
            background: var(--accent); border-radius: 50%;
            box-shadow: 0 0 16px var(--accent);
        }

        .lock-title {
            font-size: 1.5rem; font-weight: 700;
            letter-spacing: -0.02em; margin-bottom: 6px;
        }
        .lock-subtitle {
            font-size: 0.82rem; color: var(--text-muted);
            margin-bottom: 36px;
        }

        /* PIN Dots */
        .pin-dots {
            display: flex; gap: 16px; margin-bottom: 36px;
        }
        .pin-dot {
            width: 16px; height: 16px;
            border-radius: 50%;
            border: 2px solid rgba(255,255,255,0.15);
            transition: all 0.2s ease;
        }
        .pin-dot.filled {
            background: var(--accent);
            border-color: var(--accent);
            box-shadow: 0 0 12px rgba(0,255,136,0.4);
        }
        .pin-dot.error {
            background: var(--danger);
            border-color: var(--danger);
            box-shadow: 0 0 12px rgba(255,59,92,0.4);
            animation: shake 0.4s ease;
        }
        @keyframes shake {
            0%, 100% { transform: translateX(0); }
            20%, 60% { transform: translateX(-6px); }
            40%, 80% { transform: translateX(6px); }
        }

        /* Numpad */
        .numpad {
            display: grid;
            grid-template-columns: repeat(3, 72px);
            gap: 14px;
        }
        .numpad-btn {
            width: 72px; height: 72px;
            border-radius: 50%;
            border: 1px solid rgba(255,255,255,0.08);
            background: rgba(255,255,255,0.03);
            color: var(--text-primary);
            font-family: 'Outfit', sans-serif;
            font-size: 1.5rem; font-weight: 600;
            cursor: pointer;
            transition: all 0.15s ease;
            display: flex; align-items: center; justify-content: center;
            outline: none;
            -webkit-tap-highlight-color: transparent;
        }
        .numpad-btn:active {
            background: var(--accent-dim);
            border-color: var(--accent);
            transform: scale(0.92);
        }
        .numpad-btn.fn { font-size: 1.1rem; }
        .numpad-btn.biometric {
            border-color: rgba(0,255,136,0.2);
            background: rgba(0,255,136,0.05);
        }

        .lock-error-msg {
            color: var(--danger);
            font-size: 0.8rem;
            margin-top: 18px;
            min-height: 20px;
            transition: opacity 0.3s;
        }

        .lock-setup-hint {
            font-size: 0.75rem;
            color: var(--text-muted);
            margin-top: 8px;
        }

        /* =========================================
           ALARM OVERLAY
           ========================================= */
        .alarm-overlay {
            position: fixed; inset: 0;
            z-index: 5000;
            background: rgba(255,20,50,0.12);
            display: flex; flex-direction: column;
            align-items: center; justify-content: center;
            pointer-events: none;
            opacity: 0;
            transition: opacity 0.3s ease;
        }
        .alarm-overlay.active {
            opacity: 1;
            pointer-events: all;
            animation: alarmFlash 0.8s ease infinite alternate;
        }
        @keyframes alarmFlash {
            0% { background: rgba(255,20,50,0.05); }
            100% { background: rgba(255,20,50,0.15); }
        }
        .alarm-text {
            font-size: 2rem; font-weight: 800;
            color: var(--danger);
            letter-spacing: 0.15em;
            text-transform: uppercase;
            animation: alarmPulse 0.6s ease infinite alternate;
        }
        @keyframes alarmPulse {
            0% { transform: scale(1); opacity: 0.8; }
            100% { transform: scale(1.08); opacity: 1; }
        }
        .alarm-sub {
            font-size: 0.9rem; color: rgba(255,255,255,0.6);
            margin-top: 10px;
        }
        .btn-silence {
            margin-top: 30px;
            padding: 14px 40px;
            border: 2px solid var(--danger);
            border-radius: 14px;
            background: var(--danger-dim);
            color: var(--danger);
            font-family: 'Outfit', sans-serif;
            font-size: 1rem; font-weight: 700;
            cursor: pointer;
            letter-spacing: 0.05em;
            transition: all 0.2s ease;
            outline: none;
        }
        .btn-silence:active {
            background: var(--danger);
            color: white;
            transform: scale(0.95);
        }

        /* =========================================
           INSTALL BANNER
           ========================================= */
        .install-banner {
            position: fixed; bottom: 0; left: 0; right: 0;
            z-index: 2000;
            background: linear-gradient(135deg, #0d1117 0%, #111827 100%);
            border-top: 1px solid rgba(0,255,136,0.15);
            padding: 18px 20px;
            display: flex; align-items: center; justify-content: space-between;
            gap: 14px;
            transform: translateY(100%);
            transition: transform 0.5s cubic-bezier(0.22,1,0.36,1);
            backdrop-filter: blur(20px);
            -webkit-backdrop-filter: blur(20px);
        }
        .install-banner.visible { transform: translateY(0); }

        .install-info { flex: 1; }
        .install-info .title {
            font-size: 0.95rem; font-weight: 700;
            display: flex; align-items: center; gap: 8px;
        }
        .install-info .desc {
            font-size: 0.72rem; color: var(--text-muted); margin-top: 3px;
        }
        .btn-install {
            padding: 10px 24px;
            border: none; border-radius: 12px;
            background: var(--accent);
            color: #07070a;
            font-family: 'Outfit', sans-serif;
            font-size: 0.85rem; font-weight: 700;
            cursor: pointer;
            letter-spacing: 0.03em;
            transition: all 0.2s ease;
            white-space: nowrap;
        }
        .btn-install:active { transform: scale(0.95); }
        .btn-dismiss {
            padding: 10px 14px;
            border: 1px solid var(--border); border-radius: 12px;
            background: transparent; color: var(--text-muted);
            font-family: 'Outfit', sans-serif;
            font-size: 0.75rem; font-weight: 500;
            cursor: pointer;
        }

        /* =========================================
           MAIN CONTAINER & LAYOUT
           ========================================= */
        .container {
            width: 100%;
            max-width: 1200px;
            display: flex;
            flex-direction: column;
            gap: 20px;
            position: relative;
            z-index: 1;
            opacity: 0;
            transition: opacity 0.5s ease;
        }
        .container.visible { opacity: 1; }

        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: var(--glass);
            border: 1px solid var(--card-border);
            backdrop-filter: blur(20px);
            border-radius: 16px;
            padding: 16px 24px;
        }

        .logo-area {
            display: flex;
            align-items: center;
            gap: 12px;
        }
        .logo-dot {
            width: 10px; height: 10px;
            background: var(--danger);
            border-radius: 50%;
            box-shadow: 0 0 12px var(--danger);
            animation: pulseDot 1.5s infinite alternate;
        }
        .logo-text h1 {
            font-size: 16px; font-weight: 800; letter-spacing: 1px;
            text-transform: uppercase;
        }
        .logo-text .subtitle {
            font-size: 9px; color: var(--text-muted); font-weight: 600;
            letter-spacing: 2px; text-transform: uppercase; margin-top: 1px;
        }

        .header-controls {
            display: flex; align-items: center; gap: 12px;
        }

        .audio-btn {
            background: rgba(255, 255, 255, 0.03);
            border: 1px solid var(--border);
            color: var(--text-primary);
            padding: 8px 16px; border-radius: 30px;
            cursor: pointer; font-size: 11px; font-weight: 700;
            letter-spacing: 0.5px; display: inline-flex;
            align-items: center; gap: 8px; transition: all 0.3s ease;
            outline: none;
        }
        .audio-btn:hover { background: rgba(255,255,255,0.08); }
        .audio-btn.active {
            background: rgba(0, 255, 136, 0.08);
            border-color: var(--accent);
            color: var(--accent);
            box-shadow: 0 0 10px var(--accent-dim);
        }

        .conn-badge {
            display: flex; align-items: center; gap: 6px;
            padding: 8px 16px; border-radius: 30px;
            font-size: 11px; font-weight: 700;
            letter-spacing: 0.05em;
        }
        .conn-badge.online { background: var(--accent-dim); color: var(--accent); border: 1px solid rgba(0, 255, 136, 0.3); }
        .conn-badge.offline { background: var(--danger-dim); color: var(--danger); border: 1px solid rgba(255, 59, 92, 0.3); }
        .conn-badge.standby { background: rgba(255, 255, 255, 0.05); color: var(--text-muted); border: 1px solid var(--border); }
        .conn-badge .bdot {
            width: 7px; height: 7px; border-radius: 50%;
            background: currentColor;
        }
        .conn-badge.online .bdot { animation: pulseDot 1.5s infinite; }

        @keyframes pulseDot {
            0%, 100% { opacity: 1; transform: scale(1); }
            50% { opacity: 0.4; transform: scale(0.7); }
        }

        /* Stats Grid */
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
            gap: 16px;
        }
        .stat-card {
            background: var(--glass);
            border: 1px solid var(--card-border);
            border-radius: 16px;
            padding: 16px 20px;
            backdrop-filter: blur(10px);
            transition: all 0.3s ease;
        }
        .stat-card:hover {
            transform: translateY(-2px);
            border-color: rgba(255,255,255,0.1);
        }
        .stat-card.alert-active {
            border-color: rgba(255, 51, 68, 0.3);
            box-shadow: 0 0 16px rgba(255, 51, 68, 0.1);
        }
        .stat-label {
            font-size: 10px; font-weight: 700; color: var(--text-muted);
            text-transform: uppercase; letter-spacing: 1.5px; margin-bottom: 6px;
        }
        .stat-value {
            font-size: 26px; font-weight: 800;
        }
        .stat-value.danger { color: var(--danger); text-shadow: 0 0 10px var(--danger-dim); }
        .stat-value.warning { color: var(--warning); }
        .stat-subtext {
            font-size: 10px; color: var(--text-muted); margin-top: 4px; font-weight: 500;
        }

        /* Settings Panel */
        .settings-panel {
            background: var(--glass);
            border: 1px solid var(--accent);
            box-shadow: 0 0 15px rgba(0, 255, 136, 0.08);
            border-radius: 16px;
            padding: 16px;
            backdrop-filter: blur(10px);
            display: none;
            flex-direction: column;
            gap: 12px;
            animation: slideDown 0.3s ease;
        }
        @keyframes slideDown {
            from { opacity: 0; transform: translateY(-10px); }
            to { opacity: 1; transform: translateY(0); }
        }

        .switch-container {
            display: flex; align-items: center;
            justify-content: space-between; width: 100%;
            padding: 4px 0;
        }
        .switch {
            position: relative; display: inline-block;
            width: 44px; height: 24px;
        }
        .switch input { opacity: 0; width: 0; height: 0; }
        .slider {
            position: absolute; cursor: pointer;
            top: 0; left: 0; right: 0; bottom: 0;
            background-color: rgba(255, 255, 255, 0.08);
            border: 1px solid var(--border);
            transition: .3s; border-radius: 24px;
        }
        .slider:before {
            position: absolute; content: "";
            height: 16px; width: 16px; left: 3px; bottom: 3px;
            background-color: var(--text-muted);
            transition: .3s; border-radius: 50%;
        }
        input:checked + .slider {
            background-color: var(--accent-dim);
            border-color: var(--accent);
        }
        input:checked + .slider:before {
            transform: translateX(20px);
            background-color: var(--accent);
            box-shadow: 0 0 8px var(--accent);
        }

        .settings-input-group {
            display: flex; flex-direction: column; gap: 6px;
        }
        .settings-input-row { display: flex; gap: 8px; }
        .settings-input {
            flex: 1; background: rgba(0, 0, 0, 0.35);
            border: 1px solid var(--border); border-radius: 8px;
            padding: 8px 12px; color: var(--text-primary);
            font-family: 'JetBrains Mono', monospace; font-size: 0.82rem;
            outline: none; transition: border-color 0.2s;
        }
        .settings-input:focus { border-color: var(--accent); }
        .settings-btn {
            background: var(--accent-dim); border: 1px solid var(--accent);
            color: var(--accent); border-radius: 8px; padding: 8px 14px;
            font-family: 'Outfit', sans-serif; font-size: 0.75rem;
            font-weight: 600; cursor: pointer; transition: all 0.2s;
            outline: none;
        }
        .settings-btn:active { transform: scale(0.95); }

        /* Dashboard Body columns */
        .dashboard-body {
            display: grid;
            grid-template-columns: 320px 1fr;
            gap: 20px;
        }
        @media (max-width: 900px) {
            .dashboard-body {
                grid-template-columns: 1fr;
            }
        }

        .panel-card {
            background: var(--glass);
            border: 1px solid var(--card-border);
            backdrop-filter: blur(20px);
            border-radius: 16px;
            padding: 20px;
        }
        .panel-title {
            font-size: 11px; font-weight: 800; color: var(--text-muted);
            text-transform: uppercase; letter-spacing: 1.5px;
            border-bottom: 1px solid var(--border);
            padding-bottom: 10px; margin-bottom: 16px;
        }

        /* Radar Component */
        .radar-panel {
            background: rgba(0, 0, 0, 0.15);
            border-radius: 12px;
            border: 1px solid rgba(255,255,255,0.01);
            padding: 20px 10px;
            display: flex; flex-direction: column;
            align-items: center; justify-content: center;
            margin-bottom: 16px;
        }
        .radar {
            position: relative; width: 120px; height: 120px;
            border-radius: 50%; border: 2px solid rgba(0, 255, 136, 0.1);
            background: radial-gradient(circle, rgba(0,255,136,0.02) 0%, transparent 70%);
            margin-bottom: 12px; overflow: hidden;
        }
        .radar::before {
            content: ''; position: absolute; inset: 0;
            border-radius: 50%; border: 1px solid rgba(0,255,136,0.04);
            transform: scale(0.66);
        }
        .radar::after {
            content: ''; position: absolute; inset: 0;
            border-radius: 50%; border: 1px solid rgba(0,255,136,0.04);
            transform: scale(0.33);
        }
        .radar-sweep {
            position: absolute; inset: 0;
            background: conic-gradient(from 0deg, rgba(0,255,136,0.12) 0deg, transparent 120deg);
            border-radius: 50%; animation: radarSweep 4s linear infinite;
            transform-origin: center;
        }
        .radar-status-text {
            font-size: 10px; font-weight: 700; color: var(--accent);
            letter-spacing: 1px; animation: pulseOpacity 1.5s infinite alternate;
        }
        @keyframes radarSweep { from { transform: rotate(0); } to { transform: rotate(360deg); } }
        @keyframes pulseOpacity { from { opacity: 0.4; } to { opacity: 1; } }

        /* Diagnostic Lists */
        .info-list { display: flex; flex-direction: column; gap: 10px; }
        .info-item {
            display: flex; justify-content: space-between; align-items: center;
            font-size: 12px; border-bottom: 1px dashed rgba(255,255,255,0.03);
            padding-bottom: 8px;
        }
        .info-key { color: var(--text-muted); font-weight: 500; }
        .info-val { font-family: 'JetBrains Mono', monospace; font-weight: 600; }

        /* WiFi manager network list item */
        .wifi-net-item {
            font-size: 12px; padding: 8px 10px;
            background: rgba(255,255,255,0.02);
            border: 1px solid var(--border); border-radius: 8px;
            cursor: pointer; display: flex; justify-content: space-between;
            align-items: center; margin-top: 4px; transition: all 0.2s;
        }
        .wifi-net-item:hover { background: rgba(255,255,255,0.05); border-color: rgba(255,255,255,0.1); }

        /* Alerts card styles */
        .alert-list { display: flex; flex-direction: column; gap: 12px; }
        .alert-card {
            background: rgba(255, 255, 255, 0.02);
            border: 1px solid var(--border);
            border-radius: 14px; padding: 16px;
            position: relative; overflow: hidden;
            transition: border-color 0.2s;
        }
        .alert-card::before {
            content: ''; position: absolute; left: 0; top: 0; bottom: 0;
            width: 4px; background: var(--danger);
        }
        .alert-card.resolved::before { background: var(--accent); }

        .alert-top { display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; }
        .alert-id {
            font-family: 'JetBrains Mono', monospace; font-size: 0.82rem; font-weight: 700;
            color: var(--danger); display: flex; align-items: center; gap: 6px;
        }
        .alert-card.resolved .alert-id { color: var(--accent); }
        .alert-time { font-size: 0.68rem; color: var(--text-muted); font-family: 'JetBrains Mono', monospace; }

        .alert-coords { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
        .coord-box { background: rgba(0,0,0,0.25); border-radius: 8px; padding: 8px 10px; }
        .coord-lbl { font-size: 9px; text-transform: uppercase; color: var(--text-muted); letter-spacing: 0.5px; margin-bottom: 2px; }
        .coord-val { font-family: 'JetBrains Mono', monospace; font-size: 0.78rem; font-weight: 600; }

        .distance-indicator {
            font-size: 0.72rem; color: var(--accent); display: flex; align-items: center; gap: 4px;
            margin-top: 8px; font-family: 'JetBrains Mono', monospace;
        }
        .distance-indicator.warning { color: var(--warning); }

        .alert-footer { display: flex; justify-content: space-between; align-items: center; margin-top: 12px; }
        .rssi-badge {
            font-family: 'JetBrains Mono', monospace; font-size: 0.68rem; padding: 4px 8px;
            border-radius: 6px; background: rgba(255,255,255,0.03); color: var(--text-muted);
        }
        
        .btn-map {
            display: inline-flex; align-items: center; gap: 4px;
            padding: 6px 12px; border: 1px solid var(--border);
            border-radius: 8px; background: transparent; color: var(--text-primary);
            font-size: 0.72rem; font-weight: 500; cursor: pointer; text-decoration: none;
            transition: all 0.25s ease;
        }
        .btn-map:hover { border-color: var(--accent); color: var(--accent); background: var(--accent-dim); }

        .btn-resolve {
            padding: 6px 14px; border: none; border-radius: 8px;
            font-family: 'Outfit'; font-size: 0.72rem; font-weight: 700;
            cursor: pointer; letter-spacing: 0.02em; transition: all 0.2s ease;
            background: var(--danger-dim); color: var(--danger); outline: none;
        }
        .btn-resolve:hover { background: var(--danger); color: white; box-shadow: 0 0 10px rgba(255, 59, 92, 0.2); }
        .btn-resolve:active { transform: scale(0.96); }
        
        .btn-resolved {
            padding: 6px 14px; border: none; border-radius: 8px;
            font-family: 'Outfit'; font-size: 0.72rem; font-weight: 700;
            background: var(--accent-dim); color: var(--accent); cursor: default;
        }

        .filter-pill {
            background: rgba(255,255,255,0.02); border: 1px solid var(--border);
            border-radius: 16px; padding: 6px 14px; font-size: 0.7rem;
            font-weight: 600; color: var(--text-muted); cursor: pointer; transition: all 0.2s;
            user-select: none;
        }
        .filter-pill:hover { color: var(--text-primary); border-color: rgba(255,255,255,0.1); }
        .filter-pill.active { background: var(--accent-dim); border-color: var(--accent); color: var(--accent); }

        .empty-state { text-align: center; padding: 48px 16px; color: var(--text-muted); }

        /* Audio + Settings Buttons */
        .fab-row {
            position: fixed; bottom: 20px; right: 20px;
            display: flex; flex-direction: column; gap: 10px;
            z-index: 100;
        }
        .fab-btn {
            width: 44px; height: 44px; border-radius: 50%;
            border: 1px solid var(--border); background: var(--glass);
            backdrop-filter: blur(10px);
            color: var(--text-muted); font-size: 1.1rem;
            cursor: pointer; transition: all 0.25s ease;
            display: flex; align-items: center; justify-content: center;
            outline: none;
        }
        .fab-btn.active { border-color: var(--accent); color: var(--accent); background: var(--accent-dim); }
        .fab-btn.alarm-active { border-color: var(--danger); color: var(--danger); background: var(--danger-dim); animation: pulseDot 1s ease infinite; }

        ::-webkit-scrollbar { width: 4px; height: 4px; }
        ::-webkit-scrollbar-track { background: transparent; }
        ::-webkit-scrollbar-thumb { background: var(--border); border-radius: 4px; }
    </style>
</head>
<body>

    <!-- ======== LOCK SCREEN ======== -->
    <div class="lock-screen" id="lockScreen">
        <div class="lock-logo">
            <div class="radar-ring"></div>
            <div class="radar-ring"></div>
            <div class="radar-ring"></div>
            <div class="radar-dot"></div>
        </div>
        <div class="lock-title" id="lockTitle">Rescue HQ</div>
        <div class="lock-subtitle" id="lockSubtitle">Enter PIN to unlock</div>

        <div class="pin-dots" id="pinDots">
            <div class="pin-dot"></div>
            <div class="pin-dot"></div>
            <div class="pin-dot"></div>
            <div class="pin-dot"></div>
        </div>

        <div class="numpad" id="numpad">
            <button class="numpad-btn" data-key="1">1</button>
            <button class="numpad-btn" data-key="2">2</button>
            <button class="numpad-btn" data-key="3">3</button>
            <button class="numpad-btn" data-key="4">4</button>
            <button class="numpad-btn" data-key="5">5</button>
            <button class="numpad-btn" data-key="6">6</button>
            <button class="numpad-btn" data-key="7">7</button>
            <button class="numpad-btn" data-key="8">8</button>
            <button class="numpad-btn" data-key="9">9</button>
            <button class="numpad-btn fn biometric" id="btnBiometric" data-key="bio">🔒</button>
            <button class="numpad-btn" data-key="0">0</button>
            <button class="numpad-btn fn" data-key="del">⌫</button>
        </div>

        <div class="lock-error-msg" id="lockError"></div>
        <div class="lock-setup-hint" id="lockSetupHint"></div>
    </div>

    <!-- ======== ALARM OVERLAY ======== -->
    <div class="alarm-overlay" id="alarmOverlay">
        <div class="alarm-text">🚨 SOS RECEIVED 🚨</div>
        <div class="alarm-sub">New emergency alert incoming</div>
        <button class="btn-silence" onclick="silenceAlarm()">SILENCE ALARM</button>
    </div>

    <!-- ======== INSTALL BANNER ======== -->
    <div class="install-banner" id="installBanner">
        <div class="install-info">
            <div class="title">📲 Install Rescue HQ</div>
            <div class="desc">Add as app for instant access</div>
        </div>
        <button class="btn-dismiss" onclick="dismissInstall()">Later</button>
        <button class="btn-install" id="btnInstall" onclick="installApp()">Install</button>
    </div>

    <!-- ======== MAIN CONTAINER ======== -->
    <div class="container" id="dashboard">
        <!-- Header -->
        <header>
            <div class="logo-area">
                <div class="logo-dot" id="headerSosDot" style="display: none;"></div>
                <div class="logo-text">
                    <h1>Rescue Command Center</h1>
                    <div class="subtitle">Tactical Monitor Station</div>
                </div>
            </div>
            <div class="header-controls">
                <button class="audio-btn" id="btnSettingsToggle" onclick="toggleSettingsPanel()" title="Connection Settings">
                    ⚙️ SETTINGS
                </button>
                <button class="audio-btn" id="audioToggleBtn" onclick="toggleAudioMute()">
                    <span>🔇</span> AUDIO MUTED
                </button>
                <div class="conn-badge standby" id="connBadge">
                    <div class="bdot"></div> STANDBY
                </div>
            </div>
        </header>

        <!-- Stats Grid -->
        <div class="stats-grid">
            <div class="stat-card" id="alertCountCard">
                <div class="stat-label">Active Emergencies</div>
                <div class="stat-value danger" id="activeCount">0</div>
                <div class="stat-subtext" id="alertCardSub">All areas currently secure</div>
            </div>
            
            <div class="stat-card">
                <div class="stat-label">Resolved Beacons</div>
                <div class="stat-value" id="resolvedCount">0</div>
                <div class="stat-subtext">Saved operator records</div>
            </div>
            
            <div class="stat-card">
                <div class="stat-label">Total Signals</div>
                <div class="stat-value warning" id="totalCount">0</div>
                <div class="stat-subtext" id="lastUpdated">Last updated: —</div>
            </div>
        </div>

        <!-- Collapsible Settings Panel -->
        <div class="settings-panel" id="settingsPanel">
            <div style="display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--border); padding-bottom: 8px; margin-bottom: 8px;">
                <span style="font-size: 0.78rem; font-weight: 700; text-transform: uppercase; letter-spacing: 0.05em; color: var(--accent);">📡 Live Sync Settings</span>
                <span id="syncStatusLabel" style="font-size: 0.68rem; color: var(--text-muted); font-family: 'JetBrains Mono', monospace;">STANDBY</span>
            </div>
            <div class="switch-container">
                <span style="font-size: 0.85rem; color: var(--text-primary); font-weight: 500;">Enable Live Data Sync</span>
                <label class="switch">
                    <input type="checkbox" id="syncToggle" onchange="toggleSync(this.checked)">
                    <span class="slider"></span>
                </label>
            </div>
        </div>

        <!-- Dashboard Body Layout -->
        <div class="dashboard-body">
            
            <!-- Left Sidebar -->
            <div class="panel-card monitor-card">
                <div class="panel-title">Radar Scan & Diagnostics</div>
                
                <div class="radar-panel">
                    <div class="radar">
                        <div class="radar-sweep"></div>
                    </div>
                    <div class="radar-status-text" id="radarStatusText">MONITORING FREQ...</div>
                </div>

                <div class="info-list">
                    <div class="info-item">
                        <span class="info-key">Radio Config</span>
                        <span class="info-val">Semtech SX1278</span>
                    </div>
                    <div class="info-item">
                        <span class="info-key">Frequency</span>
                        <span class="info-val" style="color: var(--accent);">433.00 MHz</span>
                    </div>
                    <div class="info-item">
                        <span class="info-key">Session Signals</span>
                        <span class="info-val" id="sessionPackets">0</span>
                    </div>
                    <div class="info-item">
                        <span class="info-key">Station Uptime</span>
                        <span class="info-val" id="stationUptime">0:00:00</span>
                    </div>
                </div>

                <!-- WiFi Station Connection Manager -->
                <div class="panel-title" style="margin-top: 20px; border-top: 1px solid var(--border); padding-top: 15px;">WiFi Router Settings</div>
                <div class="wifi-manager-panel" style="background: rgba(0, 0, 0, 0.25); border-radius: 12px; padding: 15px; display: flex; flex-direction: column; gap: 10px;">
                    <div id="wifiStatusDisplay" style="font-size: 13px; color: var(--text-secondary); line-height: 1.4;">
                        Status: <span id="currentWifiSsid" style="color: var(--text-primary); font-weight: 600;">DISCONNECTED</span><br>
                        Local IP: <span id="currentWifiIp" style="color: var(--accent); font-weight: 600;">-</span>
                    </div>
                    <button class="btn-scan-wifi" id="scanWifiBtn" onclick="scanWifiNetworks()" style="background: rgba(51, 168, 255, 0.08); border: 1px solid rgba(51, 168, 255, 0.2); color: var(--accent); padding: 8px; border-radius: 8px; cursor: pointer; font-size: 12px; font-weight: 600; text-transform: uppercase; transition: all 0.2s; outline: none; border-style: solid;">Scan WiFi Networks</button>
                    <div id="wifiNetworksList" style="max-height: 120px; overflow-y: auto; display: flex; flex-direction: column; gap: 5px;"></div>
                    <div id="wifiConnectForm" style="display: none; flex-direction: column; gap: 8px; border-top: 1px solid var(--border); padding-top: 8px; margin-top: 5px;">
                        <span id="selectedSsidText" style="font-size: 12px; font-weight: 600; color: var(--warning);">SSID: -</span>
                        <input type="password" id="wifiPasswordInput" placeholder="Enter WiFi Password" style="background: rgba(0, 0, 0, 0.35); border: 1px solid var(--border); border-radius: 6px; padding: 8px 10px; color: #fff; font-size: 12px; outline: none; width: 100%;">
                        <button onclick="connectToWifi()" style="background: rgba(0, 255, 136, 0.1); border: 1px solid rgba(0, 255, 136, 0.25); color: var(--success); padding: 7px; border-radius: 6px; cursor: pointer; font-size: 11px; font-weight: 700; text-transform: uppercase; transition: all 0.2s; outline: none; border-style: solid;">Connect Gateway</button>
                    </div>
                </div>
            </div>

            <!-- Right Signal Feed -->
            <div class="panel-card table-panel" style="padding: 24px;">
                <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 16px;">
                    <div class="panel-title" style="border-bottom: none; padding-bottom: 0; margin-bottom: 0;">Emergency Signal Feed</div>
                    <button class="btn-export" onclick="exportLog()" title="Export CSV log" style="background: var(--glass); border: 1px solid var(--border); color: var(--text-muted); border-radius: 8px; padding: 6px 12px; font-size: 0.72rem; font-weight: 600; cursor: pointer;">📥 Export CSV</button>
                </div>

                <!-- Filters -->
                <div class="filter-section" style="display: flex; flex-direction: column; gap: 10px; margin-bottom: 20px;">
                    <div class="search-box" style="position: relative; width: 100%;">
                        <span class="search-icon" style="position: absolute; left: 12px; top: 50%; transform: translateY(-50%); color: var(--text-muted); font-size: 0.85rem;">🔍</span>
                        <input type="text" class="search-input" id="searchInput" placeholder="Search by SOS ID, Lat, Lon..." oninput="applyFilters()" style="width: 100%; background: rgba(255,255,255,0.03); border: 1px solid var(--border); border-radius: 12px; padding: 10px 14px 10px 36px; color: var(--text-primary); font-family: 'Outfit', sans-serif; font-size: 0.85rem; outline: none;">
                    </div>
                    <div class="filter-pills" style="display: flex; gap: 8px;">
                        <div class="filter-pill active" id="pillAll" onclick="setFilter('all')">All</div>
                        <div class="filter-pill" id="pillActive" onclick="setFilter('active')">Active</div>
                        <div class="filter-pill" id="pillResolved" onclick="setFilter('resolved')">Resolved</div>
                    </div>
                </div>

                <div class="alert-list" id="alertList">
                    <div class="empty-state">
                        <div class="icon" style="font-size: 3rem; margin-bottom: 16px; opacity: 0.3;">🛡️</div>
                        <h3>All Clear</h3>
                        <p>No active SOS alerts.<br>Monitoring all channels…</p>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <!-- ======== FAB BUTTONS ======== -->
    <div class="fab-row" id="fabRow" style="display:none;">
        <button class="fab-btn" id="btnAlarmToggle" title="Mute audio alarm" onclick="toggleAudioMute()">🔕</button>
        <button class="fab-btn" id="btnLock" title="Lock screen" onclick="lockApp()">🔒</button>
    </div>

    <script>
    // =========================================
    // STATE & AUTO-ADAPTATION
    // =========================================
    const isRunningLocally = (window.location.hostname === '192.168.5.1' || window.location.hostname === 'rescue.local' || (!window.location.hostname.includes('.') && window.location.hostname !== 'localhost'));
    
    // Set connection defaults based on host origin
    let syncEnabled;
    if (localStorage.getItem('rescue_sync_enabled') !== null) {
        syncEnabled = localStorage.getItem('rescue_sync_enabled') === 'true';
    } else {
        syncEnabled = isRunningLocally; // Sync defaults ON locally, OFF on remote Surge PWA
    }
    
    const BASE_URL = isRunningLocally ? window.location.origin : 'http://192.168.5.1';

    let isConnected = false;
    let isUnlocked = false;
    let alarmMuted = true;
    let alarmPlaying = false;
    let alarmOscillators = [];
    let prevAlertIds = new Set();
    let audioCtx = null;
    let deferredInstallPrompt = null;
    
    // Feed States
    let currentFilter = 'all'; 
    let currentSearch = '';
    let operatorCoords = null;
    let allAlertsData = [];
    let lastServerMillis = 0;

    // Watch Operator GPS Location
    if ('geolocation' in navigator) {
        navigator.geolocation.watchPosition(
            (pos) => {
                operatorCoords = { lat: pos.coords.latitude, lon: pos.coords.longitude };
                if (allAlertsData.length > 0) renderAlerts({alerts: allAlertsData, serverMillis: lastServerMillis}, true);
            },
            (err) => console.log("GPS unavailable:", err),
            { enableHighAccuracy: true, timeout: 5000, maximumAge: 0 }
        );
    }

    // =========================================
    // DOM REFS
    // =========================================
    const lockScreen = document.getElementById('lockScreen');
    const dashboard = document.getElementById('dashboard');
    const alarmOverlay = document.getElementById('alarmOverlay');
    const installBanner = document.getElementById('installBanner');
    const fabRow = document.getElementById('fabRow');
    const lockError = document.getElementById('lockError');
    const lockTitle = document.getElementById('lockTitle');
    const lockSubtitle = document.getElementById('lockSubtitle');
    const lockSetupHint = document.getElementById('lockSetupHint');
    const pinDotsEl = document.getElementById('pinDots');
    const connBadge = document.getElementById('connBadge');
    const alertList = document.getElementById('alertList');
    const activeCount = document.getElementById('activeCount');
    const resolvedCount = document.getElementById('resolvedCount');
    const totalCount = document.getElementById('totalCount');
    const lastUpdated = document.getElementById('lastUpdated');
    const btnBiometric = document.getElementById('btnBiometric');
    const settingsPanel = document.getElementById('settingsPanel');
    const syncToggle = document.getElementById('syncToggle');
    const syncStatusLabel = document.getElementById('syncStatusLabel');

    // =========================================
    // PIN LOCK SYSTEM
    // =========================================
    let pinInput = '';
    let isSettingPin = !localStorage.getItem('rescue_pin');
    let confirmPin = '';
    let settingStep = 0; 

    if (isSettingPin) {
        lockTitle.textContent = 'Set Your PIN';
        lockSubtitle.textContent = 'Create a 4-digit PIN';
        lockSetupHint.textContent = 'First time setup';
        btnBiometric.style.visibility = 'hidden';
    }

    // Check platform biometric availability
    let biometricAvailable = false;
    if (window.PublicKeyCredential) {
        PublicKeyCredential.isUserVerifyingPlatformAuthenticatorAvailable()
            .then(available => {
                biometricAvailable = available;
                if (available && !isSettingPin) {
                    btnBiometric.textContent = '👆';
                    btnBiometric.title = 'Use Biometrics';
                }
            }).catch(() => {});
    }

    document.getElementById('numpad').addEventListener('click', (e) => {
        const btn = e.target.closest('.numpad-btn');
        if (!btn) return;
        const key = btn.dataset.key;

        if (key === 'bio') { attemptBiometric(); return; }
        if (key === 'del') { pinInput = pinInput.slice(0, -1); updatePinDots(); return; }

        if (pinInput.length >= 4) return;
        pinInput += key;
        updatePinDots();

        if (pinInput.length === 4) {
            setTimeout(() => handlePinComplete(), 200);
        }
    });

    function updatePinDots() {
        const dots = pinDotsEl.querySelectorAll('.pin-dot');
        dots.forEach((d, i) => {
            d.classList.toggle('filled', i < pinInput.length);
            d.classList.remove('error');
        });
    }

    function handlePinComplete() {
        if (isSettingPin) {
            if (settingStep === 0) {
                confirmPin = pinInput;
                pinInput = '';
                settingStep = 1;
                lockSubtitle.textContent = 'Confirm your PIN';
                lockSetupHint.textContent = 'Re-enter the same PIN';
                updatePinDots();
            } else {
                if (pinInput === confirmPin) {
                    localStorage.setItem('rescue_pin', pinInput);
                    isSettingPin = false;
                    unlockApp();
                } else {
                    showPinError('PINs don\'t match. Try again.');
                    settingStep = 0;
                    confirmPin = '';
                    lockSubtitle.textContent = 'Create a 4-digit PIN';
                    lockSetupHint.textContent = 'First time setup';
                }
            }
        } else {
            const savedPin = localStorage.getItem('rescue_pin');
            if (pinInput === savedPin) {
                unlockApp();
            } else {
                showPinError('Wrong PIN');
            }
        }
    }

    function showPinError(msg) {
        pinInput = '';
        const dots = pinDotsEl.querySelectorAll('.pin-dot');
        dots.forEach(d => { d.classList.add('error'); d.classList.remove('filled'); });
        lockError.textContent = msg;
        setTimeout(() => {
            dots.forEach(d => d.classList.remove('error'));
            lockError.textContent = '';
        }, 1200);
    }

    async function attemptBiometric() {
        if (isSettingPin) return;
        try {
            const credId = localStorage.getItem('rescue_cred_id');
            if (!credId) {
                const challenge = new Uint8Array(32);
                crypto.getRandomValues(challenge);
                const createOptions = {
                    publicKey: {
                        challenge: challenge,
                        rp: { name: 'Rescue HQ', id: location.hostname || 'localhost' },
                        user: { id: new Uint8Array(16), name: 'rescue_user', displayName: 'Rescue Operator' },
                        pubKeyCredParams: [{ type: 'public-key', alg: -7 }],
                        authenticatorSelection: { authenticatorAttachment: 'platform', userVerification: 'required' },
                        timeout: 60000
                    }
                };
                const cred = await navigator.credentials.create(createOptions);
                if (cred) {
                    const idArray = Array.from(new Uint8Array(cred.rawId));
                    localStorage.setItem('rescue_cred_id', JSON.stringify(idArray));
                    unlockApp();
                }
            } else {
                const challenge = new Uint8Array(32);
                crypto.getRandomValues(challenge);
                const storedId = new Uint8Array(JSON.parse(credId));
                const getOptions = {
                    publicKey: {
                        challenge: challenge,
                        allowCredentials: [{ type: 'public-key', id: storedId, transports: ['internal'] }],
                        userVerification: 'required', timeout: 60000
                    }
                };
                const assertion = await navigator.credentials.get(getOptions);
                if (assertion) unlockApp();
            }
        } catch (err) {
            showPinError('Use PIN instead');
        }
    }

    function unlockApp() {
        isUnlocked = true;
        lockScreen.classList.add('hidden');
        dashboard.style.display = 'flex';
        dashboard.classList.add('visible');
        fabRow.style.display = 'flex';
        initAudio();
        
        // Populate inputs
        syncToggle.checked = syncEnabled;
        
        updateSyncStatusUI();
        fetchAlerts(); 
        pollWifiStatus();
    }

    function lockApp() {
        isUnlocked = false;
        pinInput = '';
        updatePinDots();
        lockScreen.classList.remove('hidden');
        dashboard.style.display = 'none';
        dashboard.classList.remove('visible');
        fabRow.style.display = 'none';
        silenceAlarm();
    }

    // =========================================
    // AUDIO & ALARM Siren SYSTEM
    // =========================================
    function initAudio() {
        if (!audioCtx) {
            audioCtx = new (window.AudioContext || window.webkitAudioContext)();
        }
    }

    function playAlertSound() {
        if (alarmMuted || !audioCtx) return;
        try {
            if (audioCtx.state === 'suspended') audioCtx.resume();
            const osc = audioCtx.createOscillator();
            const gain = audioCtx.createGain();
            osc.connect(gain);
            gain.connect(audioCtx.destination);
            osc.type = 'sine';
            osc.frequency.setValueAtTime(1000, audioCtx.currentTime); 
            osc.frequency.exponentialRampToValueAtTime(250, audioCtx.currentTime + 0.5); 
            gain.gain.setValueAtTime(0.2, audioCtx.currentTime);
            gain.gain.exponentialRampToValueAtTime(0.001, audioCtx.currentTime + 0.6); 
            osc.start();
            osc.stop(audioCtx.currentTime + 0.6);
        } catch(e) {}
    }

    function startAlarm() {
        if (alarmMuted || alarmPlaying || !audioCtx) return;
        alarmPlaying = true;
        alarmOverlay.classList.add('active');

        function createSirenCycle() {
            if (!alarmPlaying) return;
            const now = audioCtx.currentTime;
            const duration = 0.6;

            const osc1 = audioCtx.createOscillator();
            const gain1 = audioCtx.createGain();
            osc1.type = 'sawtooth';
            osc1.frequency.setValueAtTime(880, now);
            osc1.frequency.linearRampToValueAtTime(1200, now + duration);
            gain1.gain.setValueAtTime(0.3, now);
            gain1.gain.setValueAtTime(0.3, now + duration - 0.05);
            gain1.gain.linearRampToValueAtTime(0, now + duration);
            osc1.connect(gain1); gain1.connect(audioCtx.destination);
            osc1.start(now); osc1.stop(now + duration);
            alarmOscillators.push(osc1);

            const osc2 = audioCtx.createOscillator();
            const gain2 = audioCtx.createGain();
            osc2.type = 'sawtooth';
            osc2.frequency.setValueAtTime(1200, now + duration);
            osc2.frequency.linearRampToValueAtTime(880, now + duration * 2);
            gain2.gain.setValueAtTime(0.3, now + duration);
            gain2.gain.setValueAtTime(0.3, now + duration * 2 - 0.05);
            gain2.gain.linearRampToValueAtTime(0, now + duration * 2);
            osc2.connect(gain2); gain2.connect(audioCtx.destination);
            osc2.start(now + duration); osc2.stop(now + duration * 2);
            alarmOscillators.push(osc2);

            setTimeout(createSirenCycle, duration * 2 * 1000);
        }

        createSirenCycle();
        if (navigator.vibrate) navigator.vibrate([300, 100, 300, 100, 300]);
    }

    function silenceAlarm() {
        alarmPlaying = false;
        alarmOverlay.classList.remove('active');
        alarmOscillators.forEach(o => { try { o.stop(); } catch(e) {} });
        alarmOscillators = [];
        if (navigator.vibrate) navigator.vibrate(0);
    }

    function toggleAudioMute() {
        alarmMuted = !alarmMuted;
        const btn = document.getElementById('audioToggleBtn');
        const fab = document.getElementById('btnAlarmToggle');
        
        btn.textContent = alarmMuted ? '🔇 AUDIO MUTED' : '🔊 AUDIO ACTIVE';
        btn.classList.toggle('active', !alarmMuted);
        
        fab.textContent = alarmMuted ? '🔕' : '🔔';
        fab.classList.toggle('active', !alarmMuted);
        
        if (alarmMuted) silenceAlarm();
        else playAlertSound(); // test sound
    }

    // =========================================
    // PWA INSTALL
    // =========================================
    window.addEventListener('beforeinstallprompt', (e) => {
        e.preventDefault();
        deferredInstallPrompt = e;
        setTimeout(() => {
            installBanner.classList.add('visible');
        }, 2000);
    });

    function installApp() {
        if (deferredInstallPrompt) {
            deferredInstallPrompt.prompt();
            deferredInstallPrompt.userChoice.then(result => {
                installBanner.classList.remove('visible');
                deferredInstallPrompt = null;
            });
        }
    }

    function dismissInstall() { installBanner.classList.remove('visible'); }

    window.addEventListener('appinstalled', () => {
        installBanner.classList.remove('visible');
        deferredInstallPrompt = null;
    });

    if ('serviceWorker' in navigator) {
        navigator.serviceWorker.register('./sw.js').catch(() => {});
    }

    // =========================================
    // SYNC SETTINGS HANDLERS
    // =========================================
    function toggleSettingsPanel() {
        settingsPanel.style.display = (settingsPanel.style.display === 'flex') ? 'none' : 'flex';
    }

    function toggleSync(enabled) {
        syncEnabled = enabled;
        localStorage.setItem('rescue_sync_enabled', enabled ? 'true' : 'false');
        updateSyncStatusUI();
        if (enabled) fetchAlerts();
        else showDisconnected();
    }



    function updateSyncStatusUI() {
        if (syncEnabled) {
            syncStatusLabel.textContent = 'ACTIVE POLLING';
            syncStatusLabel.style.color = 'var(--accent)';
            document.getElementById('radarStatusText').textContent = 'MONITORING FREQ...';
        } else {
            syncStatusLabel.textContent = 'STANDBY';
            syncStatusLabel.style.color = 'var(--text-muted)';
            document.getElementById('radarStatusText').textContent = 'STANDBY MODE';
        }
    }

    // =========================================
    // CLIENT DASHBOARD DATA STREAM
    // =========================================
    function getSignalBars(rssi) {
        if (rssi > -70) return '▉▉▉▉';
        if (rssi > -85) return '▉▉▉░';
        if (rssi > -100) return '▉▉░░';
        if (rssi > -110) return '▉░░░';
        return '░░░░';
    }

    function formatTime(ms) {
        const s = Math.floor(ms / 1000);
        const m = Math.floor(s / 60);
        const h = Math.floor(m / 60);
        if (h > 0) return `${h}h ${m%60}m ago`;
        if (m > 0) return `${m}m ${s%60}s ago`;
        return `${s}s ago`;
    }

    function showConnected() {
        if (!isConnected) {
            isConnected = true;
            connBadge.className = 'conn-badge online';
            connBadge.innerHTML = '<div class="bdot"></div> LIVE';
        }
    }

    function showDisconnected() {
        isConnected = false;
        connBadge.className = syncEnabled ? 'conn-badge offline' : 'conn-badge standby';
        connBadge.innerHTML = syncEnabled ? '<div class="bdot"></div> OFFLINE' : '<div class="bdot"></div> STANDBY';
    }

    function getDistance(lat1, lon1, lat2, lon2) {
        const R = 6371; 
        const dLat = (lat2 - lat1) * Math.PI / 180;
        const dLon = (lon2 - lon1) * Math.PI / 180;
        const a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
                  Math.cos(lat1 * Math.PI / 180) * Math.cos(lat2 * Math.PI / 180) *
                  Math.sin(dLon / 2) * Math.sin(dLon / 2);
        const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
        const d = R * c;
        if (d < 1) return `${Math.round(d * 1000)}m`;
        return `${d.toFixed(1)}km`;
    }

    function getBearing(lat1, lon1, lat2, lon2) {
        const dLon = (lon2 - lon1) * Math.PI / 180;
        const lat1Rad = lat1 * Math.PI / 180;
        const lat2Rad = lat2 * Math.PI / 180;
        const y = Math.sin(dLon) * Math.cos(lat2Rad);
        const x = Math.cos(lat1Rad) * Math.sin(lat2Rad) -
                  Math.sin(lat1Rad) * Math.cos(lat2Rad) * Math.cos(dLon);
        const brng = (Math.atan2(y, x) * 180 / Math.PI + 360) % 360;
        const directions = ['N', 'NNE', 'NE', 'ENE', 'E', 'ESE', 'SE', 'SSE', 'S', 'SSW', 'SW', 'WSW', 'W', 'WNW', 'NW', 'NNW'];
        return directions[Math.round(brng / 22.5) % 16];
    }

    function setFilter(filterType) {
        currentFilter = filterType;
        document.querySelectorAll('.filter-pill').forEach(el => el.classList.remove('active'));
        if (filterType === 'all') document.getElementById('pillAll').classList.add('active');
        if (filterType === 'active') document.getElementById('pillActive').classList.add('active');
        if (filterType === 'resolved') document.getElementById('pillResolved').classList.add('active');
        applyFilters();
    }

    function applyFilters() {
        currentSearch = document.getElementById('searchInput').value.toLowerCase();
        if (allAlertsData.length > 0) {
            renderAlerts({alerts: allAlertsData, serverMillis: lastServerMillis}, true); 
        }
    }

    function exportLog() {
        if (allAlertsData.length === 0) return alert("No data to export.");
        let csvContent = "data:text/csv;charset=utf-8,ID,Status,Latitude,Longitude,RSSI,Timestamp\n";
        allAlertsData.forEach(a => {
            let status = a.active ? "Active" : "Resolved";
            let dateStr = new Date(a.timestamp || a.time).toISOString();
            csvContent += `SOS-${a.id},${status},${a.lat},${a.lon},${a.rssi},${dateStr}\n`;
        });
        const encodedUri = encodeURI(csvContent);
        const link = document.createElement("a");
        link.setAttribute("href", encodedUri);
        link.setAttribute("download", `RescueHQ_Log_${new Date().toISOString().split('T')[0]}.csv`);
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
    }

    function renderAlerts(data, isFilterRefresh = false) {
        const alerts = data.alerts || [];
        lastServerMillis = data.serverMillis || 0;
        if (!isFilterRefresh) allAlertsData = alerts; 

        let active = 0, resolved = 0;
        alerts.forEach(a => a.active ? active++ : resolved++);

        activeCount.textContent = active;
        resolvedCount.textContent = resolved;
        totalCount.textContent = alerts.length;

        document.body.classList.toggle('has-alerts', active > 0);
        document.getElementById('alertCountCard').classList.toggle('alert-active', active > 0);
        document.getElementById('alertCardSub').textContent = active > 0 ? `${active} urgent emergencies active` : "All areas currently secure";

        if (!isFilterRefresh) {
            const currentActiveIds = new Set();
            alerts.forEach(a => { if (a.active) currentActiveIds.add(a.id); });

            let hasNew = false;
            currentActiveIds.forEach(id => { if (!prevAlertIds.has(id)) hasNew = true; });

            if (hasNew) {
                playAlertSound();
                if (!alarmPlaying) startAlarm();
            }
            prevAlertIds = currentActiveIds;
        }

        let filteredAlerts = alerts.filter(a => {
            if (currentFilter === 'active' && !a.active) return false;
            if (currentFilter === 'resolved' && a.active) return false;
            if (currentSearch) {
                const searchStr = `sos-${a.id} ${a.lat} ${a.lon}`.toLowerCase();
                if (!searchStr.includes(currentSearch)) return false;
            }
            return true;
        });

        if (filteredAlerts.length === 0) {
            alertList.innerHTML = `
                <div class="empty-state">
                    <div class="icon" style="font-size: 3rem; margin-bottom: 16px; opacity: 0.3;">🛡️</div>
                    <h3>No Beacons</h3>
                    <p>No alerts match your active selection filters.</p>
                </div>`;
            return;
        }

        let html = '';
        filteredAlerts.forEach(a => {
            const elapsed = formatTime(lastServerMillis - (a.timestamp || a.time));
            const isActive = a.active;
            
            let distHtml = '';
            if (operatorCoords && a.lat && a.lon) {
                const alat = parseFloat(a.lat);
                const alon = parseFloat(a.lon);
                if (!isNaN(alat) && !isNaN(alon) && alat !== 0 && alon !== 0) {
                    const d = getDistance(operatorCoords.lat, operatorCoords.lon, alat, alon);
                    const b = getBearing(operatorCoords.lat, operatorCoords.lon, alat, alon);
                    distHtml = `<div class="distance-indicator ${isActive ? 'warning' : ''}">🧭 ${d} away (${b})</div>`;
                }
            }

            html += `
            <div class="alert-card ${isActive ? '' : 'resolved'}">
                <div class="alert-top">
                    <div class="alert-id">
                        ${isActive ? '🔴' : '✅'} SOS-${String(a.id).padStart(3, '0')}
                    </div>
                    <div class="alert-time">${elapsed}</div>
                </div>
                <div class="alert-coords">
                    <div class="coord-box">
                        <div class="coord-lbl">Latitude</div>
                        <div class="coord-val">${a.lat}</div>
                    </div>
                    <div class="coord-box">
                        <div class="coord-lbl">Longitude</div>
                        <div class="coord-val">${a.lon}</div>
                    </div>
                </div>
                ${distHtml}
                <div class="alert-footer">
                    <span class="rssi-badge">
                        ${getSignalBars(a.rssi)} ${a.rssi} dBm
                    </span>
                    <div style="display:flex;gap:8px;">
                        <a class="btn-map" href="https://www.google.com/maps?q=${a.lat},${a.lon}" target="_blank">
                            📍 Map
                        </a>
                        ${isActive
                            ? `<button class="btn-resolve" onclick="resolveAlert(${a.id}, this)">RESOLVE</button>`
                            : `<span class="btn-resolved">✓ Resolved</span>`
                        }
                    </div>
                </div>
            </div>`;
        });

        alertList.innerHTML = html;
    }

    function resolveAlert(id, btn) {
        if (btn) { btn.disabled = true; btn.textContent = 'ACKING...'; }
        fetch(`${BASE_URL}/resolve?id=${id}`)
            .then(res => {
                if (res.ok) {
                    if (btn) btn.textContent = 'SAFE';
                    fetchAlerts();
                    silenceAlarm();
                } else {
                    if (btn) { btn.disabled = false; btn.textContent = 'RESOLVE'; }
                }
            })
            .catch(() => {
                if (btn) { btn.disabled = false; btn.textContent = 'Retry'; }
            });
    }

    function fetchAlerts() {
        if (!isUnlocked) return;
        if (!syncEnabled) { showDisconnected(); return; }

        fetch(`${BASE_URL}/get_alerts`, { mode: 'cors', cache: 'no-cache' })
            .then(res => {
                if (!res.ok) throw new Error('fail');
                return res.json();
            })
            .then(data => {
                showConnected();
                renderAlerts(data);
                const now = new Date();
                lastUpdated.textContent = `Last updated: ${now.toLocaleTimeString()}`;
            })
            .catch(() => {
                showDisconnected();
            });
    }

    // =========================================
    // WIFI MANAGER SETTINGS CALLS
    // =========================================
    let selectedSsid = '';
    let wifiPollTimer = null;

    function pollWifiStatus() {
        if (!isUnlocked) return;
        fetch(`${BASE_URL}/wifi_status`)
            .then(res => res.json())
            .then(data => {
                const ssidEl = document.getElementById('currentWifiSsid');
                const ipEl = document.getElementById('currentWifiIp');
                if (data.status === 'CONNECTED') {
                    ssidEl.textContent = data.ssid;
                    ipEl.textContent = data.ip;
                    ssidEl.style.color = 'var(--accent)';
                    if (wifiPollTimer) { clearInterval(wifiPollTimer); wifiPollTimer = null; }
                    document.getElementById('wifiConnectForm').style.display = 'none';
                } else if (data.status === 'CONNECTING') {
                    ssidEl.textContent = 'CONNECTING...';
                    ipEl.textContent = '-';
                    ssidEl.style.color = 'var(--warning)';
                } else {
                    ssidEl.textContent = 'DISCONNECTED';
                    ipEl.textContent = '-';
                    ssidEl.style.color = 'var(--text-muted)';
                }
            })
            .catch(() => {});
    }

    function scanWifiNetworks() {
        const btn = document.getElementById('scanWifiBtn');
        const list = document.getElementById('wifiNetworksList');
        btn.disabled = true;
        btn.textContent = 'Scanning...';
        list.innerHTML = '<div style="font-size: 11px; color: var(--text-muted); padding: 5px;">Scanning WiFi frequencies...</div>';

        fetch(`${BASE_URL}/scan_wifi`)
            .then(res => res.json())
            .then(data => {
                btn.disabled = false;
                btn.textContent = 'Scan WiFi Networks';
                list.innerHTML = '';
                if (data.networks && data.networks.length > 0) {
                    data.networks.forEach(net => {
                        const item = document.createElement('div');
                        item.className = 'wifi-net-item';
                        item.onclick = () => selectWifiNetwork(net.ssid);
                        const secureIcon = net.secure ? '🔒' : '🔓';
                        item.innerHTML = `<span>📶 ${net.ssid}</span> <span style="font-size: 10px; color: var(--text-muted);">${secureIcon} ${net.rssi} dBm</span>`;
                        list.appendChild(item);
                    });
                } else {
                    list.innerHTML = '<div style="font-size: 11px; color: var(--text-muted); padding: 5px;">No networks detected</div>';
                }
            })
            .catch(() => {
                btn.disabled = false;
                btn.textContent = 'Scan WiFi Networks';
                list.innerHTML = '<div style="font-size: 11px; color: var(--danger); padding: 5px;">Scan failed. Try again.</div>';
            });
    }

    function selectWifiNetwork(ssid) {
        selectedSsid = ssid;
        document.getElementById('selectedSsidText').textContent = 'Selected SSID: ' + ssid;
        document.getElementById('wifiConnectForm').style.display = 'flex';
        document.getElementById('wifiPasswordInput').value = '';
        document.getElementById('wifiPasswordInput').focus();
    }

    function connectToWifi() {
        const pass = document.getElementById('wifiPasswordInput').value;
        const ssidEl = document.getElementById('currentWifiSsid');
        ssidEl.textContent = 'CONNECTING...';
        ssidEl.style.color = 'var(--warning)';

        fetch(`${BASE_URL}/connect_wifi?ssid=${encodeURIComponent(selectedSsid)}&pass=${encodeURIComponent(pass)}`)
            .then(res => res.json())
            .then(() => {
                if (wifiPollTimer) clearInterval(wifiPollTimer);
                wifiPollTimer = setInterval(pollWifiStatus, 1500);
            })
            .catch(() => {
                ssidEl.textContent = 'ERROR';
                ssidEl.style.color = 'var(--danger)';
            });
    }

    // =========================================
    // LOOPS & INITIALIZATION
    // =========================================
    setInterval(fetchAlerts, 2000);
    setInterval(pollWifiStatus, 5000);
    </script>
</body>
</html>
)rawliteral";

#endif
