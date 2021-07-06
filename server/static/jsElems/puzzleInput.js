import { puzzleTiles, tilesToString } from "./puzzleSpace.js";

const puzzleInput = document.getElementById('puzzle-IO');

puzzleInput.value = tilesToString();

export default { puzzleInput };
