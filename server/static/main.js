import './jsUtils/fetchAPI.js';
import compile from './jsElems/compile.js';
import { keyManager } from './jsUtils/keyManager.js';
import './jsElems/puzzleSpace.js';
import './jsElems/puzzleInput.js';
import './jsElems/puzzleContext.js';
import './jsElems/solvePuzzle.js';

keyManager.registerKeys(compile.compileKey, 'c', 'C');
