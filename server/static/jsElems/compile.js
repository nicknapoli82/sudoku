import { fetchAPI } from "../jsUtils/fetchAPI.js";

const compileButton = document.getElementById('compile-button');
const compileResults = document.getElementById('compile-results');

function compileRes(data, error = null) {
  compileResults.innerHTML = `<pre>${error || data.stderr || 'Success'}</pre>`;
}

function compileKey() {
  fetchAPI('./api/compile', 'JSON', compileRes)();
}

compileButton.onclick = fetchAPI('./api/compile', 'JSON', compileRes);

export default { compileKey };
