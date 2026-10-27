static const char* const PAGE_HTML = R"HTML(
<!doctype html>
<html lang="ru">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Аудит ИБ УК</title>
<style>
:root {
  --bg: #f3f5f8; --card: #ffffff; --text: #1c2733; --muted: #66758a; --line: #dde3ea;
  --primary: #2457c5; --primary-d: #1b449b; --primary-bg: #e8effc;
  --low: #1d8a4b; --low-bg: #e3f5ea; --mid: #a86a00; --mid-bg: #fdf1d9; --high: #c0392b; --high-bg: #fbe5e2;
  --radius: 10px; --shadow: 0 1px 2px rgba(20,35,60,.06), 0 4px 16px rgba(20,35,60,.06);
}
@media (prefers-color-scheme: dark) {
  :root {
    --bg: #12171e; --card: #1b222c; --text: #e6ebf2; --muted: #93a1b5; --line: #2c3745;
    --primary: #6d9bff; --primary-d: #8fb2ff; --primary-bg: #1f2d4a;
    --low: #52c985; --low-bg: #173325; --mid: #f0b44c; --mid-bg: #3a2d12; --high: #ff7d6e; --high-bg: #3d1d1a;
    --shadow: 0 1px 2px rgba(0,0,0,.4);
  }
}
* { box-sizing: border-box; }
html, body { margin: 0; }
body { font: 15px/1.45 -apple-system, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; background: var(--bg); color: var(--text); }
.hidden { display: none !important; }
h1, h2, h3 { margin: 0; font-weight: 650; }
button, input, select { font: inherit; color: inherit; }

