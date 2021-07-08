import { puzzleSpace } from "./puzzleSpace.js";

const puzzleInput = document.getElementById('puzzle-IO');

puzzleInput.value = puzzleSpace.tilesToString();

export default { puzzleInput };
