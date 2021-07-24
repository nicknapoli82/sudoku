import { puzzleInput, setInputIO } from './puzzleInput.js';
import { fetchAPI } from '../jsUtils/fetchAPI.js';

export const solvePuzzle = document.getElementById('solve-puzzle');
const sendPuzzle = fetchAPI('./api/solvePuzzle', 'text', sendAndUpdate);
solvePuzzle.onclick = () => sendPuzzle(puzzleInput.value);

function sendAndUpdate(data, err = null) {
  const compileResults = document.getElementById('compile-results');
  if (err) {
    compileResults.innerHTML = `<pre>${err || data.stderr || 'Success'}</pre>`;
  }
  else {
    let result = [];
    data = JSON.parse(data);
    data.stdout.split('').forEach((c) => {
      if (Number.parseInt(c))
        result.push(c);
    });
    result = result.join('');
    compileResults.innerHTML = `<pre>${err || data.stdout || JSON.stringify(data.error)}</pre>`;
    setInputIO(result);
  }
}

export default { solvePuzzle };