/* вход */
.auth { min-height: 100vh; display: grid; place-items: center; padding: 16px; }
.auth-card { width: 100%; max-width: 380px; background: var(--card); border-radius: 14px; box-shadow: var(--shadow); padding: 28px; }
.auth-card h1 { font-size: 22px; margin-bottom: 4px; }
.auth-card p { color: var(--muted); margin: 0 0 18px; }
.logo { width: 44px; height: 44px; border-radius: 12px; background: var(--primary); color: #fff; display: grid; place-items: center; font-weight: 700; margin-bottom: 14px; }

/* поля */
label.f { display: block; margin-bottom: 14px; }
label.f > span { display: block; font-size: 13px; color: var(--muted); margin-bottom: 4px; }
input[type=text], input[type=password], input[type=date], select {
  width: 100%; padding: 9px 11px; border: 1px solid var(--line); border-radius: 8px; background: var(--card); outline: none;
}
input:focus, select:focus { border-color: var(--primary); box-shadow: 0 0 0 3px var(--primary-bg); }
input[readonly] { background: var(--bg); color: var(--muted); }
.check { display: flex; align-items: center; gap: 10px; padding: 9px 0; cursor: pointer; }
.check input { width: 18px; height: 18px; accent-color: var(--primary); }

/* кнопки */
.btn { border: 1px solid var(--line); background: var(--card); padding: 8px 14px; border-radius: 8px; cursor: pointer; }
.btn:hover { background: var(--bg); }
.btn.primary { background: var(--primary); border-color: var(--primary); color: #fff; font-weight: 600; }
.btn.primary:hover { background: var(--primary-d); }
.btn.danger { background: var(--high); border-color: var(--high); color: #fff; font-weight: 600; }
.btn.wide { width: 100%; padding: 10px; }
.btn.icon { padding: 5px 9px; }
.err { color: var(--high); min-height: 20px; font-size: 14px; margin: 0 0 8px; }

/* шапка */
header { background: var(--card); border-bottom: 1px solid var(--line); position: sticky; top: 0; z-index: 5; }
.bar { max-width: 1180px; margin: 0 auto; padding: 0 16px; height: 58px; display: flex; align-items: center; gap: 22px; }
.brand { display: flex; align-items: center; gap: 10px; font-weight: 700; font-size: 17px; }
.brand .logo { width: 32px; height: 32px; border-radius: 9px; margin: 0; font-size: 14px; }
nav { display: flex; gap: 4px; flex: 1; }
nav button { border: 0; background: none; padding: 8px 14px; border-radius: 8px; cursor: pointer; color: var(--muted); font-weight: 550; }
nav button:hover { background: var(--bg); }
nav button.on { background: var(--primary-bg); color: var(--primary); }
.who { color: var(--muted); font-size: 14px; display: flex; align-items: center; gap: 12px; }

main { max-width: 1180px; margin: 0 auto; padding: 22px 16px 60px; }
.head { display: flex; align-items: center; justify-content: space-between; gap: 12px; flex-wrap: wrap; margin-bottom: 16px; }
.head h2 { font-size: 22px; }
.card { background: var(--card); border-radius: var(--radius); box-shadow: var(--shadow); padding: 18px; }

/* карточки сводки */
.stats { display: grid; grid-template-columns: repeat(4, 1fr); gap: 14px; margin-bottom: 18px; }
.stat { background: var(--card); border-radius: var(--radius); box-shadow: var(--shadow); padding: 16px 18px; border-left: 5px solid var(--line); }
.stat .n { font-size: 30px; font-weight: 700; line-height: 1.1; }
.stat .t { color: var(--muted); font-size: 13px; margin-top: 4px; }
.stat.low { border-left-color: var(--low); } .stat.mid { border-left-color: var(--mid); } .stat.high { border-left-color: var(--high); }
.two { display: grid; grid-template-columns: 3fr 2fr; gap: 14px; }
.bars .row { margin-bottom: 14px; }
.bars .lab { display: flex; justify-content: space-between; font-size: 14px; margin-bottom: 5px; }
.bars .track { height: 9px; background: var(--bg); border-radius: 6px; overflow: hidden; }
.bars .fill { height: 100%; background: var(--primary); border-radius: 6px; }
.card h3 { font-size: 16px; margin-bottom: 12px; }

/* таблицы */
.tools { display: flex; gap: 10px; flex-wrap: wrap; align-items: center; margin-bottom: 14px; }
.tools input[type=text] { width: 230px; }
.tools select { width: auto; }
.tablewrap { overflow-x: auto; }
table { width: 100%; border-collapse: collapse; }
th { text-align: left; font-size: 12.5px; color: var(--muted); font-weight: 600; padding: 10px 12px; border-bottom: 1px solid var(--line); white-space: nowrap; }
td b { white-space: nowrap; }
td { padding: 11px 12px; border-bottom: 1px solid var(--line); vertical-align: middle; }
tbody tr.click { cursor: pointer; }
tbody tr.click:hover { background: var(--bg); }
td.act { white-space: nowrap; text-align: right; }
.empty { text-align: center; color: var(--muted); padding: 38px 12px; }
.badge { display: inline-block; padding: 3px 10px; border-radius: 999px; font-size: 13px; font-weight: 600; white-space: nowrap; }
.badge.c0 { background: var(--low-bg); color: var(--low); }
.badge.c1 { background: var(--mid-bg); color: var(--mid); }
.badge.c2 { background: var(--high-bg); color: var(--high); }
.muted { color: var(--muted); }
.viol { font-size: 13.5px; color: var(--muted); }

/* окно */
.overlay { position: fixed; inset: 0; background: rgba(10,18,30,.5); display: grid; place-items: center; padding: 16px; z-index: 20; overflow-y: auto; }
.modal { background: var(--card); border-radius: 14px; width: 100%; max-width: 560px; padding: 24px; box-shadow: 0 10px 40px rgba(0,0,0,.3); }
.modal h2 { font-size: 20px; margin-bottom: 16px; }
.grid2 { display: grid; grid-template-columns: 1fr 1fr; gap: 0 14px; }
.actions { display: flex; justify-content: flex-end; gap: 10px; margin-top: 16px; }
.preview { background: var(--bg); border-radius: 10px; padding: 12px 14px; margin-top: 8px; font-size: 14px; }
.kv { display: grid; grid-template-columns: 190px 1fr; gap: 8px 12px; font-size: 14.5px; }
.kv dt { color: var(--muted); } .kv dd { margin: 0; }
#toasts { position: fixed; right: 16px; bottom: 16px; display: flex; flex-direction: column; gap: 8px; z-index: 30; }
.toast { background: #1c2733; color: #fff; padding: 11px 16px; border-radius: 10px; box-shadow: var(--shadow); max-width: 340px; }
.toast.bad { background: var(--high); }

@media (max-width: 820px) {
  .stats { grid-template-columns: 1fr 1fr; } .two { grid-template-columns: 1fr; }
  .bar { gap: 10px; } .brand span, .who span { display: none; } .grid2 { grid-template-columns: 1fr; } .kv { grid-template-columns: 1fr; }
}
</style>
</head>
<body>

<!-- вход и создание администратора -->
<div id="auth" class="auth hidden">
  <form class="auth-card" id="authForm">
    <div class="logo">ИБ</div>
    <h1 id="authTitle">Вход в систему</h1>
    <p id="authSub">Аудит информационной безопасности управляющей компании</p>
    <label class="f"><span>Логин</span><input type="text" id="login" autocomplete="username" autofocus></label>
    <label class="f"><span>Пароль</span><input type="password" id="password" autocomplete="current-password"></label>
    <label class="f hidden" id="pass2box"><span>Повторите пароль</span><input type="password" id="password2" autocomplete="new-password"></label>
    <p class="err" id="authErr"></p>
    <button class="btn primary wide" id="authBtn" type="submit">Войти</button>
  </form>
</div>

<!-- основное окно -->
<div id="app" class="hidden">
  <header><div class="bar">
    <div class="brand"><div class="logo">ИБ</div><span>Аудит ИБ УК</span></div>
    <nav id="nav">
      <button data-tab="overview" class="on">Обзор</button>
      <button data-tab="objects">Объекты</button>
      <button data-tab="report">Отчёт</button>
    </nav>
    <div class="who"><span id="whoName">Администратор</span><button class="btn" id="logoutBtn">Выйти</button></div>
  </div></header>

  <main>
    <!-- Обзор -->
    <section id="tab-overview">
      <div class="head"><h2>Обзор</h2><button class="btn primary" id="ovAdd">+ Добавить объект</button></div>
      <div class="stats">
        <div class="stat"><div class="n" id="stTotal">0</div><div class="t">Всего объектов</div></div>
        <div class="stat high"><div class="n" id="stHigh">0</div><div class="t">Высокий риск</div></div>
        <div class="stat mid"><div class="n" id="stMid">0</div><div class="t">Средний риск</div></div>
        <div class="stat low"><div class="n" id="stLow">0</div><div class="t">Низкий риск</div></div>
      </div>
      <div class="two">
        <div class="card"><h3>Самые рискованные объекты</h3><div class="tablewrap"><table><thead><tr><th>Инв. номер</th><th>Отдел</th><th>Ответственный</th><th>Риск</th></tr></thead><tbody id="topBody"></tbody></table></div></div>
        <div class="card bars"><h3>Нарушения</h3><div id="bars"></div></div>
      </div>
    </section>

    <!-- Объекты -->
    <section id="tab-objects" class="hidden">
      <div class="head"><h2>Объекты аудита</h2><button class="btn primary" id="objAdd">+ Добавить объект</button></div>
      <div class="card">
        <div class="tools">
          <input type="text" id="q" placeholder="Поиск по номеру или ответственному">
          <select id="fKind"><option value="none">Без фильтра</option><option value="dept">Отдел</option><option value="type">Тип объекта</option><option value="risk">Уровень риска</option></select>
          <select id="fVal" class="hidden"></select>
          <select id="sKind"><option value="none">Без сортировки</option><option value="risk">По уровню риска</option><option value="date">По дате проверки</option><option value="inv">По инвентарному номеру</option></select>
          <select id="sDir"><option value="0">По возрастанию</option><option value="1">По убыванию</option></select>
        </div>
        <div class="tablewrap"><table>
          <thead><tr><th>№</th><th>Инв. номер</th><th>Тип</th><th>Отдел</th><th>Ответственный</th><th>Проверка</th><th>Риск</th><th></th></tr></thead>
          <tbody id="objBody"></tbody>
        </table></div>
        <div class="muted" id="objCount" style="margin-top:10px"></div>
      </div>
    </section>

    <!-- Отчёт -->
    <section id="tab-report" class="hidden">
      <div class="head"><h2>Отчёт о несоответствиях</h2></div>
      <div class="card" style="margin-bottom:14px">
        <div class="tools" style="margin:0">
          <select id="rMin">
            <option value="1">Все с нарушениями (от 1 балла)</option>
            <option value="3">Средний и высокий риск (от 3 баллов)</option>
            <option value="6">Только высокий риск (от 6 баллов)</option>
          </select>
          <button class="btn primary" id="rBuild">Сформировать отчёт</button>
          <span class="muted" id="rInfo"></span>
        </div>
      </div>
      <div class="card"><div class="tablewrap"><table>
        <thead><tr><th>Инв. номер</th><th>Отдел</th><th>Ответственный</th><th>Нарушения</th><th>Риск</th></tr></thead>
        <tbody id="repBody"></tbody>
      </table></div></div>
    </section>
  </main>
</div>

<div id="modalRoot"></div>
<div id="toasts"></div>

<script>
"use strict";
const $ = id => document.getElementById(id);
const esc = s => String(s).replace(/[&<>"']/g, c => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c]));
let meta = { types: [], departments: [] };
let tab = "overview";

// ---------- запросы к серверу ----------
async function api(path, data) {
  const opt = {};
  if (data) {
    opt.method = "POST";
    opt.headers = { "Content-Type": "application/x-www-form-urlencoded" };
    opt.body = new URLSearchParams(data).toString();
  }
  const r = await fetch(path, opt);
  let j = {};
  try { j = await r.json(); } catch (e) {}
  if (r.status === 401 && path !== "/api/login") { showAuth(true); throw new Error(j.error || "Требуется вход"); }
  if (!r.ok) throw new Error(j.error || "Ошибка " + r.status);
  return j;
}
function toast(text, bad) {
  const t = document.createElement("div");
  t.className = "toast" + (bad ? " bad" : "");
  t.textContent = text;
  $("toasts").appendChild(t);
  setTimeout(() => t.remove(), 3800);
}
const fail = e => toast(e.message, true);

// ---------- даты ----------
const toIso = d => { const p = d.split("."); return p.length === 3 ? p[2] + "-" + p[1] + "-" + p[0] : ""; };
const fromIso = s => { const p = s.split("-"); return p.length === 3 ? p[2] + "." + p[1] + "." + p[0] : ""; };
const todayIso = () => { const d = new Date(); return d.getFullYear() + "-" + String(d.getMonth() + 1).padStart(2, "0") + "-" + String(d.getDate()).padStart(2, "0"); };
const badge = (risk, cls, level) => `<span class="badge c${cls}">${risk} · ${esc(level)}</span>`;

// ---------- вход ----------
let registering = false;
function showAuth(show, hasUsers) {
  $("auth").classList.toggle("hidden", !show);
  $("app").classList.toggle("hidden", show);
  if (!show) return;
  if (hasUsers !== undefined) registering = !hasUsers;
  $("authTitle").textContent = registering ? "Создание администратора" : "Вход в систему";
  $("authSub").textContent = registering ? "Это первый запуск. Задайте логин и пароль." : "Аудит информационной безопасности управляющей компании";
  $("pass2box").classList.toggle("hidden", !registering);
  $("authBtn").textContent = registering ? "Создать и войти" : "Войти";
  $("password").autocomplete = registering ? "new-password" : "current-password";
  $("authErr").textContent = "";
  $("password").value = ""; $("password2").value = "";
}
$("authForm").addEventListener("submit", async e => {
  e.preventDefault();
  $("authErr").textContent = "";
  try {
    if (registering) await api("/api/register", { login: $("login").value, password: $("password").value, password2: $("password2").value });
    else await api("/api/login", { login: $("login").value, password: $("password").value });
    await start();
  } catch (err) { $("authErr").textContent = err.message; }
});
$("logoutBtn").onclick = async () => { await api("/api/logout", {}).catch(() => {}); await start(); };

async function start() {
  const s = await fetch("/api/state").then(r => r.json());
  meta = s;
  if (!s.loggedIn) { showAuth(true, s.hasUsers); return; }
  showAuth(false);
  fillFilterValues();
  setTab(tab);
}

// ---------- вкладки ----------
function setTab(t) {
  tab = t;
  for (const b of document.querySelectorAll("#nav button")) b.classList.toggle("on", b.dataset.tab === t);
  for (const n of ["overview", "objects", "report"]) $("tab-" + n).classList.toggle("hidden", n !== t);
  if (t === "overview") loadOverview(); else if (t === "objects") loadObjects(); else loadReport();
}
$("nav").addEventListener("click", e => { if (e.target.dataset.tab) setTab(e.target.dataset.tab); });

// ---------- обзор ----------
async function loadOverview() {
  try {
    const s = await api("/api/stats");
    $("stTotal").textContent = s.total; $("stHigh").textContent = s.high; $("stMid").textContent = s.medium; $("stLow").textContent = s.low;
    $("topBody").innerHTML = s.top.length ? s.top.map(o =>
      `<tr><td><b>${esc(o.inv)}</b></td><td>${esc(o.departmentName)}</td><td>${esc(o.responsible)}</td><td>${badge(o.risk, o.riskClass, o.level)}</td></tr>`).join("")
      : `<tr><td colspan="4" class="empty">Объектов с нарушениями нет</td></tr>`;
    const items = [["Нет антивируса", s.noAntivirus], ["Устарели антивирусные базы", s.oldAntivirus],
                   ["Пароль не по политике", s.weakPassword], ["ПДн на объектах с нарушениями", s.pdAtRisk]];
    $("bars").innerHTML = items.map(([n, v]) =>
      `<div class="row"><div class="lab"><span>${n}</span><b>${v}</b></div><div class="track"><div class="fill" style="width:${s.total ? v * 100 / s.total : 0}%"></div></div></div>`).join("");
  } catch (e) { fail(e); }
}
$("ovAdd").onclick = () => openForm(null);

// ---------- объекты ----------
function fillFilterValues() {
  const k = $("fKind").value, sel = $("fVal");
  const lists = { dept: meta.departments, type: meta.types, risk: ["Низкий", "Средний", "Высокий"] };
  sel.classList.toggle("hidden", k === "none");
  sel.innerHTML = k === "none" ? "" : lists[k].map((n, i) => `<option value="${i}">${esc(n)}</option>`).join("");
}
let loadTimer = null;
function reloadSoon() { clearTimeout(loadTimer); loadTimer = setTimeout(loadObjects, 200); }
$("fKind").onchange = () => { fillFilterValues(); loadObjects(); };
for (const id of ["fVal", "sKind", "sDir"]) $(id).onchange = loadObjects;
$("q").oninput = reloadSoon;
$("objAdd").onclick = () => openForm(null);

let objects = [];
async function loadObjects() {
  const p = new URLSearchParams({ filter: $("fKind").value, value: $("fVal").value || "0", sort: $("sKind").value, desc: $("sDir").value, q: $("q").value });
  try {
    objects = await api("/api/objects?" + p.toString());
    $("objBody").innerHTML = objects.length ? objects.map((o, i) =>
      `<tr class="click" data-i="${i}"><td class="muted">${i + 1}</td><td><b>${esc(o.inv)}</b></td><td>${esc(o.typeName)}</td><td>${esc(o.departmentName)}</td>` +
      `<td>${esc(o.responsible)}</td><td>${esc(o.checkDate)}</td><td>${badge(o.risk, o.riskClass, o.level)}</td>` +
      `<td class="act"><button class="btn icon" data-act="edit" data-i="${i}" title="Изменить">Изменить</button> <button class="btn icon" data-act="del" data-i="${i}" title="Удалить">Удалить</button></td></tr>`).join("")
      : `<tr><td colspan="8" class="empty">Объекты не найдены</td></tr>`;
    $("objCount").textContent = "Показано объектов: " + objects.length;
  } catch (e) { fail(e); }
}
$("objBody").addEventListener("click", e => {
  const b = e.target.closest("button"), tr = e.target.closest("tr");
  if (!tr || tr.dataset.i === undefined) return;
  const o = objects[+tr.dataset.i];
  if (b && b.dataset.act === "edit") openForm(o);
  else if (b && b.dataset.act === "del") confirmDelete(o);
  else openCard(o);
});

// ---------- окна ----------
function openModal(html) {
  $("modalRoot").innerHTML = `<div class="overlay" id="ov"><div class="modal">${html}</div></div>`;
  $("ov").addEventListener("mousedown", e => { if (e.target.id === "ov") closeModal(); });
}
const closeModal = () => { $("modalRoot").innerHTML = ""; };
document.addEventListener("keydown", e => { if (e.key === "Escape") closeModal(); });

function openCard(o) {
  openModal(`<h2>${esc(o.inv)}</h2><dl class="kv">
    <dt>Тип объекта</dt><dd>${esc(o.typeName)}</dd><dt>Отдел</dt><dd>${esc(o.departmentName)}</dd>
    <dt>Ответственный</dt><dd>${esc(o.responsible)}</dd><dt>Операционная система</dt><dd>${esc(o.os)}</dd>
    <dt>Дата проверки</dt><dd>${esc(o.checkDate)}</dd>
    <dt>Антивирус</dt><dd>${o.antivirus ? "установлен, базы от " + esc(o.avUpdate) : "не установлен"}</dd>
    <dt>Пароль</dt><dd>${o.passwordOk ? "соответствует политике" : "не соответствует политике"}</dd>
    <dt>Персональные данные</dt><dd>${o.personalData ? "обрабатываются" : "не обрабатываются"}</dd>
    <dt>Уровень риска</dt><dd>${badge(o.risk, o.riskClass, o.level)}</dd>
    <dt>Нарушения</dt><dd>${esc(o.violations)}</dd></dl>
    <div class="actions"><button class="btn" id="cClose">Закрыть</button><button class="btn primary" id="cEdit">Изменить</button></div>`);
  $("cClose").onclick = closeModal;
  $("cEdit").onclick = () => openForm(o);
}

function confirmDelete(o) {
  openModal(`<h2>Удалить объект?</h2><p>Объект <b>${esc(o.inv)}</b> (${esc(o.responsible)}) будет удалён безвозвратно.</p>
    <div class="actions"><button class="btn" id="dNo">Отмена</button><button class="btn danger" id="dYes">Удалить</button></div>`);
  $("dNo").onclick = closeModal;
  $("dYes").onclick = async () => {
    try { await api("/api/object/delete", { inv: o.inv }); closeModal(); toast("Объект удалён"); refresh(); } catch (e) { fail(e); }
  };
}

function openForm(o) {
  const edit = !!o;
  const opts = (arr, sel) => arr.map((n, i) => `<option value="${i}"${i === sel ? " selected" : ""}>${esc(n)}</option>`).join("");
  openModal(`<h2>${edit ? "Изменение объекта" : "Новый объект аудита"}</h2>
    <form id="oForm">
      <div class="grid2">
        <label class="f"><span>Инвентарный номер</span><input type="text" id="oInv" maxlength="30" placeholder="УК-ПК-001" ${edit ? "readonly" : ""} value="${edit ? esc(o.inv) : ""}"></label>
        <label class="f"><span>Дата проверки</span><input type="date" id="oCheck" value="${edit ? toIso(o.checkDate) : todayIso()}"></label>
        <label class="f"><span>Тип объекта</span><select id="oType">${opts(meta.types, edit ? o.type : 0)}</select></label>
        <label class="f"><span>Отдел</span><select id="oDept">${opts(meta.departments, edit ? o.department : 0)}</select></label>
        <label class="f"><span>Ответственный</span><input type="text" id="oResp" placeholder="Иванова А. С." value="${edit ? esc(o.responsible) : ""}"></label>
        <label class="f"><span>Операционная система</span><input type="text" id="oOs" placeholder="Windows 10" value="${edit ? esc(o.os) : ""}"></label>
      </div>
      <label class="check"><input type="checkbox" id="oAv"${edit && o.antivirus ? " checked" : ""}> Антивирус установлен</label>
      <label class="f" id="avBox"><span>Дата обновления антивирусных баз</span><input type="date" id="oAvDate" value="${edit && o.antivirus ? toIso(o.avUpdate) : todayIso()}"></label>
      <label class="check"><input type="checkbox" id="oPw"${!edit || o.passwordOk ? " checked" : ""}> Пароль соответствует парольной политике</label>
      <label class="check"><input type="checkbox" id="oPd"${edit && o.personalData ? " checked" : ""}> Обрабатываются персональные данные жильцов</label>
      <div class="preview" id="oPrev">Расчёт риска…</div>
      <p class="err" id="oErr" style="margin-top:10px"></p>
      <div class="actions" style="margin-top:6px"><button type="button" class="btn" id="oCancel">Отмена</button><button class="btn primary" type="submit">${edit ? "Сохранить" : "Добавить"}</button></div>
    </form>`);
  const data = () => ({
    inv: $("oInv").value, type: $("oType").value, department: $("oDept").value, responsible: $("oResp").value, os: $("oOs").value,
    checkDate: fromIso($("oCheck").value), antivirus: $("oAv").checked ? "1" : "0", avUpdate: fromIso($("oAvDate").value),
    passwordOk: $("oPw").checked ? "1" : "0", personalData: $("oPd").checked ? "1" : "0"
  });
  let t = null;
  const preview = () => {
    $("avBox").classList.toggle("hidden", !$("oAv").checked);
    clearTimeout(t);
    t = setTimeout(async () => {
      try {
        const p = await api("/api/preview", data());
        $("oPrev").innerHTML = `Уровень риска: ${badge(p.risk, p.riskClass, p.level)}<div class="viol" style="margin-top:6px">${esc(p.violations)}</div>`;
      } catch (e) {}
    }, 120);
  };
  for (const el of $("oForm").querySelectorAll("input,select")) { el.addEventListener("input", preview); el.addEventListener("change", preview); }
  preview();
  $("oCancel").onclick = closeModal;
  $("oForm").addEventListener("submit", async e => {
    e.preventDefault();
    $("oErr").textContent = "";
    try {
      await api(edit ? "/api/object/update" : "/api/object", data());
      closeModal(); toast(edit ? "Изменения сохранены" : "Объект добавлен"); refresh();
    } catch (err) { $("oErr").textContent = err.message; }
  });
  (edit ? $("oResp") : $("oInv")).focus();
}

// ---------- отчёт ----------
async function loadReport() {
  try {
    const r = await api("/api/report");
    $("repBody").innerHTML = r.items.length ? r.items.map(x =>
      `<tr><td><b>${esc(x.inv)}</b></td><td>${esc(x.departmentName)}</td><td>${esc(x.responsible)}</td><td class="viol">${esc(x.violations)}</td><td>${badge(x.risk, x.riskClass, x.level)}</td></tr>`).join("")
      : `<tr><td colspan="5" class="empty">${r.exists ? "В отчёте нет объектов" : "Отчёт ещё не сформирован. Выберите порог и нажмите «Сформировать отчёт»."}</td></tr>`;
    $("rInfo").textContent = r.exists ? "Объектов в отчёте: " + r.items.length : "";
  } catch (e) { fail(e); }
}
$("rBuild").onclick = async () => {
  try {
    const r = await api("/api/report/build", { minRisk: $("rMin").value });
    toast("Отчёт сформирован: объектов " + r.count);
    loadReport();
  } catch (e) { fail(e); }
};

function refresh() { setTab(tab); }
start().catch(fail);
</script>
</body>
</html>
)HTML";
