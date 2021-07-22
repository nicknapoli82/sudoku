import help from './help.js';
import puzzlesTable from './puzzlesTable.js';

const puzzleContext = document.getElementById('puzzles-context');

const contexts = {
  'puzzles': puzzlesTable,
  active: 'puzzles'
};

export function swapContext(c) {
  puzzleContext.removeChild(contexts[contexts.active]);
  puzzleContext.appendChild(contexts[c]);
  contexts.active = c;
}

puzzleContext.appendChild(puzzlesTable);

export default swapContext;
