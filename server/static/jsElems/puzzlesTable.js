import { fetchAPI } from '../jsUtils/fetchAPI.js';
import puzzleInput from './puzzleInput.js';
//import { puzzleSpace } from './puzzleSpace.js';

const puzzlesTable = document.createElement('table');
const tbody = document.createElement('tbody');

const puzzleRowToggleActive = (e) => {
  if (e.target.nodeName === 'TR')
    e.target.classList.toggle('puzzle-table-active');
  else if (e.target.nodeName === 'TD') {
    puzzleInput.setInputIO(e.target.value);
    //    puzzleSpace.fromString(e.target.value);
  }
};

const createPuzzles = (data, error = null) => {
  if (error) {
    console.log(error);
  }
  else {
    for (const elem of data) {
      if (!elem.files.length)
        continue;
      const dir = document.createElement('tr');
      dir.classList.add('puzzles-tr');
      dir.innerText = elem.puzzleDir;
      dir.onclick = puzzleRowToggleActive;

      for (const file of elem.files) {
        const td = document.createElement('td');
        td.innerText = file.f;
        td.value = file.data;

        dir.appendChild(td);
      }
      tbody.appendChild(dir);
      dir.classList.add('puzzle-table-active');
    }
  }
  puzzlesTable.appendChild(tbody);
};

const getPuzzlesAvailable = fetchAPI('./api/getPuzzlesAvailable', 'JSON', createPuzzles)();

export default puzzlesTable;
