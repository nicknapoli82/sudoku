import { fetchAPI } from "../jsUtils/fetchAPI.js";

const compileButton = document.getElementById('compile');
const compileResults = document.getElementById('compile-results');

function compileRes(data, error = null) {
  compileResults.innerHTML = `<pre>${error || data.stderr || 'Success'}</pre>`;
}

compileButton.onclick = fetchAPI('./api/compile', compileRes);

export default { compileButton, compileResults };
