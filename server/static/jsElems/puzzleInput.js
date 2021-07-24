import { puzzleSpace } from "./puzzleSpace.js";

export const puzzleInput = document.getElementById('puzzle-IO');

const subbedCBs = [];

puzzleInput.oninput = () => {
  subbedCBs.forEach((cb) => cb());
};

export function setInputIO(str) {
  puzzleInput.value = str;
  subbedCBs.forEach((cb) => cb());
}

export function subscribeInputChange(cb) {
  subbedCBs.push(cb);
  puzzleInput.addEventListener('input', cb);
}

export default { puzzleInput, setInputIO, subscribeInputChange };
