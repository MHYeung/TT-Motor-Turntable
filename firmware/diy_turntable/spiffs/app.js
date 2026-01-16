// Simple dashboard for ESP-IDF esp_http_server
// API:
//   GET /api/motor              -> returns {"ok":true,"speed":x,"dir":y}
//   GET /api/motor?dir=&speed=  -> sets and returns state

const speedEl = document.getElementById('speed');
const spdText = document.getElementById('spdText');
const dirText = document.getElementById('dirText');
const statusText = document.getElementById('statusText');
const connBadge = document.getElementById('connBadge');

const revBtn = document.getElementById('revBtn');
const stopBtn = document.getElementById('stopBtn');
const fwdBtn = document.getElementById('fwdBtn');
const minusBtn = document.getElementById('minusBtn');
const plusBtn = document.getElementById('plusBtn');

const refreshBtn = document.getElementById('refreshBtn');
const btn10 = document.getElementById('btn10');
const btn30 = document.getElementById('btn30');
const btn50 = document.getElementById('btn50');
const btn80 = document.getElementById('btn80');

let dir = 0; // -1 reverse, 0 stop, 1 forward
let lastOkTs = 0;
let debounceTimer = null;

function clamp(v, lo, hi) {
  return Math.max(lo, Math.min(hi, v));
}

function dirLabel(d) {
  if (d > 0) return "FWD";
  if (d < 0) return "REV";
  return "STOP";
}

function setConn(ok, msg) {
  if (ok) {
    connBadge.textContent = "Online";
    connBadge.classList.add("ok");
    connBadge.classList.remove("bad");
  } else {
    connBadge.textContent = "Offline";
    connBadge.classList.add("bad");
    connBadge.classList.remove("ok");
  }
  if (msg) statusText.textContent = msg;
}

function updateUI(speedPct, d) {
  spdText.textContent = String(speedPct);
  dirText.textContent = dirLabel(d);
  speedEl.value = String(speedPct);

  // highlight buttons
  revBtn.classList.toggle("active", d < 0);
  fwdBtn.classList.toggle("active", d > 0);
  stopBtn.classList.toggle("active", d === 0);
}

async function apiGet(path) {
  const r = await fetch(path, { cache: "no-store" });
  if (!r.ok) throw new Error(`HTTP ${r.status}`);
  return r.json();
}

async function refreshState() {
  try {
    const j = await apiGet(`/api/motor`);
    // Expect j.speed and j.dir
    dir = Number(j.dir ?? 0);
    const spd = Number(j.speed ?? 0);
    updateUI(clamp(spd, 0, 100), clamp(dir, -1, 1));
    lastOkTs = Date.now();
    setConn(true, "State updated");
  } catch (e) {
    setConn(false, `Refresh failed: ${e.message}`);
  }
}

async function sendState(speedPct, d) {
  speedPct = clamp(speedPct, 0, 100);
  d = clamp(d, -1, 1);

  // if STOP, always speed 0
  if (d === 0) speedPct = 0;

  // optimistic UI update
  updateUI(speedPct, d);

  try {
    const j = await apiGet(`/api/motor?dir=${d}&speed=${speedPct}`);
    dir = Number(j.dir ?? d);
    const spd = Number(j.speed ?? speedPct);
    updateUI(clamp(spd, 0, 100), clamp(dir, -1, 1));
    lastOkTs = Date.now();
    setConn(true, `OK · speed=${spd}% dir=${dirLabel(dir)}`);
  } catch (e) {
    setConn(false, `Send failed: ${e.message}`);
  }
}

function debounceSend(speedPct, d) {
  clearTimeout(debounceTimer);
  debounceTimer = setTimeout(() => sendState(speedPct, d), 80);
}

// --- Events ---
speedEl.addEventListener('input', () => {
  const spd = Number(speedEl.value);
  spdText.textContent = String(spd);
  // If user raises speed while stopped, keep STOP until they press FWD/REV.
  // (More predictable UX)
  debounceSend(spd, dir);
});

fwdBtn.onclick = () => {
  const spd = Number(speedEl.value);
  dir = 1;
  sendState(spd, dir);
};

revBtn.onclick = () => {
  const spd = Number(speedEl.value);
  dir = -1;
  sendState(spd, dir);
};

stopBtn.onclick = () => {
  dir = 0;
  sendState(0, dir);
};

minusBtn.onclick = () => {
  const spd = clamp(Number(speedEl.value) - 5, 0, 100);
  sendState(spd, dir);
};

plusBtn.onclick = () => {
  const spd = clamp(Number(speedEl.value) + 5, 0, 100);
  sendState(spd, dir);
};

btn10.onclick = () => sendState(10, dir === 0 ? 1 : dir);
btn30.onclick = () => sendState(30, dir === 0 ? 1 : dir);
btn50.onclick = () => sendState(50, dir === 0 ? 1 : dir);
btn80.onclick = () => sendState(80, dir === 0 ? 1 : dir);

refreshBtn.onclick = () => refreshState();

// Periodic connectivity check (lightweight)
setInterval(() => {
  const age = Date.now() - lastOkTs;
  if (age > 5000) {
    // if we haven't had a success recently, try refresh
    refreshState();
  }
}, 5000);

// Init
updateUI(0, 0);
refreshState();
