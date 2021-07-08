import fetchAPI from './jsUtils/fetchAPI.js';
import compile from './jsElems/compile.js';
import puzzle from './jsElems/puzzleSpace.js';
import puzzleInput from './jsElems/puzzleInput.js';
import { keyManager } from './jsUtils/keyManager.js';

keyManager.registerKeys(compile.compileKey, 'c', 'C');
